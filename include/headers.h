//
// Created by bymk on 10/8/25.
//

#pragma once
#include "spdlog/spdlog.h"
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include <atomic>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <csignal>
#include <iostream>
#include <chrono>
#include <optional>
#include <cerrno>

#ifdef _WIN32
    #include <windows.h>
    #include <io.h>       
    #include <process.h>  
#else
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif