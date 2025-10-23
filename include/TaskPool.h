//
// Created by bymk on 10/8/25.
//

#pragma once

#include "Task.h"
#include "headers.h"

// Configuration
static constexpr size_t MAX_TASKS = 16;
static constexpr size_t POOL_SIZE = 64 * 1024; // 64KB pool

using namespace boost::interprocess;

struct TaskMetadata {
    int process_id = 0;
    int process_task_id = 0; // process specific task id
    size_t data_size = 0;    // Size of the data inside the task
    size_t data_offset = 0;  // Offset in data_pool where data starts
    size_t instr_size = 0;   // Size of instructions inside the task
    size_t instr_offset = 0; // Offset in instr_pool where data starts
    bool valid = false;      // Is this task slot occupied?
    TaskMetadata() {}        // TODO: handle constructor (add args)
};

// The actual shared memory structure
struct TaskPool {
    interprocess_mutex mutex;
    interprocess_condition cond_task_added;
    interprocess_condition cond_space_available;

    TaskMetadata tasks[MAX_TASKS] = {};
    size_t next_write_slot = 0;
    size_t next_read_slot = 0;
    size_t active_task_count = 0;

    uint8_t data_pool_to_fpga[POOL_SIZE] = {};
    size_t data_pool_to_fpga_offset = 0;

    uint8_t data_pool_from_fpga[POOL_SIZE] = {};
    size_t data_pool_from_fpga_offset = 0;

    uint8_t instr_pool_to_fpga[POOL_SIZE] = {};
    size_t instr_pool_to_fpga_offset = 0;

    bool running = false;

    TaskPool() {
        for (auto& task : tasks) {
            task.valid = false;
        }
    }

    // TODO: handle return types.
    void enqueue_task(const Task& task);
    bool dequeue_task(int& proc_id, int& user_tid, std::vector<uint64_t>& data,
                      std::vector<uint64_t>& instr);
    void shutdown();
};