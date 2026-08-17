#ifndef KMP_HPP
#define KMP_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

class KMP {
private:
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

public:
    // Compute Longest Prefix Suffix (LPS) array
    static std::vector<int> buildLPSArray(const std::string& pattern) {
        int m = static_cast<int>(pattern.length());
        std::vector<int> lps(m, 0);
        int len = 0;
        int i = 1;

        while (i < m) {
            if (pattern[i] == pattern[len]) {
                len++;
                lps[i] = len;
                i++;
            } else {
                if (len != 0) {
                    len = lps[len - 1];
                } else {
                    lps[i] = 0;
                    i++;
                }
            }
        }
        return lps;
    }

    // Find all starting indices of pattern in text (Case-Sensitive or Insensitive)
    static std::vector<int> search(const std::string& text, const std::string& pattern, bool caseSensitive = false) {
        if (pattern.empty() || text.empty()) return {};

        std::string txt = caseSensitive ? text : toLower(text);
        std::string pat = caseSensitive ? pattern : toLower(pattern);

        int n = static_cast<int>(txt.length());
        int m = static_cast<int>(pat.length());

        if (m > n) return {};

        std::vector<int> lps = buildLPSArray(pat);
        std::vector<int> matchIndices;

        int i = 0; // index for txt
        int j = 0; // index for pat

        while (i < n) {
            if (pat[j] == txt[i]) {
                i++;
                j++;
            }

            if (j == m) {
                matchIndices.push_back(i - j);
                j = lps[j - 1];
            } else if (i < n && pat[j] != txt[i]) {
                if (j != 0) {
                    j = lps[j - 1];
                } else {
                    i++;
                }
            }
        }

        return matchIndices;
    }

    static bool contains(const std::string& text, const std::string& pattern, bool caseSensitive = false) {
        return !search(text, pattern, caseSensitive).empty();
    }

    static int countOccurrences(const std::string& text, const std::string& pattern, bool caseSensitive = false) {
        return static_cast<int>(search(text, pattern, caseSensitive).size());
    }
};

#endif // KMP_HPP
