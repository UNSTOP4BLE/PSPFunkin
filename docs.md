# PSPFUNKIN FORMATS AND DOCUMENTATION ! ! ! !

Okay so this is gonna be a basic document just to help clear out some confusion while modding.  

# CHART FORMAT AND CONVERTER    
The chart format is very simple. It contains a header:   
```cpp
char magic[16]; //magic string to make sure its the pspfunkin chart, not something else    
float speed;      
double bpm;     
```
and it also contains notedata:
```cpp
int32_t sectioncount;    
int32_t notecount;   
```
All the converter does is it parses this data from a fnf chart and outputs binary.

# CODE  

## SCREENS  
Screens are basically whats currently going on in the game (duh).  
They can also change without editing main.cpp or doing some weird stuff using while loops.
```cpp
class Screen {
public:
    inline Screen(void) {}
    virtual void update(void) {}
    virtual void draw(void) {}
    virtual ~Screen(void) {}
};
```
Set them using the setScreen funcition.
```cpp
void setScreen(Screen *scr);
```
## PLAYSTATE FUNCTIONS
```cpp
UNFINISHED!!!
```
