#ifndef AUTOMATIC_GARDEN_SYSTEM_TASK_H
#define AUTOMATIC_GARDEN_SYSTEM_TASK_H

#include <Arduino.h>
#include <functional>

class Task final {
public:
    typedef std::function<void(Task &current_task)> task_function_t;
private:
    uint32_t _last_exec = UINT32_MAX;
    uint32_t _delta_exec_ms{};
    bool _enabled = true;
    task_function_t _task_function = nullptr;
public:
    Task(uint32_t timer_ms);
    Task(uint32_t timer_ms, task_function_t function);

    void set_task_function(task_function_t function);

    void force_call_function();

    void set_delta_exec_ms(uint32_t delta);

    void disable();
    void enable();

    void tick();
};

#endif //AUTOMATIC_GARDEN_SYSTEM_TASK_H
