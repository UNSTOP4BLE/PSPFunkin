#pragma once
#include "psp/animation.h"

class Character
{
public:
    Character(std::string path, std::string objstr, float _x, float _y);
    void setPos(float _x, float _y);
    void setFocus(float x, float y, float zoom);
    virtual void setAnim(int anim, AnimationModes mode);
    void setIcon(int i);
    int getAnim(void);
    int getFrame(void);
    virtual ~Character(void);

    virtual void tick(void);
    virtual void draw(float cx, float cy, float cz);
    float x, y, camx, camy, camzoom;
    bool issinging;
    int singendtime;
    int icon;
protected:
    Anim_OBJECT *obj;
    std::string type;
};

class GFCharacter : public Character
{
public:
    GFCharacter(std::string path, std::string objstr, float _x, float _y);
    void setAnim(int anim, AnimationModes mode);
    ~GFCharacter(void);

    void tick(void);
    void draw(float cx, float cy, float cz);
private:
    Anim_OBJECT *obj_speaker;
};
