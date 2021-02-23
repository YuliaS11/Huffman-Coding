#ifndef BYTE_H
#define BYTE_H

#include <vector>

using std::vector;

class Binary_Storage;
class Simple_Tree;

class Shrunk_Byte
{
  private:
    unsigned char byte; // The original byte expression
    unsigned int count; // Count of given byte in file
    Shrunk_Byte * parent; // Parent Node. For easily tranversing the tree
    Shrunk_Byte * left; // Left Node
    Shrunk_Byte * right; // Right Node
    bool visited; // true if visited in tree traversal. Reset on tree tranversal
    vector<unsigned char> path; // The location inside the tree, to quickly encode the compression. 0 = left, true = 1
  public:
    Shrunk_Byte(int _byte, int _count); // Constructor, shrunk is set false on all 8 bit
    bool operator > (const Shrunk_Byte & other); // Compare only count
    void assign_left(Shrunk_Byte * _left);
    void assign_right(Shrunk_Byte * _right);
    friend class Binary_Storage;
    friend class Simple_Tree;
};

#endif
