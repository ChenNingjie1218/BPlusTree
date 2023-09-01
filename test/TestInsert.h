#ifndef TESTINSERT_H
#define TESTINSERT_H
#include "B_Plus_Tree.h"
#include <utility>
/**
 * @brief 空树插入
 * 
*/
template <typename T>
void insertTest_1(pair< T, uint64_t> data){
    uint64_t degree = 3;
    BPlusTree<T>* tree = new BPlusTree<T>(degree);
    tree->BFS();
    tree->B_Plus_Tree_Insert(data);
    tree->BFS();
}

/**
 * @brief 插入非空树且不导致叶子节点分裂
 * 
*/
template <typename T>
void insertTest_2(const uint64_t degree){
    BPlusTree<T>* tree = new BPlusTree<T>(degree);
    tree->B_Plus_Tree_Insert(make_pair(1, 1));
    tree->BFS();
    tree->B_Plus_Tree_Insert(make_pair(2, 2));
    tree->BFS();
}

/**
 * @brief 插入非空树且导致叶子节点分裂
 * 
*/
template <typename T>
void insertTest_3(const uint64_t degree){
    BPlusTree<T>* tree = new BPlusTree<T>(degree);
    uint64_t i;
    for(i = 1; i < degree; ++i){
        tree->B_Plus_Tree_Insert(make_pair(i, i));
    }
    tree->BFS();
    tree->B_Plus_Tree_Insert(make_pair(i, i));
    tree->BFS();
}

/**
 * @brief 插入非空树且导致内部节点分裂
 * 
*/
template <typename T>
void insertTest_4(){
    uint64_t degree = 3;
    // uint64_t degree = 4;
    BPlusTree<T>* tree = new BPlusTree<T>(degree);
    tree->B_Plus_Tree_Insert(make_pair(1, 1));
    tree->B_Plus_Tree_Insert(make_pair(2, 2));
    tree->B_Plus_Tree_Insert(make_pair(3, 3));
    tree->B_Plus_Tree_Insert(make_pair(4, 4));
    tree->B_Plus_Tree_Insert(make_pair(5, 5));
    tree->B_Plus_Tree_Insert(make_pair(1, 1));
    tree->B_Plus_Tree_Insert(make_pair(1, 1));

}

/**
 * @brief 插入非空树且导致顶部节点分裂
 * 
*/
template <typename T>
void insertTest_5(){

}
#endif