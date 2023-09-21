#include "performanceTest.h"
vector<int> srcData;
void initVector() {
  for (int i = 0; i < 10000; ++i) {
    srcData.push_back(i);
  }
  random_shuffle(srcData.begin(), srcData.end());
}
BPlusTree<int> *TestInsert(int degree) {
  BPlusTree<int> *bplustree = new BPlusTree<int>(degree);
  for (int i = 0; i < 10000; ++i) {
    bplustree->B_Plus_Tree_Insert(make_pair(srcData[i], srcData[i]));
  }
  return bplustree;
}

void TestSearch(BPlusTree<int> *bplustree) {
  for (int i = 0; i < 10000; ++i) {
    bplustree->B_Plus_Tree_Search(srcData[i]);
  }
  bplustree->B_Plus_Tree_Search(-1);
  bplustree->B_Plus_Tree_Search(10001);
}

void TestDelete(BPlusTree<int> *bplustree) {
  for (int i = 0; i < 10000; ++i) {
    bplustree->B_Plus_Tree_Delete(srcData[i]);
  }
}

void performanceTest() {
  initVector();
  for (int i = 3; i < 100; ++i) {
    BPlusTree<int> *bplustree = TestInsert(i);
    TestSearch(bplustree);
    TestDelete(bplustree);
    delete bplustree;
    bplustree = nullptr;
  }
}