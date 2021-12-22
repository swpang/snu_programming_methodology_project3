#include "SmartController.h"
#include "Controller.h"
#include "Food.h"

#include <algorithm>

/**
 * Get maximum height among the foods in this shelf
 * 
 * @return maximum height of the foods.
 */
int SmartController::maxHeight(Shelf &shelf)
{
    if (shelf.vec.empty())
        return 0;
    auto comp = [](const FoodPtr a, const FoodPtr b)
    {
        return (a->getSize().second < b->getSize().second);
    };
    auto it_shelf = max_element(shelf.vec.begin(), shelf.vec.end(), comp);

    return (*it_shelf)->getSize().second;
}

/**
 * Store a given food object into the refrigerator
 * 
 * This function should follows the 'Shelf First Fit (Shelf-FF) algorithm, which is given in the PPT slide.
 * Your implemented algorithm should find the proper position of the new food.
 * Also, the given food object may in the stack memory. You should save it in the heap memory and reference
 * it from both food_list and shelves.
 * 
 * @param name The name of the food to be stored
 * @param size width and height of the food
 * @param exp the expire date of the food
 * @return true if inserting the food for both food_list and shelves has been succeed
 */
bool SmartController::stackFood(const string name, intPair foodSize, int exp)
{   
    FoodPtr food;

    bool placed = false;
    int x, y;

    if (shelves.size() == 0) // if no shelves, place the first shelf
        shelves.push_back(Shelf(0));
    
    if (foodList.find(name) == foodList.end()) // 
        foodList.insert(pair<string, vector<FoodPtr>>(name, { }));

    if (foodSize.first > size.first) // 음식이 냉장고보다 큰 경우
        return false;

    vector<FoodPtr> &v = foodList[name];

    for (auto & elem : shelves) {
        int n = elem.vec.size();
        
        if (n >= 1) { // shelf 가 비지 않은 경우
            if (elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first + foodSize.first > size.first
                || elem.vec[n - 1]->getPos().second + foodSize.second > size.second) 
                continue;
            else {
                x = elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first;
                y = elem.height;
                placed = true;
            }
        }

        else { // shelf 가 빈 경우
            x = 0;
            y = elem.height;
            placed = true;
        } 

        if (placed) {
            food = new FoodInFridge(Food(name, foodSize, exp), x, y);  
            elem.vec.push_back(food);
            v.push_back(food);

            cout << "Inserting " << name << " into x: " << x << ", y: " << y << endl;
            return true;
        }
    }

    // 새로운 shelf를 추가해야한 경우 (모든 shelf를 다 돌았는데 배치를 못받은 경우에 대해)
    if (shelves.back().height + maxHeight(shelves.back()) > size.second) return false;
    if (shelves.back().height + maxHeight(shelves.back()) + foodSize.second > size.second) return false;
    shelves.push_back(Shelf(shelves.back().height + maxHeight(shelves.back())));
    x = 0;
    y = shelves.back().height;
    food = new FoodInFridge(Food(name, foodSize, exp), x, y);

    shelves.back().vec.push_back(food);
    v.push_back(food);

    cout << "Inserting " << name << " into x: 0, y: " << shelves.back().height << endl;
    return true;
}

/**
 * Pop food with shortest expire date from both foodList and shelves.
 * This function does not return the object.
 * The pop process should follows the algorithm in PPT slide.
 * 
 * @param food_name a name of food to be popped
 * @return 
 */
bool SmartController::popFood(const string food_name) // void
{
    FoodPtr min;
    bool moveX = false;
    bool moveY = false;
    bool erased = false;
    int foodSize(0);
    int deltaY(0);
    vector<FoodPtr> temp = {};

    if (foodList.find(food_name) == foodList.end()) return false;

    min = *findMinExpFood(food_name);
    vector<FoodPtr> &v = foodList[food_name];

    foodSize = v.front()->getSize().first;

    for (auto & shelf : shelves)
        for (auto & elem : shelf.vec)
            if (elem->getName() == food_name && elem->getExp() == min->getExp())
                temp.push_back(elem);
    if (temp.size() == 0)
        return false;
    
    sort(temp.begin(), temp.end(), comparePos);

    cout << "position to pop : x: " << temp.back()->getPos().first << ", y: " << temp.back()->getPos().second << endl;
    cout << food_name << " with expire date " << min->getExp() << " has been popped" << endl;

    for (int i = 0; i < v.size(); i++) {
        if (v[i]->getPos() == temp.back()->getPos()) {
            v.erase(v.begin() + i);
            break;
        }
    }

    for (int i = 0; i < shelves.size(); i++) {
        int oldMax = maxHeight(shelves[i]);
        if (moveY) shelves[i].height -= deltaY;
        int n = shelves[i].vec.size();
        for (int j = 0; j < n; j++) {
            if (temp.back()->getPos() == shelves[i].vec[j]->getPos() && temp.back()->getExp() == shelves[i].vec[j]->getExp() 
                && temp.back()->getName() == shelves[i].vec[j]->getName()
                && !erased) {
                shelves[i].vec.erase(shelves[i].vec.begin() + j);
                if (shelves[i].vec.size() == 0)
                    shelves.erase(shelves.begin() + i);
                erased = true;
                // shelf 재정렬 관련 모든게 다 여기에 들어가야함!
                if (shelves[i].height != shelves.back().height) { // 가장 위 shelf가 아닌 경우
                    int newMax = maxHeight(shelves[i]);
                    if (newMax < oldMax) { moveY = true; deltaY = oldMax - newMax; }
                }
                moveX = true;
                continue;
            }
            if (moveX) {
                int newX = shelves[i].vec[j - 1]->getPos().first - foodSize;
                int y = shelves[i].height;
                intPair newPos(newX, y);

                vector<FoodPtr> &temp = foodList[shelves[i].vec[i - 1]->getName()];
                for (int k = 0; k < temp.size(); k++) {
                    if (temp[k]->getExp() == shelves[i].vec[j - 1]->getExp() && temp[k]->getPos() == shelves[i].vec[j - 1]->getPos()) {
                        temp[k]->setPos(newPos);
                        break;
                    }
                }
                shelves[i].vec[j - 1]->setPos(newPos);
            }
            if (moveY && !moveX) {
                int x = shelves[i].vec[j]->getPos().first;
                int newY = shelves[i].height;
                intPair newPos(x, newY);

                vector<FoodPtr> &temp = foodList[shelves[i].vec[i]->getName()];
                for (int k = 0; k < temp.size(); k++) {
                    if (temp[k]->getExp() == shelves[i].vec[j]->getExp() && temp[k]->getPos() == shelves[i].vec[j]->getPos()) {
                        temp[k]->setPos(newPos);
                        break;
                    }
                }
                shelves[i].vec[j]->setPos(newPos);
            }
        } 
        moveX = false;
    }
    return true;
}