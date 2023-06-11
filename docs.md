# PSPFUNKIN FORMATS AND DOCUMENTATION ! ! ! !

okay so this is gonna be a basic document just to help out some confusion    

# CHART FORMAT AND CONVERTER    
the chart format is very simple. It contains a header:   
```
char magic[16]; //magic string to make sure its the pspfunkin chart, not something else    
float speed;      
double bpm;     
```
and it also contains notedata   
```
int32_t sectioncount;    
int32_t notecount;   
```
all the converter does is it parses this data from a fnf chart and outputs binary  

# CODE  
