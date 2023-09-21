#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H
#include <uuid/uuid.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "bplustree.pb.h"
using namespace std;

#define NDEBUG
// ---------------------------B+树的类-------------------------
/**
 * @brief B+树节点基类
 * @tparam T 关键字类型，目前仅支持整型和string类型
 */
template <typename T>
class BNode {
  typedef typename vector<T>::size_type size_type;

 public:
  BNode() : _keyNum(0), _isLeaf(false) {}
  BNode(bool isLeaf) : _keyNum(0), _isLeaf(isLeaf) {}
  BNode(const BNode<T> &bnode)
      : _keyNum(bnode._keyNum), _isLeaf(bnode._isLeaf), _key(bnode._key) {}
  /*序列化的构造函数*/
  BNode(const bplustree::BNode &pb_bnode)
      : _keyNum(pb_bnode._keynum()),
        _isLeaf(pb_bnode._isleaf()),
        _key(begin(pb_bnode._key()), end(pb_bnode._key())) {
    char str[36];
    strncpy(str, pb_bnode._uuid().c_str(), 36);
    uuid_parse(str, _uuid);
  }
  virtual ~BNode() {}

  /**
   * @brief 分裂用的特殊构造函数
   * @param SIZE 从_key的多少位开始
   * */
  BNode(BNode<T> *bnode, const size_type &SIZE)
      : _isLeaf(bnode->isLeaf()),
        _key(make_move_iterator(bnode->_key.begin() + SIZE),
             make_move_iterator(bnode->_key.end())) {
    updateKeyNum();
  }

  /* 获取关键字数量 */
  size_type getKeyNum() const { return _keyNum; }

  /* 更新关键字数量 */
  void updateKeyNum() { _keyNum = _key.size(); }

  /* 是否是安全节点 */
  // bool isSafe(){
  //     if(_keyNum > MAX_SIZE / 2 - 1 && _keyNum < MAX_SIZE - 1){
  //         return true;
  //     }
  //     return false;
  // }

  /* 是否是叶子节点 */
  const bool isLeaf() const { return _isLeaf; }

  /* 插入关键字 */
  virtual void insertKey(const pair<T, uint64_t> &kv,
                         const size_type &MAX_SIZE) = 0;
  /* 删除关键字 */
  virtual T deleteKey(const T &k, const size_type &MAX_SIZE) = 0;

  /* 在该节点中添加关键字 */
  size_type addKey(const T &k) {
    size_type insertIndex = getInsertIndex(k);
    _key.insert(_key.begin() + insertIndex, k);
    updateKeyNum();
    return insertIndex;
  }

  /* 在该节点中移除关键字 */
  size_type removeKey(const T &k) {
    size_type deleteIndex = getKeyIndex(k);
    if (deleteIndex != _keyNum) {
      _key.erase(_key.begin() + deleteIndex);
    }
    updateKeyNum();
    return deleteIndex;
  }

  /* 获取插入的关键字位置 */
  size_type getInsertIndex(const T &k) const {
    size_type l = 0;
    size_type r = _keyNum;
    size_type mid = (l + r) / 2;
    while (l < r) {
      mid = (l + r) / 2;
      if (_key[mid] < k) {
        l = mid + 1;
      } else {
        r = mid;
      }
    }
    return l;
  }

  /* 找关键字的Index */
  size_type getKeyIndex(const T &k) const {
    size_type l = 0;
    size_type r = _keyNum;
    size_type mid = (l + r) / 2;
    while (l <= r) {
      mid = (l + r) / 2;
      if (mid == _keyNum) {
        break;
      }
      if (_key[mid] == k) {
        return mid;
      } else if (_key[mid] < k) {
        l = mid + 1;
      } else {
        if (mid) {
          r = mid - 1;
        } else {
          break;
        }
      }
    }
    return _keyNum;
  }

  /* 查找关键字 */
  virtual pair<T, uint64_t *> searchKey(const T &k) const = 0;
  /* 范围查询关键字 */
  virtual void searchKeyForRange(const T &l, const T &r,
                                 vector<pair<T, uint64_t>> &seq,
                                 const bool &continueFlag = false) const = 0;
  /* 输出所有关键字 */
  virtual void outputAllKeys(vector<T> &seq) const = 0;
  /* 关键字分裂 */
  virtual void keySplit(const bool &isLeft, const size_type &MAX_SIZE) = 0;
  /* 获取关键字 */
  T getKey(const size_type &index) const { return _key[index]; }
  /* 借关键字 */
  virtual T borrowKey(BNode<T> *const &silbing, const bool &isRight,
                      const T &key) = 0;
  /* 获取关键字数组 */
  vector<T> getAllKeys() const { return _key; }
  /* 序列化 */
  virtual void Serialize(string dir) = 0;
  /* 获取uuid */
  void getUUID(uuid_t &uuid) {
    if (uuid_is_null(_uuid)) {
      uuid_generate(_uuid);
    }
    uuid_copy(uuid, _uuid);
  }
  /* 设置uuid */
  void setUUID(uuid_t &uuid) { uuid_copy(_uuid, uuid); }

 protected:
  size_type _keyNum;
  const bool _isLeaf;
  vector<T> _key;
  uuid_t _uuid = "";
};

/**
 * @brief 叶子节点
 * @tparam T 关键字类型 默认为int 目前仅支持整型和string类型
 *
 *
 */
template <typename T = int>
class LeafBNode : public BNode<T> {
  typedef typename vector<T>::size_type size_type;

 public:
  LeafBNode() : BNode<T>(true), _next(nullptr), _prev(nullptr){};
  LeafBNode(const LeafBNode &leafbnode)
      : BNode<T>(leafbnode),
        _next(leafbnode._next),
        _prev(leafbnode._prev),
        _value(leafbnode._value) {}

  /* 分裂用的特殊构造函数 */
  LeafBNode(LeafBNode *&leafbnode, const size_type &MAX_SIZE)
      : BNode<T>(leafbnode, MAX_SIZE / 2),
        _next(leafbnode->_next),
        _prev(leafbnode),
        _value(make_move_iterator(leafbnode->_value.begin() + MAX_SIZE / 2),
               make_move_iterator(leafbnode->_value.end())) {}
  /*序列化的构造函数*/
  LeafBNode(const bplustree::BNode &pb_bnode)
      : BNode<T>(pb_bnode), _next(nullptr), _prev(nullptr) {
    for (int i = 0; i < pb_bnode._value_size(); ++i) {
      _value.push_back(new uint64_t(pb_bnode._value(i)));
    }
    // if (pb_bnode.has__next()) {
    //   string next = pb_bnode._next();
    //   ifstream fr;
    //   fr.open("./bin/" + next);
    //   if (fr) {
    //     string str;
    //     fr >> str;
    //     bplustree::BNode pb_bnode;
    //     pb_bnode.ParseFromString(str);
    //     _next = new LeafBNode<T>(pb_bnode, this);
    //     fr.close();
    //   } else {
    //     cerr << "open error:" << next << endl;
    //   }
    // }
    // _prev = prev;
  }
  ~LeafBNode() {
    for (auto &value : _value) {
      delete value;
      value = nullptr;
    }
  }

  /* 获取右兄弟 */
  LeafBNode *getNext() const { return _next; }

  /* 设置新的右兄弟，返回旧的 */
  LeafBNode *setNext(LeafBNode *const &leafnode) {
    LeafBNode *old = _next;
    _next = leafnode;
    return old;
  }

  /* 获取左兄弟 */
  LeafBNode *getPrev() const { return _prev; }

  /* 设置新的左兄弟，返回旧的 */
  LeafBNode *setPrev(LeafBNode *const &leafnode) {
    LeafBNode *old = _prev;
    _prev = leafnode;
    return old;
  }

  /* 搜索关键字 */
  pair<T, uint64_t *> searchKey(const T &k) const override {
    size_type keyindex = this->getKeyIndex(k);
    if (this->_keyNum != keyindex) {
      return make_pair(k, _value[keyindex]);
    }
    return make_pair(k, nullptr);
  }

  /* 插入关键字 */
  void insertKey(const pair<T, uint64_t> &kv,
                 const size_type &MAX_SIZE) override {
    size_type insertIndex = this->addKey(kv.first);
    uint64_t *p_v = new uint64_t(kv.second);
    _value.insert(_value.begin() + insertIndex, p_v);
  }

  /* 删除关键字 */
  T deleteKey(const T &k, const size_type &MAX_SIZE) override {
    size_type removeIndex = this->removeKey(k);
    if (removeIndex != _value.size()) {
#ifndef NDEBUG
      cout << "----------------已删除<" << k << ", " << *_value[removeIndex]
           << ">-------------" << endl;
#endif
      delete _value[removeIndex];
      _value.erase(_value.begin() + removeIndex);
      //返回更新的关键字
      if (removeIndex < this->_keyNum) {
        return this->_key[removeIndex];
      } else if (_next) {
        return _next->getKey(0);
      }
    }
    return k;
  }

  /* 输出所有关键字 */
  void outputAllKeys(vector<T> &seq) const override {
    cout << " [";
    for (size_type i = 0; i < this->_keyNum; ++i) {
      seq.push_back(this->_key[i]);
      cout << " <" << this->_key[i] << ", " << *_value[i] << ">";
    }
    cout << " ]";
  }

  /* 范围查询关键字 */
  void searchKeyForRange(const T &l, const T &r, vector<pair<T, uint64_t>> &seq,
                         const bool &continueFlag = false) const override {
    size_type index = 0;
    if (!continueFlag) {
      index = this->getInsertIndex(l);
    }

    while (index < this->_keyNum && this->_key[index] < r) {
      seq.push_back(make_pair(this->_key[index], *_value[index]));
      cout << " <" << this->_key[index] << ", " << *_value[index] << ">";
      ++index;
    }
    if (_next && index == this->_keyNum) {
      _next->searchKeyForRange(l, r, seq, true);
    }
  }

  /* 分裂关键字和值 */
  void keySplit(const bool &isLeft, const size_type &MAX_SIZE) override {
    if (isLeft) {
      this->_key.erase(this->_key.begin() + MAX_SIZE / 2, this->_key.end());
      _value.erase(_value.begin() + MAX_SIZE / 2, _value.end());
    } else {
      this->_key.erase(this->_key.begin(), this->_key.begin() + MAX_SIZE / 2);
      _value.erase(_value.begin(), _value.begin() + MAX_SIZE / 2);
    }
    this->updateKeyNum();
  }

  /* 借关键字 */
  T borrowKey(BNode<T> *const &silbing, const bool &isRight,
              const T &key) override {
    pair<T, uint64_t *> data =
        static_cast<LeafBNode<T> *>(silbing)->provideKey(isRight);
    if (isRight) {
      this->_key.push_back(data.first);
      this->updateKeyNum();
      _value.push_back(data.second);
      return silbing->getKey(0);
    } else {
      this->_key.insert(this->_key.begin(), data.first);
      this->updateKeyNum();
      _value.insert(_value.begin(), data.second);
      return data.first;
    }
  }

  /* 提供借出的关键字及数据 */
  pair<T, uint64_t *> provideKey(const bool isRight) {
    T key;
    uint64_t *value;
    if (isRight) {
      key = this->_key[0];
      this->_key.erase(this->_key.begin());
      value = _value[0];
      _value.erase(_value.begin());
    } else {
      key = this->_key.back();
      this->_key.erase(this->_key.end() - 1);
      value = _value.back();
      _value.erase(_value.end() - 1);
    }
    this->updateKeyNum();
#ifndef NDEBUG
    cout << "-------------------叶子节点找" << (isRight ? "右" : "左")
         << "兄弟借" << key << "----------------------" << endl;
#endif
    return make_pair(key, value);
  }
  /* 获取值指针的数组*/
  vector<uint64_t *> getAllValues() { return _value; }
  /* 合并关键字 */
  void mergeKeys(vector<T> &&keys) noexcept {
    this->_key.insert(this->_key.end(), keys.begin(), keys.end());
    this->updateKeyNum();
  }
  /* 合并值 */
  void mergeValues(vector<uint64_t *> &&values) noexcept {
    _value.insert(_value.end(), values.begin(), values.end());
  }
  /* 合并时，value移动后清空value的vector，以免析构的时候把值清了*/
  void clearValues() { _value.clear(); }
  uint64_t getValue(size_type &index) {
    if (index < this->_keyNum) {
      return *_value[index];
    } else {
      cerr << "读value越界" << endl;
      return 0;
    }
  }

  /* 序列化 */
  void Serialize(string dir) override {
    bplustree::BNode pb_bnode;
    pb_bnode.set__isleaf(this->_isLeaf);
    pb_bnode.set__keynum(this->_keyNum);
    uuid_t uuid;
    char str[36];

    // prev
    if (_prev) {
      _prev->getUUID(uuid);
      uuid_unparse(uuid, str);
      string prev(begin(str), end(str));
      pb_bnode.set__prev(prev);
    }
    if (_next) {  // next
      _next->getUUID(uuid);
      uuid_unparse(uuid, str);
      string next(begin(str), end(str));
      pb_bnode.set__next(next);
    }
    // uuid
    uuid_unparse(this->_uuid, str);
    string name(begin(str), end(str));
    pb_bnode.set__uuid(name);
    for (size_t i = 0; i < this->_keyNum; ++i) {
      pb_bnode.add__key(this->_key[i]);
      pb_bnode.add__value(*_value[i]);
    }
    ofstream fw;
    fw.open(dir + name, ios::out | ios::binary);
    if (fw) {
      string serialize_data;
      pb_bnode.SerializeToString(&serialize_data);
      fw << serialize_data;
      fw.close();
    } else {
      cerr << "序列化时" << dir + name << "打开失败" << endl;
    }
  }

 private:
  LeafBNode *_next;
  LeafBNode *_prev;
  vector<uint64_t *> _value;
};

/* 反序列化时存prev */
template <typename T>
LeafBNode<T> *deserialize_prev = nullptr;
/* 反序列化时存head */
template <typename T>
LeafBNode<T> *deserialize_head = nullptr;

/**
 * @brief 非叶子节点
 * @tparam T 关键字类型 默认为int 目前仅支持整型和string类型
 *
 *
 */
template <typename T>
class InnerBNode : public BNode<T> {
  typedef typename vector<T>::size_type size_type;

 public:
  InnerBNode() : BNode<T>(false) {}
  InnerBNode(const InnerBNode<T> &innerbnode)
      : BNode<T>(innerbnode), p(innerbnode.p) {}
  ~InnerBNode() {}

  /* 内部节点分裂用的特殊构造函数 */
  InnerBNode(InnerBNode<T> *&innerbnode, const size_type &MAX_SIZE)
      : BNode<T>(innerbnode, MAX_SIZE / 2 + 1),
        p(make_move_iterator(innerbnode->p.begin() + MAX_SIZE / 2 + 1),
          make_move_iterator(innerbnode->p.end())) {}
  /*顶层分裂调用*/
  InnerBNode(BNode<T> *const &root, const size_type &MAX_SIZE)
      : BNode<T>(false) {
    pair<BNode<T> *, T> info = split(root, MAX_SIZE);
    this->addKey(info.second);
    p.push_back(root);
    p.push_back(info.first);
  }
  /* 序列化构造函数*/
  InnerBNode(const bplustree::BNode &pb_bnode, string dir)
      : BNode<T>(pb_bnode) {
    typename vector<BNode<T> *>::size_type child_size = pb_bnode._child_size();
    for (typename vector<BNode<T> *>::size_type i = 0; i < child_size; ++i) {
      ifstream fr;
      fr.open(dir + pb_bnode._child(i));
      if (fr) {
        string str;
        fr >> str;
        bplustree::BNode pb_child;
        pb_child.ParseFromString(str);
        if (pb_child._isleaf()) {
          LeafBNode<T> *child = new LeafBNode<T>(pb_child);
          p.push_back(child);

          //设置head
          if (!deserialize_head<T>) {
            deserialize_head<T> = child;
          }
          //设置prev
          if (deserialize_prev<T>) {
            deserialize_prev<T>->setNext(child);
          }
          child->setPrev(deserialize_prev<T>);
          deserialize_prev<T> = child;
        } else {
          p.push_back(new InnerBNode<T>(pb_child, dir));
        }
        fr.close();
      } else {
        cerr << "open error:" << pb_bnode._child(i) << endl;
      }
    }
  }

  /* 分裂某孩子节点 */
  pair<BNode<T> *, T> split(BNode<T> *const &BNode, const size_type &MAX_SIZE) {
    T newkey = BNode->getKey(MAX_SIZE / 2);
    if (BNode->isLeaf()) {
      LeafBNode<T> *firstNode = static_cast<LeafBNode<T> *>(BNode);
      LeafBNode<T> *newNode = new LeafBNode<T>(firstNode, MAX_SIZE);

      // LeafBNode<T>* newNode = new LeafBNode<T>(*firstNode);
      firstNode->setNext(newNode);
      // newNode->setPrev(firstNode);
      firstNode->keySplit(true, MAX_SIZE);
      // newNode->keySplit(false, MAX_SIZE);
      return make_pair(newNode, newkey);
    } else {
      InnerBNode<T> *firstNode = static_cast<InnerBNode<T> *>(BNode);
      InnerBNode<T> *newNode = new InnerBNode<T>(firstNode, MAX_SIZE);

      // InnerBNode<T>* newNode = new InnerBNode<T>(*firstNode);
      firstNode->keySplit(true, MAX_SIZE);
      // newNode->keySplit(false, MAX_SIZE);
      return make_pair(newNode, newkey);
    }
  }

  /* 合并某孩子节点 */
  void merge(BNode<T> *const &left, BNode<T> *const &right, const T &&key) {
    if (left->isLeaf()) {
      //叶子节点的合并
      LeafBNode<T> *leafLeft = static_cast<LeafBNode<T> *>(left);
      LeafBNode<T> *leafRight = static_cast<LeafBNode<T> *>(right);
      leafLeft->mergeKeys(leafRight->getAllKeys());
      leafLeft->mergeValues(leafRight->getAllValues());
      leafRight->clearValues();
      leafLeft->setNext(leafRight->getNext());
      if (leafRight->getNext()) {
        leafRight->getNext()->setPrev(leafLeft);
      }
    } else {
      //非叶子节点的合并
      InnerBNode<T> *innerLeft = static_cast<InnerBNode<T> *>(left);
      InnerBNode<T> *innerRight = static_cast<InnerBNode<T> *>(right);
      innerLeft->mergeKeys(innerRight->getAllKeys(), move(key));
      innerLeft->mergePs(innerRight->getAllPs());
    }
    delete right;
  }

  /* 搜索目标key值 */
  pair<T, uint64_t *> searchKey(const T &k) const override {
    size_type index = this->getInsertIndex(k);
    if (index < this->_keyNum && k == this->_key[index]) {
      return p[index + 1]->searchKey(k);
    } else {
      return p[index]->searchKey(k);
    }
  }

  /* 插入关键字 */
  void insertKey(const pair<T, uint64_t> &kv,
                 const size_type &MAX_SIZE) override {
    size_type insertIndex = this->getInsertIndex(kv.first);
    //默认不插入相等的key，都是左插
    p[insertIndex]->insertKey(kv, MAX_SIZE);

    //孩子插入后需要分裂
    if (p[insertIndex]->getKeyNum() == MAX_SIZE) {
      pair<BNode<T> *, T> info = split(p[insertIndex], MAX_SIZE);
      insertIndex = this->addKey(info.second);
      p.insert(p.begin() + insertIndex + 1, info.first);
    }
  }

  /* 删除关键字 */
  T deleteKey(const T &k, const size_type &MAX_SIZE) override {
    size_type deleteIndex = this->getInsertIndex(k);
    T newKey;
    if (deleteIndex < this->_keyNum && k == this->_key[deleteIndex]) {
      ++deleteIndex;
      //遇见关键字向右找
      newKey = p[deleteIndex]->deleteKey(k, MAX_SIZE);
      this->_key[deleteIndex - 1] = newKey;
    } else {
      newKey = p[deleteIndex]->deleteKey(k, MAX_SIZE);
    }
    //孩子删除后需要借
    if (p[deleteIndex]->getKeyNum() < ceil(1.0 * MAX_SIZE / 2) - 1) {
      if (deleteIndex + 1 < p.size() &&
          p[deleteIndex + 1]->getKeyNum() > ceil(1.0 * MAX_SIZE / 2) - 1) {
        //找右边兄弟借
        this->_key[deleteIndex] = p[deleteIndex]->borrowKey(
            p[deleteIndex + 1], true, this->_key[deleteIndex]);
      } else if (deleteIndex && p[deleteIndex - 1]->getKeyNum() >
                                    ceil(1.0 * MAX_SIZE / 2) - 1) {
        //找左边兄弟借
        this->_key[deleteIndex - 1] = p[deleteIndex]->borrowKey(
            p[deleteIndex - 1], false, this->_key[deleteIndex - 1]);
      } else if (deleteIndex + 1 < p.size()) {
        //跟右兄弟合并
        merge(p[deleteIndex], p[deleteIndex + 1], this->getKey(deleteIndex));
        this->_key.erase(this->_key.begin() + deleteIndex);
        this->updateKeyNum();
        p.erase(p.begin() + deleteIndex + 1);
#ifndef NDEBUG
        cout << "-------------------跟右兄弟合并----------------------" << endl;
#endif
      } else {
        //跟左兄弟合并
        merge(p[deleteIndex - 1], p[deleteIndex],
              this->getKey(deleteIndex - 1));
        this->_key.erase(this->_key.begin() + deleteIndex - 1);
        this->updateKeyNum();
        p.erase(p.begin() + deleteIndex);
#ifndef NDEBUG
        cout << "-------------------跟左兄弟合并----------------------" << endl;
#endif
      }
    }
    return newKey;
  }

  /* 范围查询关键字 */
  void searchKeyForRange(const T &l, const T &r, vector<pair<T, uint64_t>> &seq,
                         const bool &continueFlag = false) const override {
    size_type index = this->getInsertIndex(l);
    if (index < this->_keyNum && l == this->_key[index]) {
      p[index + 1]->searchKeyForRange(l, r, seq);
    } else {
      p[index]->searchKeyForRange(l, r, seq);
    }
  }

  /* 输出所有关键字 */
  void outputAllKeys(vector<T> &seq) const override {
    cout << " [";
    for (size_type i = 0; i < this->_keyNum; ++i) {
      if (i) {
        cout << " ";
      }
      cout << this->_key[i];
      seq.push_back(this->_key[i]);
    }
    cout << "]";
  }
  /* 分裂关键字和指针 */
  void keySplit(const bool &isLeft, const size_type &MAX_SIZE) override {
    if (isLeft) {
      this->_key.erase(this->_key.begin() + MAX_SIZE / 2, this->_key.end());
      p.erase(p.begin() + MAX_SIZE / 2 + 1, p.end());
    } else {
      this->_key.erase(this->_key.begin(),
                       this->_key.begin() + MAX_SIZE / 2 + 1);
      p.erase(p.begin(), p.begin() + MAX_SIZE / 2 + 1);
    }
    this->updateKeyNum();
  }

  BNode<T> *getChild(const size_type &index) const { return p[index]; }
  size_type getChildNum() const { return p.size(); }

  /* 借关键字 */
  T borrowKey(BNode<T> *const &silbing, const bool &isRight,
              const T &key) override {
    pair<T, BNode<T> *> data =
        static_cast<InnerBNode<T> *>(silbing)->provideKey(isRight);
    if (isRight) {
      this->_key.push_back(key);
      p.push_back(data.second);
    } else {
      this->_key.insert(this->_key.begin(), key);
      p.insert(p.begin(), data.second);
    }
    this->updateKeyNum();
    return data.first;
  }

  /* 提供借出的关键字及数据 */
  pair<T, BNode<T> *> provideKey(const bool &isRight) {
    T key;
    BNode<T> *child;
    if (isRight) {
      key = this->_key[0];
      this->_key.erase(this->_key.begin());
      child = p[0];
      p.erase(p.begin());
    } else {
      key = this->_key[this->_keyNum - 1];
      this->_key.erase(this->_key.end() - 1);
      child = p[p.size() - 1];
      p.erase(p.end() - 1);
    }
    this->updateKeyNum();
#ifndef NDEBUG
    cout << "-------------------内部节点找" << (isRight ? "右" : "左")
         << "兄弟借" << key << "----------------------" << endl;
#endif
    return make_pair(key, child);
  }
  /* 获取指针的数组 */
  vector<BNode<T> *> getAllPs() const { return p; }
  /* 合并关键字 */
  void mergeKeys(vector<T> &&keys, const T &&key) noexcept {
    this->_key.push_back(key);
    this->_key.insert(this->_key.end(), keys.begin(), keys.end());
    this->updateKeyNum();
  }
  /* 合并指针 */
  void mergePs(vector<BNode<T> *> &&ps) noexcept {
    p.insert(p.end(), ps.begin(), ps.end());
  }

  /* 序列化 */
  void Serialize(string dir) override {
    bplustree::BNode pb_bnode;
    pb_bnode.set__isleaf(this->_isLeaf);
    pb_bnode.set__keynum(this->_keyNum);
    uuid_t uuid;
    char str[36];
    for (size_t i = 0; i < this->_keyNum; ++i) {
      pb_bnode.add__key(this->_key[i]);
      p[i]->getUUID(uuid);
      uuid_unparse(uuid, str);
      string child(begin(str), end(str));
      pb_bnode.add__child(child);
    }
    p[this->_keyNum]->getUUID(uuid);
    uuid_unparse(uuid, str);
    string child(begin(str), end(str));
    pb_bnode.add__child(child);
    // uuid
    uuid_unparse(this->_uuid, str);
    string name(begin(str), end(str));
    pb_bnode.set__uuid(name);

    ofstream fw;
    fw.open(dir + name, ios::out | ios::binary);
    if (fw) {
      string serialize_data;
      pb_bnode.SerializeToString(&serialize_data);
      fw << serialize_data;
      fw.close();
    } else {
      cerr << "序列化时" + dir + name << "打开失败" << endl;
    }
  }

 private:
  vector<BNode<T> *> p;
};

template <typename T>
class BPlusTree {
  typedef typename vector<T>::size_type size_type;

 public:
  BPlusTree() : _MAX_SIZE(3), _name("testTree") { B_Plus_Tree_Create(); }
  BPlusTree(const size_type &max_size, string name)
      : _MAX_SIZE(max_size), _name(name) {
    B_Plus_Tree_Create();
  }
  BPlusTree(const bplustree::BPlusTree &pb_bplustree)
      : _MAX_SIZE(pb_bplustree._max_size()), _name(pb_bplustree._name()) {
    if (pb_bplustree.has__root()) {
      string root = pb_bplustree._root();
      ifstream fr;
      string dir = "./" + _name + "/";
      fr.open(dir + root, ios::in | ios::binary);
      if (fr) {
        string str;
        fr >> str;
        bplustree::BNode pb_bnode;
        pb_bnode.ParseFromString(str);
        if (pb_bnode._isleaf()) {
          _root = new LeafBNode<T>(pb_bnode);
          setHead();
        } else {
          deserialize_head<T> = nullptr;
          deserialize_prev<T> = nullptr;
          _root = new InnerBNode<T>(pb_bnode, dir);
          _Head = deserialize_head<T>;
        }
        fr.close();
      } else {
        cerr << "open error:" << root << endl;
      }
    }

    // if (pb_bplustree.has__head() &&
    //     pb_bplustree._head() != pb_bplustree._root()) {
    //   string head = pb_bplustree._head();
    //   ifstream fr;
    //   fr.open("./bin/" + head, ios::in | ios::binary);
    //   if (fr) {
    //     string str;
    //     fr >> str;
    //     bplustree::BNode pb_bnode;
    //     pb_bnode.ParseFromString(str);
    //     _Head = new LeafBNode<T>(pb_bnode, nullptr);
    //     fr.close();
    //   } else {
    //     cerr << "open error:" << head << endl;
    //   }
    // }
  }
  ~BPlusTree() { B_Plus_Tree_Clear(); }

  /**
   * @brief 搜索B树
   * @return 查找成功返回键值对，失败返回的值是一个空指针
   * @param k 查找的关键字
   */
  pair<T, uint64_t *> B_Plus_Tree_Search(const T &k) const {
    if (_root->getKeyNum()) {
      return _root->searchKey(k);
    }
    return make_pair(k, nullptr);
  }

  /**
   * @brief 向B+树中插入一个关键字
   * @param data 键值对
   */
  void B_Plus_Tree_Insert(const pair<T, uint64_t> &data) {
#ifndef NDEBUG
    cout << "---------------向B+树中插入<" << data.first << ", " << data.second
         << ">--------------" << endl;
#endif

    _root->insertKey(data, _MAX_SIZE);
    if (_root->getKeyNum() == _MAX_SIZE) {
#ifndef NDEBUG
      cout << "-------------------顶层节点满了---------------" << endl;
#endif
      _root = new InnerBNode<T>(_root, _MAX_SIZE);
    }
  }

  /**
   * @brief 向B树中删除一个关键字
   * @param k 待删除的关键字
   */
  void B_Plus_Tree_Delete(const T &k) {
    if (!_root->getKeyNum()) {
      cout << "无法删除" << endl;
      return;
    }
#ifndef NDEBUG
    cout << "------------------开始删除<" << k << ">------------------" << endl;
#endif
    _root->deleteKey(k, _MAX_SIZE);
    //顶层没节点了
    if (_root->getKeyNum() == 0 && !_root->isLeaf()) {
      InnerBNode<T> *oldRoot = dynamic_cast<InnerBNode<T> *>(_root);
      if (oldRoot) {
        _root = oldRoot->getChild(0);
        delete oldRoot;
      }
    }
  }

  /**
   * @brief B树的范围查询
   * @param l 范围左域
   * @param r 范围右域
   */
  vector<pair<T, uint64_t>> B_Plus_Tree_Search_For_Range(const T &l,
                                                         const T &r) const {
    vector<pair<T, uint64_t>> rangeSearchResult;
    _root->searchKeyForRange(l, r, rangeSearchResult);
    if (rangeSearchResult.empty()) {
      cout << "没有该范围的关键字";
    }
    cout << endl;
    return rangeSearchResult;
  }

  /**
   * @brief 层序遍历
   * @tparam T 关键字类型 默认为int 目前仅支持整型和string类型
   * @tparam x 树的根节点
   *
   */
  vector<T> BFS() const {
    typedef typename vector<T>::size_type size_type;
    queue<BNode<T> *> q;
    q.push(_root);
    BNode<T> *lastLayer = _root;
    vector<T> bfsSeq;
    while (!q.empty()) {
      BNode<T> *temp = q.front();
      q.pop();
      temp->outputAllKeys(bfsSeq);
      if (!temp->isLeaf()) {
        InnerBNode<T> *tempInner = static_cast<InnerBNode<T> *>(temp);
        for (size_type i = 0; i < tempInner->getChildNum(); ++i) {
          q.push(tempInner->getChild(i));
        }
        if (temp == lastLayer) {
          lastLayer = tempInner->getChild(tempInner->getChildNum() - 1);
          cout << endl;
        }
      }
    }
    cout << endl;
    return bfsSeq;
  }

  /**
   * @brief 全遍历叶子节点
   *
   */
  vector<T> OutPutAllTheKeys() const {
    const LeafBNode<T> *p = _Head;
    vector<T> allKeySeq;
    while (p) {
      p->outputAllKeys(allKeySeq);
      p = p->getNext();
    }
    cout << endl;
    return allKeySeq;
  }
  /* 重置树 */
  void B_Plus_Tree_Reset() {
    B_Plus_Tree_Clear();
    B_Plus_Tree_Create();
  }
  /* 序列化 */
  void Serialize() {
    bplustree::BPlusTree pb_bplustree;
    pb_bplustree.set__max_size(_MAX_SIZE);
    pb_bplustree.set__name(_name);
    uuid_t uuid;
    char str[36];
    // root
    _root->getUUID(uuid);
    uuid_unparse(uuid, str);
    string root(begin(str), end(str));
    pb_bplustree.set__root(root);
    // head
    _Head->getUUID(uuid);
    uuid_unparse(uuid, str);
    string head(begin(str), end(str));
    pb_bplustree.set__head(head);

    string path = "./" + _name;
    string cmd = "rm -rf ";
    system((cmd + path).c_str());
    cmd = "mkdir ";
    system((cmd + path).c_str());
    path += "/" + _name;
    ofstream fw;
    fw.open(path, ios::out | ios::binary);
    if (fw) {
      string serialize_data;
      pb_bplustree.SerializeToString(&serialize_data);
      fw << serialize_data;
      fw.close();
    } else {
      cerr << "序列化时./" + path << "打开失败" << endl;
    }
  }
  void serializeAll() {
    Serialize();
    typedef typename vector<T>::size_type size_type;
    queue<BNode<T> *> q;
    q.push(_root);
    while (!q.empty()) {
      BNode<T> *temp = q.front();
      q.pop();
      if (!temp->isLeaf()) {
        InnerBNode<T> *tempInner = static_cast<InnerBNode<T> *>(temp);
        for (size_type i = 0; i < tempInner->getChildNum(); ++i) {
          q.push(tempInner->getChild(i));
        }
      }
      temp->Serialize("./" + _name + "/");
    }
  }
  size_type getMAX_SIZE() { return _MAX_SIZE; }
  string getName() { return _name; }
  void setName(string name) { _name = name; }

 private:
  /**
   * @brief 创建一个空的B树
   * @return 指向该节点的指针
   * @tparam T 关键字类型 默认为int 目前仅支持整型和string类型
   */
  void B_Plus_Tree_Create() {
#ifndef NDEBUG
    cout << "---------------创建一课空的B+树--------------" << endl;
#endif
    if (!_root) {
      _root = new LeafBNode<T>();
      setHead();
    }
  }
  void setHead() { _Head = static_cast<LeafBNode<T> *>(_root); }
  /**
   * @brief 利用层序遍历清空树
   */
  void B_Plus_Tree_Clear() {
    typedef typename vector<T>::size_type size_type;
    queue<BNode<T> *> q;
    q.push(_root);
    while (!q.empty()) {
      BNode<T> *temp = q.front();
      q.pop();
      if (!temp->isLeaf()) {
        InnerBNode<T> *tempInner = static_cast<InnerBNode<T> *>(temp);
        for (size_type i = 0; i < tempInner->getChildNum(); ++i) {
          q.push(tempInner->getChild(i));
        }
      }
      delete temp;
    }
    _root = nullptr;
    _Head = nullptr;
#ifndef NDEBUG
    cout << "----------------B+树已清空----------------" << endl;
#endif
  }
  BNode<T> *_root = nullptr;
  const size_type _MAX_SIZE;
  LeafBNode<T> *_Head = nullptr;
  string _name;
};

#endif