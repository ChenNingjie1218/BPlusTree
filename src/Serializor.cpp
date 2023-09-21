#include "Serializor.h"

#include "B_Plus_Tree.h"
#include "bplustree.pb.h"

Serializer::Serializer(string path) { openFile(path); }

Serializer::~Serializer() { closeFile(); }

void Serializer::openFile(string path) {
  fw.open(path, ios::out | ios::binary);
}

void Serializer::closeFile() { fw.close(); }

void Serializer::SaveTree(BPlusTree<int> *&obj_bplustree, bool all = false) {
  bplustree::BPlusTree pb_bplustree;
  obj_bplustree->Serialize();

  string str;
  pb_bplustree.SerializeToString(&str);
  fw << str;
}

void Serializer::SaveBNode(BNode<int> *&obj_bnode, bool all = false) {
  bplustree::BNode pb_bnode;
  obj_bnode->Serialize();
  string str;
  pb_bnode.SerializeToString(&str);
  fw << str;
}

Deserializer::Deserializer(string path) { openFile(path); }

Deserializer::~Deserializer() { closeFile(); }

BPlusTree<int> *Deserializer::LoadTree() {
  string str;
  fr >> str;
  bplustree::BPlusTree pb_bplustree;
  pb_bplustree.ParseFromString(str);

  BPlusTree<int> *obj_bplustree =
      new BPlusTree<int>(pb_bplustree._max_size(), pb_bplustree._name());
  return obj_bplustree;
}

BNode<int> *Deserializer::LoadBNode() {
  string str;
  fr >> str;
  bplustree::BNode pb_bnode;
  pb_bnode.ParseFromString(str);
  if (pb_bnode._isleaf()) {
    LeafBNode<int> *obj_leaf = new LeafBNode<int>();
    return obj_leaf;
  } else {
    InnerBNode<int> *obj_inner = new InnerBNode<int>();
    return obj_inner;
  }
}

void Deserializer::openFile(string path) {
  fr.open(path, ios::in | ios::binary);
}

void Deserializer::closeFile() { fr.close(); }