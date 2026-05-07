/*
 * ╔═══════════════════════════════════════════════════════════════════╗
 * ║          SpellCheckerPro — main.cpp                               ║
 * ║                                                                   ║
 * ║  A DSA-focused spell-checker & corrector using:                   ║
 * ║    ► Binary Search Tree (BST) — O(log n) dictionary lookup        ║
 * ║    ► Trie                    — O(k) prefix autocomplete           ║
 * ║    ► Edit Distance (DP)      — suggestion ranking                 ║
 * ║                                                                   ║
 * ║  Author : Kashif Muneer  |  Course : Data Structures & Algorithms   ║
 * ╚═══════════════════════════════════════════════════════════════════╝
 *
 *  Compile:  g++ -std=c++17 -O2 -o spellchecker main.cpp
 *  Run    :  ./spellchecker
 *            ./spellchecker dictionary.txt          (custom dictionary)
 *            ./spellchecker dictionary.txt file.txt (check a file)
 */

#include "spell_checker.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// ─── ANSI colour helpers ──────────────────────────────────────────────────
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[97m"

// ─── Banner ───────────────────────────────────────────────────────────────
void printBanner() {
    std::cout << CYAN << BOLD;
    std::cout << R"(
  ╔══════════════════════════════════════════════════════════════╗
  ║   ____              _ _  ____ _               _             ║
  ║  / ___| _ __   ___| | |/ ___| |__   ___  ___| | _____ _ __ ║
  ║  \___ \| '_ \ / _ \ | | |   | '_ \ / _ \/ __| |/ / _ \ '__| ║
  ║   ___) | |_) |  __/ | | |___| | | |  __/ (__|   <  __/ |   ║
  ║  |____/| .__/ \___|_|_|\____|_| |_|\___|\___|_|\_\___|_|   ║
  ║        |_|                                P  R  O           ║
  ╠══════════════════════════════════════════════════════════════╣
  ║  DSA Project  │  BST + Trie + Edit Distance DP              ║
  ╚══════════════════════════════════════════════════════════════╝
)" << RESET << "\n";
}

// ─── Help Menu ────────────────────────────────────────────────────────────
void printHelp() {
    std::cout << YELLOW << BOLD << "\n  Commands:\n" << RESET;
    std::cout << "  " << GREEN << "check   <word>     " << RESET << "  Check if a single word is spelled correctly\n";
    std::cout << "  " << GREEN << "suggest <word>     " << RESET << "  Get spelling suggestions (top 5)\n";
    std::cout << "  " << GREEN << "correct <sentence> " << RESET << "  Auto-correct an entire sentence\n";
    std::cout << "  " << GREEN << "scan    <sentence> " << RESET << "  Find all misspelled words in a sentence\n";
    std::cout << "  " << GREEN << "file    <path>     " << RESET << "  Spell-check a .txt file and show report\n";
    std::cout << "  " << GREEN << "add     <word>     " << RESET << "  Add a word to the dictionary\n";
    std::cout << "  " << GREEN << "info               " << RESET << "  Show dictionary statistics\n";
    std::cout << "  " << GREEN << "demo               " << RESET << "  Run built-in demo examples\n";
    std::cout << "  " << GREEN << "help               " << RESET << "  Show this menu\n";
    std::cout << "  " << GREEN << "exit               " << RESET << "  Quit\n\n";
}

// ─── Demo mode ────────────────────────────────────────────────────────────
void runDemo(const SpellChecker& sc) {
    std::cout << MAGENTA << BOLD << "\n  ── DEMO MODE ──\n" << RESET;

    auto demo_check = [&](const std::string& w) {
        std::cout << "  check(\"" << w << "\")  →  ";
        if (sc.isCorrect(w))
            std::cout << GREEN << "✓ Correct" << RESET << "\n";
        else
            std::cout << RED << "✗ Misspelled" << RESET << "\n";
    };

    auto demo_suggest = [&](const std::string& w) {
        std::cout << "  suggest(\"" << w << "\")  →  ";
        auto sug = sc.suggest(w);
        if (sug.empty()) { std::cout << "(no suggestions)\n"; return; }
        for (int i = 0; i < (int)sug.size(); ++i)
            std::cout << (i ? ", " : "") << CYAN << sug[i] << RESET;
        std::cout << "\n";
    };

    demo_check("hello");    demo_check("helllo");
    demo_check("world");    demo_check("wrodl");
    demo_check("algorithm");demo_check("algorythm");
    std::cout << "\n";
    demo_suggest("speling");
    demo_suggest("recieve");
    demo_suggest("algoritm");
    std::cout << "\n";
    std::string sent = "Ths is a smple sentance with errros.";
    std::cout << "  Input   : \"" << sent << "\"\n";
    std::cout << "  Output  : \"" << CYAN << sc.autocorrect(sent) << RESET << "\"\n\n";
}

// ─── File checker ─────────────────────────────────────────────────────────
void checkFile(const SpellChecker& sc, const std::string& path) {
    std::ifstream fin(path);
    if (!fin.is_open()) {
        std::cout << RED << "  Error: cannot open file \"" << path << "\"\n" << RESET;
        return;
    }
    int lineNo = 0, totalWords = 0, errorWords = 0;
    std::string line;
    std::cout << YELLOW << BOLD << "\n  Spell-check Report: " << path << "\n" << RESET;
    std::cout << "  " << std::string(58, '-') << "\n";
    while (std::getline(fin, line)) {
        ++lineNo;
        auto wrong = sc.checkSentence(line);
        for (const auto& w : wrong) {
            ++errorWords;
            auto sug = sc.suggest(w, 3);
            std::cout << "  Line " << lineNo << "  " << RED << w << RESET
                      << "  →  ";
            if (sug.empty()) std::cout << "(no suggestions)";
            else for (int i = 0; i < (int)sug.size(); ++i)
                std::cout << (i ? " / " : "") << GREEN << sug[i] << RESET;
            std::cout << "\n";
        }
        // count total words
        std::istringstream iss(line);
        std::string tok;
        while (iss >> tok) {
            std::string c;
            for (char ch : tok) if (std::isalpha((unsigned char)ch)) c += ch;
            if (!c.empty()) ++totalWords;
        }
    }
    std::cout << "  " << std::string(58, '-') << "\n";
    std::cout << "  Total words: " << totalWords
              << "   Errors: " << RED << errorWords << RESET
              << "   Accuracy: " << GREEN
              << (totalWords > 0 ? (100.0*(totalWords-errorWords)/totalWords) : 100.0)
              << "%" << RESET << "\n\n";
}

// ─── Interactive REPL ─────────────────────────────────────────────────────
void repl(SpellChecker& sc) {
    printHelp();
    std::string line;
    while (true) {
        std::cout << BOLD << CYAN << "  > " << RESET;
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        std::string rest;
        std::getline(iss, rest);
        if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);

        if (cmd == "exit" || cmd == "quit") {
            std::cout << CYAN << "\n  Goodbye! Happy writing.\n\n" << RESET;
            break;
        }
        else if (cmd == "help") {
            printHelp();
        }
        else if (cmd == "info") {
            std::cout << "  Dictionary size : " << GREEN << sc.dictionarySize()
                      << " words" << RESET << "\n  Data Structures : BST (lookup) + Trie (suggestions)\n\n";
        }
        else if (cmd == "demo") {
            runDemo(sc);
        }
        else if (cmd == "check") {
            if (rest.empty()) { std::cout << RED << "  Usage: check <word>\n" << RESET; continue; }
            std::string w = toLower(rest);
            if (sc.isCorrect(w))
                std::cout << "  " << GREEN << "✓ \"" << w << "\" is spelled correctly.\n" << RESET;
            else
                std::cout << "  " << RED << "✗ \"" << w << "\" is NOT in the dictionary.\n" << RESET;
            std::cout << "\n";
        }
        else if (cmd == "suggest") {
            if (rest.empty()) { std::cout << RED << "  Usage: suggest <word>\n" << RESET; continue; }
            auto sug = sc.suggest(toLower(rest));
            std::cout << "  Suggestions for \"" << rest << "\":\n";
            if (sug.empty()) std::cout << "    (none found)\n";
            else for (int i = 0; i < (int)sug.size(); ++i)
                std::cout << "    " << (i+1) << ". " << CYAN << sug[i] << RESET << "\n";
            std::cout << "\n";
        }
        else if (cmd == "correct") {
            if (rest.empty()) { std::cout << RED << "  Usage: correct <sentence>\n" << RESET; continue; }
            std::cout << "  Input  : \"" << rest << "\"\n";
            std::cout << "  Output : \"" << GREEN << sc.autocorrect(rest) << RESET << "\"\n\n";
        }
        else if (cmd == "scan") {
            if (rest.empty()) { std::cout << RED << "  Usage: scan <sentence>\n" << RESET; continue; }
            auto wrong = sc.checkSentence(rest);
            if (wrong.empty())
                std::cout << "  " << GREEN << "✓ No spelling errors found.\n" << RESET;
            else {
                std::cout << "  Misspelled words (" << wrong.size() << "):\n";
                for (const auto& w : wrong) {
                    auto sug = sc.suggest(w, 3);
                    std::cout << "    " << RED << w << RESET << "  →  ";
                    for (int i = 0; i < (int)sug.size(); ++i)
                        std::cout << (i ? " / " : "") << GREEN << sug[i] << RESET;
                    std::cout << "\n";
                }
            }
            std::cout << "\n";
        }
        else if (cmd == "file") {
            if (rest.empty()) { std::cout << RED << "  Usage: file <path>\n" << RESET; continue; }
            checkFile(sc, rest);
        }
        else if (cmd == "add") {
            if (rest.empty()) { std::cout << RED << "  Usage: add <word>\n" << RESET; continue; }
            sc.addWord(rest);
            std::cout << "  " << GREEN << "\"" << toLower(rest) << "\" added to dictionary.\n\n" << RESET;
        }
        else {
            std::cout << RED << "  Unknown command. Type 'help' for the list.\n\n" << RESET;
        }
    }
}

// ─── Built-in fallback word list (used when no dict file provided) ─────────
static const char* BUILTIN_WORDS[] = {
    "a","about","above","after","again","against","ago","all","also","always",
    "am","an","and","any","are","around","as","at","back","be","because","been",
    "before","being","below","between","both","but","by","came","can","come",
    "could","day","did","different","do","does","done","down","each","end","even",
    "every","example","few","find","first","for","form","found","from","gave","get",
    "give","go","good","got","great","had","has","have","he","help","her","here",
    "him","his","home","how","i","if","important","in","is","it","its","just",
    "keep","know","large","last","later","learn","left","let","like","line","long",
    "look","made","make","man","many","may","me","might","more","most","move",
    "much","my","name","need","never","new","next","no","not","now","number",
    "of","off","often","old","on","once","one","only","open","or","other","our",
    "out","over","own","part","people","place","play","point","put","read","right",
    "said","same","see","seem","set","she","should","show","since","small","so",
    "some","something","soon","still","such","take","tell","than","that","the",
    "their","them","then","there","these","they","thing","think","this","those",
    "though","thought","through","time","to","together","too","try","turn","two",
    "under","until","up","us","use","very","want","was","way","we","well","went",
    "were","what","when","where","which","while","who","why","will","with","word",
    "work","world","would","year","yet","you","your","algorithm","data","structure",
    "binary","tree","search","insert","delete","sort","array","linked","list",
    "queue","stack","hash","table","graph","node","edge","path","depth","height",
    "balance","leaf","root","pointer","memory","compile","code","program","function",
    "variable","loop","string","integer","float","double","class","object","spell",
    "check","correct","dictionary","suggestion","error","letter","word","sentence",
    "hello","world","computer","science","language","english","write","read",
    nullptr
};

// ─── main ─────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    printBanner();

    SpellChecker sc;

    // Load dictionary
    bool loaded = false;
    if (argc >= 2) {
        std::cout << "  Loading dictionary: " << argv[1] << " ...\n";
        loaded = sc.loadDictionary(argv[1]);
        if (!loaded)
            std::cout << RED << "  Warning: could not open " << argv[1]
                      << ". Using built-in word list.\n" << RESET;
    }

    if (!loaded) {
        std::cout << "  Using built-in word list ...\n";
        for (int i = 0; BUILTIN_WORDS[i]; ++i)
            sc.addWord(BUILTIN_WORDS[i]);
    }

    std::cout << "  " << GREEN << sc.dictionarySize() << " words loaded." << RESET << "\n";

    // Optional: check a file directly from CLI args
    if (argc >= 3) {
        checkFile(sc, argv[2]);
        return 0;
    }

    // Interactive mode
    repl(sc);
    return 0;
}
