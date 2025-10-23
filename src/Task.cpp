//
// Created by bymk on 10/9/25.
//
#include "Task.h"
#include "TaskManager.h"

void Task::submit() const {
    TaskManager::instance().submit(*this);
}