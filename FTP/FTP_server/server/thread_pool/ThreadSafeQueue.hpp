#include "include.hpp"

template <class T>
class threadsafequeue{
public:
    void Push(const T& test);
    bool Pop(T& test);

private:

    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable cond;
};