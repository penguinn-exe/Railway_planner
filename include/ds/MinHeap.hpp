#ifndef MIN_HEAP_HPP
#define MIN_HEAP_HPP

#include <vector>
#include <stdexcept>
#include <utility>
#include <functional>

template <typename T, typename Compare = std::less<T>>
class MinHeap {
private:
    std::vector<T> heap;
    Compare comp;

    void heapifyUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (comp(heap[index], heap[parent])) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapifyDown(size_t index) {
        size_t size = heap.size();
        while (index * 2 + 1 < size) {
            size_t leftChild = index * 2 + 1;
            size_t rightChild = index * 2 + 2;
            size_t smallest = index;

            if (leftChild < size && comp(heap[leftChild], heap[smallest])) {
                smallest = leftChild;
            }
            if (rightChild < size && comp(heap[rightChild], heap[smallest])) {
                smallest = rightChild;
            }

            if (smallest != index) {
                std::swap(heap[index], heap[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    MinHeap(const Compare& compare = Compare()) : comp(compare) {}

    void push(const T& value) {
        heap.push_back(value);
        heapifyUp(heap.size() - 1);
    }

    void push(T&& value) {
        heap.push_back(std::move(value));
        heapifyUp(heap.size() - 1);
    }

    T pop() {
        if (empty()) {
            throw std::underflow_error("MinHeap is empty");
        }
        T topVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!empty()) {
            heapifyDown(0);
        }
        return topVal;
    }

    const T& top() const {
        if (empty()) {
            throw std::underflow_error("MinHeap is empty");
        }
        return heap[0];
    }

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }

    void clear() {
        heap.clear();
    }

    // Convert vector to heap in O(N)
    static MinHeap buildHeap(const std::vector<T>& vec, const Compare& compare = Compare()) {
        MinHeap h(compare);
        h.heap = vec;
        if (!h.heap.empty()) {
            for (int i = (int)h.heap.size() / 2 - 1; i >= 0; --i) {
                h.heapifyDown(i);
            }
        }
        return h;
    }
};

#endif // MIN_HEAP_HPP
