/** @file DeltaTime.hpp
 *  @brief Holds
 *  
 *  Includes triangle and vertex
 *  
 *
 *  @author Zachary Walker-Liang
 *  @bug No known bugs.
 */

#pragma once
#include <chrono>

class DeltaTime   {
    private:
        std::__1::chrono::system_clock::time_point lastFrameUpdateTime;
        std::__1::chrono::system_clock::time_point currentFrameUpdateTime;
    public:
        DeltaTime();
        void Update();
        double GetDeltaTime(); // Returns time since last frame in seconds
        long long GetSystemTimeMs(); // Returns system time in epoch
};