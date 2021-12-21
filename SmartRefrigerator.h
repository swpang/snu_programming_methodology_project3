#ifndef __SMARTREFRIGERATOR_H_
#define __SMARTREFRIGERATOR_H_
#include <map>

#include "Food.h"
#include "Recipe.h"
#include "Refrigerator.h"
#include "SmartController.h"

typedef vector<Recipe> recipeVec;

class SmartRefrigerator : public Refrigerator {
public:
    SmartRefrigerator();
    void addRecipeFromFile();
    void showRecipe();
    void recommendMealCourses();
    int menuSelect();
    static bool compareScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2);
    static bool compareExpScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2);
    static bool compareSatScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2);

private:
    recipeVec recipes;
};

#endif
