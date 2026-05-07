#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H

/*
 * ╔══════════════════════════════════════════════════════╗
 * ║       SpellCheckerPro — DSA Header (spell_checker.h) ║
 * ║  Data Structures Used: BST + Trie                    ║
 * ╚══════════════════════════════════════════════════════╝
 *
 *  BST  → Balanced storage of the dictionary; O(log n) lookup
 *  Trie → Prefix-based autocomplete & suggestion generation
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>

// ─────────────────────────────────────────────
//  UTILITY
// ─────────────────────────────────────────────
inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

inline bool isAlpha(char c) { return std::isalpha((unsigned char)c); }

// ═══════════════════════════════════════════════
//  BST — Binary Search Tree Dictionary
// ═══════════════════════════════════════════════
struct BSTNode {
    std::string word;
    BSTNode* left;
    BSTNode* right;
    int height; // for AVL balancing info (display only)

    BSTNode(const std::string& w)
        : word(w), left(nullptr), right(nullptr), height(1) {}
};

class BST {
public:
    BST() : root(nullptr), size(0) {}
    ~BST() { destroyTree(root); }

    // Insert a word into the BST
    void insert(const std::string& word) {
        root = insertRec(root, word);
        ++size;
    }

    // Return true if word exists in BST (O log n average)
    bool search(const std::string& word) const {
        return searchRec(root, word);
    }

    // In-order traversal → alphabetically sorted words
    void inOrder(std::vector<std::string>& result) const {
        inOrderRec(root, result);
    }

    int getSize() const { return size; }

private:
    BSTNode* root;
    int size;

    BSTNode* insertRec(BSTNode* node, const std::string& word) {
        if (!node) return new BSTNode(word);
        if (word < node->word)
            node->left = insertRec(node->left, word);
        else if (word > node->word)
            node->right = insertRec(node->right, word);
        // duplicate: ignore
        return node;
    }

    bool searchRec(BSTNode* node, const std::string& word) const {
        if (!node) return false;
        if (word == node->word) return true;
        if (word < node->word) return searchRec(node->left, word);
        return searchRec(node->right, word);
    }

    void inOrderRec(BSTNode* node, std::vector<std::string>& result) const {
        if (!node) return;
        inOrderRec(node->left, result);
        result.push_back(node->word);
        inOrderRec(node->right, result);
    }

    void destroyTree(BSTNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
};


// ═══════════════════════════════════════════════
//  TRIE — Prefix Tree for Suggestions
// ═══════════════════════════════════════════════
const int ALPHA = 26;

struct TrieNode {
    TrieNode* children[ALPHA];
    bool isEnd;

    TrieNode() : isEnd(false) {
        for (int i = 0; i < ALPHA; ++i) children[i] = nullptr;
    }
};

class Trie {
public:
    Trie() : root(new TrieNode()) {}
    ~Trie() { destroyTrie(root); }

    void insert(const std::string& word) {
        TrieNode* cur = root;
        for (char c : word) {
            int idx = c - 'a';
            if (idx < 0 || idx >= ALPHA) return; // skip non-alpha
            if (!cur->children[idx])
                cur->children[idx] = new TrieNode();
            cur = cur->children[idx];
        }
        cur->isEnd = true;
    }

    // Collect all words that begin with 'prefix'
    std::vector<std::string> autocomplete(const std::string& prefix, int maxResults = 10) const {
        std::vector<std::string> results;
        TrieNode* cur = root;
        for (char c : prefix) {
            int idx = c - 'a';
            if (idx < 0 || idx >= ALPHA || !cur->children[idx])
                return results; // prefix not found
            cur = cur->children[idx];
        }
        collectWords(cur, prefix, results, maxResults);
        return results;
    }

private:
    TrieNode* root;

    void collectWords(TrieNode* node, const std::string& cur,
                      std::vector<std::string>& results, int maxResults) const {
        if ((int)results.size() >= maxResults) return;
        if (node->isEnd) results.push_back(cur);
        for (int i = 0; i < ALPHA; ++i) {
            if (node->children[i])
                collectWords(node->children[i], cur + (char)('a' + i), results, maxResults);
        }
    }

    void destroyTrie(TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < ALPHA; ++i) destroyTrie(node->children[i]);
        delete node;
    }
};


// ═══════════════════════════════════════════════
//  EDIT DISTANCE (Wagner-Fischer DP)
// ═══════════════════════════════════════════════
inline int editDistance(const std::string& a, const std::string& b) {
    int m = (int)a.size(), n = (int)b.size();
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    for (int i = 1; i <= m; ++i)
        for (int j = 1; j <= n; ++j) {
            if (a[i-1] == b[j-1]) dp[i][j] = dp[i-1][j-1];
            else dp[i][j] = 1 + std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    return dp[m][n];
}


// ═══════════════════════════════════════════════
//  SPELL CHECKER + CORRECTOR  (main engine)
// ═══════════════════════════════════════════════
class SpellChecker {
public:
    SpellChecker() {}

    // Load dictionary from file (one word per line or space-separated)
    bool loadDictionary(const std::string& path) {
        std::ifstream fin(path);
        if (!fin.is_open()) return false;
        std::string word;
        while (fin >> word) {
            word = toLower(word);
            // strip non-alpha characters
            std::string clean;
            for (char c : word) if (isAlpha(c)) clean += c;
            if (clean.empty() || clean.size() > 30) continue;
            if (!bst.search(clean)) {
                bst.insert(clean);
                trie.insert(clean);
                allWords.push_back(clean);
            }
        }
        return true;
    }

    // Add a single word manually
    void addWord(const std::string& raw) {
        std::string w = toLower(raw);
        std::string clean;
        for (char c : w) if (isAlpha(c)) clean += c;
        if (clean.empty() || bst.search(clean)) return;
        bst.insert(clean);
        trie.insert(clean);
        allWords.push_back(clean);
    }

    // Check if word is correctly spelled (BST lookup)
    bool isCorrect(const std::string& word) const {
        return bst.search(toLower(word));
    }

    // Return up to `maxSug` suggestions sorted by edit distance
    std::vector<std::string> suggest(const std::string& rawWord, int maxSug = 5) const {
        std::string word = toLower(rawWord);
        // 1) Trie prefix suggestions (same prefix)
        std::string prefix = word.substr(0, std::min((int)word.size(), 3));
        std::vector<std::string> candidates = trie.autocomplete(prefix, 50);

        // 2) If too few, also scan the full sorted list (BST inorder)
        if ((int)candidates.size() < 20) {
            for (const auto& w : allWords)
                if (editDistance(word, w) <= 3)
                    candidates.push_back(w);
        }

        // Remove duplicates
        std::sort(candidates.begin(), candidates.end());
        candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

        // Score by edit distance
        std::vector<std::pair<int,std::string>> scored;
        for (const auto& c : candidates)
            scored.push_back({editDistance(word, c), c});
        std::sort(scored.begin(), scored.end());

        std::vector<std::string> result;
        for (int i = 0; i < (int)scored.size() && i < maxSug; ++i)
            result.push_back(scored[i].second);
        return result;
    }

    // Spell-check an entire sentence; return list of wrong words
    std::vector<std::string> checkSentence(const std::string& sentence) const {
        std::vector<std::string> wrong;
        std::istringstream iss(sentence);
        std::string token;
        while (iss >> token) {
            std::string clean;
            for (char c : token) if (isAlpha(c)) clean += c;
            if (!clean.empty() && !isCorrect(clean))
                wrong.push_back(clean);
        }
        return wrong;
    }

    // Auto-correct a sentence (replace misspelled words with top suggestion)
    std::string autocorrect(const std::string& sentence) const {
        std::istringstream iss(sentence);
        std::string token, result;
        bool first = true;
        while (iss >> token) {
            std::string clean;
            for (char c : token) if (isAlpha(c)) clean += c;
            std::string corrected = token;
            if (!clean.empty() && !isCorrect(clean)) {
                auto sug = suggest(clean, 1);
                if (!sug.empty()) corrected = sug[0];
            }
            if (!first) result += ' ';
            result += corrected;
            first = false;
        }
        return result;
    }

    int dictionarySize() const { return bst.getSize(); }

private:
    BST  bst;
    Trie trie;
    std::vector<std::string> allWords; // for edit-distance scan
};

#endif // SPELL_CHECKER_H
