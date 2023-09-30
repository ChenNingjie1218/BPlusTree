#ifndef INTERFACE_H
#define INTERFACE_H
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "B_Plus_Tree.h"
#include "bplustree.pb.h"
#include "performanceTest.h"
/**
 * @brief 初始化指令列表
 */
void interfaceInit();
/**
 * @brief 命令帮助
 */
void help();

/**
 * @brief 操作接口
 */
template <typename T>
void interface() {
  interfaceInit();
  string cmd;
  string option;
  BPlusTree<T> *tree = nullptr;
  // cout << "请输入指令:" << endl;
  while (getline(cin, cmd)) {
    istringstream line(cmd);
    option.clear();
    line >> option;
    if (option == string("insert")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      T key;
      uint64_t value;
      // int i = 0, n;
      // cout << "请输入要插入的键值对数量：" << endl;
      // cin >> n;
      // while(i++ < n){
      // cout << "请输入要插入的键值对:" << endl;
      // cin >> key >> value;
      if (line >> key >> value) {
        pair<T, uint64_t> tempData = make_pair(key, value);
        tree->B_Plus_Tree_Insert(tempData);
      } else {
        cout << "输入的键值对有问题" << endl;
      }
      // }
    } else if (option == string("bfs")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      // cout << "进行层序遍历:" << endl;
      tree->BFS();
    } else if (option == string("delete")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      // cout << "请输入要删除的键:" << endl;
      T key;
      // cin >> key;
      line >> key;
      tree->B_Plus_Tree_Delete(key);
    } else if (option == string("quit")) {
      delete tree;
      break;
    } else if (option == string("search")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      T key;
      // cout << "请输入要查找的键:" << endl;
      // cin >> key;
      line >> key;
      pair<T, uint64_t *> ans = tree->B_Plus_Tree_Search(key);
      if (ans.second) {
        cout << "关键字" << ans.first << "对应的值为" << *ans.second << endl;
      } else {
        cout << "没有该关键字" << endl;
      }
    } else if (option == string("rangesearch")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      T key1, key2;
      // cout << "请输入要查找的键范围:" << endl;
      // cin >> key1 >> key2;
      if (line >> key1 >> key2) {
        if (key2 < key1) {
          swap(key1, key2);
        }
        tree->B_Plus_Tree_Search_For_Range(key1, key2);
      } else {
        cout << "key有问题" << endl;
      }
    } else if (option == string("outputall")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
        continue;
      }
      tree->OutPutAllTheKeys();
    } else if (option == string("clear")) {
      delete tree;
      tree = nullptr;
    } else if (option == string("help")) {
      help();
    } else if (option == string("cls")) {
      cout << "\033c";
    } else if (option == string("create")) {
      if (tree) {
        cout << "已经创建了一个树" << endl;
      }
      int maxDegree = 3;        //默认3
      string name("testTree");  //默认为test
      line >> maxDegree >> name;
      // maxDegree = 4;
      tree = new BPlusTree<T>(maxDegree, name);
    } else if (option == string("reset")) {
      tree->B_Plus_Tree_Reset();
    } else if (option == string("serialize")) {
      if (!tree) {
        cout << "您还没有建树" << endl;
      }
      tree->serializeAll();
    } else if (option == string("deserialize")) {
      string tree_name;
      line >> tree_name;
      ifstream fr;
      fr.open("./" + tree_name + "/" + tree_name, ios::in | ios::binary);
      if (fr) {
        bplustree::BPlusTree pb_bplustree;
        pb_bplustree.ParseFromIstream(&fr);
        if (tree) {
          cout << "您当前的树还没有保存，请问是否舍弃？" << endl;
          string discard;
          getline(cin, discard);
          if (discard != "yes") {
            continue;
          }
          delete tree;
        }
        tree = new BPlusTree<T>(pb_bplustree);
        fr.close();
      } else {
        cerr << "open error:./" + tree_name + "/" + tree_name << endl;
      }
    } else if (option == string("test")) {
      performanceTest();
    } else if (option == string("unittest")) {
      int id = fork();
      if (id == 0) {
        system("../test/unittest");
        exit(0);
      }
    } else {
      cout << "还没有这个指令" << endl;
    }
    // cout << "请输入指令:" << endl;
  }
}
#endif