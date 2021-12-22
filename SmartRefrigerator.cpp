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

bool SmartRefrigerator::compareScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2)
{
    int score1 = get<3>(t1) + get<4>(t1);
    int score2 = get<3>(t2) + get<4>(t2);

    if (score1 < score2)
        return true;
    else
        return false;
}

bool SmartRefrigerator::compareExpScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2)
{
    int score1 = get<4>(t1);
    int score2 = get<4>(t2);

    if (score1 < score2)
        return true;
    else
        return false;
}

bool SmartRefrigerator::compareSatScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2)
{
    int score1 = get<3>(t1);
    int score2 = get<3>(t2);

    if (score1 < score2)
        return true;
    else
        return false;
}

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
    }
    else
    {
        int line(0);
        while (recipe_list.peek() != EOF)
        {
            Recipe *recipe;

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
            while (iss >> temp)
            {
                if (idx == 0)
                    mealName = temp;
                else if (temp != "/")
                {
                    foodName = temp;
                    iss >> temp;
                    foodCount = stoi(temp);
                    ingredient.first = foodName;
                    ingredient.second = foodCount;
                    ingredient_vec.push_back(ingredient);
                }
                else
                {
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
void SmartRefrigerator::recommendMealCourses()
{
    vector<tuple<string, string, string, int, int>> total_score; //만들 수 있는 3가지 음식의 (음식이름, 음식이름, 음식이름, satisfy_score 총합, expiration_score 총합)
    for (auto iter1 = recipes.begin(); iter1 <= recipes.end() - 3; iter1++)
    {
        for (auto iter2 = (iter1 + 1); iter2 <= recipes.end() - 2; iter2++)
        {
            for (auto iter3 = (iter2 + 1); iter3 <= recipes.end() - 1; iter3++)
            {
                bool possible_course;           //만들 수 있는 3가지 음식 조합인가?
                vector<string> all_ingredients; // 3가지 음식을 만드는데 필요한 식재료의 이름을 종류별로 중복되지 않게 all_ingredients 벡터에 저장
                for (int i = 0; i < iter1->getIngredients().size(); i++)
                {
                    all_ingredients.push_back(iter1->getIngredients()[i].first);
                }
                for (int i = 0; i < iter2->getIngredients().size(); i++)
                {
                    all_ingredients.push_back(iter2->getIngredients()[i].first);
                }
                for (int i = 0; i < iter3->getIngredients().size(); i++)
                {
                    all_ingredients.push_back(iter3->getIngredients()[i].first);
                }
                all_ingredients.erase(unique(all_ingredients.begin(), all_ingredients.end()), all_ingredients.end());
                vector<pair<string, int>> course_require; // course_require에 all_ingredients에 저장되어 있는 (식재료의 종류, 갯수)를 pair형태로 push_back
                for (int i = 0; i < all_ingredients.size(); i++)
                {
                    course_require.push_back(make_pair(all_ingredients[i], 0));
                }
                for (int i = 0; i < course_require.size(); i++)
                {
                    for (int j = 0; j < iter1->getIngredients().size(); j++)
                    {
                        if (iter1->getIngredients()[j].first == course_require[i].first)
                        {
                            course_require[i].second =  course_require[i].second + iter1->getIngredients()[j].second;
                        }
                    }
                    for (int j = 0; j < iter2->getIngredients().size(); j++)
                    {
                        if (iter2->getIngredients()[j].first == course_require[i].first)
                        {
                            course_require[i].second =  course_require[i].second + iter2->getIngredients()[j].second;
                        }
                    }
                    for (int j = 0; j < iter3->getIngredients().size(); j++)
                    {
                        if (iter3->getIngredients()[j].first == course_require[i].first)
                        {
                            course_require[i].second =  course_require[i].second + iter3->getIngredients()[j].second;
                        }
                    }
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
                    temp1 = temp1 + iter1->getScore() + iter2 ->getScore() + iter3 ->getScore();
                    int temp2 = 0; // expiration score
                    for (int i = 0; i < course_require.size(); i++)
                    {
                        string temp = course_require[i].first;
                        temp2 = temp2 + (foodList[temp].size() - course_require[i].second) * foodList[temp][0]->getExp();
                    }
                    total_score.push_back(make_tuple(iter1->getName(), iter2->getName(), iter3->getName(), temp1, temp2));
                }
            }
        }
    }
    //만들 수 있는 3가지 조합들 중 가장 높은 expiration score + satisfy score를 가지는 조합의 total_score 내에서의 index를 highest_index
    // 2번째 높은 조합의 index를 second_highest_index, 3번째 높은 조합의 index를 third_highest_index

    sort(total_score.begin(), total_score.end(), compareExpScore);
    double e_norm = get<4>(total_score.back());
    sort(total_score.begin(), total_score.end(), compareSatScore);
    double s_norm = get<3>(total_score.back());

    sort(total_score.begin(), total_score.end(), compareScore);

    int count(0);
    while (total_score.size() != 0 && count < 3)
    {
        count++;
        cout << "1. " << get<0>(total_score.back()) << "  2. " << get<1>(total_score.back()) << "  3. " << get<2>(total_score.back()) << "  /  total score sum : ";
        cout << get<3>(total_score.back()) / s_norm + get<4>(total_score.back()) / e_norm << "  (" << get<3>(total_score.back()) / s_norm << " / " << get<4>(total_score.back()) / e_norm << ")" << endl;
        total_score.pop_back();
    }
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
        if (i < 1 || i > 7)
        {
            cout << "Type an integer from 1 to 7!" << endl;
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