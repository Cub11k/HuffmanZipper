#include "huffman_tree.h"

int compress(char *src, char *dest)
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
    long n = lseek(fin, 0, SEEK_END);
    lseek(fin, 0, SEEK_SET);
    std::vector<unsigned int> chars(MAX_CHARS_CNT);
    long blocks_num = n / BUFSIZE + 1;
    unsigned short chars_cnt = 0;
    for (long i = 0; i < blocks_num; ++i) {
        unsigned char c[BUFSIZE];
        ssize_t tmp = 0;
        if ((tmp = read(fin, c, BUFSIZE)) == -1) {
            perror("Read failed");
            close(fin);
            close(fout);
            return 1;
        }
        for (ssize_t j = 0; j < tmp; ++j) {
            if (chars[c[j]] == 0) {
                chars_cnt++;
            }
            chars[c[j]]++;
        }
    }
    lseek(fin, 0, SEEK_SET);
    std::list<Node *> nodes(chars_cnt);
    auto iter = nodes.begin();
    for (int i = 0; i < MAX_CHARS_CNT; ++i) {
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
    std::vector<unsigned char> codes[MAX_CHARS_CNT];
    for (auto &code : codes) {
        code.resize(64);
    }
    std::vector<unsigned int> code_lens(MAX_CHARS_CNT);
    getCodes(*nodes.begin(), codes, code_lens, 0, 0);

    unsigned long bits_num = 0;
    if (write(fout, &chars_cnt, sizeof(unsigned short)) != sizeof(unsigned short)) {
        perror("Write failed");
        freeHuffmanTree(*nodes.begin());
        close(fin);
        close(fout);
        return 1;
    }
    unsigned char buf[256 * (sizeof(unsigned char) + sizeof(unsigned int))] = {0};
    unsigned idx = 0;
    for (int i = 0; i < MAX_CHARS_CNT; ++i) {
        if (chars[i] > 0) {
            buf[idx] = (unsigned char) i;
            idx++;
        }
        bits_num += chars[i] * code_lens[i];
    }
    for (int i = 0; i < MAX_CHARS_CNT; ++i) {
        if (chars[i] > 0) {
            char shift = 0;
            for (unsigned long j = 0; j < sizeof(unsigned int); ++j) {
                buf[idx] = (unsigned char) (chars[i] >> shift);
                idx++;
                shift += 8;
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
    unsigned char alignment_bits = (CHAR_BIT - (unsigned char) bits_num % CHAR_BIT) % CHAR_BIT;
    if (write(fout, &alignment_bits, sizeof(unsigned char)) != sizeof(unsigned char)) {
        perror("Write failed");
        freeHuffmanTree(*nodes.begin());
        close(fin);
        close(fout);
        return 1;
    }
    unsigned char byte = 0;
    char shift = CHAR_BIT - 1;
    unsigned char write_buf[BUFSIZE];
    idx = 0;
    for (long i = 0; i < blocks_num; ++i) {
        unsigned char c[BUFSIZE];
        ssize_t tmp = 0;
        if ((tmp = read(fin, c, BUFSIZE)) == -1) {
            perror("Read failed");
            close(fin);
            close(fout);
            return 1;
        }
        for (ssize_t j = 0; j < tmp; ++j) {
            for (unsigned int k = 0; k < code_lens[c[j]]; ++k) {
                byte |= (codes[c[j]][k] << shift);
                shift--;
                if (shift == -1) {
                    write_buf[idx] = byte;
                    idx++;
                    if (idx == BUFSIZE) {
                        if (write(fout, write_buf, idx) != idx) {
                            perror("Write failed");
                            freeHuffmanTree(*nodes.begin());
                            close(fin);
                            close(fout);
                            return 1;
                        }
                        for (unsigned char & l : write_buf) {
                            l = 0;
                        }
                        idx = 0;
                    }
                    byte = 0;
                    shift = CHAR_BIT - 1;
                }
            }
        }
    }
    if (idx > 0) {
        if (write(fout, write_buf, idx) != idx) {
            perror("Write failed");
            freeHuffmanTree(*nodes.begin());
            close(fin);
            close(fout);
            return 1;
        }
    }
    if (alignment_bits > 0) {
        if (write(fout, &byte, sizeof(unsigned char)) != sizeof(unsigned char)) {
            perror("Write failed");
            freeHuffmanTree(*nodes.begin());
            close(fin);
            close(fout);
            return 1;
        }
    }

    freeHuffmanTree(*nodes.begin());
    close(fin);
    close(fout);
    return 0;
}