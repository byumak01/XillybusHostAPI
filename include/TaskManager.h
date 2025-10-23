#pragma once

#include "TaskPool.h"
#include "headers.h"

using namespace boost::interprocess;

struct Task;

class TaskManager {
private:
    inline static TaskManager* instance_ = nullptr;
    inline static std::once_flag init_flag_;

    shared_memory_object shm_;
    mapped_region region_;
    TaskPool* data_;

    // Private constructor - opens existing shared memory
    TaskManager()
        : shm_(open_only, "TaskPool", read_write), region_(shm_, read_write),
          data_(static_cast<TaskPool*>(region_.get_address())) {
        spdlog::info("TaskManager client connected to shared memory named TaskPool");
    }

public:
    TaskManager(const TaskManager&) = delete;
    TaskManager& operator=(const TaskManager&) = delete;

    // Singleton accessor for client processes
    static TaskManager& instance() {
        std::call_once(init_flag_, []() { instance_ = new TaskManager(); });
        return *instance_;
    }

    // Client-facing API
    // TODO: handle return type if enqueue_task return type is changed.
    void submit(const Task& task) const {
        data_->enqueue_task(task);
    }

    // For daemon use - creates the shared memory
    static TaskPool* create_shared() {
        struct shm_remove {
            shm_remove() {
                shared_memory_object::remove("TaskPool");
            }
            ~shm_remove() {
                shared_memory_object::remove("TaskPool");
            }
        } remover;

        shared_memory_object shm(create_only, "TaskPool", read_write);
        shm.truncate(sizeof(TaskPool));

        const mapped_region region(shm, read_write);
        auto* data = new (region.get_address()) TaskPool();

        spdlog::info("Shared memory named TaskPool created");
        return data;
    }

    // For daemon use - direct access to data
    [[nodiscard]] TaskPool* get_data() const {
        return data_;
    }

    ~TaskManager() = default;
};
