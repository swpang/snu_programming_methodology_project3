#include "NormalController.h"
#include "Controller.h"
#include "Food.h"

#include <algorithm>


/**
 * Store a given food object into the refrigerator. 
 * 
 * This function should follows the 'Modified Shelf First Fit (Shelf-FF) algorithm, which is given in the PPT slide.
 * Your implemented algorithm should find the proper position of the new food object.
 * The new food object should be initialized in the heap memory and referenced from both food_list and shelves.
 * 
 * @param name The name of the food to be stored
 * @param size width and height of the food
 * @param exp the expire date of the food
 * @return true if inserting the food for both food_list and shelves has been succeed
 */
bool NormalController::stackFood(const string name, intPair foodSize, int exp)
{
    FoodPtr food;

    bool placed = false;
    int x, y;

    if (shelves.size() == 0)
        shelves.push_back(Shelf(0));
    
    if (foodList.find(name) == foodList.end())
        foodList.insert(pair<string, vector<FoodPtr>>(name, { }));

    if (foodSize.first > size.first) // 음식이 냉장고보다 큰 경우
        return false;

    vector<FoodPtr> &v = foodList[name];

    for (auto & elem : shelves) {
        int n = elem.vec.size();
        
        if (n >= 1) { // shelf 가 비지 않은 경우
            for (int i = 0; i < n - 1; i++) {
                if (foodSize.first <= elem.vec[i + 1]->getPos().first - elem.vec[i]->getPos().first 
                        - elem.vec[i]->getSize().first) { // shelf에 새로운 food가 들어갈 만한 자리가 있는지 확인
                    x = elem.vec[i]->getPos().first + elem.vec[i]->getSize().first;
                    y = elem.height;
                    placed = true;
                    break;
                }
            }

            if (!placed) {
                if (elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first + foodSize.first >= size.first) 
                    continue;
                else {
                    x = elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first;
                    y = elem.height;
                    placed = true;
                }
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
    if ((shelves.size() + 1) * MAX_HEIGHT > size.second)
        return false;
    
    shelves.push_back(Shelf(shelves.size() * MAX_HEIGHT));
    x = 0;
    y = shelves.back().height;
    food = new FoodInFridge(Food(name, foodSize, exp), x, y);

    shelves.back().vec.push_back(food);
    v.push_back(food);

    cout << "Inserting " << name << " into x: 0, y: " << shelves.back().height << endl;
    return true;
}


/**
 * Pop a food with shortest expire date from both foodList and shelves.
 * This function does not return the object.
 * The pop process should follows the algorithm in PPT slide.
 * 
 * @param food_name a name of food to be popped
 * @return 
 */
bool NormalController::popFood(const string food_name)
{
    FoodPtr min;
    bool erased = false;
    vector<FoodPtr> temp = {};

    min = *findMinExpFood(food_name);
    vector<FoodPtr> &v = foodList[food_name];

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

    for (auto & shelf : shelves) {
        for (int i = 0; i < shelf.vec.size(); i++) {
            if (temp.back()->getPos() == shelf.vec[i]->getPos() && temp.back()->getExp() == shelf.vec[i]->getExp() && temp.back()->getName() == shelf.vec[i]->getName()) {
                shelf.vec.erase(shelf.vec.begin() + i);
                break;
            }
        }
    }

    return true;
}
