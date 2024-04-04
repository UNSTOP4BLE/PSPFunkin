#pragma once
#include "../timer.h"

class LinearEasing {
public:
    static inline float apply(float value) {
        return value;
    }
};
class QuadInEasing {
public:
    static inline float apply(float value) {
        return value * value;
    }
}; 
class QuadInOutEasing {
public:
    static inline float apply(float value) {
        if (value < 0.5)
            return 2.0 * value * value;
        else
            return 1.0 - 2.0 * (value - 1.0) * (value - 1.0);
    }
};

enum TimeSources {
    Step,
    Chrono
};

/* ...other easing functions... */
class StepTimeSource {
public:
    static inline float getTweenTime(void) {
        return getStep();
    }
};
class ChronoTimeSource {
public:
    static inline float getTweenTime(void) {
        return getTimeSeconds();
    }
};

template<typename T, typename E, TimeSources src> class Tween {
private:
    T     _base, _delta;
    float _endTime, _timeScale;
    StepTimeSource SStep;
    ChronoTimeSource SChrono;

    inline float getTweenTime(void) {
        switch(timesource) {
            case Step:
                return SStep.getTweenTime();
                break; 
            case Chrono:
                return SChrono.getTweenTime();
                break; 
        }
        return 0;
    }

public:
    TimeSources timesource;
    inline Tween(void) {
        setValue(static_cast<T>(0));
        timesource = src;
    }
    inline Tween(T start) {
        setValue(start);
        timesource = src;
    }

    inline T getValue(void) {
        float time = _endTime - getTweenTime();
        if (time <= 0)
            return _base + _delta;
        return _base + _delta * E::apply(1.0 - time * _timeScale);
    }
    inline bool isDone(void) {
        return (getTweenTime() <= _endTime);
    }

    inline void setValue(T start, T target, float duration) {
        _base  = start;
        _delta = target - start;

        _endTime   = getTweenTime() + duration;
        _timeScale = 1.0 / duration;
    }
    inline void setValue(T target, float duration) {
        setValue(getValue(), target, duration);

    }
    inline void setValue(T target) {
        _base    = target;
        _delta   = static_cast<T>(0);
        _endTime = 0;
    }
};