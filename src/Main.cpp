#include <google/protobuf/generated_message_table_driven.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "B_Plus_Tree.h"
#include "Interface.h"
#include "Serializor.h"
// #include "bplustree.pb.h"
// #include "performanceTest.h"
using namespace std;
int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  // cout << "string or int?" << endl;
  // string type;
  // getline(cin, type);
  // if (type == string("string")) {
  //   interface<string>();
  // } else {
  interface<int>();
  // }
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
