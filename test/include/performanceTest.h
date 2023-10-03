#ifndef PERFORMANCE_TEST
#define PERFORMANCE_TEST
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>
#include <utility>

#include "B_Plus_Tree.h"
#define MAX_DEGREE 400        //最大度数
#define MIN_DEGREE 3          //最小度数
#define NUMBER_SAMPLES 10000  //样本数量
#define NUMBER_THREADS 40     //线程数量
#define CONCURRENT_DEGREE 10  //并发时的度数

BPlusTree<int> *TestInsert(int degree);
void TestSearch(BPlusTree<int> *bplustree);
void TestDelete(BPlusTree<int> *bplustree);
void initVector();
void performanceTest();
#endif