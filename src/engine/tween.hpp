//done
#pragma once
#include <cmath>

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

class SineInOutEasing {
public:
    static inline float apply(float value) {
        return 0.5f * (1 - cosf(value * 3.14159265f));
    }
};

template<typename T, typename E> class Tween {
private:
    T     _base, _delta;
    float _endTime, _timeScale;

    inline float getTweenTime(void) {
        return tweentime;
    }

public:
    float tweentime;
    inline Tween(void) {
        setValue(static_cast<T>(0));
    }
    inline Tween(T start) {
        setValue(start);
    }
    
    inline void setTweenTime(float val) {
        tweentime = val;
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