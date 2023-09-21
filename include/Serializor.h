#include <fstream>

#include "B_Plus_Tree.h"

#ifndef SERIALIZE_H
#define SERIALIZE_H
using namespace std;
class Serializer {
 public:
  Serializer(string path);
  ~Serializer();
  void SaveTree(BPlusTree<int> *&obj_bplustree, bool all);
  void SaveBNode(BNode<int> *&obj_bnode, bool all);

 private:
  void openFile(string path);
  void closeFile();
  ofstream fw;
};

class Deserializer {
 public:
  Deserializer(string path);
  ~Deserializer();
  BPlusTree<int> *LoadTree();
  BNode<int> *LoadBNode();

 private:
  void openFile(string path);
  void closeFile();
  ifstream fr;
};
#endif