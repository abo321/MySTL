/**
 * list test
 * platform: win32, visual studio 2005/2010; Linux, gcc4.1.2
 */

#ifndef _MY_LIST_H_
#define _MY_LIST_H_

#include <stddef.h>

struct My_List_Node_Base
{
    My_List_Node_Base *prev_;
    My_List_Node_Base *next_;

public:
    My_List_Node_Base():
        prev_(0), next_(0){}
    ~My_List_Node_Base()
    {prev_ = 0, next_ = 0;}
};

template<class Index_T, class Data_T>
struct My_List_Node: public My_List_Node_Base
{
    Index_T index_;
    Data_T data_;

public:
    My_List_Node(){}
    My_List_Node(const Index_T& index, const Data_T& data):
        index_(index), data_(data) {}
    ~My_List_Node(){}
    void dump()
    {
        index_.dump();
        data_.dump();
    }
};

struct My_List_Iterator_Base
{
    My_List_Node_Base *node_;

    My_List_Iterator_Base(My_List_Node_Base* node = 0):
        node_(node){}

    void incr(){node_ = node_->next_;}
    void decr(){node_ = node_->prev_;}

    bool operator==(const My_List_Iterator_Base& rhs)
    {
        return node_ == rhs.node_;
    }
    bool operator!=(const My_List_Iterator_Base& rhs)
    {
        return node_ != rhs.node_;
    }
};

template<class Index_T, class Index_Ref_T, class Index_Ptr_T, class Data_T, class Data_Ref_T, class Data_Ptr_T>
struct My_List_Iterator: public My_List_Iterator_Base
{
    typedef My_List_Iterator<Index_T, Index_T&, Index_T*, Data_T, Data_T&, Data_T*> iterator;
    typedef My_List_Iterator<Index_T, const Index_T&, const Index_T*, Data_T, const Data_T&, const Data_T*> const_iterator;
    typedef My_List_Iterator<Index_T, Index_Ref_T, Index_Ptr_T, Data_T, Data_Ref_T, Data_Ptr_T> self_type;

    typedef Index_T        index_type;
    typedef Index_Ref_T index_reference;
    typedef Index_Ptr_T  index_pointer;
    typedef Data_T          data_type;
    typedef Data_Ref_T   data_reference;
    typedef Data_Ptr_T    data_pointer;

    typedef My_List_Node<Index_T, Data_T> node_type;
    typedef My_List_Node<const Index_T, const Data_T> const_node;

    My_List_Iterator(node_type *node = 0):
        My_List_Iterator_Base(node){}
    My_List_Iterator(const iterator& iter):
        My_List_Iterator_Base(iter.node_){}

    node_type& operator*() const {return *((node_type*)node_);}
    node_type* operator->() const {return &(operator*());}

    self_type& operator++()
    {
        this->incr();
        return *this;
    }

    self_type operator++(int)
    {
        self_type temp = *this;
        this->incr();
        return temp;
    }

    self_type& operator--()
    {
        this->decr();
        return *this;
    }

    self_type operator--(int)
    {
        self_type temp = *this;
        this->decr();
        return temp;
    }
};

//a loop double-list with an empty head node
template<class Index_T, class Data_T>
class My_List_Base
{
protected:
    typedef My_List_Node<Index_T, Data_T> node_type;
    node_type* node_;   //the tail node

public:
    My_List_Base()
    {
        node_ = allocate_node();
        node_->prev_ = node_;
        node_->next_ = node_;
    }

    ~My_List_Base()
    {
        clear();
        deallocate_node(node_);
    }

    void clear();

protected:
    node_type* allocate_node(){return new node_type;}
    void deallocate_node(node_type* node){delete node; node = 0;}
};

template<class Index_T, class Data_T>
void My_List_Base<Index_T, Data_T>::clear()
{
    node_type* cur = (node_type*)node_->next_;
    while (cur != node_)
    {
        node_type* temp = cur;
        cur = (node_type*)cur->next_;
        deallocate_node(temp);
    }
    node_->next_ = node_;
    node_->prev_ = node_;
}

template <class Index_T, class Data_T>
class My_List : protected My_List_Base<Index_T, Data_T>
{
    typedef My_List_Base<Index_T, Data_T> base_type;

public:
    typedef Index_T        index_type;
    typedef index_type&   index_reference;
    typedef index_type*    index_pointer;
    typedef Data_T          data_type;
    typedef data_type&    data_reference;
    typedef data_type*     data_pointer;

    typedef My_List_Node<Index_T, Data_T> node_type;
    typedef My_List_Node<const Index_T, const Data_T> const_node;
    typedef size_t size_type;

public:
    typedef My_List_Iterator<Index_T, Index_T&, Index_T*, Data_T, Data_T&, Data_T*> iterator;
    typedef My_List_Iterator<Index_T, const Index_T&, const Index_T*, Data_T, const Data_T&, const Data_T*> const_iterator;

protected:
    using base_type::node_;
    using base_type::allocate_node;
    using base_type::deallocate_node;

    node_type* create_node(const Index_T& index, const Data_T& data)
    {
        node_type* node = new node_type(index, data); //allocate_node();
        return node;
    }

    node_type* create_node()
    {
        node_type* node = new node_type; //allocate_node();
        return node;
    }

public:
    explicit My_List():base_type(){}
    ~My_List() {}

    iterator begin(){return (node_type*)(node_->next_);}
    const_iterator begin() const {return (node_type*)(node_->next_);}
    iterator end(){return node_;}
    const_iterator end() const {return node_;}

    bool empty() const {return node_->next_ == node_;}
    //size_type size() const {return std::distance(begin(), end());}
    size_type max_size() const {return size_type(-1);}

    node_type front() {return *begin();}
    const_node front() const {return *begin();}
    node_type back() {return *(--end());}
    const_node back() const {return *(--end());}

    iterator insert(iterator position, const Index_T& index, const Data_T& data)
    {
        node_type *node = create_node(index, data);
        node->next_ = position.node_;
        node->prev_ = position.node_->prev_;
        position.node_->prev_->next_ = node;
        position.node_->prev_ = node;
        return node;
    }

    iterator insert(iterator position){return insert(position, Data_T());}

    void push_front(const Index_T& index, const Data_T& data) {insert(begin(), index, data);}
    void push_front() {insert(begin());}
    void push_back(const Index_T& index, const Data_T& data) {insert(end(), index, data);}
    void push_back() {insert(end());}

    iterator erase(iterator position)
    {
        My_List_Node_Base *next = position.node_->next_;
        My_List_Node_Base *prev = position.node_->prev_;
        node_type *node = (node_type*)position.node_;
        prev->next_ = next;
        next->prev_ = prev;
        //destroy(&node->data_);
        deallocate_node(node);
        return iterator((node_type*)next);
    }

    void clear() {base_type::clear();}
    void pop_front() {erase(begin());}
    void pop_back()
    {
        iterator temp = end();
        erase(--temp);
    }

    void remove(const Data_T& data)
    {
        iterator first = begin();
        iterator last = end();
        while (first != last)
        {
            iterator next = first;
            ++next;
            if (*first == data)
                erase(first);
            first = next;
        }
    }
};


#endif


