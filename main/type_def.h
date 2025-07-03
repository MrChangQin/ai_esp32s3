#pragma once
#include <functional>
#include <queue>

// 任务函数定义（无参数、无返回值）
using FuncVoid = std::function<void()>;

// 任务队列容器（可替换为其他容器如 std::list）
using ListFunction = std::queue<FuncVoid>;


