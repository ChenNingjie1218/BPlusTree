#ifndef PERFORMANCE_TEST
#define PERFORMANCE_TEST
#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>

#include "B_Plus_Tree.h"
BPlusTree<int> *TestInsert(int degree);
void TestSearch(BPlusTree<int> *bplustree);
void TestDelete(BPlusTree<int> *bplustree);
void initVector();
void performanceTest();
#endif