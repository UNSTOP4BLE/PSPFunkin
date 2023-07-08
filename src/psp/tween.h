#pragma once
#include "../chartparser.h"
#include "../app.h"

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
        return app->timer.elapsedMS() / 1000;
    }
};

template<typename T, typename E, typename S> class Tween {
private:
    T     _base, _delta;
    float _endTime, _timeScale;

public:
    inline Tween(void) {
        setValue(static_cast<T>(0));
    }
    inline Tween(T start) {
        setValue(start);
    }

    inline T getValue(void) {
        float time = _endTime - S::getTweenTime();
        if (time <= 0)
            return _base + _delta;
        return _base + _delta * E::apply(1.0 - time * _timeScale);
    }
    inline bool isDone(void) {
        return (S::getTweenTime() <= _endTime);
    }

    inline void setValue(T start, T target, float duration) {
        _base  = start;
        _delta = target - start;

        _endTime   = S::getTweenTime() + duration;
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