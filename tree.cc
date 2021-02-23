#include "storage.h"

Simple_Tree::Simple_Tree(): root{nullptr}, ite{nullptr} {
    nodes.reserve(512);
}

void Simple_Tree::load_node() {
    nodes.emplace_back(make_unique<Shrunk_Byte>(0,0));
    Shrunk_Byte * right = _stack.top();
    _stack.pop();
    Shrunk_Byte * left = _stack.top();
    _stack.pop();
    
    nodes.back()->assign_left(left);
    nodes.back()->assign_right(right);
    _stack.push(nodes.back().get());
}

void Simple_Tree::load_node(unsigned char _byte) {
    nodes.emplace_back(make_unique<Shrunk_Byte>(_byte,0));
    _stack.push(nodes.back().get());
}

void Simple_Tree::move_left() {
    if (ite->left == nullptr) throw Null_Leaf();
    ite = ite->left;
}

void Simple_Tree::move_right() {
    if (ite->right == nullptr) throw Null_Leaf();
    ite = ite->right;
}

unsigned char Simple_Tree::get_byte() {
    unsigned char _byte = ite->byte;
    ite = root;
    return _byte;
}

bool Simple_Tree::at_leaf() {
    return ite->left == nullptr && ite->right == nullptr;
}

void Simple_Tree::clear_up() {
    nodes.shrink_to_fit();
    root = nodes.back().get();
    ite = root;
}

void Simple_Tree::print_tree (ostream & os) {
    for (auto & i: nodes) { i->visited = false; }
    unsigned short i;
    Shrunk_Byte * cur = root;
    while (cur != nullptr) {
        if (cur->left == nullptr && cur->right == nullptr) {
            cur->visited = true;
            i = cur->byte;
            os << i << ' ';
            cur = cur->parent;
        }
        else if (!(cur->left->visited)) {
            cur = cur->left;
        }
        else if (!(cur->right->visited)) {
            cur = cur->right;
        }
        else {
            if (cur->parent) { os << "256 ";}
            else { os << "256"; }
            cur->visited = true;
            cur = cur->parent;
        }
    }
    os << std::endl;
}
