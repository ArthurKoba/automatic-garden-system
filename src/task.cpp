#include "task.h"


Task::Task(uint32_t timer_ms) {
    _delta_exec_ms = timer_ms;
}

Task::Task(uint32_t timer_ms, Task::task_function_t function) {
    _delta_exec_ms = timer_ms;
    set_task_function(function);
}

void Task::set_task_function(Task::task_function_t function) {
    _task_function = function;
}

void Task::force_call_function() {
    if (_task_function and _enabled) {
        _task_function(*this);
    }
    _last_exec = millis();
}

void Task::tick() {
    if (millis() - _last_exec < _delta_exec_ms) return;
    force_call_function();
}

void Task::enable() {
    if (_enabled) return;
    _enabled = true;
    force_call_function();
}

void Task::disable() {
    if (not _enabled) return;
    _enabled = false;
    _last_exec = 0;
}

void Task::set_delta_exec_ms(uint32_t delta) {
    _delta_exec_ms = delta;
}
