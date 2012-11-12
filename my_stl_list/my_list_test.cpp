/**
 * List test
 * platform: win32, visual studio 2005/2010; Linux, gcc4.1.2
 */

#include "my_list.hpp"
#include <iostream>

using namespace std;

struct point
{
    int x;
    int y;

public:
    point(int a = 0, int b = 0):x(a), y(b){}
    ~point(){cout << "(x=" << x << ", y=" << y << ") destructed" << endl;}
    void dump(){cout << "(x=" << x << ", y=" << y << ")" << endl;}
};

void my_list_test(point pt[], int count)
{
    My_List<point> list;
    for (int loop = 0; loop < count; loop++)
        list.push_back(pt[loop]);

    My_List<point>::iterator iter = list.begin();
    for (; iter != list.end(); iter++)
        iter->dump();
    cout << endl;

    list.erase(list.begin());
    cout << endl;
    list.erase(list.begin());
    cout << endl;
}

int main(/*int argc,char* argv[]*/)
{
    point pt[] = { point(10, 10), point(20, 20), point(30, 30), point(40, 40) };
    my_list_test(pt, 4);
    cout << endl;
    return 0;    
}

