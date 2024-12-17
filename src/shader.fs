#version 130
out vec4 FragColor;
in vec4 vs_Color;
void main()
{
    FragColor = vs_Color;
}
