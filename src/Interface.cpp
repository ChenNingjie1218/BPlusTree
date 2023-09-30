#include "Interface.h"
map<string, string> op;
void interfaceInit() {
  op.clear();
  op["insert"] = "插入一个键值对 eg:insert 1 1";
  op["delete"] = "删除一个关键字 eg:delete 1";
  op["search"] = "查询一个关键字 eg:search 1";
  op["rangesearch"] = "范围查找 eg:rangesearch 1 9";
  op["bfs"] = "层序遍历 eg:bfs";
  op["quit"] = "退出 eg:quit";
  op["outputall"] = "全遍历 eg:outputall";
  op["help"] = "帮助 eg:help";
  op["clear"] = "清空树 eg:clear";
  op["cls"] = "清屏 eg:cls";
  op["create"] = "创建一课空树 eg:create 3";
  op["reset"] = "重置树 eg:reset";
  op["serialize"] = "序列化树 eg:serialize";
  op["deserialize"] = "反序列化某个树 eg:serialize testTree";
  op["test"] = "性能测试";
  op["unittest"] = "单元测试";
}

void help() {
  for (auto oneop : op) {
    cout << setiosflags(ios::left) << setw(15) << oneop.first << oneop.second
         << endl;
  }
}