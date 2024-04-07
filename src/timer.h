#pragma once

#include <chrono>

int getStep(void);
double getTimeSeconds(void);

class Timer {
public:
    void start(void);
    void stop(void);
    double elapsedMS(void);
private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};