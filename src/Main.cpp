#include <google/protobuf/generated_message_table_driven.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "B_Plus_Tree.h"
#include "Interface.h"
using namespace std;
int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  interface<int>();
  google::protobuf::ShutdownProtobufLibrary();
  return 0;
}
