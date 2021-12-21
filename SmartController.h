#ifndef __SMARTCONTROLLER_H_
#define __SMARTCONTROLLER_H_
#include "Controller.h"

class SmartController : public Controller
{
public:
    SmartController(intPair &size_, foodListType &foodList_) : Controller(size_, foodList_) {}

    bool stackFood(const string, intPair, int);
    bool popFood(const string);
    static bool compareScore(const tuple<string, string, string, int, int> t1, const tuple<string, string, string, int, int> t2);

private:
    int maxHeight(Shelf &);
};

#endif
