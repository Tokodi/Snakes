#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>

namespace net {
namespace common {

template<class T>
class ts_queue {
  public:
    ts_queue() = default;
    ts_queue(const ts_queue<T>&) = delete; // Disable copy const because of mutex

    const T& front();
    T pop_front();
    void push_front(const T& item);

    const T& back();
    T pop_back();
    void push_back(const T& item);

    bool empty();
    std::size_t size();

    void wait();

  private:
    std::mutex _muxQueue;
    std::deque<T> _deqQueue;
    std::condition_variable _cvBlocking;
    std::mutex _muxBlocking;
};

template<typename T>
const T& ts_queue<T>::front() {
    std::scoped_lock lock(_muxQueue);
    return _deqQueue.front();
}

template<typename T>
T ts_queue<T>::pop_front() {
    std::scoped_lock lock(_muxQueue);
    auto t = std::move(_deqQueue.front());
    _deqQueue.pop_front();
    return t;
}

template<typename T>
void ts_queue<T>::push_front(const T& item) {
    std::scoped_lock lock(_muxQueue);
    const bool wasEmpty = _deqQueue.empty();
    _deqQueue.emplace_front(std::move(item));

    if(wasEmpty) {
        _cvBlocking.notify_one();
    }
}

template<typename T>
const T& ts_queue<T>::back() {
    std::scoped_lock lock(_muxQueue);
    return _deqQueue.back();
}

template<typename T>
T ts_queue<T>::pop_back() {
    std::scoped_lock lock(_muxQueue);
    auto t = std::move(_deqQueue.back());
    _deqQueue.pop_back();
    return t;
}

template<typename T>
void ts_queue<T>::push_back(const T& item) {
    std::scoped_lock lock(_muxQueue);
    const bool wasEmpty = _deqQueue.empty();
    _deqQueue.emplace_back(std::move(item));

    if(wasEmpty) {
        _cvBlocking.notify_one();
    }
}

template<typename T>
bool ts_queue<T>::empty() {
    std::scoped_lock lock(_muxQueue);
    return _deqQueue.empty();
}

template<typename T>
std::size_t ts_queue<T>::size() {
    std::scoped_lock lock(_muxQueue);
    return _deqQueue.size();
}

template<typename T>
void ts_queue<T>::wait() {
    while (empty()) {
        std::unique_lock<std::mutex> ul(_muxBlocking);
        _cvBlocking.wait(ul);
    }
}

} // ns common
} // ns net
