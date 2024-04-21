#include "combo.h"
#include "psp/hash.h"

void Combo::init(void)
{
    objectindex = 0;
    combo = 0;
    for (int i = 0; i < COMBO_OBJ_MAX; i++)
    {
        objects[i].tick = false;
        objects[i].alpha = 0;
        objects[i].x = 0;
        objects[i].y.setValue(0);
        objects[i].ymin = 0;
        objects[i].timer = 0;
        objects[i].falling = false;
        objects[i].img = {0, 0, 0, 0};
        objects[i].disp = {0, 0, 0, 0};
    }
}
void Combo::spawnNew(std::string name)
{
    /*
    if (objectindex > COMBO_OBJ_MAX)
        objectindex = 0;

    objects[objectindex].alpha = 255;
    objects[objectindex].tick = true;
    objects[objectindex].x = 100;
    objects[objectindex].y.setValue(100);
    objects[objectindex].falling = false;
    objects[objectindex].ymin = 150;
    objects[objectindex].timer = 0;

    objects[objectindex].img = {411, 1, 100, 50};
    objects[objectindex].disp = {static_cast<int>(objects[objectindex].x), static_cast<int>(objects[objectindex].y.getValue()), 100, 50};
    switch (Hash::FromString(name.c_str()))
    {
        case "sick"_h:
        objects[objectindex].img.y = 1;
            break;
        case "good"_h:
        objects[objectindex].img.y = 52;
            break;
        case "bad"_h:
        objects[objectindex].img.y = 103;
            break;
        case "shit"_h:
        objects[objectindex].img.y = 154;
            break;
        default: return; break;
    }
    objectindex ++;
    objects[objectindex].y.setValue(static_cast<float>(60), 0.3);
*/
}

void Combo::tick(void)
{
    /*
    for (int i = 0; i < COMBO_OBJ_MAX; i++)
    {
        if (!objects[i].tick)
            continue;

        if (!objects[i].falling)
            objects[i].timer --;
        if (objects[i].y.getValue() < 100-10)
            objects[i].falling = true;
        
        if (objects[i].falling) {
            objects[objectindex].y.setValue(GFX::SCREEN_HEIGHT, 0.3);
            if (objects[i].timer > 0)
                objects[i].alpha -= 4;
        }
        //destroy object
        if (objects[i].y.getValue() >= GFX::SCREEN_HEIGHT)
            objects[i].tick = false;
        objects[i].disp = {static_cast<int>(objects[i].x), static_cast<int>(objects[i].y.getValue()), 100, 50};
    }*/
}

void Combo::draw(GFX::Texture *tex, float zoom)
{
    /*
    for (int i = 0; i < COMBO_OBJ_MAX; i++)
    {
        if (!objects[i].tick)
            continue;
        if (objects[i].alpha < 0)
            objects[i].alpha = 0;
//        GFX::drawTex<int>(tex, &objects[i].img, &objects[i].disp, 0, static_cast<int>(objects[i].alpha), zoom);
    }*/
    
}