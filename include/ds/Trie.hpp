#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

class Trie {
public:
    struct TrieNode {
        std::unordered_map<char, TrieNode*> children;
        bool isEndOfWord;
        int frequency;
        std::string originalWord;

        TrieNode() : isEndOfWord(false), frequency(0) {}

        ~TrieNode() {
            for (auto& pair : children) {
                delete pair.second;
            }
        }
    };

private:
    TrieNode* root;
    size_t wordCount;

    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    void collectWords(TrieNode* node, std::string currentPrefix,
                      std::vector<std::pair<std::string, int>>& results) const {
        if (!node) return;

        if (node->isEndOfWord) {
            results.push_back({node->originalWord.empty() ? currentPrefix : node->originalWord, node->frequency});
        }

        for (const auto& pair : node->children) {
            collectWords(pair.second, currentPrefix + pair.first, results);
        }
    }

public:
    Trie() : wordCount(0) {
        root = new TrieNode();
    }

    ~Trie() {
        delete root;
    }

    void insert(const std::string& word) {
        if (word.empty()) return;

        std::string lower = toLower(word);
        TrieNode* curr = root;

        for (char c : lower) {
            if (curr->children.find(c) == curr->children.end()) {
                curr->children[c] = new TrieNode();
            }
            curr = curr->children[c];
        }

        if (!curr->isEndOfWord) {
            curr->isEndOfWord = true;
            curr->originalWord = word;
            wordCount++;
        }
        curr->frequency++;
    }

    bool search(const std::string& word) const {
        std::string lower = toLower(word);
        TrieNode* curr = root;

        for (char c : lower) {
            if (curr->children.find(c) == curr->children.end()) {
                return false;
            }
            curr = curr->children.at(c);
        }

        return curr && curr->isEndOfWord;
    }

    bool startsWith(const std::string& prefix) const {
        std::string lower = toLower(prefix);
        TrieNode* curr = root;

        for (char c : lower) {
            if (curr->children.find(c) == curr->children.end()) {
                return false;
            }
            curr = curr->children.at(c);
        }

        return curr != nullptr;
    }

    std::vector<std::string> getSuggestions(const std::string& prefix, size_t maxLimit = 10) const {
        std::string lower = toLower(prefix);
        TrieNode* curr = root;

        for (char c : lower) {
            if (curr->children.find(c) == curr->children.end()) {
                return {};
            }
            curr = curr->children.at(c);
        }

        std::vector<std::pair<std::string, int>> wordFreqPairs;
        collectWords(curr, lower, wordFreqPairs);

        // Sort by frequency descending
        std::sort(wordFreqPairs.begin(), wordFreqPairs.end(),
                  [](const auto& a, const auto& b) {
                      if (a.second != b.second) return a.second > b.second;
                      return a.first < b.first;
                  });

        std::vector<std::string> suggestions;
        for (size_t i = 0; i < std::min(maxLimit, wordFreqPairs.size()); ++i) {
            suggestions.push_back(wordFreqPairs[i].first);
        }

        return suggestions;
    }

    size_t size() const {
        return wordCount;
    }

    void clear() {
        delete root;
        root = new TrieNode();
        wordCount = 0;
    }
};

#endif // TRIE_HPP
