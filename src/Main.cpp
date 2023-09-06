#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "B_Plus_Tree.h"
#include "Interface.h"
#include "performanceTest.h"
using namespace std;

int main() {
  // interface<int>();
  for (int i = 3; i < 100; ++i) {
    BPlusTree<int> *bplustree = TestInsert(i);
    TestSearch(bplustree);
    TestDelete(bplustree);
    delete bplustree;
  }
  return 0;
}
