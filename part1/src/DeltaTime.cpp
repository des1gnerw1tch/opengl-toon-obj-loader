#include "DeltaTime.hpp"

#include <iostream>
#include <chrono>
#include <ctime>

DeltaTime::DeltaTime()  {
    lastFrameUpdateTime = std::chrono::system_clock::now();
    currentFrameUpdateTime = std::chrono::system_clock::now();
}

void DeltaTime::Update() {
    lastFrameUpdateTime = currentFrameUpdateTime;
    currentFrameUpdateTime = std::chrono::system_clock::now();
}

double DeltaTime::GetDeltaTime() {
    std::chrono::duration<double, std::milli> elapsed = currentFrameUpdateTime - lastFrameUpdateTime;
    return elapsed.count() / 1000.0f;
}

long long DeltaTime::GetSystemTimeMs()   {
    //std::time_t now = std::time(nullptr);
    //return now;

    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}