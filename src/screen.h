#ifndef __SCREEN_H__
#define __SCREEN_H__

class Screen {
public:
    virtual void init(void) {}
    virtual void update(double delta) {}
};

class ErrorScreen : public Screen {
public:
    void update(const char *format, ...);
};

class TitleScreen : public Screen {
public:
    void init(void); 
    void update(double delta);
};
  
#endif