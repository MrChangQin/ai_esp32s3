#pragma once
#include <functional>
#include <list>
#include <mutex>

// 任务函数定义（无参数、无返回值）
using FuncVoid = std::function<void()>;

// 任务队列容器（可替换为其他容器如 std::list）
using ListFunction = std::list<FuncVoid>;


using MutexLockGuard = std::lock_guard<std::mutex>;
using MutexUniqueLock = std::unique_lock<std::mutex>;