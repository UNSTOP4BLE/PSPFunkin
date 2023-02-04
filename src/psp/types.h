#pragma once

//chartparser
struct Section
{
	double sectionNotes[3];
	//int typeOfSection;
	bool mustHitSection;
	//int bpm;
	//bool changeBPM;
	bool altAnim;
};

struct Parser
{
	int curBeat;
	int curStep;
	int songPos;
	bool justStep;
	double initbpm;
	double initspeed;
	double crochet;
	double step_crochet;
};

//animation
#define CountOf(x) (sizeof(x) / sizeof(x[0]))

struct AnimFrames
{
	int tex;
	int x, y, w, h, offsetx, offsety;
};

struct Anim_OBJECT
{
	float speed;
	float time;
	float angle;
	int curframe;
	int size;
	int alpha;
	bool linear;
	bool flipped;
	bool visible;
	bool tick;
	bool cananimate;
	AnimFrames *frames;
	int *conf;
	
	inline Anim_OBJECT(void)
	{
		speed = 0;
		time = 0;
		angle = 0;
		curframe = 0;
		size = 0;
		alpha = 255;
		linear = false;
		flipped = false;
		visible = true;
		tick = false;
		cananimate = false;
	}
};

//character
struct Character
{
	Anim_OBJECT obj;
	AnimFrames *frames;
	bool player;
	int icon;
	int x;
	int y;
	int camx, camy, camzoom;
	void (*setAnim)(CharAnims anim);
	void (*tick)(void);
	void (*free)(void);

    inline Character(void) {
       	player = false;
		icon = 0;
		x = 0;
		y = 0;
		camx = 0;
		camy = 0;
		camzoom = 0;
    }
};