/**
 * rb-tree test
 * platform: win32, visual studio 2005/2010; Linux, gcc4.1.2
 */

#include "my_rb_tree.hpp"

/*
 * a global function
 * the new inserted node color is Red, and its parent color is Red too, 
 * then, must rotate it. and x is the parent, which is the rotate position
 */
void rb_tree_rotate_left(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root)
{
    RB_Tree_Node_Base* y = x->right_;
    x->right_ = y->left_;
    if (y->left_ != 0)
        y->left_->parent_ = x;
    y->parent_ = x->parent_;

    if (x == root)
        root = y;
    else if (x == x->parent_->left_)  //x is the left child node
        x->parent_->left_ = y;
    else  //x is the right child node
        x->parent_->right_ = y;
    y->left_ = x;
    x->parent_ = y;
}

void rb_tree_rotate_right(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root)
{
    RB_Tree_Node_Base* y = x->left_;
    x->left_ = y->right_;
    if (y->right_ != 0)
        y->right_->parent_ = x;
    y->parent_ = x->parent_;

    if (x == root)
        root = y;
    else if (x == x->parent_->right_)  //x is the right child node
        x->parent_->right_ = y;
    else  //x is the left child node
        x->parent_->left_ = y;
    y->right_ = x;
    x->parent_ = y;
}

/*
 * a global function
 * x is the new inserted node, root is the root of the rb-tree
 */
void rb_tree_rebalance(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root)
{
    x->color_ = RB_Tree_Node_Red;
    while (x != root && x->parent_->color_ == RB_Tree_Node_Red)  //the parent of x is Red
    {
        if (x->parent_ == x->parent_->parent_->left_)   //the parent of x is the left child of its grand parent
        {
            RB_Tree_Node_Base* y = x->parent_->parent_->right_; //y is the sibling node of the parent of x
            if (y && y->color_ == RB_Tree_Node_Red)  //y is not null and is Red
            {
                x->parent_->color_ = RB_Tree_Node_Black;
                y->color_ = RB_Tree_Node_Black;
                x->parent_->parent_->color_ = RB_Tree_Node_Red;
                x = x->parent_->parent_;
            }
            else  //y is null, or is Black
            {
                if (x == x->parent_->right_)  //x is the right child of its parent
                {
                    x = x->parent_;
                    rb_tree_rotate_left(x, root);
                }
                x->parent_->color_ = RB_Tree_Node_Black;
                x->parent_->parent_->color_ = RB_Tree_Node_Red;
                rb_tree_rotate_right(x->parent_->parent_, root);
            }
        }
        else  //the parent of x is the right child of its grand parent
        {
            RB_Tree_Node_Base* y = x->parent_->parent_->left_;  //y is the sibling node of the parent of x
            if (y && y->color_ == RB_Tree_Node_Red)  //y is not null and is Red
            {
                x->parent_->color_ = RB_Tree_Node_Black;
                y->color_ = RB_Tree_Node_Black;
                x->parent_->parent_->color_ = RB_Tree_Node_Red;
                x = x->parent_->parent_;
            }
            else  //y is null, or is Black
            {
                if (x == x->parent_->left_)  //x is the left child of its parent
                {
                    x = x->parent_;
                    rb_tree_rotate_right(x, root);
                }
                x->parent_->color_ = RB_Tree_Node_Black;
                x->parent_->parent_->color_ = RB_Tree_Node_Red;
                rb_tree_rotate_left(x->parent_->parent_, root);
            }
        }
    }

    root->color_ = RB_Tree_Node_Black;  //the root color is always black
}

/*
 * a global function
 * z is the node to be removed, root is the root of the rb-tree
 * lmost is the most-left node, and rmost is the most-right node.
 */
RB_Tree_Node_Base* rb_tree_rebalance_for_erase(RB_Tree_Node_Base* z, 
    RB_Tree_Node_Base*& root, RB_Tree_Node_Base*& lmost, RB_Tree_Node_Base*& rmost)
{
    RB_Tree_Node_Base *y = z;
    RB_Tree_Node_Base *x = 0;
    RB_Tree_Node_Base *xparent = 0;

    if (y->left_ == 0)  //z has at most one non-num child, y==z
        x = y->right_;  //x might be null
    else if (y->right_ == 0) //z has exactly one non-null child, y==z
        x = y->left_;
    else  //z has two non-null child, set y to z's successor, x might be null
    {
        y = y->right_;
        while (y->left_ != 0)
            y = y->left_;
        x = y->right_;
    }

    if (y != z)  //relink y in place of z.  y is z's successor
    {
        z->left_->parent_ = y;
        y->left_ = z->left_;
        if (y != z->right_)
        {
            xparent = y->parent_;
            if (x)
                x->parent_ = y->parent_;
            y->parent_->left_ = x;  //y must be a child of left_
            y->right_ = z->right_;
            z->right_->parent_ = y;
        }
        else
            xparent = y;

        if (root ==z)
            root = y;
        else if (z->parent_->left_ == z)  //z is the left child of its parent
            z->parent_->left_ = y;
        else
            z->parent_->right_ = y;

        y->parent_ = z->parent_;
        std::swap(y->color_, z->color_);
        y = z;  //y now points to node to be actually removed
    }
    else  //y == z
    {
        xparent = y->parent_;
        if (x)
            x->parent_ = y->parent_;
        if (root == z)
            root = x;
        else if (z->parent_->left_ == z)
            z->parent_->left_ = x;
        else
            z->parent_->right_ = x;

        if (lmost == z)
        {
            if (z->right_ == 0)  //z->left_ must be null also
                lmost = z->parent_;
            else
                lmost = RB_Tree_Node_Base::min_node(x);
        }

        if (rmost == z)
        {
            if (z->left_ == 0)  //z->right_ must be null also
                rmost = z->parent_;
            else  //x == z->left_
                rmost = RB_Tree_Node_Base::max_node(x);
        }
    }

    if (y->color_ == RB_Tree_Node_Red)
        return y;

    while (x != root && (x == 0 || x->color_ == RB_Tree_Node_Black))
    {
        if (x == xparent->left_)
        {
            RB_Tree_Node_Base *w = xparent->right_;
            if (w->color_ == RB_Tree_Node_Red)
            {
                w->color_ = RB_Tree_Node_Black;
                xparent->color_ = RB_Tree_Node_Red;
                rb_tree_rotate_left(xparent, root);
                w = xparent->right_;
            }

            if (     (w->left_ == 0 || w->left_->color_ == RB_Tree_Node_Black)
                && (w->right_ == 0 || w->right_->color_ == RB_Tree_Node_Black))
            {
                w->color_ = RB_Tree_Node_Red;
                x = xparent;
                xparent = xparent->parent_;
            }
            else
            {
                if (w->right_ ==0 || w->right_->color_ == RB_Tree_Node_Black)
                {
                    if (w->left_)
                        w->left_->color_ = RB_Tree_Node_Black;
                    w->color_ = RB_Tree_Node_Red;
                    rb_tree_rotate_right(w, root);
                    w = xparent->right_;
                }
                w->color_ = xparent->color_;
                xparent->color_ = RB_Tree_Node_Black;
                if (w->right_)
                    w->right_->color_ = RB_Tree_Node_Black;
                rb_tree_rotate_left(xparent, root);
                break;
            }
        }
        else
        {
            //same as above, with right_ <-> left_
            RB_Tree_Node_Base *w = xparent->left_;
            if (w->color_ == RB_Tree_Node_Red)
            {
                w->color_ = RB_Tree_Node_Black;
                xparent->color_ = RB_Tree_Node_Red;
                rb_tree_rotate_right(xparent, root);
                w = xparent->left_;
            }

            if (     (w->right_ == 0 || w->right_->color_ == RB_Tree_Node_Black) 
                && (w->left_ == 0 || w->left_->color_ == RB_Tree_Node_Black))
            {
                w->color_ = RB_Tree_Node_Red;
                x = xparent;
                xparent = xparent->parent_;
            }
            else
            {
                if (w->left_ == 0 || w->left_->color_ == RB_Tree_Node_Black)
                {
                    if (w->right_)
                        w->right_->color_ = RB_Tree_Node_Black;
                    w->color_ = RB_Tree_Node_Red;
                    rb_tree_rotate_left(w, root);
                    w = xparent->left_;
                }
                w->color_ = xparent->color_;
                xparent->color_ = RB_Tree_Node_Black;
                if (w->left_)
                    w->left_->color_ = RB_Tree_Node_Black;
                rb_tree_rotate_right(xparent, root);
                break;
            }
        }
    }

    if (x)
        x->color_ = RB_Tree_Node_Black;
    return y;
}

/*
 * a global function
 * node is the node whose black node will be counted, root is the root of the rb-tree
 */
int black_count(RB_Tree_Node_Base* node, RB_Tree_Node_Base* root)
{
    if (node == 0)
        return 0;

    int bc = node->color_ == RB_Tree_Node_Black? 1: 0;
    if (node == root)
        return bc;
    return bc + black_count(node->parent_, root);
}


