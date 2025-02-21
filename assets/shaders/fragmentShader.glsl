#version 330 core
out vec4 FragColor;  
in vec4 fragCol;
  
void main()
{
    FragColor = fragCol;
}