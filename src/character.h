#pragma once
#include "psp/animation.h"

class Character
{
public:
    Character(std::string path, std::string objstr, float _x, float _y);
    void setPos(float _x, float _y);
    void setFocus(float x, float y, float zoom);
    void setAnim(int anim, AnimationModes mode);
    void setIcon(int i);
    int getAnim(void);
    int getFrame(void);
    ~Character(void);

    void tick(void);
    void draw(float cx, float cy, float cz);
    float x, y, camx, camy, camzoom;
    bool issinging;
    int singendtime;
    int icon;
private:
    Anim_OBJECT *obj;
    Anim_OBJECT *obj_speaker;
    std::string type;
};
