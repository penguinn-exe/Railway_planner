#ifndef QUICKSORT_HPP
#define QUICKSORT_HPP

#include <vector>
#include <utility>
#include <functional>
#include <algorithm>

class Quicksort {
private:
    template <typename T, typename Compare>
    static int medianOfThree(std::vector<T>& arr, int low, int high, Compare comp) {
        int mid = low + (high - low) / 2;
        if (comp(arr[mid], arr[low])) std::swap(arr[low], arr[mid]);
        if (comp(arr[high], arr[low])) std::swap(arr[low], arr[high]);
        if (comp(arr[high], arr[mid])) std::swap(arr[mid], arr[high]);
        return mid;
    }

    template <typename T, typename Compare>
    static int partition(std::vector<T>& arr, int low, int high, Compare comp) {
        int pivotIndex = medianOfThree(arr, low, high, comp);
        std::swap(arr[pivotIndex], arr[high]);
        T pivot = arr[high];

        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (comp(arr[j], pivot) || (!comp(pivot, arr[j]) && !comp(arr[j], pivot))) {
                // arr[j] <= pivot according to comp
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        std::swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    template <typename T, typename Compare>
    static void quicksortRecursive(std::vector<T>& arr, int low, int high, Compare comp) {
        while (low < high) {
            // Insertion sort for small sub-arrays (threshold = 10)
            if (high - low < 10) {
                for (int i = low + 1; i <= high; ++i) {
                    T key = arr[i];
                    int j = i - 1;
                    while (j >= low && comp(key, arr[j])) {
                        arr[j + 1] = arr[j];
                        j--;
                    }
                    arr[j + 1] = key;
                }
                break;
            }

            int p = partition(arr, low, high, comp);

            // Recurse on smaller partition first to limit stack depth to O(log N)
            if (p - low < high - p) {
                quicksortRecursive(arr, low, p - 1, comp);
                low = p + 1;
            } else {
                quicksortRecursive(arr, p + 1, high, comp);
                high = p - 1;
            }
        }
    }

public:
    template <typename T, typename Compare = std::less<T>>
    static void sort(std::vector<T>& arr, Compare comp = Compare()) {
        if (arr.size() <= 1) return;
        quicksortRecursive(arr, 0, static_cast<int>(arr.size()) - 1, comp);
    }
};

#endif // QUICKSORT_HPP
