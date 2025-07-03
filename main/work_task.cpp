#include "work_task.h"


void WorkTask::work_task_loop() {
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("work task loop\n");
    }
}


WorkTask::WorkTask(uint32_t stack_size)
    : task_count(0)
{
    // 创建后台工作任务（消费者线程）
    xTaskCreate([](void *pvParameters) {
        WorkTask *work_task = static_cast<WorkTask *>(pvParameters);
        work_task->work_task_loop();
        vTaskDelete(NULL);
    }, 
    "WorkTask", stack_size, this, 3, NULL);
}


WorkTask::~WorkTask() {
    // 资源清理如果需要
}

void WorkTask::add_task(FuncVoid task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        task_list.push(task);
        task_count++;
    }
    // 唤醒后台任务线程执行
    condition_variable_.notify_one();
}


// void WorkTask::work_task_loop() {
//     while (true) {
//         FuncVoid task;

//         {
//             std::unique_lock<std::mutex> lock(mutex_);
//             condition_variable_.wait(lock, [&] { return !task_list.empty(); }); // 队列为空则阻塞

//             task = task_list.front();
//             task_list.pop();
//             task_count--;
//         }

//         if (task) {
//             task(); // 执行任务
//         }

//         vTaskDelay(1); // 防止任务饿死CPU，可调节
//     }
// }