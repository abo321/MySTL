/**
 * rb-tree test
 * platform: Linux, gcc4.1.2
 */

#ifndef _MY_RBTREE_H_
#define _MY_RBTREE_H_

#include <stddef.h>
#include <iterator>
#include <utility>

/*
#include <bits/stl_iterator_base_types.h>
#include <bits/stl_iterator.h>
#include <bits/stl_pair.h>
*/

//the color of node for rb-tree
typedef enum
{
    RB_Tree_Node_Red = 0,
    RB_Tree_Node_Black
}RB_Tree_Node_Color;

struct RB_Tree_Node_Base
{
    typedef RB_Tree_Node_Color  color_type;
    typedef RB_Tree_Node_Base* base_pointer;

    color_type color_;
    base_pointer parent_;
    base_pointer left_;
    base_pointer right_;

    //the left most node (node of the minumum value) for node as the 'root'
    static base_pointer min_node(base_pointer node)
    {
        while (node->left_ !=0)
            node = node->left_;
        return node;
    }

    //the most right node (node of the maximum value) for node as the 'root'
    static base_pointer max_node(base_pointer node)
    {
        while (node->right_ != 0)
            node = node->right_;
        return node;
    }
};

template <class Value_T>
struct RB_Tree_Node: public RB_Tree_Node_Base
{
    Value_T value_;
};

struct RB_Tree_Iterator_Base
{
    typedef RB_Tree_Node_Base::base_pointer base_pointer;

    base_pointer node_;

    RB_Tree_Iterator_Base(base_pointer node):
        node_(node)
    {}

    void increment()
    {
        if (node_->right_ != 0)   //the leftmost node of right subtree
        {
            node_ = node_->right_;
            while(node_->left_ != 0)
                node_ = node_->left_;            
        }
        else  //no right subtree, the next iterator is the topmost parent
        {
            base_pointer parent = node_->parent_;
            while (node_ == parent->right_)
            {
                node_ = parent;
                parent = parent->parent_;
            }
            if (node_->right_ != parent)
                node_ = parent;
        }
    }


    void decrement()
    {
        if (node_->color_ == RB_Tree_Node_Red && node_->parent_->parent_ == node_)
            node_ = node_->right_;
        else if (node_->left_ != 0) //the rightmost node of left subtree
        {
/*
            base_node left = node_->left_;
            while (left->right_ != 0)
                left = left->right_;
            node_ = left;
*/
            node_ = node_->left_;
            while(node_->right_ != 0)
                node_ = node_->right_;            
        }
        else  //no left subtree, the prev iterator is the topmost parent
        {
            base_pointer parent = node_->parent_;
            while (node_ == parent->left_)
            {
                node_ = parent;
                parent = parent->parent_;
            }
            node_ = parent;
        }
    }

    bool operator==(const RB_Tree_Iterator_Base& rhs)
    {
        return node_ == rhs.node_;
    }
    bool operator!=(const RB_Tree_Iterator_Base& rhs)
    {
        return node_ != rhs.node_;
    }
};

template <class Value_T, class Reference_T, class Pointer_T>
struct RB_Tree_Iterator: public RB_Tree_Iterator_Base
{
    typedef Value_T        value_type;
    typedef Reference_T reference_type;
    typedef Pointer_T      pointer_type;

    typedef RB_Tree_Iterator<Value_T, Value_T&, Value_T*> iterator;
    typedef RB_Tree_Iterator<Value_T, const Value_T&, const Value_T*> const_iterator;
    typedef RB_Tree_Iterator<Value_T, Reference_T, Pointer_T> self_type;

    typedef RB_Tree_Node<Value_T>* node_pointer;

    RB_Tree_Iterator(){}
    RB_Tree_Iterator(node_pointer node):
        RB_Tree_Iterator_Base(node){}
    RB_Tree_Iterator(const iterator& iter):
        RB_Tree_Iterator_Base(iter.node_){}

    reference_type operator*() const {return node_pointer(node_)->value_;}
    pointer_type operator->() const {return &(operator*());}

    self_type& operator++()
    {
        increment();
        return *this;
    }

    self_type operator++(int)
    {
        self_type temp = *this;
        increment();
        return temp;
    }

    self_type& operator--()
    {
        decrement();
        return *this;
    }

    self_type operator--(int)
    {
        self_type temp = *this;
        decrement();
        return temp;
    }
};

template <class Value_T>
struct RB_Tree_Base
{
protected:
    typedef RB_Tree_Node<Value_T>  node_type;
    typedef RB_Tree_Node<Value_T>* node_pointer;
    node_pointer header_;

public:
    RB_Tree_Base():
        header_(0)
    {
        header_ = allocate_node();
    }
    ~RB_Tree_Base()
    {
        deallocate_node(header_);
    }

protected:
    node_pointer allocate_node(){return new node_type;}
    void deallocate_node(node_pointer node){delete node; node = 0;}
};

/*
 * the class of RB-tree
 * KeyOfValue_T is a function to extract key from value, such as std::_Identity
 * Compare_T is a comparision function, such as std::less
 */
template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
class RB_Tree: protected RB_Tree_Base<Value_T>
{
    typedef RB_Tree_Base<Value_T> base_tree;

protected:
    typedef RB_Tree_Node_Base*       base_node_ptr;
    typedef RB_Tree_Node<Value_T> node_type;
    typedef RB_Tree_Node_Color         color_type;

public:
    typedef Key_T key_type;
    typedef Value_T value_type;
    typedef value_type* pointer_type;
    typedef value_type& reference_type;
    typedef const value_type* const_pointer;
    typedef const value_type& const_reference;

    typedef node_type* node_pointer;
    typedef size_t size_type;

    typedef RB_Tree_Iterator<value_type, reference_type, pointer_type> iterator;
    typedef RB_Tree_Iterator<value_type, const_reference, const_pointer> const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

protected:
    using base_tree::allocate_node;
    using base_tree::deallocate_node;
    using base_tree::header_;

    size_type node_count_; // keeps track of size of tree
    Compare_T key_compare_;


protected:
    node_pointer create_node(const value_type& v)
    {
        node_pointer tmp = allocate_node();  //can pass val to allocate_node as the parameter
        tmp->value_ = v;
        return tmp;
    }

    node_pointer clone_node(node_pointer node)
    {
        node_pointer tmp = create_node(node->value_);
        tmp->color_ = node->color_;
        tmp->left_ = node->left_;
        tmp->right_ = node->right_;
        return tmp;
    }

    void destroy_node(node_pointer node)
    {
        deallocate_node(node);
    }

    node_pointer& root() const {return (node_pointer&)header_->parent_;}
    node_pointer& leftmost() const {return (node_pointer&)header_->left_;}
    node_pointer& rightmost() const {return (node_pointer&)header_->right_;}

    static node_pointer& parent(node_pointer node) {return (node_pointer&)(node->parent_);}
    static node_pointer& left(node_pointer node) {return (node_pointer&)(node->left_);}
    static node_pointer& right(node_pointer node) {return (node_pointer&)(node->right_);}

    //Key of Value: std::_Identity
    static reference_type value(node_pointer node) {return node->value_;}
    static const key_type& key(node_pointer node) {return KeyOfValue_T()(value(node));}
    static color_type& color(node_pointer node) {return (color_type&)node->color_;}

    static node_pointer& parent(base_node_ptr node) {return (node_pointer&)node->parent_;}
    static node_pointer& left(base_node_ptr node) {return (node_pointer&)node->left_;}
    static node_pointer& right(base_node_ptr node) {return (node_pointer&)node->right_;}

    //Key of Value: std::_Identity
    static reference_type value(base_node_ptr node) {return ((node_pointer)node)->value_;}
    static const key_type& key(base_node_ptr node) {return KeyOfValue_T()(value(node_pointer(node)));}
    static color_type& color(base_node_ptr node) {return (color_type&)(node_pointer(node)->color_);}

    static node_pointer min_node(node_pointer node)
    { return (node_pointer)RB_Tree_Node_Base::min_node(node);}

    static node_pointer max_node(node_pointer node)
    { return (node_pointer)RB_Tree_Node_Base::max_node(node);}

private:
    iterator insert(base_node_ptr _x, base_node_ptr _y, const value_type& v);
    node_pointer copy(node_pointer x, node_pointer p);
    void erase_(node_pointer x);

    void init()
    {
        color(header_) = RB_Tree_Node_Red;  //used to distinguish header from root, in iterator, operator++
        root() = 0;
        leftmost() = header_;
        rightmost() = header_;
    }


public:
    //header_ will be allocated and initialized in the base tree (RB_Tree_Node_Base)
    RB_Tree(): node_count_(0), key_compare_()
    {
        init();
    }

    RB_Tree(const Compare_T& comp): node_count_(0), key_compare_(comp)
    {
        init();
    }

    RB_Tree(const RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& x):
        node_count_(0), key_compare_(x.key_compare_)
    {
        if (x.root() == 0)
            init();
        else
        {
            color(header_) = RB_Tree_Node_Red;
            root() = copy(x.root(), header_);
            leftmost() = min_node(root());
            rightmost() = max_node(root());
        }
        node_count_ = x.node_count_;
    }

    ~RB_Tree() {clear();}

    //copy assignment
    RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& 
    operator=(const RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& x);

public:
    Compare_T key_compare() const {return key_compare_;}

    iterator begin() {return leftmost();}
    const_iterator begin() const {return leftmost();}
    iterator end() {return header_;}
    const_iterator end() const {return header_;}

    reverse_iterator rbegin() {return reverse_iterator(end());}
    const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
    reverse_iterator rend() {return reverse_iterator(begin());}
    const_reverse_iterator rend() const {return const_reverse_iterator(begin());}

    bool empty() const {return node_count_ == 0;}
    size_type size() const {return node_count_;}
    size_type max_size() const {return size_type(-1);}

    void swap(RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& tree)
    {
        swap(header_, tree.header_);
        swap(node_count_, tree.node_count_);
        swap(key_compare_, tree.key_compare_);
    }

public:    
    std::pair<iterator, bool> insert_unique(const value_type& v);
    iterator insert_equal(const value_type& v);

    iterator insert_unique(iterator position, const value_type& v);
    iterator insert_equal(iterator position, const value_type& v);

    void erase(iterator position);
    size_type erase(const key_type& x);
    void erase(iterator first, iterator last);
    void erase(const key_type* first, const key_type* last);

    void clear()
    {
        if (node_count_ != 0)
        {
            erase_(root());
            leftmost() = header_;
            root() = 0;
            rightmost() = header_;
            node_count_ = 0;
        }
    }

public:
    iterator find(const key_type& k);
    const_iterator find(const key_type& k) const;
    size_type count(const key_type& k) const;

    iterator lower_bound(const key_type& k);
    const_iterator lower_bound(const key_type& k) const;
    iterator upper_bound(const key_type& k);
    const_iterator upper_bound(const key_type& k) const;

    std::pair<iterator, iterator> equal_range(const key_type& k);
    std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;

public:
    //debugging function
    bool rb_verify() const;
};

extern void rb_tree_rebalance(RB_Tree_Node_Base* x, RB_Tree_Node_Base*& root);
extern RB_Tree_Node_Base* rb_tree_rebalance_for_erase(RB_Tree_Node_Base* z, 
    RB_Tree_Node_Base*& root, RB_Tree_Node_Base*& lmost, RB_Tree_Node_Base*& rmost);

/*
 * insert element (with <key, value>) into the RB-tree
 * _x is the insertion position of the new value, _y is the parent of _x, v is the new value
 * KeyOfValue_T can be std::_Identity, and Compare_T can be std::less or std::less_equal
 */
template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::insert(base_node_ptr _x, base_node_ptr _y, 
    const value_type& v)
{
    node_pointer x = (node_pointer)_x;
    node_pointer y = (node_pointer)_y;
    node_pointer z = create_node(v);  //create a new node for v

    if (y == header_ || x != 0 || key_compare_(KeyOfValue_T()(v), key(y)))
    {
        left(y) = z;
        if (y == header_)   //when y == header_, also makes leftmost() = z
        {
            root() = z;   //this is a design skill, that is, makes parent of header is z (its left child)
            rightmost() = z;
        }
        else if (y == leftmost())  //maintain leftmost() to point the min node
        {
            leftmost() = z;
        }
    }
    else
    {
        right(y) = z;
        if (y == rightmost())  //maintain rightmost() to point to the max node
        {
            rightmost() = z;
        }
    }

    parent(z) = y;  //settings of the new node
    left(z) = 0;
    right(z) = 0;
    rb_tree_rebalance(z, header_->parent_);  //rebalance the tree
    ++node_count_;
    return iterator(z);
}

/*
 * insert element (with <key, value>) into the RB-tree
 * _x is the insertion position of the new value, _y is the parent of _x, v is the new value
 * KeyOfValue_T can be std::_Identity, and Compare_T can be std::less or std::less_equal
 */
template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::node_pointer
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::copy(node_pointer x, node_pointer p)
{
    node_pointer node = clone_node(x);
    node->parent_ = p;

    if (x->right_)
        node->right_ = copy(right(x), node);
    p = node;
    x = left(x);

    while (x != 0)
    {
        node_pointer y = clone_node(x);
        p->left_ = y;
        y->parent_ = p;
        if (x->right_)
            y->right_ = copy(right(x), y);
        p = y;
        x = left(x);
    }

    return node;
}


template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
void RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::erase_(node_pointer x)
{
    while (x != 0)
    {
        erase_(right(x));
        node_pointer y = left(x);
        destroy_node(x);
        x = y;
    }
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::operator=(
    const RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>& x)
{
    if (this == &x)
        return *this;

    clear();
    node_count_ = 0;
    key_compare_ = x.key_compare_;

    if (x.root() == 0)
    {
        root() = 0;
        leftmost() = header_;
        rightmost() = header_;
    }
    else
    {
        root() = copy(x.root(), header_);
        leftmost() = min_node(root());
        rightmost() = max_node(root());
        node_count_ = x.node_count_;
    }
    return *this;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
std::pair<typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator, bool> 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::insert_unique(const value_type& v)
{
    node_pointer y = header_;
    node_pointer x = root();
    bool comp = true;

    while (x != 0)
    {
        y = x;
        comp = key_compare_(KeyOfValue_T()(v), key(x));
        x = comp? left(x): right(x);
    }

    iterator iter = iterator(y);
    if (comp)
    {
        if (iter == begin())
            return std::pair<iterator, bool>(insert(x, y, v), true);
        else
            --iter;
    }

    if (key_compare_(key(iter.node_), KeyOfValue_T()(v)))
        return std::pair<iterator, bool>(insert(x, y, v), true);
    return std::pair<iterator, bool>(iter, false);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::insert_equal(const value_type& v)
{
    node_pointer y = header_;
    node_pointer x = root();
    while (x != 0)
    {
        y = x;
        x = key_compare_(KeyOfValue_T()(v), key(x))? left(x): right(x);
    }
    return insert(x, y, v);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::insert_unique(iterator position, const value_type& v)
{
    if (position.node_ == header_->left_)  //begin()
    {
        if (size() >0 && key_compare_(KeyOfValue_T()(v), key(position.node_)))
            return insert(position.node_, position.node_, v);
        else
            return insert_unique(v).first;
    }
    else if (position.node_ == header_)  //end()
    {
        if (key_compare_(key(rightmost()), KeyOfValue_T()(v)))
            return insert(0, rightmost(), v);
        else
            return insert_unique(v).first;
    }
    else
    {
        iterator before = position;
        --before;
        if (     key_compare_(key(before.node_), KeyOfValue_T()(v)) 
            && key_compare_(KeyOfValue_T()(v), key(position.node_)))
        {
            if (right(before.node_) == 0)
                return insert(0, before.node_, v);
            else
                return insert(position.node_, position.node_, v);
        }
        else
        {
            return insert_unique(v).first;
        }
    }
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::insert_equal(iterator position, const value_type& v)
{
    if (position.node_ == header_->left_)  //begin()
    {
        if (size() >0 && !key_compare_(key(position.node_), KeyOfValue_T()(v)))
            return insert(position.node_, position.node_, v);
        else
            return insert_equal(v);
    }
    else if (position.node_ == header_)  //end()
    {
        if (!key_compare_(KeyOfValue_T()(v), key(rightmost())))
            return insert(0, rightmost(), v);
        else
            return insert_equal(v);
    }
    else
    {
        iterator before = position;
        --before;
        if (     !key_compare_(KeyOfValue_T()(v), key(before.node_))
            && !key_compare_(key(position.node_), KeyOfValue_T()(v)))
        {
            if (right(before.node_) == 0)
                return insert(0, before.node_, v);
            else
                return insert(position.node_, position.node_, v);
        }
        else
            return insert_equal(v);
    }
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
inline void RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::erase(iterator position)
{
    node_pointer y = (node_pointer)rb_tree_rebalance_for_erase(position->node_, 
        header_->parent_, header_->left_, header_->right_);
    destroy_node(y);
    --node_count_;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::size_type 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::erase(const Key_T& x)
{
    std::pair<iterator, iterator> pa = equal_range(x);
    size_type n = 0;
    distance(pa.first, pa.second, n);
    erase(pa.first, pa.second);
    return n;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
void RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::erase(iterator first, iterator last)
{
    if (first == begin() && last == end())
        clear();
    else
        while (first != last) erase(first++);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
void RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::erase(const Key_T* first, const Key_T* last)
{
    while (first != last) erase(*first++);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::find(const key_type& k)
{
    node_pointer y = header_;  //last node which is not less than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (!key_compare_(key(x), k))
            y =x, x = left(x);
        else
            x = right(x);
    }

    iterator iter = iterator(y);
    return (iter == end() || key_compare_(k, key(iter.node_)))? end(): iter;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::const_iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::find(const key_type& k) const
{
    node_pointer y = header_;  //last node which is not less than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (!key_compare_(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    }

    const_iterator iter = const_iterator(y);
    return (iter == end() || key_compare_(k, key(iter.node_)))? end(): iter;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::size_type 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::count(const key_type& k) const
{
    std::pair<const_iterator, const_iterator> pa = equal_range(k);
    size_type n = 0;
    distance(pa.first, pa.second);
    return n;
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::lower_bound(const key_type& k)
{
    node_pointer y = header_;  //last node which is not less than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (!key_compare_(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    }

    return iterator(y);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::const_iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::lower_bound(const key_type& k) const
{
    node_pointer y = header_;  //last node which is not less than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (!key_compare_(key(x), k))
            y = x, x = left(x);
        else
            x = right(x);
    }

    return const_iterator(y);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::upper_bound(const key_type& k)
{
    node_pointer y = header_;  //last node which is not greater than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (key_compare_(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    }
    return iterator(y);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::const_iterator 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::upper_bound(const key_type& k) const
{
    node_pointer y = header_;  //last node which is not greater than k
    node_pointer x = root();  //current node
    while (x != 0)
    {
        if (key_compare_(k, key(x)))
            y = x, x = left(x);
        else
            x = right(x);
    }
    return const_iterator(y);
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
inline
std::pair<typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator, 
    typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::iterator> 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::equal_range(const key_type& k)
{
    return std::pair<iterator, iterator>(lower_bound(k), upper_bound(k));
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T>
inline
std::pair<typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::const_iterator, 
    typename RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::const_iterator> 
RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::equal_range(const key_type& k) const
{
    return std::pair<const_iterator, const_iterator>(lower_bound(k), upper_bound(k));
}

template <class Key_T, class Value_T, class KeyOfValue_T, class Compare_T> 
bool RB_Tree<Key_T, Value_T, KeyOfValue_T, Compare_T>::rb_verify() const
{
    if (node_count_ == 0 || begin() == end())
        return node_count_ == 0 && begin() == end() && header_->left_ == header_ && header_->right_ == header_;

    int len = black_count(leftmost(), root());
    for (const_iterator iter = begin(); iter != end(); ++iter)
    {
        node_pointer x = (node_pointer)iter.node_;
        node_pointer lc = left(x);  //left child
        node_pointer rc = right(x);  //right child

        if (x->color_ == RB_Tree_Node_Red)
        {
            if ((lc && lc->color_ == RB_Tree_Node_Red) || (rc && rc->color_ == RB_Tree_Node_Red))
                return false;
        }

        if (lc && key_compare_(key(x), key(lc)))
            return false;
        if (rc && key_compare_(key(rc), key(x)))
            return false;

        if (!lc && !rc && black_count(x, root()) != len)
            return false;
    }

    if (leftmost() != RB_Tree_Node_Base::min_node(root()))
        return false;
    if (rightmost() != RB_Tree_Node_Base::max_node(root()))
        return false;

    return true;
}


#endif


