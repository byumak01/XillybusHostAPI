#include "XillybusManager.h"

XillybusManager::XillybusManager() {
    spdlog::info("XillybusManager created");
}

XillybusManager::~XillybusManager() {
    close_devices();
}

bool XillybusManager::open_devices() {
    spdlog::info("Opening Xillybus devices");

#ifdef _WIN32
    instr_handle_ = CreateFileA(
        instr_device_,
        GENERIC_WRITE,
        0,  // No sharing
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (instr_handle_ == INVALID_HANDLE_VALUE) {
        spdlog::error("Failed to open {}: Error code {}", instr_device_, GetLastError());
        return false;
    }
    spdlog::info("Opened {} successfully", instr_device_);

    data_handle_ = CreateFileA(
        data_device_,
        GENERIC_READ | GENERIC_WRITE,
        0,  // No sharing
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (data_handle_ == INVALID_HANDLE_VALUE) {
        spdlog::error("Failed to open {}: Error code {}", data_device_, GetLastError());
        return false;
    }
    spdlog::info("Opened {} successfully", data_device_);

#else
    instr_fd_ = open(instr_device_, O_WRONLY);
    if (instr_fd_ < 0) {
        spdlog::error("Failed to open {}: {} (errno: {})", instr_device_, strerror(errno),
                      errno);
        return false;
    }
    spdlog::info("Opened {} successfully (fd: {})", instr_device_, instr_fd_);

    data_fd_ = open(data_device_, O_RDWR);
    if (data_fd_ < 0) {
        spdlog::error("Failed to open {}: {} (errno: {})", data_device_, strerror(errno),
                      errno);
        return false;
    }
    spdlog::info("Opened {} successfully (fd: {})", data_device_, data_fd_);
#endif

    return true;
}

void XillybusManager::close_devices() {
#ifdef _WIN32
    if (instr_handle_ != INVALID_HANDLE_VALUE) {
        spdlog::info("Closing {}", instr_device_);
        CloseHandle(instr_handle_);
        instr_handle_ = INVALID_HANDLE_VALUE;
    }

    if (data_handle_ != INVALID_HANDLE_VALUE) {
        spdlog::info("Closing {}", data_device_);
        CloseHandle(data_handle_);
        data_handle_ = INVALID_HANDLE_VALUE; 

    }

#else
    if (instr_fd_ >= 0) {
        spdlog::info("Closing {} (fd: {})", instr_device_, instr_fd_);
        close(instr_fd_);
        instr_fd_ = -1;
    }

    if (data_fd_ >= 0) {
        spdlog::info("Closing {} (fd: {})", data_device_, data_fd_);
        close(data_fd_);
        data_fd_ = -1;
    }
#endif
}
