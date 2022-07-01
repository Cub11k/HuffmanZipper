# Huffman Zipper

Simple zipper based on Huffman codes

### TO DO
- [ ] Consider using canonical Huffman codes
- [ ] Update Huffman code length limitations
- [ ] Reduce deflated size
  - [ ] Encode Huffman code lengths instead of codes itself
  - [ ] Add RLE encoding for Huffman code lengths

## Installation

```bash
cd <path_you_prefer>
git clone https://github.com/Cub1tor/HuffmanZipper.git
cd HuffmanZipper
```

## Build

```bash
make
# or
make huffman

# You might also want to add this folder to your PATH environmental variable
export PATH="$PATH:<path_you_prefer>/HuffmanZipper/"
```

## Usage
```bash
./huffman --help
# or, if you've added this folder to your PATH environmental variable
huffman --help
```
