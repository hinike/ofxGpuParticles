#version 330

layout (points) in;

layout (line_strip, max_vertices = 2) out;

uniform vec4 particleColor;

out vec4 geomOutputColor;

void main() {  
    gl_Position = gl_in[0].gl_Position + vec4(-10.0, 0.0, 0.0, 0.0); 
    geomOutputColor = particleColor;

    EmitVertex();
    
    gl_Position = gl_in[0].gl_Position + vec4(10.0, 0.0, 0.0, 0.0);
    geomOutputColor = particleColor;

    EmitVertex();
    
    EndPrimitive();
}
    