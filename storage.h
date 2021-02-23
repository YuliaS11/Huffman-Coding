#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <string>
#include <stack>
#include <iostream>
#include "byte.h"

using std::ostream;
using std::unique_ptr;
using std::make_unique;
using std::stack;
using std::cout;
using std::endl;

class Binary_Storage {
  private:
    vector<unique_ptr<Shrunk_Byte>> bytes; // Store Allocated Nodes, irrelevant to the
      //actual implementation
    vector<unique_ptr<Shrunk_Byte>> leaves; // Store Empty Nodes
    Shrunk_Byte * root; // Root node of tree, does not handle resource management.
    void update_path(); // Update path of nodes

  public:
    Binary_Storage(); // Default Contructor
    void increment_byte(unsigned char _byte); // Increment the count of the byte.
    void build_tree(); // Construct the tree
    void print_tree(ostream & os); // Print the structure of the tree, Post-Order
    vector<unsigned char> & get_byte_path(unsigned char _byte); // Return the path of the byte
}; // For Compression

class Not_Leaf {}; // Exception, If trying to access value of a non leaf
class Null_Leaf {}; // Exception, If trying to move into a nullptr (only leaves have value)
class Simple_Tree {
  private:
    vector<unique_ptr<Shrunk_Byte>> nodes;
    stack<Shrunk_Byte *> _stack;
    Shrunk_Byte * root; // root of the tree.
    Shrunk_Byte * ite; // Iterator
  public:
    Simple_Tree(); // Default Constructor
    void load_node(unsigned char _byte); // load the leave node onto the tree.
    void load_node(); // load the brach node onto the tree.
    void move_left(); // Move the iterator to the left node.
    void move_right(); // Move the iterator to the right node.
    unsigned char get_byte(); // Return the iterator leaf node's value, and return to root.
    bool at_leaf(); // Is iterator at leaf;
    void clear_up(); // Free resource, set root.
    void print_tree(ostream & os); // Print the structure of the tree, Post-Order
}; // For Decompression

#endif
