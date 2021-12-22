#ifndef __SMARTREFRIGERATOR_H_
#define __SMARTREFRIGERATOR_H_
#include <map>

#include "Food.h"
#include "Recipe.h"
#include "Refrigerator.h"
#include "SmartController.h"

typedef vector<Recipe> recipeVec;

typedef pair<string, double> RecipeInfo;
typedef tuple<vector<RecipeInfo>, double, double> Course;

class SmartRefrigerator : public Refrigerator {
public:
    SmartRefrigerator();
    void addRecipeFromFile();
    void showRecipe();
    void recommendMealCourses();
    int menuSelect();
    static bool compareScore(const tuple<vector<RecipeInfo>, double, double> t1, tuple<vector<RecipeInfo>, double, double> t2);
    static bool compareExpScore(const tuple<vector<RecipeInfo>, double, double> t1, tuple<vector<RecipeInfo>, double, double> t2);
    static bool compareSatScore(const tuple<vector<RecipeInfo>, double, double> t1, tuple<vector<RecipeInfo>, double, double> t2);

private:
    recipeVec recipes;
};

#endif
