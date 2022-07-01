#ifndef HUFFMAN__HUFFMAN_TREE_H_
#define HUFFMAN__HUFFMAN_TREE_H_

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <fcntl.h>
#include <climits>
#include <cstring>
#include <cstdlib>

#include <vector>
#include <list>

#define MAX_CHARS_CNT 256
#define BUFSIZE 131072

struct Node {
    unsigned char c;
    unsigned int weight;
    Node *left;
    Node *right;
};

void buildHuffmanTree(std::list<Node *> &nodes);
void freeHuffmanTree(Node *root);
void getCodes(const Node *root,
              std::vector<unsigned char> *codes,
              std::vector<unsigned int> &code_lens,
              unsigned long long code,
              unsigned int code_len);

int compress(char *src, char *dest);
int decompress(char *src, char *dest);
int test();

#endif //HUFFMAN__HUFFMAN_TREE_H_
