#ifndef PERFORMANCE_TEST
#define PERFORMANCE_TEST
#include "B_Plus_Tree.h"
BPlusTree<int> *TestInsert(int degree);
void TestSearch(BPlusTree<int> *bplustree);
void TestDelete(BPlusTree<int> *bplustree);
#endif