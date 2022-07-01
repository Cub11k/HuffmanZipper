#include "huffman_tree.h"

int main(int argc, char *argv[])
{
    bool compress_mode = false;
    bool decompress_mode = false;
    bool test_mode = false;
    char *src = nullptr;
    char *dest = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            std::cout << "Usage: ./huffman [OPTIONS]\n"
                      << "Compress or decompress data using Huffman codes\n"
                      << "Effective on files with small number of different symbols, for example english texts\n\n"
                      << "Mandatory:\n"
                      << "  --src <src_file>     source file to compress data from\n"
                      << "  --dest <dest_file>   destination file for compressed data, will be truncated\n\n"
                      << "Optional:\n"
                      << "  -h, --help           display this help and exit\n"
                      << "  -c, --compress       (default) launch compression, sets the mode\n"
                      << "  -d, --decompress     launch decompression, sets the mode\n"
                      << "  -t, --test           launch test scripts, sets the mode\n\n"
                      << "Exit codes:\n"
                      << "  0  OK\n"
                      << "  1  ERROR\n"
                      << std::endl;
            return 1;
        }
        if (!strcmp(argv[i], "--compress") || !strcmp(argv[i], "-c")) {
            if (decompress_mode) {
                std::cout << "huffman: conflicting options \'--compress\' and \'--decompress\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            if (test_mode) {
                std::cout << "huffman: conflicting options \'--compress\' and \'--test\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            compress_mode = true;
        } else if (!strcmp(argv[i], "--decompress") || !strcmp(argv[i], "-d")) {
            if (compress_mode) {
                std::cout << "huffman: conflicting options \'--compress\' and \'--decompress\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            if (test_mode) {
                std::cout << "huffman: conflicting options \'--decompress\' and \'--test\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            decompress_mode = true;
        } else if (!strcmp(argv[i], "--test") || !strcmp(argv[i], "-t")) {
            if (compress_mode) {
                std::cout << "huffman: conflicting options \'--test\' and \'--compress\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            if (decompress_mode) {
                std::cout << "huffman: conflicting options \'--test\' and \'--decompress\'\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            test_mode = true;
        } else if (!strcmp(argv[i], "--src")) {
            i++;
            if (i == argc) {
                std::cout << "huffman: source file path not specified\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            src = argv[i];
        } else if (!strcmp(argv[i], "--dest")) {
            i++;
            if (i == argc) {
                std::cout << "huffman: destination file path not specified\n"
                          << "Try \'./huffman --help\' for more information." << std::endl;
                return 1;
            }
            dest = argv[i];
        } else {
            std::cout << "huffman: invalid option -- \'" << argv[i] << "\'\n"
                      << "Try \'./huffman --help\' for more information." << std::endl;
            return 1;
        }
    }
    if (!compress_mode && !decompress_mode && !test_mode) {
        compress_mode = true;
    }
    if (compress_mode || decompress_mode) {
        if (!src) {
            std::cout << "huffman: source file path not specified\n"
                      << "Try \'./huffman --help\' for more information." << std::endl;
            return 1;
        }
        if (!dest) {
            std::cout << "huffman: destination file path not specified\n"
                      << "Try \'./huffman --help\' for more information." << std::endl;
            return 1;
        }
    }

    if (compress_mode) {
        return compress(src, dest);
    } else if (decompress_mode) {
        return decompress(src, dest);
    } else {
        return test();
    }
}