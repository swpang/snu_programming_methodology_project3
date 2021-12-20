#include "NormalController.h"
#include "Controller.h"
#include "Food.h"


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

    if (shelves.size() == 0)
        shelves.push_back(Shelf(0));
    
    if (foodList.find(name) == foodList.end())
        foodList.insert(pair<string, vector<FoodPtr>>(name, { }));

    vector<FoodPtr> &v = foodList[name];

    for (auto & elem : shelves) {
        int n = elem.vec.size();
        if (n >= 1) { // shelf 가 비지 않은 경우
            int emptyPos = elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first;
            if (emptyPos + foodSize.first >= size.first) // 이 shelf에는 못들어간다!
                continue;
            else { // 이 shelf에 들어갈만한 빈공간이 충분하다!
                food = new FoodInFridge(Food(name, foodSize, exp), emptyPos, elem.height);

                elem.vec.push_back(food);
                v.push_back(food);
                cout << "Inserting " << name << " into x: " << emptyPos << ", y: " << elem.height << endl;
                return true;
            }
        }
        else { // shelf 가 빈 경우
            food = new FoodInFridge(Food(name, foodSize, exp), 0, elem.height);

            elem.vec.push_back(food);
            cout << elem.vec.size() << endl;
            v.push_back(food);
            cout << "Inserting " << name << " into x: 0, y: " << elem.height << endl;
            return true;
        } 
    }

    // 새로운 shelf를 추가해야한 경우 (모든 shelf를 다 돌았는데 배치를 못받은 경우에 대해)
    if ((shelves.size() + 1) * MAX_HEIGHT > size.second)
        return false;
    
    shelves.push_back(Shelf(shelves.size() * MAX_HEIGHT));
    food = new FoodInFridge(Food(name, foodSize, exp), 0, shelves.back().height);

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
    min = *findMinExpFood(food_name);
    vector<FoodPtr> &v = foodList[food_name];

    for (auto & shelf : shelves) {
        for (auto & elem : shelf.vec) {
            if (elem->getName() == food_name && elem->getExp() == min->getExp()) {
                cout << "position to pop : x: " << elem->getPos().first << ", y: " << elem->getPos().second << endl;
                cout << "A(n) " << food_name << " with expire date " << min->getExp() << " has been popped" << endl;
                v.erase(findMinExpFood(food_name));
                return true;
            }
        }
    }
    
    return false;
}
