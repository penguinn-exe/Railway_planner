#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <algorithm>

#include "../include/ds/AVLTree.hpp"
#include "../include/ds/HashTable.hpp"
#include "../include/ds/MinHeap.hpp"
#include "../include/ds/Trie.hpp"
#include "../include/algo/KMP.hpp"
#include "../include/algo/Quicksort.hpp"
#include "../include/core/Graph.hpp"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "❌ TEST FAILED: " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false; \
        } \
    } while (0)

bool testAVLTree() {
    std::cout << "[Test] AVL Tree Operations... ";
    AVLTree<int, std::string> avl;

    // Insert elements
    avl.insert(10, "Ten");
    avl.insert(20, "Twenty");
    avl.insert(30, "Thirty"); // Triggers Left Rotation
    avl.insert(40, "Forty");
    avl.insert(50, "Fifty");  // Triggers Left Rotation
    avl.insert(25, "TwentyFive"); // Triggers Right-Left Rotation

    TEST_ASSERT(avl.size() == 6, "Size should be 6");
    TEST_ASSERT(avl.height() <= 3, "Height of balanced AVL with 6 nodes must be <= 3");
    TEST_ASSERT(avl.contains(25), "Should contain 25");
    TEST_ASSERT(*avl.find(25) == "TwentyFive", "Value of 25 must match");

    // Range search
    auto range = avl.rangeSearch(20, 40);
    TEST_ASSERT(range.size() == 4, "Range 20..40 should contain 4 elements");

    // Deletions: leaf, 1-child, 2-children nodes
    bool rem = avl.remove(30);
    TEST_ASSERT(rem, "Removal of 30 should succeed");
    TEST_ASSERT(!avl.contains(30), "Should no longer contain 30");
    TEST_ASSERT(avl.size() == 5, "Size should be 5");

    // Clear and test empty
    avl.clear();
    TEST_ASSERT(avl.empty(), "Tree should be empty after clear()");
    TEST_ASSERT(avl.size() == 0, "Size should be 0 after clear()");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testHashTable() {
    std::cout << "[Test] Hash Table Operations & Collision Handling... ";
    HashTable<std::string, int> ht(4, 0.75f);

    ht.put("CSMT", 1);
    ht.put("NDLS", 2);
    ht.put("HWH", 3);
    ht.put("SBC", 4);
    ht.put("MAS", 5); // Should trigger rehash

    TEST_ASSERT(ht.size() == 5, "Size should be 5");
    TEST_ASSERT(ht.contains("NDLS"), "Should contain NDLS");
    TEST_ASSERT(*ht.get("NDLS") == 2, "NDLS value should be 2");
    TEST_ASSERT(ht.bucketCount() > 4, "Bucket count should have grown after rehash");

    // Mass insertion test (100 elements to stress test collision & rehash)
    for (int i = 0; i < 100; ++i) {
        ht.put("PNR" + std::to_string(i), i * 100);
    }
    TEST_ASSERT(ht.size() == 105, "Size should be 105 after mass insertion");
    TEST_ASSERT(*ht.get("PNR42") == 4200, "PNR42 value should be 4200");

    bool rem = ht.remove("HWH");
    TEST_ASSERT(rem, "Removal of HWH should succeed");
    TEST_ASSERT(!ht.contains("HWH"), "Should no longer contain HWH");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testMinHeap() {
    std::cout << "[Test] MinHeap Operations... ";
    MinHeap<int> heap;

    heap.push(50);
    heap.push(10);
    heap.push(30);
    heap.push(5);
    heap.push(20);

    TEST_ASSERT(heap.size() == 5, "Size should be 5");
    TEST_ASSERT(heap.top() == 5, "Min element should be 5");

    TEST_ASSERT(heap.pop() == 5, "Pop should return 5");
    TEST_ASSERT(heap.pop() == 10, "Pop should return 10");
    TEST_ASSERT(heap.pop() == 20, "Pop should return 20");
    TEST_ASSERT(heap.pop() == 30, "Pop should return 30");
    TEST_ASSERT(heap.pop() == 50, "Pop should return 50");
    TEST_ASSERT(heap.empty(), "Heap should be empty");

    // Build heap static method test
    std::vector<int> nums = {45, 12, 89, 3, 21};
    MinHeap<int> builtHeap = MinHeap<int>::buildHeap(nums);
    TEST_ASSERT(builtHeap.top() == 3, "Build heap min element must be 3");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testTrie() {
    std::cout << "[Test] Trie Auto-Complete... ";
    Trie trie;

    trie.insert("cleanliness");
    trie.insert("clean");
    trie.insert("comfort");
    trie.insert("catering");
    trie.insert("crowded");

    TEST_ASSERT(trie.search("clean"), "Search for 'clean' should be true");
    TEST_ASSERT(trie.startsWith("clea"), "startsWith 'clea' should be true");

    auto sugg = trie.getSuggestions("CLE"); // Case-insensitive test
    TEST_ASSERT(sugg.size() == 2, "Suggestions for 'CLE' should return 2 words");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testKMP() {
    std::cout << "[Test] KMP String Matching Edge Cases... ";
    std::string text = "The railway station was clean, very clean and comfortable.";
    std::string pattern = "clean";

    auto matches = KMP::search(text, pattern);
    TEST_ASSERT(matches.size() == 2, "Pattern 'clean' should match twice");
    TEST_ASSERT(matches[0] == 24, "First match at index 24");
    TEST_ASSERT(matches[1] == 36, "Second match at index 36");

    TEST_ASSERT(KMP::contains(text, "COMFORTABLE"), "Case insensitive matching should work");

    // Edge case: Non-matching pattern
    auto noMatches = KMP::search(text, "nonexistent");
    TEST_ASSERT(noMatches.empty(), "Non-existent pattern should return empty vector");

    // Edge case: Pattern longer than text
    auto longMatches = KMP::search("short", "verylongpattern");
    TEST_ASSERT(longMatches.empty(), "Long pattern should return empty vector");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testQuicksort() {
    std::cout << "[Test] Quicksort Algorithm Edge Cases... ";
    std::vector<int> nums = {42, 12, 88, 3, 27, 95, 1, 64};
    Quicksort::sort(nums);

    TEST_ASSERT(std::is_sorted(nums.begin(), nums.end()), "Vector must be sorted");
    TEST_ASSERT(nums[0] == 1 && nums.back() == 95, "Min and max bounds correct");

    // Test with custom comparator (descending)
    Quicksort::sort(nums, std::greater<int>());
    TEST_ASSERT(nums[0] == 95 && nums.back() == 1, "Descending sort correct");

    // Edge case: Already sorted vector
    Quicksort::sort(nums, std::less<int>());
    TEST_ASSERT(std::is_sorted(nums.begin(), nums.end()), "Already sorted vector stays sorted");

    // Edge case: Single element and duplicate elements
    std::vector<int> duplicates = {5, 5, 5, 1, 2, 5, 2, 1};
    Quicksort::sort(duplicates);
    TEST_ASSERT(std::is_sorted(duplicates.begin(), duplicates.end()), "Duplicates vector must be sorted");

    std::cout << "PASSED ✓\n";
    return true;
}

bool testGraphDijkstra() {
    std::cout << "[Test] Graph Dijkstra Route Finder... ";
    Graph g;

    // CSMT -> BRC (392km) -> NDLS (992km)
    RouteEdge e1{"CSMT", "BRC", 12951, "Rajdhani", 392.0, 240, 650.0, 1010, 1250};
    RouteEdge e2{"BRC", "NDLS", 12951, "Rajdhani", 992.0, 600, 1550.0, 1260, 510};
    RouteEdge e3{"CSMT", "PUNE", 11077, "Jhelum", 192.0, 180, 250.0, 1040, 1220};

    g.addEdge(e1);
    g.addEdge(e2);
    g.addEdge(e3);

    auto itinerary = g.findShortestPath("CSMT", "NDLS", WeightCriterion::DISTANCE);
    TEST_ASSERT(!itinerary.segments.empty(), "Path should exist");
    TEST_ASSERT(itinerary.totalDistanceKm == 1384.0, "Total distance should be 1384 km");

    // Unreachable destination test
    auto noPath = g.findShortestPath("CSMT", "UNREACHABLE_CODE");
    TEST_ASSERT(noPath.segments.empty(), "Unreachable station should return empty itinerary");

    std::cout << "PASSED ✓\n";
    return true;
}

int main() {
    std::cout << "===================================================\n";
    std::cout << "   RUNNING RAILWAY PLANNER DSA UNIT TEST SUITE    \n";
    std::cout << "===================================================\n";

    bool allPassed = true;
    allPassed &= testAVLTree();
    allPassed &= testHashTable();
    allPassed &= testMinHeap();
    allPassed &= testTrie();
    allPassed &= testKMP();
    allPassed &= testQuicksort();
    allPassed &= testGraphDijkstra();

    std::cout << "===================================================\n";
    if (allPassed) {
        std::cout << "🎉 ALL UNIT TESTS PASSED SUCCESSFULLY!\n";
        return 0;
    } else {
        std::cout << "❌ SOME UNIT TESTS FAILED!\n";
        return 1;
    }
}
