#include "performanceTest.h"
BPlusTree<int> *TestInsert(int degree) {
  BPlusTree<int> *bplustree = new BPlusTree<int>(degree);
  for (int i = 0; i < 10000; ++i) {
    bplustree->B_Plus_Tree_Insert(make_pair(i, i));
  }
  return bplustree;
}

void TestSearch(BPlusTree<int> *bplustree) {
  for (int i = -1; i < 10001; ++i) {
    bplustree->B_Plus_Tree_Search(i);
  }
}

void TestDelete(BPlusTree<int> *bplustree) {
  for (int i = 0; i < 10000; ++i) {
    bplustree->B_Plus_Tree_Delete(i);
  }
}
