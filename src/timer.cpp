#include "timer.h"
#include "app.h"

void Timer::start(void) {
    m_StartTime = std::chrono::system_clock::now();
    m_bRunning = true;
}

void Timer::stop(void) {
    m_EndTime = std::chrono::system_clock::now();
    m_bRunning = false;
}

double Timer::elapsedMS(void)
{
    std::chrono::time_point<std::chrono::system_clock> endTime;
     
    if(m_bRunning)
    {
        endTime = std::chrono::system_clock::now();
    }
    else
    {
        endTime = m_EndTime;
    }
        
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
}
    
double Timer::elapsedS(void)
{
    std::chrono::time_point<std::chrono::system_clock> endTime;
     
    if(m_bRunning)
    {
        endTime = std::chrono::system_clock::now();
    }
    else
    {
        endTime = m_EndTime;
    }
        
    return std::chrono::duration_cast<std::chrono::seconds>(endTime - m_StartTime).count();
}

int getStep(void) {
    return app->parser.curStep;
}

double getTimeSeconds(void) {
    return app->timer.elapsedMS() / 1000;
}