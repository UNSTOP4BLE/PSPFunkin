#pragma once

template<typename A> struct GlobalJob {
public:
    void (*func)(A arg);
    A arg;
};

template<typename T, typename A> struct ClassMemberJob {
public:
    void (T::*func)(A arg);
    T *obj;
    A arg;
};

class MediaEngine
{
public:
private:

};