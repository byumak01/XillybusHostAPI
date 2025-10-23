#include "Server.h"
#include "TaskManager.h"

// Global atomic flag for graceful shutdown
static std::atomic<bool> g_running{true};

// Signal handler function
static void signal_handler(const int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        std::cout << "Received shutdown signal (" << signal << "), stopping gracefully..."
                  << std::endl;
        g_running = false;
    }
}

int Server::run() {
    std::cout << "Starting mydaemon..." << std::endl;

    // Setup signal handling
    setup_signals();

    // Create Xillybus manager
    xillybus_manager_.emplace();

    if (!xillybus_manager_->open_devices()) {
        spdlog::error("Failed to open Xillybus devices");
        xillybus_manager_.reset();
        return 1;
    }

    std::cout << "Server initialized successfully" << std::endl;

    // Main daemon loop
    main_loop();

    // Cleanup before exit
    cleanup();

    std::cout << "Server stopped" << std::endl;
    return 0;
}

void Server::setup_signals() {
    // Install signal handlers
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGINT, signal_handler);

    // Ignore SIGPIPE (broken pipe)
    std::signal(SIGPIPE, SIG_IGN);

    std::cout << "Signal handlers installed" << std::endl;
}

void Server::main_loop() {

    while (g_running) {
        // Your main daemon work goes here
        do_work();
    }
}

void Server::do_work() {

    const auto task_manager = TaskManager::instance().get_data();

    std::cout << "=== DAEMON STARTED ===\n";
    std::cout << "Max tasks: " << MAX_TASKS << "\n";
    std::cout << "Data pool size: " << POOL_SIZE << " bytes\n\n";

    // Process tasks
    int proc_id, user_task_id;
    std::vector<uint64_t> data;
    std::vector<uint64_t> instr;

    while (task_manager->dequeue_task(proc_id, user_task_id, data, instr)) {
        spdlog::info("Trying to process Task {} from Process {}...", user_task_id, proc_id);
#ifdef _WIN32
        auto bytesWritten = 0; // this seems necessary for windows, to have last parameter as NULL.
        if (!WriteFile(xillybus_manager_->data_handle_, data.data(), static_cast<DWORD>data.size(), &bytesWritten, NULL) {
            spdlog::error("Failed to write data to Xillybus");
        } else if (bytesWritten != data.size()) {
            spdlog::warn("Partial write to data_fd_: wrote {} of {} bytes", bytesWritten,
                         data.size());
        }

        bytesWritten = 0;
        if (!WriteFile(xillybus_manager_->instr_handle_, instr.data(), static_cast<DWORD>instr.size(), &bytesWritten, NULL) {
            spdlog::error("Failed to write instructions to Xillybus");
        } else if (bytesWritten != data.size()) {
            spdlog::warn("Partial write to data_fd_: wrote {} of {} bytes", bytesWritten,
                         data.size());
        }
#else
        // Write data
        ssize_t bytesWritten = write(xillybus_manager_->data_fd_, data.data(), data.size());
        if (bytesWritten < 0) {
            spdlog::error("Failed to write data to Xillybus: {}", strerror(errno));
        } else if (bytesWritten != data.size()) {
            spdlog::warn("Partial write to data_fd_: wrote {} of {} bytes", bytesWritten,
                         data.size());
        }

        // Write instr
        bytesWritten = write(xillybus_manager_->instr_fd_, instr.data(), instr.size());
        if (bytesWritten < 0) {
            spdlog::error("Failed to write instr to Xillybus: {}", strerror(errno));
        } else if (bytesWritten != instr.size()) {
            spdlog::warn("Partial write to instr_fd_: wrote {} of {} bytes", bytesWritten,
                         instr.size());
        }
#endif
    }

    std::cout << "\n[Daemon] Shutdown complete\n";
}

void Server::cleanup() {
    std::cout << "Performing cleanup..." << std::endl;

    // Close Xillybus devices
    if (xillybus_manager_) {
        xillybus_manager_.reset();
    }
}
