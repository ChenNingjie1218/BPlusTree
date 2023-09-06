#include <gtest/gtest.h>

#include <utility>

#include "B_Plus_Tree.h"
#include "gmock/gmock.h"
class NULLTREE : public ::testing::Test {
public:
  BPlusTree<int> *test_tree = new BPlusTree<int>(3);
  void SetUp() override {}
  void TearDown() override { delete test_tree; }
};
TEST_F(NULLTREE, insert_test) {
  vector<int> ans;
  ASSERT_EQ(test_tree->BFS(), ans);
  test_tree->B_Plus_Tree_Insert(make_pair(1, 1));
  ans.push_back(1);
  ASSERT_EQ(test_tree->BFS(), ans) << "test insert to a null tree";

  test_tree->B_Plus_Tree_Insert(make_pair(2, 2));
  ans.push_back(2);
  ASSERT_EQ(test_tree->BFS(), ans) << "test insert and no split";

  test_tree->B_Plus_Tree_Insert(make_pair(3, 3));
  ans = {2, 1, 2, 3};
  ASSERT_EQ(test_tree->BFS(), ans) << "test leafnode split";

  test_tree->B_Plus_Tree_Insert(make_pair(4, 4));
  test_tree->BFS();
  test_tree->B_Plus_Tree_Insert(make_pair(5, 5));
  ans = {3, 2, 4, 1, 2, 3, 4, 5};
  ASSERT_EQ(test_tree->BFS(), ans) << "test top node split";

  test_tree->B_Plus_Tree_Insert(make_pair(6, 6));
  test_tree->BFS();
  test_tree->B_Plus_Tree_Insert(make_pair(7, 7));
  ans = {3, 5, 2, 4, 6, 1, 2, 3, 4, 5, 6, 7};
  ASSERT_EQ(test_tree->BFS(), ans) << "test innernode split";
};

class SEARCH_TREE : public ::testing::Test {
public:
  BPlusTree<int> *_test_tree = new BPlusTree<int>(10);
  void SetUp() override {
    for (int i = 0; i < 100; i++) {
      pair<int, uint64_t> data = make_pair(i, i);
      _test_tree->B_Plus_Tree_Insert(data);
    }
  }
  void TearDown() override { delete _test_tree; }
};
TEST_F(SEARCH_TREE, search_test) {
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(*(_test_tree->B_Plus_Tree_Search(i).second), i)
        << "test search succeed";
  }
  EXPECT_THAT(_test_tree->B_Plus_Tree_Search(-1).second, testing::IsNull())
      << "test search failed";
  EXPECT_THAT(_test_tree->B_Plus_Tree_Search(1000001).second, testing::IsNull())
      << "test search failed";
}

TEST_F(SEARCH_TREE, range_search_test) {
  // _test_tree->BFS();
  vector<pair<int, uint64_t>> ans;
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(-10, -5), ans);
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(200, 300), ans);

  for (int i = 0; i < 50; ++i) {
    ans.push_back(make_pair(i, i));
  }
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(-10, 50), ans);

  for (int i = 50; i < 100; ++i) {
    ans.push_back(make_pair(i, i));
  }
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(-10, 200), ans);

  ans.clear();
  for (int i = 1; i < 4; ++i) {
    ans.push_back(make_pair(i, i));
  }
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(1, 4), ans);

  ans.clear();
  for (int i = 31; i < 90; ++i) {
    ans.push_back(make_pair(i, i));
  }
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(31, 90), ans);

  for (int i = 90; i < 100; ++i) {
    ans.push_back(make_pair(i, i));
  }
  EXPECT_EQ(_test_tree->B_Plus_Tree_Search_For_Range(31, 200), ans);
}

TEST_F(NULLTREE, delete_test) {
  vector<int> insertData = {0, 50, 100, 75, 30, 40, 80, 45, 20};
  for (vector<int>::size_type i = 0; i < insertData.size(); ++i) {
    test_tree->B_Plus_Tree_Insert(make_pair(insertData[i], insertData[i]));
  }

  vector<int> ans = {50, 30, 40, 75, 80, 0, 20, 30, 40, 45, 50, 75, 80, 100};
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(-1);
  test_tree->B_Plus_Tree_Delete(10);
  test_tree->B_Plus_Tree_Delete(110);
  ASSERT_EQ(test_tree->BFS(), ans) << "don't have";

  ans = {50, 30, 40, 75, 80, 0, 30, 40, 45, 50, 75, 80, 100};
  test_tree->B_Plus_Tree_Delete(20);
  ASSERT_EQ(test_tree->BFS(), ans) << "not on the path";

  ans = {50, 30, 40, 75, 100, 0, 30, 40, 45, 50, 75, 100};
  test_tree->B_Plus_Tree_Delete(80);
  ASSERT_EQ(test_tree->BFS(), ans) << "on the path and next is leafnode";

  ans = {50, 40, 45, 75, 100, 0, 40, 45, 50, 75, 100};
  test_tree->B_Plus_Tree_Delete(30);
  ASSERT_EQ(test_tree->BFS(), ans) << "borrow from right silbing";

  test_tree->B_Plus_Tree_Insert(make_pair(30, 30));
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(40);
  ans = {50, 30, 45, 75, 100, 0, 30, 45, 50, 75, 100};
  ASSERT_EQ(test_tree->BFS(), ans) << "borrow from left silbing";

  test_tree->B_Plus_Tree_Delete(0);
  ans = {50, 45, 75, 100, 30, 45, 50, 75, 100};
  ASSERT_EQ(test_tree->BFS(), ans) << "merge with right silbing";

  test_tree->B_Plus_Tree_Delete(100);
  ans = {50, 45, 75, 30, 45, 50, 75};
  ASSERT_EQ(test_tree->BFS(), ans) << "merge with left silbing";

  test_tree->B_Plus_Tree_Insert(make_pair(60, 60));
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(50);
  ans = {60, 45, 75, 30, 45, 60, 75};
  ASSERT_EQ(test_tree->BFS(), ans) << "on the path, next is not the leafnode";

  test_tree->B_Plus_Tree_Delete(30);
  ans = {60, 75, 45, 60, 75};
  ASSERT_EQ(test_tree->BFS(), ans) << "innernode merge with right silbing";

  test_tree->B_Plus_Tree_Insert(make_pair(80, 80));
  test_tree->B_Plus_Tree_Insert(make_pair(90, 90));
  test_tree->B_Plus_Tree_Delete(80);
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(90);
  ans = {60, 75, 45, 60, 75};
  ASSERT_EQ(test_tree->BFS(), ans) << "innernode merge with left silbing";

  test_tree->B_Plus_Tree_Insert(make_pair(80, 80));
  test_tree->B_Plus_Tree_Insert(make_pair(90, 90));
  test_tree->B_Plus_Tree_Insert(make_pair(100, 100));
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(45);
  ans = {80, 75, 90, 60, 75, 80, 90, 100};
  ASSERT_EQ(test_tree->BFS(), ans) << "innernode borrow from right silbing";

  test_tree->B_Plus_Tree_Insert(make_pair(40, 40));
  test_tree->B_Plus_Tree_Insert(make_pair(50, 50));
  test_tree->B_Plus_Tree_Delete(100);
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(80);
  ans = {75, 50, 90, 40, 50, 60, 75, 90};
  ASSERT_EQ(test_tree->BFS(), ans) << "innernode borrow from left silbing";

  test_tree->B_Plus_Tree_Delete(40);
  test_tree->B_Plus_Tree_Delete(50);
  test_tree->B_Plus_Tree_Delete(60);
  test_tree->B_Plus_Tree_Delete(75);
  test_tree->BFS();
  test_tree->B_Plus_Tree_Delete(90);
  ans = {};
  ASSERT_EQ(test_tree->BFS(), ans) << "to null tree";
}

int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}