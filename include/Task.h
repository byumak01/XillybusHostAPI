#pragma once

#include "headers.h"

struct Task {
    Task(const std::vector<uint64_t>& instructions, const std::vector<uint64_t>& data)
        : instructions_(instructions), data_(data), process_task_id_(process_task_counter_++),
          process_id_(getpid()) {}

    // Clean user-facing API
    // TODO: handle return type if return type of submit is changed.
    void submit() const;

    // Getters
    [[nodiscard]] std::span<const uint64_t> get_instructions() const {
        return instructions_;
    }

    [[nodiscard]] std::span<const uint64_t> get_data() const {
        return data_;
    }

    [[nodiscard]] int get_process_task_id() const {
        return process_task_id_;
    }

    [[nodiscard]] int get_process_id() const {
        return process_id_;
    }

private:
    inline static std::atomic<int> process_task_counter_{0};
    const std::span<const uint64_t> instructions_;
    const std::span<const uint64_t> data_;
    const int process_task_id_;
    const int process_id_;
};