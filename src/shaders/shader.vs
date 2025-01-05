#version 330
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
uniform vec3 Color;
uniform mat4 MVPMatrix;
uniform mat4 MVMatrix;
uniform mat3 NormalMatrix;
out vec4 vs_Color;
out vec3 vs_Normal;
out vec4 vs_Position;
out vec2 vs_TexCoord;
void main(){
     gl_Position = MVPMatrix*vec4(Position, 1.0);
     vs_Position = MVMatrix *vec4(Position, 1.0);
     vs_Normal = NormalMatrix*Normal;
     vs_Color = vec4(Color.x,Color.y, Color.z, 1.0);
     vs_TexCoord = TexCoord;
}