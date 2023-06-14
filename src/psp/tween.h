#pragma once

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
class QuadOutEasing {
public:
    static inline float apply(float value) {
        return 1.0 - (1.0 - value) * (1.0 - value);
    }
};

/* ...other easing functions... */
template<typename T, typename E> class Tween {
private:
    T     _base, _delta;
    float _endTime, _timeScale, _time;

public:
    inline Tween(void) {
        setValue(static_cast<T>(0));
    }
    inline Tween(T start) {
        setValue(start);
    }

    inline T getValue(void) {
        float time = _endTime - _time;
        if (time <= 0)
            return _base + _delta;

        return _base + _delta * E::apply(1.0 - time * _timeScale);
    }
    inline bool isDone(void) {
        return (_time <= _endTime);
    }

    inline void setValue(T start, T target, float duration, float time) {
        _base  = start;
        _time  = time;
        _delta = target - start;

        _endTime   = _time + duration;
        _timeScale = 1.0 / duration;
    }
    inline void setValue(T target, float duration, float time) {
        setValue(getValue(), target, duration, time);
    }
    inline void setValue(T target) {
        _base    = target;
        _delta   = static_cast<T>(0);
        _endTime = 0;
    }
};