#ifndef TIME_HPP
#define TIME_HPP

#include <chrono>
#include <stdio.h>

namespace Ebb {

namespace Time {

static long _app_start_time;

void _time_init() {
    auto now = std::chrono::high_resolution_clock::now();

    // Convert the timepoint to seconds since the epoch as a floating-point value
    auto mS = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    _app_start_time = mS.count();
}

float elapsed_seconds() {
    // Get the current timepoint using system clock
    auto now = std::chrono::high_resolution_clock::now();

    // Convert the timepoint to seconds since the epoch as a floating-point value
    auto mS = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
    // Extract the count of seconds as a floating-point number
    return ((float)(mS.count() - _app_start_time)) / 1'000.0f;
}

}; // end namespace Time
}; // end namespace Ebb

#endif