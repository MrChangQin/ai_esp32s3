#pragma once
#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <functional>
#include "type_def.h"
#include <mutex>
#include <condition_variable>


class WorkTask
{
private:
    ListFunction task_list;
    std::mutex mutex_;
    std::condition_variable condition_variable_;
    uint32_t task_count;

public:

    WorkTask(uint32_t stack_size);
    ~WorkTask();

    void work_task_loop();

    void add_task(FuncVoid task);

};