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
    if (shelves.size() == 0)
        shelves.push_back(Shelf(0));

    for (auto elem : shelves) {
        int n = elem.vec.size();
        if (n >= 1) { // shelf 가 비지 않은 경우
            int emptyPos = elem.vec[n - 1]->getPos().first + elem.vec[n - 1]->getSize().first;
            if (emptyPos + foodSize.first >= size.first) // 이 shelf에는 못들어간다!
                continue;
            else { // 이 shelf에 들어갈만한 빈공간이 충분하다!
                FoodPtr food = new FoodInFridge(Food(name, foodSize, exp), emptyPos, elem.height);

                map<string, vector<FoodPtr>>::iterator it = foodList.begin();
                while (it != foodList.end()) { // 이미 foodList에 같은 항목의 음식이 들어있다면
                    if (it->first == name) {
                        elem.vec.push_back(food);
                        it->second.push_back(food);

                        cout << "x: " << elem.height << "y: " << 0 << endl;
                        return true;
                    }                        
                }
                // 같은 항목의 음식이 들어있지 않다면
                foodList.insert(pair<string, vector<FoodPtr>>(name, { food }));
                return true;
            }
        }
        else { // shelf 가 빈 경우
            FoodPtr food = new FoodInFridge(Food(name, foodSize, exp), 0, elem.height);

            map<string, vector<FoodPtr>>::iterator it = foodList.begin();
            while (it != foodList.end()) { 
                if (it->first == name) { // 이미 foodList에 같은 항목의 음식이 들어있다면
                    elem.vec.push_back(food);
                    it->second.push_back(food);

                    cout << "x: " << elem.height << "y: " << 0 << endl;
                    return true;
                }                        
            }
            // 같은 항목의 음식이 들어있지 않다면
            foodList.insert(pair<string, vector<FoodPtr>>(name, { food }));
            return true;
        } 
    }

    // 새로운 shelf를 추가해야한 경우 (모든 shelf를 다 돌았는데 배치를 못받은 경우에 대해)
    if ((shelves.size() + 1) * MAX_HEIGHT > size.second)
        return false;
    
    shelves.push_back(Shelf(shelves.size() * MAX_HEIGHT));
    FoodPtr food = new FoodInFridge(Food(name, foodSize, exp), 0, shelves.back().height);

    map<string, vector<FoodPtr>>::iterator it = foodList.begin();
    while (it != foodList.end()) { // 이미 foodList에 같은 항목의 음식이 들어있다면
        if (it->first == name) {
            shelves.back().vec.push_back(food);
            it->second.push_back(food);

            cout << "x: " << shelves.back().height << "y: " << 0 << endl;
            return true;
        }                        
    }
    // 같은 항목의 음식이 들어있지 않다면
    foodList.insert(pair<string, vector<FoodPtr>>(name, { food }));
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
    

    return false;
}
