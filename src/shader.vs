#version 130
in vec3 Position;
uniform vec3 Color;
uniform mat4 Projection;
uniform mat4 Model;
uniform mat4 View;
out vec4 vs_Color;
void main()
{
     gl_Position = Projection*View*Model*vec4(Position, 1.0);
     vs_Color = vec4(Color.x,Color.y, Color.z, 1.0);

}