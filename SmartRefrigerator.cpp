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

bool SmartRefrigerator::compareScore(const tuple<vector<RecipeInfo>, double, double> t1, const tuple<vector<RecipeInfo>, double, double> t2)
{
    double score1 = get<1>(t1) + get<2>(t1);
    double score2 = get<1>(t2) + get<2>(t2);

    if (score1 < score2)
        return true;
    else
        return false;
}

bool SmartRefrigerator::compareExpScore(const tuple<vector<RecipeInfo>, double, double> t1, const tuple<vector<RecipeInfo>, double, double> t2)
{
    double score1 = get<2>(t1);
    double score2 = get<2>(t2);

    if (score1 < score2)
        return true;
    else
        return false;
}

bool SmartRefrigerator::compareSatScore(const tuple<vector<RecipeInfo>, double, double> t1, const tuple<vector<RecipeInfo>, double, double> t2)
{
    double score1 = get<1>(t1);
    double score2 = get<1>(t2);

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
	if (recipes.size() < 3) {
		cout << "Too few recipes to recommend a meal course!" << endl;
		return;
	}		

    vector<Course> totalScore; //만들 수 있는 3가지 음식의 (음식이름, 음식이름, 음식이름, satisfy_score 총합, expiration_score 총합)
	vector<map<string, double>> ingredients;

	// Brute Force Search for all possible meal combinations (O(n^3))
	for (int i = 0; i < recipes.size(); i++) {
        for (int j = 0; j < recipes.size(); j++) {
            for (int k = 0; k < recipes.size(); k++) {
                if (i < j && j < k) {
					vector<RecipeInfo> recipeList;

					RecipeInfo recipe1(recipes[i].getName(), i);
					RecipeInfo recipe2(recipes[j].getName(), j);
					RecipeInfo recipe3(recipes[k].getName(), k);

					recipeList.push_back(recipe1);
					recipeList.push_back(recipe2);
					recipeList.push_back(recipe3);

					Course course(recipeList, 0, 0);
					totalScore.push_back(course);
				}
            }
        }
    }

	vector<int> deleteIdx;

    for (int i = 0; i < totalScore.size(); i++) {
		map<string, double> ingredientList;
		vector<RecipeInfo> recipeList = get<0>(totalScore[i]);

		// check whether the recipe is valid
		for (int j = 0; j < recipeList.size(); j++) {
			for (int k = 0; k < recipes[recipeList[j].second].getIngredients().size(); k++) {
				string ingredientName = recipes[recipeList[j].second].getIngredients()[k].first;
				int ingredientCount = recipes[recipeList[j].second].getIngredients()[k].second;

				if (ingredientList.find(ingredientName) == ingredientList.end()) ingredientList.insert(pair<string, int>(ingredientName, ingredientCount));
				else ingredientList[ingredientName] += ingredientCount;
			}
		}

		for (auto elem : ingredientList) {
			if (foodList.find(elem.first) == foodList.end()) { deleteIdx.push_back(i); break; }
			if (foodList[elem.first].size() < elem.second) { deleteIdx.push_back(i); break; }
		}

		ingredients.push_back(ingredientList);
    }

	for (int i = 0; i < deleteIdx.size(); i++) { // remove courses with insufficient ingredients
		totalScore.erase(totalScore.begin() + deleteIdx[i] - i);
		ingredients.erase(ingredients.begin() + deleteIdx[i] - i);
	}

	for (int i = 0; i < totalScore.size(); i++) {
		double satisfaction(0.0), expiration(0.0);
        vector<string> namesVisited;

		for (map<string,double>::iterator iter = ingredients[i].begin(); iter != ingredients[i].end(); ++iter) {
			string name = iter->first;
            namesVisited.push_back(name);

			vector<FoodPtr> v = foodList[name];
			sort(v.begin(), v.end(), controller->compareExp);
            
			for (int j = 0; j < iter->second; j++)
				v.pop_back();

            for (auto elem : v)
                expiration += elem->getExp();
 		}

        for (map<string, vector<FoodPtr>>::iterator iter = foodList.begin(); iter != foodList.end(); ++iter) {
            bool contains = false;
            for (auto name : namesVisited) {
                if (name == iter->first) {
                    contains = true;
                    break;
                }
            }
            if (contains) continue;
            else
                for (auto elem : iter->second)
                    expiration += elem->getExp();
        }
        
		for (auto elem : get<0>(totalScore[i])) {
			satisfaction += recipes[elem.second].getScore();
		}

		get<1>(totalScore[i]) = satisfaction;
		get<2>(totalScore[i]) = expiration;
	}

    sort(totalScore.begin(), totalScore.end(), compareExpScore);
    double e_norm = get<2>(totalScore.back());
    sort(totalScore.begin(), totalScore.end(), compareSatScore);
    double s_norm = get<1>(totalScore.back());

    for (int i = 0; i < totalScore.size(); i++) {
        get<1>(totalScore[i]) /= s_norm;
        get<2>(totalScore[i]) /= e_norm;
    }

    sort(totalScore.begin(), totalScore.end(), compareScore);

    int count(0);
    while (totalScore.size() != 0 && count < 3) {
        count++;
        cout << "1. " << get<0>(totalScore.back())[0].first << "  2. " << get<0>(totalScore.back())[1].first << "  3. " << get<0>(totalScore.back())[2].first << "  /  total score sum : ";
        cout << get<1>(totalScore.back()) + get<2>(totalScore.back()) << "  (" << get<1>(totalScore.back()) << " / " << get<2>(totalScore.back()) << ")" << endl;
        totalScore.pop_back();
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