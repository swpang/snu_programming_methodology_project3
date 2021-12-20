#include "SmartRefrigerator.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "Food.h"
#include "Recipe.h"

/**
 * A constructor of the class
 * SmartController should be initialized in this constructor
 */
SmartRefrigerator::SmartRefrigerator()
{
    controller = new SmartController(size, foodList);
};

/**
 * add recipes from a text file (.txt)
 * This function should add a new recipe object with the given info from the
 * file to the private variable recipe (vector)
 */
void SmartRefrigerator::addRecipeFromFile()
{
    string recipeFile;
    cout << "Enter your file name : ";
    cin >> recipeFile;
    ifstream recipe_list(recipeFile);

    if (!recipe_list)
    {
        cout << "There is no recipe file!" << endl;
        return;
    } else {
        int line(0);
        while (recipe_list.peek() != EOF) {
            Recipe* recipe;

            string mealName;
            string foodName;
            int foodCount;
            double satisfaction;
            string line;

            strIntPair ingredient;
            vector<strIntPair> ingredient_vec;
            
            getline(recipe_list, line);
            istringstream iss(line);
            
            string temp;
            int idx(0);
            while (iss >> temp) {
                if (idx == 0) mealName = temp;
                else if (temp != "/") {
                    foodName = temp;
                    iss >> temp;
                    foodCount = stoi(temp);
                    ingredient.first = foodName;
                    ingredient.second = foodCount;
                    ingredient_vec.push_back(ingredient);
                }
                else {
                    iss >> temp;
                    satisfaction = stoi(temp);
                }
                idx++;
            }
            
            recipe = new Recipe(mealName, ingredient_vec, satisfaction);
            recipes.push_back(*recipe);
        }
    }
    recipe_list.close();
}

/**
 * show the current recipes of the smart refrigetrator
 * e.g. recipe name : [ingredient1, # of ingredient1]...[] / score 7
 *
 */
void SmartRefrigerator::showRecipe()
{
    recipeVec ::iterator iter;
    if (recipes.empty())
        cout << "There is no recipe yet." << endl;
    else
    {
        for (iter = recipes.begin(); iter != recipes.end(); ++iter)
        {
            cout << iter->getName() << " : ";
            for (int i = 0; i < iter->getIngredients().size(); i++)
            {
                cout << '[' << iter->getIngredients()[i].first << ',' << iter->getIngredients()[i].second << ']';
            }
            cout << " / score " << iter->getScore() << endl;
        }
    }
}

/**
 * recommend the best possible combination (with repetition) of three recipes
 * Below are some rules about the recommendation
 * 1) the number of each ingredient required by a combination should be less
 * than the number of each ingredient in the refrigerator in order to be
 * considered as a possible combination 2) the sum of recipe scores of a
 * combination should be normalized by the highest score sum of the possible
 * combination 3) the exp sum of the foods in the refrigerator considering of a
 * combination should be normalized by the longest exp (the greatest number)
 * from a possible combination
 */
void SmartRefrigerator::recommendMealCourses() {
  vector<tuple<string, string, string, int, int>> total_score;
    for (auto iter = recipes.begin(); iter <= recipes.end() - 3; iter++)
    {
        bool possible_course;
        int bacon_num = 0;
        int onion_num = 0;
        int pasta_num = 0;
        int tomato_num = 0;
        int egg_num = 0;
        int bread_num = 0;
        int lettuce_num = 0;
        int milk_num = 0;
        vector<pair<string, int>> course_require;
        for (int j = 0; j < 3; j++)
        {
            for (int i = 0; i < (iter + j)->getIngredients().size(); i++)
            {
                if ((iter + j)->getIngredients()[i].first == "bacon")
                {
                    bacon_num = bacon_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "onion")
                {
                    onion_num = onion_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "pasta")
                {
                    pasta_num = pasta_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "tomato")
                {
                    tomato_num = tomato_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "egg")
                {
                    egg_num = egg_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "bread")
                {
                    bread_num = bread_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "lettuce")
                {
                    lettuce_num = lettuce_num + (iter + j)->getIngredients()[i].second;
                }
                else if ((iter + j)->getIngredients()[i].first == "milk")
                {
                    milk_num = milk_num + (iter + j)->getIngredients()[i].second;
                }
            }
        }
        // course에 필요한 3가지 meal에 포함되는 총 식재로의 종류와 개수
        if (bacon_num != 0)
        {
            course_require.push_back(make_pair("bacon", bacon_num));
        }
        if (onion_num != 0)
        {
            course_require.push_back(make_pair("onion", onion_num));
        }
        if (pasta_num != 0)
        {
            course_require.push_back(make_pair("pasta", pasta_num));
        }
        if (tomato_num != 0)
        {
            course_require.push_back(make_pair("tomato", tomato_num));
        }
        if (egg_num != 0)
        {
            course_require.push_back(make_pair("egg", egg_num));
        }
        if (bread_num != 0)
        {
            course_require.push_back(make_pair("bread", bread_num));
        }
        if (lettuce_num != 0)
        {
            course_require.push_back(make_pair("lettuce", lettuce_num));
        }
        if (milk_num != 0)
        {
            course_require.push_back(make_pair("milk", milk_num));
        }
        for (int i = 0; i < course_require.size(); i++)
        {
            auto iter_temp = foodList.find(course_require[i].first);
            if (iter_temp == foodList.end()) // course에 필요한 식재료 중 하나라도 냉장고에 존재하지 않을 경우 impossible한 course
            {
                possible_course = false;
                break;
            }
            else
            {
                string temp = course_require[i].first;
                if (foodList[temp].size() >= course_require[i].second)
                {
                    possible_course = true;
                }
                else // course에 필요한 식재료 중 하나라도 존재하긴 하지만 필요한 개수보다 작을 경우 impossible한 course
                {
                    possible_course = false;
                    break;
                }
            }
        }
        if (possible_course == true)
        {
            int temp1 = 0; // satisfy score
            for (int i = 0; i < 3; i++)
            {
                temp1 = temp1 + (iter + i)->getScore();
            }
            int temp2 = 0; // expiration score
            for (int i = 0; i < course_require.size(); i++)
            {
                string temp = course_require[i].first;
                temp2 = temp2 + (foodList[temp].size() - course_require[i].second) * foodList[temp][0]->getExp();
            }
            total_score.push_back(make_tuple(iter->getName(), (iter + 1)->getName(), (iter + 2)->getName(), temp1, temp2));
        }
    }
    int highest_index = 0;
    int second_highest_index = 0;
    int third_highest_index = 0;
    int max1 = get<3>(total_score[0]) + get<4>(total_score[0]);
    for (int i = 0; i < total_score.size(); i++)
    {
        if (get<3>(total_score[i]) + get<4>(total_score[i]) > max1)
        {
            max1 = get<3>(total_score[i]) + get<4>(total_score[i]);
            highest_index = i;
        }
    }
    int max2 = get<3>(total_score[0]) + get<4>(total_score[0]);
    for (int i = 0; i < total_score.size(); i++)
    {
        if (i != highest_index)
        {
            if (get<3>(total_score[i]) + get<4>(total_score[i]) > max2)
            {
                max2 = get<3>(total_score[i]) + get<4>(total_score[i]);
                second_highest_index = i;
            }
        }
    }
    int max3 = get<3>(total_score[0]) + get<4>(total_score[0]);
    for (int i = 0; i < total_score.size(); i++)
    {
        if (i != highest_index && i != second_highest_index)
        {
            if (get<3>(total_score[i]) + get<4>(total_score[i]) > max3)
            {
                max3 = get<3>(total_score[i]) + get<4>(total_score[i]);
                third_highest_index = i;
            }
        }
    }
    double satis_score1 = 1;
    double satis_score2 = get<3>(total_score[second_highest_index])/get<3>(total_score[highest_index]);
    double satis_score3 = get<3>(total_score[third_highest_index])/get<3>(total_score[highest_index]);
    double expir_score1 = 1;
    double expir_score2 = get<4>(total_score[second_highest_index])/get<4>(total_score[highest_index]);
    double expir_score3 = get<4>(total_score[third_highest_index])/get<4>(total_score[highest_index]);
    cout << "1. " << get<0>(total_score[highest_index]) << "2. " << get<1>(total_score[highest_index]) << "3. " << get<2>(total_score[highest_index]) << "  /  total score sum : " << 
    satis_score1+expir_score1 << "  (" << satis_score1 << " / " << expir_score1 << ")" << endl;
    cout << "1. " << get<0>(total_score[second_highest_index]) << "2. " << get<1>(total_score[second_highest_index]) << "3. " << get<2>(total_score[second_highest_index]) << "  /  total score sum : " << 
    satis_score2+expir_score2 << "  (" << satis_score2 << " / " << expir_score2 << ")" << endl;
    cout << "1. " << get<0>(total_score[third_highest_index]) << "2. " << get<1>(total_score[third_highest_index]) << "3. " << get<2>(total_score[third_highest_index]) << "  /  total score sum : " << 
    satis_score3+expir_score3 << "  (" << satis_score3 << " / " << expir_score3 << ")" << endl;
}

/**
 * base function of the smart refrigerator
 * User should type a number between 1 and 4 to call a function of the smart
 * regrigerator Then, this function utilizes the other functions of the smart
 * refrigerator as the user's input Below is the list of options that menuSelect
 * should support 1) display 2) insert food 2-1) insert a list of foods from a
 * text file (.txt) 2-2) insert a food directly 3) pop food 4) add recipe 5)
 * show recipe 6) recommend meals 7) end program Unless the user ends the
 * program ( 7) end program ), this function does not terminate and keeps
 * requiring the user to type a number
 * @return 0 when the user ends the program by typing else 1
 */

int SmartRefrigerator::menuSelect()
{
    int i;
    while (1)
    {
        cout << "************************************************************" << endl;
        cout << "************************   Menu   **************************" << endl;
        cout << "************************************************************" << endl;

        cout << "1. display" << endl
             << "2. insert food" << endl
             << "3. pop food" << endl
             << "4. add recipe" << endl
             << "5. show recipe" << endl
             << "6. recommend meal courses" << endl
             << "7. end program" << endl
             << "Select the function you want : ";
        cin >> i;
        if (i < 1 || i > 7){
            cout << "Type an integer from 1 to 7!" << endl;
            if (cin.fail())
            {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        else {break;}
    }
    if (i == 7)
        return 0;
    else
    {
        switch (i)
        {
            case 1:
                display();
                break;
            case 2:
            {
                int j;
                while (1)
                {
                    cout << "1. Insert a list of foods from a text file" << endl;
                    cout << "2. Insert a food directly" << endl;
                    cout << "Enter how to insert your food : ";
                    cin >> j;
                    if (!(j == 1 or j == 2))
                    {
                        cout << "Type 1 or 2 only" << endl;
                        if (cin.fail())
                        {
                            cin.clear();
                            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (j == 1)
                    insertFoodFromFile();
                else
                    insertFoodDirectly();
                break;
            }
            case 3:
            {
                popFood();
                break;
            }
            case 4:
                addRecipeFromFile();
                break;
            case 5:
                showRecipe();
                break;
            case 6:
                recommendMealCourses();
                break;
        }
        return 1;
    }
}