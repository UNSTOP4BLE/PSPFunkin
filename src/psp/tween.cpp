#include "tween.h"

static int tweenTime;

int getTweenTime(void) {
    return tweenTime;
}

void setTweenTime(int t) {
    tweenTime = t;
}