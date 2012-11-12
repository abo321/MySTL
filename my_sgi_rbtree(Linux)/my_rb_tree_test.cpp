/**
 * RB-tree test
 * platform: win32, visual studio 2005/2010; Linux, gcc4.1.2
 */

#include "my_rb_tree.hpp"
#include <iostream>

using namespace std;

int main(/* int argc,char* argv[] */)
{
    typedef RB_Tree<int, int, std::_Identity<int>, std::less<int> > My_Tree_T;

    My_Tree_T mytree;
    cout << "size = " << mytree.size() << endl;

    mytree.insert_unique(10);
    mytree.insert_unique(8);
    mytree.insert_unique(7);

    mytree.insert_unique(15);
    mytree.insert_unique(5);
    mytree.insert_unique(6);

    mytree.insert_unique(11);
    mytree.insert_unique(13);
    mytree.insert_unique(12);

    cout << "size = " << mytree.size() << endl;

    My_Tree_T ::iterator iter1 = mytree.begin();
    My_Tree_T ::iterator iter2 = mytree.end();
    cout << "the tree:" << endl << "    ";
    for (; iter1 != iter2; ++iter1)
        cout << *iter1 << ", ";
    cout << endl;

    cout << "the tree with color:" << endl << "    ";
    iter1 = mytree.begin();
    for (; iter1 != iter2; ++iter1)
    {
        RB_Tree_Iterator<int, int&, int*> baseiter = RB_Tree_Iterator<int, int&, int*>(iter1);
        cout << *iter1 << "(" << baseiter.node_->color_ << "), ";
    }
    cout << endl;

    return 0;    
}
     

