#include "performanceTest.h"
vector<int> srcData;
void initVector() {
  for (int i = 0; i < NUMBER_SAMPLES; ++i) {
    srcData.push_back(i);
  }
  // 使用随机种子初始化随机数生成器
  std::random_device rd;
  std::mt19937 gen(rd());
  // 使用 std::shuffle 进行随机排序
  std::shuffle(srcData.begin(), srcData.end(), gen);
}
BPlusTree<int> *TestInsert(int degree) {
  BPlusTree<int> *bplustree = new BPlusTree<int>(degree, "testTree");
  for (int i = 0; i < NUMBER_SAMPLES; ++i) {
    bplustree->B_Plus_Tree_Insert(make_pair(srcData[i], srcData[i]));
  }
  return bplustree;
}

void TestSearch(BPlusTree<int> *bplustree) {
  for (int i = 0; i < NUMBER_SAMPLES; ++i) {
    bplustree->B_Plus_Tree_Search(srcData[i]);
  }
  bplustree->B_Plus_Tree_Search(-1);
  bplustree->B_Plus_Tree_Search(NUMBER_SAMPLES + 1);
}

void TestDelete(BPlusTree<int> *bplustree) {
  for (int i = 0; i < NUMBER_SAMPLES; ++i) {
    bplustree->B_Plus_Tree_Delete(srcData[i]);
  }
}

void performanceTest() {
  initVector();
  vector<BPlusTree<int> *> bplustrees;
  ofstream fw;

  //---------------------------插入--------------------------
  system("rm -rf ./performance_insert");
  fw.open("./performance_insert", ios::out);
  for (int i = MIN_DEGREE; i < MAX_DEGREE; i += 5) {
    auto start = std::chrono::high_resolution_clock::now();
    BPlusTree<int> *testTree = TestInsert(i);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
    bplustrees.push_back(testTree);
  }
  fw.close();

  int num_tree = bplustrees.size();
  //---------------------------查找--------------------------
  system("rm -rf ./performance_search");
  fw.open("./performance_search", ios::out);

  for (int i = 0; i < num_tree; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    TestSearch(bplustrees[i]);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
  }
  fw.close();

  //---------------------------删除--------------------------
  system("rm -rf ./performance_delete");
  fw.open("./performance_delete", ios::out);
  for (int i = 0; i < num_tree; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    TestDelete(bplustrees[i]);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
  }
  fw.close();

  //---------------------------清树--------------------------
  for (int i = 0; i < num_tree; ++i) {
    delete bplustrees[i];
  }

  //---------------------------并发插入--------------------------
  system("rm -rf ./performance_insert_concurrent");
  fw.open("./performance_insert_concurrent", ios::out);
  vector<thread> threads;
  for (int i = 1; i < NUMBER_THREADS; ++i) {
    BPlusTree<int> testTree(CONCURRENT_DEGREE, "testTree");
    threads.clear();
    int perCount = NUMBER_SAMPLES / i;  //每个线程要插的数量
    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < i; ++j) {
      threads.push_back(thread([&, j]() {
        for (int k = 0; k < perCount; ++k) {
          testTree.B_Plus_Tree_Insert(
              make_pair(srcData[j * perCount + k], srcData[j * perCount + k]));
        };
        if (j == i - 1) {
          //最后一个线程，把剩余要插的插完
          int count = (j + 1) * perCount;
          while (count < NUMBER_SAMPLES) {
            testTree.B_Plus_Tree_Insert(
                make_pair(srcData[count], srcData[count]));
            ++count;
          }
        }
      }));
    }
    for (auto &t : threads) {
      t.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
#ifndef NDEBUG
    if (testTree.OutPutAllTheKeys().size() != NUMBER_SAMPLES) {
      cerr << "沒插完" << endl;
    }
#endif
  }
  fw.close();

  BPlusTree<int> *testTree = TestInsert(CONCURRENT_DEGREE);
  //---------------------------并发查询--------------------------
  system("rm -rf ./performance_search_concurrent");
  fw.open("./performance_search_concurrent", ios::out);
  for (int i = 1; i < NUMBER_THREADS; ++i) {
    threads.clear();
    int perCount = NUMBER_SAMPLES / i;  //每个线程要查的数量
    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < i; ++j) {
      threads.push_back(thread([&, j]() {
        for (int k = 0; k < perCount; ++k) {
          testTree->B_Plus_Tree_Search(srcData[j * perCount + k]);
        };
        if (j == i - 1) {
          //最后一个线程，把剩余要查的插完
          int count = (j + 1) * perCount;
          while (count < NUMBER_SAMPLES) {
            testTree->B_Plus_Tree_Search(srcData[count]);
            ++count;
          }
        }
      }));
    }
    for (auto &t : threads) {
      t.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
  }
  fw.close();
  delete testTree;

  //---------------------------并发删除--------------------------
  system("rm -rf ./performance_delete_concurrent");
  fw.open("./performance_delete_concurrent", ios::out);
  for (int i = 1; i < NUMBER_THREADS; ++i) {
    testTree = TestInsert(CONCURRENT_DEGREE);
    threads.clear();
    int perCount = NUMBER_SAMPLES / i;  //每个线程要删的数量
    auto start = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < i; ++j) {
      threads.push_back(thread([&, j]() {
        for (int k = 0; k < perCount; ++k) {
          testTree->B_Plus_Tree_Delete(srcData[j * perCount + k]);
        };
        if (j == i - 1) {
          //最后一个线程，把剩余要删的删完
          int count = (j + 1) * perCount;
          while (count < NUMBER_SAMPLES) {
            testTree->B_Plus_Tree_Delete(srcData[count]);
            ++count;
          }
        }
      }));
    }
    for (auto &t : threads) {
      t.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    fw << duration << endl;
#ifndef NDEBUG
    if (!testTree->OutPutAllTheKeys().empty()) {
      cerr << "沒刪完" << endl;
    }
#endif
    delete testTree;
  }
  fw.close();

  cout << "------------------性能测试结束---------------" << endl;
}

int main() {
  performanceTest();
  return 0;
}