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
    point(int _x = 0, int _y = 0):x(_x), y(_y){}
    ~point(){cout << "<x=" << x << ", y=" << y << "> destructed" << endl;}
    void dump(){cout << "<x=" << x << ", y=" << y << "> - " ;}
};

struct point_data
{
    int a;
    int b;
    int c;

public:
    point_data(int _a = 0, int _b = 0, int _c = 0):a(_a), b(_b), c(_c){}
    ~point_data(){cout << "(a=" << a << ", b=" << b << ", c=" << c << ") - ";}
    void dump(){cout << "(a=" << a << ", b=" << b << ", c=" << c << ")" << endl;}
};

void my_list_test(point pt[], point_data data[], int count)
{
    My_List<point, point_data> list;
    for (int loop = 0; loop < count; loop++)
        list.push_back(pt[loop], data[loop]);

    My_List<point, point_data>::iterator iter = list.begin();
    for (; iter != list.end(); iter++)
        iter->dump();
    cout << endl;
}

void test()
{
    point pt[] = {point(10, 10), point(20, 20), point(30, 30), point(40, 40) };
    point_data data[] ={point_data(1, 2, 3), point_data(4, 5, 6), point_data(7, 8, 9), point_data(10, 11, 12)};
    my_list_test(pt, data, 4);
    cout << endl;
}

int main(/*int argc,char* argv[]*/)
{
    test();
    return 0;
}

