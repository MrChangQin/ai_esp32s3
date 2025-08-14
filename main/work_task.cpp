#include "work_task.h"


void WorkTask::add_task(FuncVoid task) {
    MutexLockGuard lock(mutex_);
    task_count++;
    task_list.emplace_back([call = std::move(task), this]() {
        call();
        {
            MutexLockGuard lock(mutex_);
            task_count--;
            if (task_count == 0 && task_list.empty())
            {
                condition_variable_.notify_all();
            }
        }
    });
    condition_variable_.notify_all();
}

void WorkTask::wait_work_task_completion() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_variable_.wait(lock, [this]() {
        return task_list.empty() && task_count == 0;
    });
}

void WorkTask::work_task_loop() {
    while (1)
    {
        MutexUniqueLock lock(mutex_);
        condition_variable_.wait(lock, [this]() { return !task_list.empty(); });

        ListFunction func_list = std::move(task_list);
        lock.unlock();

        for (auto& func : func_list)
        {
            func();
        }
    }
}

WorkTask::WorkTask(uint32_t stack_size): task_count(0)
{
    // 创建后台工作任务（消费者线程）
    xTaskCreate([](void *pvParameters) {
        WorkTask *work_task = static_cast<WorkTask *>(pvParameters);
        work_task->work_task_loop();
        vTaskDelete(NULL);
    }, "WorkTask", stack_size, this, 3, NULL);
}


WorkTask::~WorkTask() {

}
