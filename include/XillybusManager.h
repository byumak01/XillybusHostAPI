#pragma once

#include "headers.h"

class XillybusManager {
public:
    XillybusManager();
    ~XillybusManager();

    XillybusManager(const XillybusManager&) = delete;
    XillybusManager& operator=(const XillybusManager&) = delete;

    bool open_devices();
    void close_devices();

#ifdef _WIN32
    static constexpr const char* instr_device_ = "\\\\.\\xillybus_instr";
    static constexpr const char* data_device_ = "\\\\.\\xillybus_data";

    HANDLE instr_handle_ = INVALID_HANDLE_VALUE;
    HANDLE data_handle_ = INVALID_HANDLE_VALUE;
#else
    static constexpr const char* instr_device_ = "/dev/xillybus_instr";
    static constexpr const char* data_device_ = "/dev/xillybus_data";

    int instr_fd_ = -1;
    int data_fd_ = -1;
#endif
};
