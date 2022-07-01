#include "huffman_tree.h"

void buildHuffmanTree(std::list<Node *> &nodes)
{
    while (nodes.size() > 1) {
        Node *tmp1 = nodes.back();
        nodes.pop_back();
        Node *tmp2 = nodes.back();
        nodes.pop_back();
        Node *tmp = new Node;
        tmp->weight = tmp1->weight + tmp2->weight;
        tmp->left = tmp2;
        tmp->right = tmp1;
        auto iter = nodes.begin();
        while (iter != nodes.end() && (*iter)->weight >= tmp->weight) {
            iter++;
        }
        nodes.insert(iter, tmp);
    }
}

void freeHuffmanTree(Node *root)
{
    if (root == nullptr) {
        return;
    }
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    delete root;
}

void getCodes(const Node *root,
              std::vector<unsigned char> *codes,
              std::vector<unsigned int> &code_lens,
              unsigned long long code,
              unsigned int code_len)
{
    if (root == nullptr || codes == nullptr) {
        return;
    }
    if (root->left == nullptr) {
        for (unsigned int i = 0; i < code_len; ++i) {
            codes[root->c][i] = (char) ((code >> (code_len - i - 1)) & 1);
        }
        code_lens[root->c] = code_len;
    }
    getCodes(root->left, codes, code_lens, (code << 1), code_len + 1);
    getCodes(root->right, codes, code_lens, (code << 1) | 1, code_len + 1);
}