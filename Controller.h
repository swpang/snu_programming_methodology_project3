#ifndef __CONTROLLER_H_
#define __CONTROLLER_H_
#include "Food.h"
#include <iostream>
#include <vector>
#include <map>

typedef FoodInFridge *FoodPtr;
typedef map<string, vector<FoodPtr>> foodListType;

struct Shelf
{
    int height;
    vector<FoodPtr> vec;
    Shelf(int height_) : height(height_){};
};

class Controller
{
public:
    Controller(intPair, foodListType &);
    void display();

    virtual bool popFood(const string) = 0;
    virtual bool stackFood(const string, intPair, int) = 0;

protected:
    intPair size;
    vector<Shelf> shelves;
    foodListType &foodList;

    vector<FoodPtr>::iterator findMinExpFood(const string);
    static bool comparePos(const FoodPtr food1, const FoodPtr food2);
    
private:
    bool **storageGrid;
    void drawFoodsInStorageGrid(int x, int y, int w, int h);
};

#endif
