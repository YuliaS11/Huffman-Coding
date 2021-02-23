#include <algorithm>
#include "storage.h"

Binary_Storage::Binary_Storage(): root{nullptr} {
    bytes.reserve(256);
    leaves.reserve(256);
    for (size_t i = 0; i < 256; i++) {
        bytes.emplace_back(make_unique<Shrunk_Byte> (i, 0));
    }
}

void Binary_Storage::increment_byte(unsigned char _byte) {
    bytes[_byte]->count++;
}

void Binary_Storage::build_tree() {
    vector <Shrunk_Byte *> _bytes; // Raw Pointer Version of bytes to load the tree.
    _bytes.reserve(256);
    for (size_t i = 0; i < 256; i++) {
        if (bytes[i]->count > 0) _bytes.emplace_back(bytes[i].get());
    }
    std::sort(_bytes.begin(), _bytes.end(), [](Shrunk_Byte * b1, Shrunk_Byte * b2)
        { return b1->count >= b2->count; });
    _bytes.shrink_to_fit();
    unsigned short empty_nodes = 0; // the first index where the extra nodes are still unimplemented
    unsigned short used_nodes = _bytes.size(); // the first nodes that have been used to build the tree
    Shrunk_Byte * left;
    Shrunk_Byte * right;
    while (used_nodes > 0 || empty_nodes < leaves.size()) {
        if (used_nodes > 0) {
            right = _bytes[used_nodes-1];
            if (empty_nodes < leaves.size() && right->count > leaves[empty_nodes]->count) {
                right = leaves[empty_nodes].get();
                empty_nodes++;
            }
            else { used_nodes--; }
            if (used_nodes > 0) {
                left = _bytes[used_nodes-1];
                if (empty_nodes < leaves.size() && left->count > leaves[empty_nodes]->count) {
                    left = leaves[empty_nodes].get();
                    empty_nodes ++;
                }
                else { used_nodes--; }
            }
            else {
                left = leaves[empty_nodes].get();
                empty_nodes++;
            }
        }
        else {
            if (empty_nodes == leaves.size() - 1) break;
            std::sort(leaves.begin() + empty_nodes, leaves.end(), [](const unique_ptr<Shrunk_Byte> & b1,
                const unique_ptr<Shrunk_Byte> & b2)
                { return b1->count < b2->count; });
            right = leaves[empty_nodes].get();
            empty_nodes++;
            left = leaves[empty_nodes].get();
            empty_nodes++;
        }
        leaves.emplace_back(make_unique<Shrunk_Byte>(0,
            left->count + right->count));
        leaves.back()->assign_left(left);
        leaves.back()->assign_right(right);
    }
    leaves.shrink_to_fit();
    root = leaves.back().get();
    update_path();
}

void Binary_Storage::update_path() {
    for (auto & i: bytes) { i->visited = false; }
    for (auto & i: leaves) { i->visited = false; }
    vector<unsigned char> path; // current path in form of 0 = left, 1 = right
    path.reserve(64);
    Shrunk_Byte * cur = root;
    while (cur != nullptr) {
        if (cur->left == nullptr && cur->right == nullptr) {
            cur->visited = true;
            cur->path = path;
            cur->path.shrink_to_fit();
            path.pop_back();
            cur = cur->parent;
        }
        else if (!(cur->left->visited)) {
            cur = cur->left;
            path.emplace_back(0);
        }
        else if (!(cur->right->visited)) {
            cur = cur->right;
            path.emplace_back(1);
        }
        else {
            cur->visited = true;
            cur = cur->parent;
            path.pop_back();
        }
    }
}

void Binary_Storage::print_tree (ostream & os) {
    for (auto & i: bytes) { i->visited = false; }
    for (auto & i: leaves) { i->visited = false; }
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

vector<unsigned char> & Binary_Storage::get_byte_path(unsigned char _byte){
    vector <unsigned char> & path = bytes[_byte]->path;
    return path;
}
