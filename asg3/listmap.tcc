// pair-programming by Qiutong Li(qli33) & Haofan Wang (hwang108)
#include "listmap.h"

#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template<typename Key, typename Value, class Less>
listmap<Key, Value, Less>::node::node(node* anext, node* aprev,
        const value_type& value_) :
        link(anext, aprev), value(value_) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template<typename Key, typename Value, class Less>
listmap<Key, Value, Less>::~listmap() {
    DEBUGF('l', reinterpret_cast<const void*>(this));
    link* l = anchor()->next;
    while (l != anchor()) {
        link* temp = l;
        l = l->next;
        delete temp;
    }
}

//
// iterator listmap::insert (const value_type&)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator 
listmap<Key, Value, Less>::insert(
        const value_type& pair) {
    DEBUGF('l', &pair << "->" << pair);

    if (empty()) {
        node* n = new node(anchor(), anchor(), pair);
        anchor_.next = n;
        return iterator(n);
    }

    if (find(pair.first) == end()) {

        node* n = new node(anchor(), anchor(), pair);

        node* it = anchor()->next;
        if (less(n->value.first, it->value.first)) {
            n->next = it;
            it->prev = n;
            anchor_.next = n;

            return iterator(n);
        } else {
            node* pre = it;
            it = it->next;
            while (it != anchor() && 
            less(it->value.first, n->value.first)) {
                pre = it;
                it = it->next;
            }

            if (it == anchor()) {
                it = pre;
                n->prev = it;
                it->next = n;
            } else {
                n->next = it;
                n->prev = it->prev;
                it->prev->next = n;
                it->prev = n;
            }

            return iterator(n);
        }
    } else {
        //cout << (find(pair.first) == end()) << endl;
        //cout << (find(pair.first)->second) << endl;

        //cout << pair << endl;
        node* it = anchor()->next;
        while (it->value.first != pair.first) {
            it = it->next;
        }

        it->value.second = pair.second;

        return iterator(it);
    }
}

//
// listmap::find(const key_type&)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator 
listmap<Key, Value, Less>::find(
        const key_type& that) {
    DEBUGF('l', that);

    node* l = anchor()->next;
    while (l != anchor()) {
        if (l->value.first == that) {
            return iterator(l);
        }
        l = l->next;
    }

    return end();
}

template<typename Key, typename Value, class Less>
listmap<Key, Value, Less>::listmap(
        const listmap<Key, Value, Less>& constListmap) {
    node* l = &constListmap.anchor_;
    while (l != &constListmap.anchor_) {
        this->insert(l->value);
        l = l->next;
    }
}

template<typename Key, typename Value, class Less>
inline listmap<Key, Value, Less>& 
listmap<Key, Value, Less>::operator =(
        const listmap<Key, Value, Less>& constListmap) {
    link* l = anchor();
    while (l != anchor()) {
        link* temp = l;
        l = l->next;
        delete temp;
    }

    node* n = &constListmap.anchor_;
    while (n != &constListmap.anchor_) {
        this->insert(n->value);
        n = n->next;
    }
}

//
// iterator listmap::erase (iterator position)
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator 
listmap<Key, Value, Less>::erase(
        iterator position) {
    DEBUGF('l', &*position);

    node* l = anchor()->next;
    while (l != anchor()) {

        if (l == position.where) {
            node* temp = l->next;
            l->prev->next = l->next;
            l->next->prev = l->prev;
            delete l;
            return iterator(temp);
        }
        l = l->next;
    }

    return iterator();
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type&
listmap<Key, Value, Less>::iterator::operator*() {
    DEBUGF('l', where);
    return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::value_type*
listmap<Key, Value, Less>::iterator::operator->() {
    DEBUGF('l', where);
    return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator++() {
    DEBUGF('l', where);
    where = where->next;
    return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template<typename Key, typename Value, class Less>
typename listmap<Key, Value, Less>::iterator&
listmap<Key, Value, Less>::iterator::operator--() {
    DEBUGF('l', where);
    where = where->prev;
    return *this;
}

//
// bool listmap::iterator::operator== (const iterator&)
//
template<typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator==(
        const iterator& that) const {
    return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template<typename Key, typename Value, class Less>
inline bool listmap<Key, Value, Less>::iterator::operator!=(
        const iterator& that) const {
    return this->where != that.where;
}
