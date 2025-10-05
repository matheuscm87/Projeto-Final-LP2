#ifndef TSQUEUE_HPP
#define TSQUEUE_HPP

#include <mutex>
#include <condition_variable>
#include <queue>
#include <optional>

// Simple counting semaphore (portable)
class Semaphore {
private:
    std::mutex m;
    std::condition_variable cv;
    unsigned long count;
public:
    explicit Semaphore(unsigned long initial = 0) : count(initial) {}
    void notify(unsigned long n = 1) {
        {
            std::lock_guard<std::mutex> lk(m);
            count += n;
        }
        cv.notify_all();
    }
    void wait() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return count > 0; });
        --count;
    }
    bool try_wait() {
        std::lock_guard<std::mutex> lk(m);
        if (count > 0) { --count; return true; }
        return false;
    }
};

// Thread-safe queue (monitor style) with condition variable
template<typename T>
class ThreadSafeQueue {
private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable cv;
    bool closed = false;
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() { close(); }

    void push(const T &item) {
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(item);
        }
        cv.notify_one();
    }
    void push(T &&item) {
        {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::move(item));
        }
        cv.notify_one();
    }

    // returns std::nullopt if queue closed and empty
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&] { return !q.empty() || closed; });
        if (q.empty()) return std::nullopt;
        T val = std::move(q.front());
        q.pop();
        return val;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lk(m);
            closed = true;
        }
        cv.notify_all();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(m);
        return q.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lk(m);
        return q.size();
    }
};

#endif // TSQUEUE_HPP
