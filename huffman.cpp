#include <huffman.h>

#include <unordered_map>
#include <cassert>
#include <queue>
#include <vector>

using std::pair;
using std::vector;
using std::unordered_map;
using std::priority_queue;

class Node {
public:
  Node(BYTE value, int count = 0);
  Node(Node* lhs, Node* rhs);
  ~Node();

  BYTE& value() { return d_value; }
  int& count() { return d_count; }
  bool isLeaf() { return d_isLeaf; }
  Node*& lhs() { return d_lhs; }
  Node*& rhs() { return d_rhs; }

  void populate(unordered_map<BYTE, pair<int, int>>& encoding, int code, int length);
  void writeTree(vector<bool>& output);

private:
  BYTE d_value;
  int d_count;
  bool d_isLeaf;
  Node* d_lhs, * d_rhs;
};

Node::Node(BYTE value, int count)
  :d_value(value), d_count(count), d_isLeaf(true), d_lhs(nullptr), d_rhs(nullptr) {}
Node::Node(Node* lhs, Node* rhs)
  :d_value(-1), d_count(lhs->count() + rhs->count()), d_isLeaf(false), d_lhs(lhs), d_rhs(rhs) {}

Node::~Node() {
  if (!d_isLeaf) {
    delete d_lhs;
    delete d_rhs;
  }
}

void Node::populate(unordered_map<BYTE, pair<int, int>>& encoding, int code, int length) {
  if (d_isLeaf) {
    assert(encoding.find(d_value) == encoding.end());
    encoding[d_value] = { code, length };
  }
  else {
    d_lhs->populate(encoding, code << 1, length + 1);
    d_rhs->populate(encoding, (code << 1) + 1, length + 1);
  }
}

void Node::writeTree(vector<bool>& output) {
  if (d_isLeaf) {
    output.push_back(1);
    for (int b = 7; b >= 0; b--)
      output.push_back((d_value >> b) & 1);
  }
  else {
    output.push_back(0);
    d_lhs->writeTree(output);
    d_rhs->writeTree(output);
  }
}

class NodeCompare {
public:
  bool operator() (Node* a, Node* b) {
    return a->count() > b->count();
  }
};

Node* buildTree(vector<bool>::const_iterator& it) {
  if (*it++) {
    BYTE val = 0;
    for (int i = 0; i < 8; i++)
      val = (val << 1) | *it++;
    return new Node(val);
  }
  else {
    Node* lhs = buildTree(it);
    Node* rhs = buildTree(it);
    return new Node(lhs, rhs);
  }
}

void pad(vector<bool>& output) {
  int extra = output.size() % 8;
  if (extra != 0) {
    for (int i = 0; i < 8 - extra; i++) {
      output.push_back(0);
    }
  }
}

vector<bool> huffmanEncode(const vector<BYTE>& data) {
  unordered_map<BYTE, int> freq;
  for (BYTE x : data) freq[x]++;

  priority_queue<Node*, vector<Node*>, NodeCompare> pq;
  for (const pair<BYTE, int>& p : freq) {
    pq.push(new Node(p.first, p.second));
  }

  while (pq.size() != 1) {
    Node* a = pq.top();
    pq.pop();
    Node* b = pq.top();
    pq.pop();
    pq.push(new Node(a, b));
  }

  Node* root = pq.top();
  pq.pop();

  unordered_map<BYTE, pair<int, int>> encoding;
  root->populate(encoding, 0, 0);

  vector<bool> output;
  root->writeTree(output);
  pad(output);
  int headerSize = output.size();

  for (BYTE b : data) {
    const auto& [code, length] = encoding[b];
    for (int i = length - 1; i >= 0; i--) {
      output.push_back((code >> i) & 1);
    }
  }
  pad(output);

  delete root;
  return output;
}


vector<BYTE> huffmanDecode(const vector<bool>& bits, int bytes) {
  vector<bool>::const_iterator it = bits.cbegin();
  Node* root = buildTree(it);

  int used = it - bits.cbegin();
  if (used % 8 != 0) it += 8 - (used % 8);

  vector<BYTE> output;
  for (int b = 0; b < bytes; b++) {
    Node* cur = root;
    while (!cur->isLeaf()) {
      assert(it != bits.cend());
      if (*it++) cur = cur->rhs();
      else cur = cur->lhs();
    }
    output.push_back(cur->value());
  }

  return output;
}

vector<BYTE> huffmanDecode(const vector<BYTE>& data, int bytes) {
  vector<bool> bits(data.size() * 8);
  for (int i = 0; i < data.size(); i++) {
    int x = data[i];
    for (int b = 0; b < 8; b++) {
      bits[8 * i + b] = x & 1;
      x >>= 1;
    }
  }

  return huffmanDecode(bits, bytes);
}
