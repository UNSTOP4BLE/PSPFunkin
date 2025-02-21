#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in int Col;

uniform int scr_width;
uniform int scr_height;

out vec4 fragCol; // output a color to the fragment shader

void main()
{
    // Convert from screen coordinates to normalized device coordinates
    gl_Position = vec4(2.0 * (Pos.x / float(scr_width)) - 1.0, 
                       1.0 - 2.0 * (Pos.y / float(scr_height)), 
                       Pos.z, 
                       1.0);
    
    // Extract RGBA components from the integer color value
    float r = float((Col >> 24) & 0xFF) / 255.0;  // red
    float g = float((Col >> 16) & 0xFF) / 255.0;  // green
    float b = float((Col >> 8) & 0xFF) / 255.0;   // blue
    float a = float(Col & 0xFF) / 255.0;          // alpha

    fragCol = vec4(r, g, b, a); 
}
