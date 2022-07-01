#include "huffman_tree.h"

int decompress(char *src, char *dest)
{
    int fin = open(src, O_RDONLY);
    if (fin == -1) {
        perror("Can't open src file");
        return 1;
    }
    int fout = open(dest, O_WRONLY | O_TRUNC | O_CREAT, 666);
    if (fout == -1) {
        perror("Can't open dest file");
        close(fin);
        return 1;
    }
    long file_size = lseek(fin, 0, SEEK_END);
    lseek(fin, 0, SEEK_SET);
    unsigned short chars_cnt = 0;
    if (read(fin, &chars_cnt, sizeof(unsigned short)) != sizeof(unsigned short)) {
        perror("Read failed");
        close(fin);
        close(fout);
        return 1;
    }
    std::vector<unsigned int> chars(MAX_CHARS_CNT);
    unsigned char chars_in_file[chars_cnt];
    if (read(fin, chars_in_file, chars_cnt) != chars_cnt) {
        perror("Read failed");
        close(fin);
        close(fout);
        return 1;
    }
    for (int i = 0; i < chars_cnt; ++i) {
        if (read(fin, &chars[chars_in_file[i]], sizeof(unsigned int)) != sizeof(unsigned int)) {
            perror("Read failed");
            close(fin);
            close(fout);
            return 1;
        }
    }
    unsigned char alignment_bits = 0;
    if (read(fin, &alignment_bits, sizeof(unsigned char)) != sizeof(unsigned char)) {
        perror("Read failed");
        close(fin);
        close(fout);
        return 1;
    }
    std::list<Node *> nodes(chars_cnt);
    auto iter = nodes.begin();
    for (int i = 0; i < 256; ++i) {
        if (chars[i] > 0) {
            *iter = new Node;
            (*iter)->c = (char) i;
            (*iter)->weight = chars[i];
            (*iter)->left = nullptr;
            (*iter)->right = nullptr;
            iter++;
        }
    }
    nodes.sort([](const Node *a, const Node *b) {
        return a->weight > b->weight;
    });
    buildHuffmanTree(nodes);
    std::vector<unsigned char> codes[256];
    for (auto &code : codes) {
        code.resize(64);
    }
    std::vector<unsigned int> code_lens(256);
    getCodes(*nodes.begin(), codes, code_lens, 0, 0);

    Node *cur = *nodes.begin();
    long tmp = file_size - sizeof(unsigned short) - (sizeof(unsigned int) + sizeof(unsigned char)) * chars_cnt
        - sizeof(unsigned char) - 1;
    if (tmp > 0) {
        if (tmp < BUFSIZE) {
            unsigned char read_buf[BUFSIZE];
            unsigned char buf[BUFSIZE * CHAR_BIT];
            if (read(fin, read_buf, tmp) != tmp) {
                perror("Read failed");
                freeHuffmanTree(*nodes.begin());
                close(fin);
                close(fout);
                return 1;
            }
            unsigned int idx = 0;
            for (long i = 0; i < tmp; ++i) {
                for (int j = 0; j < CHAR_BIT; ++j) {
                    if (cur->left == nullptr) {
                        buf[idx] = cur->c;
                        idx++;
                        cur = *nodes.begin();
                    }
                    if ((read_buf[i] >> (7 - j)) & 1) {
                        cur = cur->right;
                    } else {
                        cur = cur->left;
                    }
                }
            }
            unsigned char last_byte = 0;
            if (read(fin, &last_byte, sizeof(unsigned char)) != sizeof(unsigned char)) {
                perror("Read failed");
                freeHuffmanTree(*nodes.begin());
                close(fin);
                close(fout);
                return 1;
            }
            int limit = 8 - alignment_bits;
            for (int i = 0; i < limit; ++i) {
                if (cur->left == nullptr) {
                    buf[idx] = cur->c;
                    idx++;
                    cur = *nodes.begin();
                }
                if ((last_byte >> (7 - i)) & 1) {
                    cur = cur->right;
                } else {
                    cur = cur->left;
                }
            }
            if (cur->left == nullptr) {
                buf[idx] = cur->c;
                idx++;
            }
            if (write(fout, buf, idx) != idx) {
                perror("Write failed");
                freeHuffmanTree(*nodes.begin());
                close(fin);
                close(fout);
                return 1;
            }
        } else {
            unsigned long iters = tmp / (BUFSIZE); // number of full bufs
            unsigned int idx = 0;
            for (unsigned long i = 0; i < iters; i++) {
                unsigned char read_buf[BUFSIZE];
                unsigned char buf[BUFSIZE * CHAR_BIT];

                if (read(fin, read_buf, BUFSIZE) != BUFSIZE) {
                    perror("Read failed");
                    freeHuffmanTree(*nodes.begin());
                    close(fin);
                    close(fout);
                    return 1;
                }
                for (unsigned char j : read_buf) {
                    for (int k = 0; k < CHAR_BIT; ++k) {
                        if (cur->left == nullptr) {
                            buf[idx] = cur->c;
                            idx++;
                            cur = *nodes.begin();
                        }
                        if ((j >> (7 - k)) & 1) {
                            cur = cur->right;
                        } else {
                            cur = cur->left;
                        }
                    }
                }
                if (write(fout, buf, idx) != idx) {
                    perror("Write failed");
                    freeHuffmanTree(*nodes.begin());
                    close(fin);
                    close(fout);
                    return 1;
                }
                idx = 0;
            }
            // last NOT full buffer
            unsigned char read_buf[BUFSIZE];
            unsigned char buf[BUFSIZE * CHAR_BIT];
            long last_size = tmp - iters * BUFSIZE; // except last byte
            if (read(fin, read_buf, last_size + 1) != last_size + 1) {
                perror("Read failed");
                freeHuffmanTree(*nodes.begin());
                close(fin);
                close(fout);
                return 1;
            }
            idx = 0;
            for (long i = 0; i < last_size; ++i) {
                for (int j = 0; j < CHAR_BIT; ++j) {
                    if (cur->left == nullptr) {
                        buf[idx] = cur->c;
                        idx++;
                        cur = *nodes.begin();
                    }
                    if ((read_buf[i] >> (7 - j)) & 1) {
                        cur = cur->right;
                    } else {
                        cur = cur->left;
                    }
                }
            }
            int limit = 8 - alignment_bits;
            for (int i = 0; i < limit; ++i) {
                if (cur->left == nullptr) {
                    buf[idx] = cur->c;
                    idx++;
                    cur = *nodes.begin();
                }
                if ((read_buf[last_size] >> (7 - i)) & 1) {
                    cur = cur->right;
                } else {
                    cur = cur->left;
                }
            }
            if (cur->left == nullptr) {
                buf[idx] = cur->c;
                idx++;
            }
            if (write(fout, buf, idx) != idx) {
                perror("Write failed");
                freeHuffmanTree(*nodes.begin());
                close(fin);
                close(fout);
                return 1;
            }
        }
    }

    freeHuffmanTree(*nodes.begin());
    close(fout);
    return 0;
}