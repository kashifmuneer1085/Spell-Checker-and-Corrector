# SpellCheckerPro 🔤

> A DSA-focused Spell Checker & Auto-Corrector built in C++17
> **Data Structures:** Binary Search Tree · Trie · Dynamic Programming (Edit Distance)

---

## Overview

SpellCheckerPro is a command-line spell-checker and auto-corrector that demonstrates core Data Structures and Algorithms concepts:

| Component | Data Structure | Purpose |
|-----------|---------------|---------|
| Dictionary Lookup | **Binary Search Tree (BST)** | O(log n) word existence check |
| Suggestion Generation | **Trie (Prefix Tree)** | O(k) prefix-based candidate retrieval |
| Suggestion Ranking | **Wagner-Fischer DP** | Edit distance scoring between words |
| Sorted Word List | BST In-Order Traversal | Alphabetical fallback for edit-distance scan |

---

## Features

- ✅ **Spell Check** — O(log n) lookup via BST
- 💡 **Smart Suggestions** — Trie prefix search + edit distance ranking
- ✨ **Auto-Correct** — Fixes every misspelled word in a sentence
- 📄 **File Checking** — Full report with line numbers and accuracy %
- ➕ **Dynamic Dictionary** — Add words at runtime (BST + Trie updated together)
- 🎨 **Coloured CLI** — ANSI terminal colours for clear output
- 📦 **No external libraries** — Pure C++17, single header

---

## Project Structure

```
SpellCheckerPro/
├── src/
│   ├── spell_checker.h   ← All DSA (BST, Trie, Edit Distance)
│   └── main.cpp          ← CLI interface and REPL
├── Makefile
├── index.html            ← Interactive web demo + startup guide
├── dictionary.txt        ← (optional) your word list, one per line
└── README.md
```

---

## Quick Start

### 1. Prerequisites

```bash
g++ --version    # Requires C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
```

| Platform | Install |
|----------|---------|
| Ubuntu / Debian | `sudo apt install g++ build-essential` |
| macOS | `xcode-select --install` |
| Windows | Install [MinGW-w64](https://winlibs.com/) or use Visual Studio |

### 2. Compile

```bash
# Using Makefile (Linux / macOS)
make

# Manual compile (all platforms)
g++ -std=c++17 -O2 -o spellchecker src/main.cpp
```

### 3. Run

```bash
# Built-in word list (works out of the box)
./spellchecker

# With a dictionary file
./spellchecker dictionary.txt

# Directly check a text file and exit
./spellchecker dictionary.txt myessay.txt
```

---

## Commands

Once running, you'll see the interactive prompt `>`:

| Command | Example | What it does |
|---------|---------|--------------|
| `check <word>` | `check speling` | BST lookup — correct or not |
| `suggest <word>` | `suggest recieve` | Top 5 suggestions ranked by edit distance |
| `correct <sentence>` | `correct Ths is wrng` | Auto-corrects every word in the sentence |
| `scan <sentence>` | `scan She sed helllo` | Lists all misspelled words + suggestions |
| `file <path>` | `file essay.txt` | Full spell-check report with accuracy % |
| `add <word>` | `add Lahore` | Adds a word to BST + Trie at runtime |
| `info` | `info` | Shows dictionary size and DS info |
| `demo` | `demo` | Runs built-in demonstration examples |
| `help` | `help` | Shows the command list |
| `exit` | `exit` | Quit the program |

---

## DSA Details

### Binary Search Tree (BST)

All dictionary words are stored in a BST. Each node holds one `std::string`.

```
Lookup path for "speling":
  root("the") → right("world") → left("spell") → right("speling") → NOT FOUND
```

- **Search:** O(log n) average, O(n) worst case
- **Insert:** O(log n) average
- **In-order traversal:** O(n) → alphabetically sorted output

### Trie

A 26-ary prefix tree built in parallel with the BST. When suggestions are needed, the Trie instantly returns all words sharing the first 1–3 characters.

```
Trie path for prefix "rec":
  root → r → e → c → [eive, ommend, ognize, ent ...]
```

- **Insert:** O(k) where k = length of word
- **Prefix search:** O(k + number of results)

### Edit Distance (Wagner-Fischer DP)

For ranking suggestions, the minimum edit distance between the misspelled word and each candidate is computed:

```
editDistance("speling", "spelling")
  Ops: insert 'l' → distance = 1  ✓ top suggestion
```

- **Time:** O(m × n) per pair
- **Threshold:** candidates with distance > 3 are discarded

### Suggestion Pipeline

```
Input: "recieve"
  1. BST lookup          → NOT FOUND
  2. Trie prefix("rec")  → [receive, recommend, recognise ...]
  3. Edit distance scan  → scored and filtered
  4. Sort by score       → [receive(1), recede(3), rewrite(4) ...]
  5. Return top 5        → suggestions shown to user
```

---

## Dictionary File Format

Any plain text file with words separated by spaces or newlines works:

```
the quick brown fox jumps over the lazy dog
hello world algorithm binary search tree...
```

Common sources:
- `/usr/share/dict/words` (Linux)
- [SCOWL Word Lists](http://wordlist.aspell.net/)
- [English word frequency lists](https://github.com/first20hours/google-10000-english)

---

## Example Session

```
  > check hello
  ✓ "hello" is spelled correctly.

  > check helllo
  ✗ "helllo" is NOT in the dictionary.

  > suggest helllo
  Suggestions for "helllo":
    1. hello
    2. hell
    3. hells

  > correct Ths is a smple sentance
  Input  : "Ths is a smple sentance"
  Output : "the is a simple sentence"

  > scan She sed helllo to her frend
  Misspelled words (3):
    sed     →  see / set / said
    helllo  →  hello
    frend   →  friend

  > file essay.txt
  Line 3  errros → errors / erros
  Line 7  recieve → receive
  ─────────────────────────────────
  Total words: 240   Errors: 2   Accuracy: 99.17%
```

---

## Complexity Summary

| Operation | Best | Average | Worst |
|-----------|------|---------|-------|
| BST Search | O(log n) | O(log n) | O(n) |
| BST Insert | O(log n) | O(log n) | O(n) |
| Trie Insert | O(k) | O(k) | O(k) |
| Trie Prefix | O(k) | O(k + r) | O(k + r) |
| Edit Distance | O(mn) | O(mn) | O(mn) |
| Suggest (full) | O(log n) | O(k + c·mn) | O(n·mn) |

*n = dictionary size, k = word length, r = results, c = candidates, m/n = word lengths*

---

## Extending the Project

Ideas for further development:

- **AVL / Red-Black BST** — Self-balancing to guarantee O(log n) worst case
- **Bloom Filter** — O(1) probabilistic pre-check before BST lookup
- **N-gram Language Model** — Context-aware correction ("their" vs "there")
- **Frequency Ranking** — Weight suggestions by word frequency
- **GUI** — Qt or web-based front-end (see `index.html`)

---

## Author

**[Your Name]**  
Course: Data Structures & Algorithms  
Department of Computer Science  

---

## License

MIT License — free to use, modify, and distribute for educational purposes.
