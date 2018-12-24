#pragma once

#include <cstdlib>

template <class T>
class Stack {
public:
    Stack() : top_(0), area_size_(1), is_ok_(true) {
        size_t size = sizeof(T) + 3 * sizeof(int);
        char* area = (char*)std::calloc(size, sizeof(char));

        left_canary_ = (int*)area;
        items_ = (T*)(area + sizeof(int));
        check_sum_ = (int*)(area + sizeof(int) + sizeof(T));
        right_canary_ = (int*)(area + 2 * sizeof(int) + sizeof(T));

        *left_canary_ = LEFT;
        *right_canary_ = RIGHT;

        *check_sum_ = CountChecksum();
    }

    void Push(T item) {
        CheckState();

        if (top_ == area_size_) {
            CopyItems(ReasonForCopy::EXPAND);
        }
        items_[top_] = item;
        ++top_;

        Update();
    }

    T Top() const {
        CheckState();

        if (top_ == 0) {
            is_ok_ = false;
            return T();
        }

        return items_[top_ - 1];
    }

    void Pop() {
        CheckState();

        if (top_ == 0) {
            is_ok_ = false;
            return;
        }
        --top_;
        if (top_ < area_size_ / 2) {
            CopyItems(ReasonForCopy::TIGHTEN);
        }

        Update();
    }

    size_t Size() const {
        return top_;
    }

    bool Empty() const {
        return top_ == 0;
    }

    bool IsOk() const {
        CheckState();
        return is_ok_;
    }

    void GetErrorInfo() const {
        if (is_ok_) {
            std::cerr << "OK\n";
            return;
        }

        switch (error_info_) {
            case ErrorReason::LEFT_CANARY:
                std::cerr << "\nIndex out of bounds: left canary was damaged\n";
                break;
            case ErrorReason::RIGHT_CANARY:
                std::cerr << "\nIndex out of bounds: right canary was damaged\n";
                break;
            case ErrorReason::CHECKSUM:
                std::cerr << "\nSomething goes wrong: buffer was damaged\n";
                break;
        }
    }

    ~Stack() {
        std::free(left_canary_);
    }

private:
    enum class ReasonForCopy {
        EXPAND,
        TIGHTEN
    };

    enum class ErrorReason {
        LEFT_CANARY,
        RIGHT_CANARY,
        CHECKSUM
    };

    void CopyItems(ReasonForCopy reason) {
        CheckState();
        T* new_items = nullptr;
        switch (reason) {
            case ReasonForCopy::EXPAND:
                area_size_ <<= 1;
                break;
            case ReasonForCopy::TIGHTEN:
                area_size_ >>= 1;
                break;
        }

        char* old_area = (char*)left_canary_;
        char* area = (char*)std::calloc(sizeof(T) * area_size_ + 2 * sizeof(int), sizeof(char));

        left_canary_ = (int*)area;
        new_items = (T*)(area + sizeof(int));
        right_canary_ = (int*)(area + sizeof(int) + sizeof(T) * area_size_);

        for (size_t i = 0; i < top_; ++i) {
            new_items[i] = items_[i];
        }

        *left_canary_ = LEFT;
        *right_canary_ = RIGHT;
        items_ = new_items;
        std::free(old_area);
        *check_sum_ = CountChecksum();
    }

    void CheckState() const {
        if (*left_canary_ != LEFT) {
            is_ok_ = false;
            error_info_ = ErrorReason::LEFT_CANARY;
            return;
        }

        if (*right_canary_ != RIGHT) {
            is_ok_ = false;
            error_info_ = ErrorReason::RIGHT_CANARY;
            return;
        }

        if (*check_sum_ != CountChecksum()) {
            is_ok_ = false;
            error_info_ = ErrorReason::CHECKSUM;
            return;
        }
    }

    int CountChecksum() const {
        int sum = 0;
        int multiplier = 1;
        char* area = (char*)items_;
        size_t size = area_size_ * sizeof(T);
        for (size_t i = 0; i < size; ++i, multiplier *= base_) {
            sum += (multiplier * area[i]) % mod_;
            sum %= mod_;
        }
        return sum;
    }

    void Update() {
        *check_sum_ = CountChecksum();
    }

    T* items_;
    size_t top_;
    size_t area_size_;

    mutable bool is_ok_;
    int* left_canary_;
    int* right_canary_;
    int* check_sum_;
    const int base_ = 2;
    const int mod_ = 10007;
    mutable ErrorReason error_info_;
    static constexpr int LEFT = 0xDEADBEEF;
    static constexpr int RIGHT = 0xBEDABEDA;
};
