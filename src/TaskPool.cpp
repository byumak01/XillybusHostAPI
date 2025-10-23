#include "TaskPool.h"

void TaskPool::enqueue_task(const Task& task) {
    // TODO: this function needs to be handled from 0.
    scoped_lock<interprocess_mutex> lock(mutex);

    const auto data_span = task.get_data();
    const auto instr_span = task.get_instructions();

    const auto data = data_span.data();
    const auto instr = instr_span.data();

    const auto data_size = data_span.size_bytes();
    const auto instr_size = instr_span.size_bytes();

    // These cases are not handled.
    if (data_size > POOL_SIZE) {
        throw std::runtime_error("Data size is bigger than POOL_SIZE");
    }

    if (instr_size > POOL_SIZE) {
        throw std::runtime_error("Instruction size is bigger than POOL_SIZE");
    }

    // Wait for space
    while ((active_task_count >= MAX_TASKS || data_pool_to_fpga_offset + data_size > POOL_SIZE ||
            instr_pool_to_fpga_offset + instr_size > POOL_SIZE) &&
           running) {
        cond_space_available.wait(lock);
    }

    // change if return type is changed
    if (!running)
        throw std::runtime_error("Server is not running!");

    TaskMetadata& metadata = tasks[next_write_slot];

    const size_t data_write_pos = data_pool_to_fpga_offset;
    const size_t instr_write_pos = instr_pool_to_fpga_offset;

    // Copy data
    std::memcpy(data_pool_to_fpga + data_write_pos, data, data_size);
    std::memcpy(instr_pool_to_fpga + instr_write_pos, instr, instr_size);

    // Fill metadata
    metadata.process_id = task.get_process_id();
    metadata.process_task_id = task.get_process_task_id();
    metadata.data_size = data_size;
    metadata.data_offset = data_write_pos;
    metadata.instr_size = instr_size;
    metadata.instr_offset = instr_write_pos;
    metadata.valid = true;

    // Update state
    data_pool_to_fpga_offset = (data_write_pos + data_size);
    instr_pool_to_fpga_offset = (instr_write_pos + instr_size);
    next_write_slot = (next_write_slot + 1) % MAX_TASKS;
    active_task_count++;

    cond_task_added.notify_one();
    // return ;
}

bool TaskPool::dequeue_task(int& proc_id, int& user_tid, std::vector<uint64_t>& data,
                            std::vector<uint64_t>& instr) {
    // TODO: this function needs to be handled from 0.
    scoped_lock<interprocess_mutex> lock(mutex);

    while (active_task_count == 0 && running) {
        cond_task_added.wait(lock);
    }

    if (active_task_count == 0 && !running) {
        return false;
    }

    TaskMetadata& task = tasks[next_read_slot];

    if (!task.valid) {
        return false;
    }

    proc_id = task.process_id;
    user_tid = task.process_task_id;

    data.resize(task.data_size);
    std::memcpy(data.data(), data_pool_to_fpga + task.data_offset, task.data_size);

    instr.resize(task.instr_size);
    std::memcpy(instr.data(), instr_pool_to_fpga + task.instr_offset, task.instr_size);

    task.valid = false;
    data_pool_to_fpga_offset -= task.data_size;
    instr_pool_to_fpga_offset -= task.instr_size;
    next_read_slot = (next_read_slot + 1) % MAX_TASKS;
    active_task_count--;

    cond_space_available.notify_one();
    return true;
}

void TaskPool::shutdown() {
    scoped_lock<interprocess_mutex> lock(mutex);
    running = false;
    cond_task_added.notify_all();
    cond_space_available.notify_all();
}