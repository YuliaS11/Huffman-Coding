#include "byte.h"

Shrunk_Byte::Shrunk_Byte(int _byte, int _count):
parent{nullptr}, left{nullptr}, right{nullptr}, visited{false} {
    byte = _byte; count = _count;
}

void Shrunk_Byte::assign_left(Shrunk_Byte * _left) {
    left = _left;
    if (left != nullptr) _left->parent = this;
}

void Shrunk_Byte::assign_right(Shrunk_Byte * _right) {
    right = _right;
    if (right != nullptr) _right->parent = this;
}
