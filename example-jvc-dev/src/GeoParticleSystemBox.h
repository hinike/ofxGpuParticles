#pragma once
#include "ofMain.h"
#include "BaseParticleSystem.h"

class GeoParticleSystemBox : public BaseParticleSystem
{
public:
    
    
    
    ofTexture particleTexture;
    ofMatrix4x4 matrix;
    ofEasyCam* camera;
    GeoParticleSystemBox()
    {
        magnitudeFactor = 500.f;
        radius = 10.0f;
        particles = NULL;
        
    }
    void setup()
    {
        ofLoadImage(particleTexture, "of.png");
        loadShaders();
    }
    
    void createParticles(int w, int h)
    {
        
        initParticles(w, h);
        
        float* particlePosns = new float[w * h * 4];
        for (unsigned y = 0; y < h; ++y)
        {
            for (unsigned x = 0; x < w; ++x)
            {
                unsigned idx = y * w + x;
                particlePosns[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
                particlePosns[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
                particlePosns[idx * 4 + 2] = ofRandomf()*100.f; // particle z
                particlePosns[idx * 4 + 3] = 0.f; // dummy
            }
        }
        particles->loadDataTexture(GpuParticles::POSITION, particlePosns);
        delete[] particlePosns;
        
        // initial velocities
        particles->zeroDataTexture(GpuParticles::VELOCITY);
        
        // listen for update event to set additonal update uniforms
        particles->listener = this;
    }
    
    void loadShaders()
    {
        
        updateVert+= STRINGIFY(
                               in vec4  position;
                               in vec2  texcoord;
                               
                               out vec2 texCoordVarying;
                               uniform mat4 modelViewProjectionMatrix;

                               void main()
                               {
                                   texCoordVarying = texcoord;
                                   gl_Position = position;
                               }
                               
                               );
        
        updateFrag += STRINGIFY(
                                // ping pong inputs
                                uniform sampler2DRect particles0;
                                uniform sampler2DRect particles1;
                                
                                uniform vec3 mouse;
                                uniform vec3 gravity;
                                uniform float radiusSquared;
                                uniform float elapsed;
                                uniform float magnitudeFactor;
                                in vec2 texCoordVarying;
                                
                                layout(location = 0) out vec4 posOut;
                                layout(location = 1) out vec4 velOut;
                                
                                void main()
                                {
                                    vec3 pos = texture(particles0, texCoordVarying.st).xyz;
                                    vec3 vel = texture(particles1, texCoordVarying.st).xyz;
                                    
                                    // mouse attraction
                                    vec3 direction = mouse - pos.xyz;
                                    float distSquared = dot(direction, direction);
                                    float magnitude = magnitudeFactor * (1.0 - distSquared / radiusSquared);
                                    vec3 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);
                                    
                                    // gravity
                                    //force += vec3(0.0, -0.5, 0.0);
                                    force += gravity;
                                    // accelerate
                                    vel += elapsed * force;
                                    
                                    // bounce off the sides
                                    vel.x *= step(abs(pos.x), 512.0) * 2.0 - 1.0;
                                    vel.y *= step(abs(pos.y), 384.0) * 2.0 - 1.0;
                                    
                                    // damping
                                    vel *= 0.995;
                                    
                                    // move
                                    pos += elapsed * vel;
                                    
                                    posOut = vec4(pos, 1.0);
                                    velOut = vec4(vel, 0.0);
                                }
                                );
        
        drawVert += STRINGIFY(
                              
                              uniform mat4 modelViewProjectionMatrix;
                              uniform sampler2DRect particles0;
                              uniform sampler2DRect particles1;
                              
                              in vec4  position;
                              in vec2  texcoord;
                              
                              out VS_OUT {
                                  vec2 texCoordVarying;
                              } vs_out;
                              
                              void main()
                              {
                                  vs_out.texCoordVarying = texcoord;
                                  gl_Position = modelViewProjectionMatrix * vec4(texture(particles0, texcoord).xyz, 1.0);
                              });
        drawFrag += STRINGIFY(
                              
                              
                              in vec4 geomOutputColor;
                              in vec2 texCoordVarying;
                              
                              out vec4 fragColor;
                              
                              in vec2 geomTexCoordVarying;

                              uniform sampler2DRect particleTexture;
                              void main()
                              {
                                  fragColor = texture(particleTexture, geomTexCoordVarying)*geomOutputColor;
                              });
        
        string drawGeom = HEADER;
        drawGeom += STRINGIFY(
                              layout (points) in;
                              
                              layout (line_strip, max_vertices = 17) out;
                              uniform mat4 modelViewProjectionMatrix;

                              uniform vec4 particleColor;
                              uniform float geomSize;
                              
                              in VS_OUT {
                                  vec2 texCoordVarying;
                              } gs_in[];
                              
                              out vec4 geomOutputColor;
                              out vec2 geomTexCoordVarying;
                              
                              void main() { 
                                  
                                  vec4 position = gl_in[0].gl_Position;
                                  
                                  float n = geomSize;
                                  float n2 = geomSize;
                                  gl_Position = position + vec4(-n,  n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n,  n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n, -n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n, -n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n, -n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n, -n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n,  n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n,  n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n,  n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n,  n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n, -n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n,  n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(n, -n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n, -n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n,  n, -n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n, -n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position = position + vec4(-n,  n,  n, n2);
                                  geomTexCoordVarying = gs_in[0].texCoordVarying;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  /*
                                  vec4 NEU = position + vec4( n,  n,  n, 0.0);
                                  vec4 NED = position + vec4( n, -n,  n, 0.0);
                                  vec4 NWU = position + vec4( n,  n, -n, 0.0);
                                  vec4 NWD = position + vec4( n, -n, -n, 0.0);
                                  vec4 SEU = position + vec4(-n,  n,  n, 0.0);
                                  vec4 SED = position + vec4(-n, -n,  n, 0.0);
                                  vec4 SWU = position + vec4(-n,  n, -n, 0.0);
                                  vec4 SWD = position + vec4(-n, -n, -n, 0.0);
                                  
                                  // Create a cube centered on the given point.
                                  gl_Position =  NED;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NWD;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SWD;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SED;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SEU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SWU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NWU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NEU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NED;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SED;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SEU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NEU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NWU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  NWD;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SWD;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  
                                  gl_Position =  SWU;
                                  geomOutputColor = particleColor;
                                  EmitVertex();
                                  */
                                  EndPrimitive();
                              }
                              
                              
                              );
        
        updateShader.setupShaderFromSource(GL_VERTEX_SHADER, updateVert);
        updateShader.setupShaderFromSource(GL_FRAGMENT_SHADER, updateFrag);
        updateShader.bindDefaults();
        updateShader.linkProgram();
        
        
        drawShader.setGeometryInputType(GL_POINTS);
        drawShader.setGeometryOutputType(GL_LINE_STRIP);
        drawShader.setGeometryOutputCount(17);
        
        
        drawShader.setupShaderFromSource(GL_VERTEX_SHADER, drawVert);
        drawShader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, drawGeom);
        drawShader.setupShaderFromSource(GL_FRAGMENT_SHADER, drawFrag);
        drawShader.bindDefaults();
        drawShader.linkProgram();
        
    }
    
    
    void onParticlesUpdate()
    {
        
        updateShader.begin();
        particles->setUniforms(&updateShader);
        ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY() , 0.f);
        updateShader.setUniform3fv("mouse", mouse.getPtr());
        updateShader.setUniform3fv("gravity", gravity.getPtr());
        updateShader.setUniform1f("elapsed", ofGetLastFrameTime());
        updateShader.setUniform1f("radiusSquared", radius*radius);
        updateShader.setUniform1f("magnitudeFactor", magnitudeFactor);
        particles->quadMesh.draw();
        updateShader.end();
    }
    
    void draw()
    {
        //ofGetCurrentRenderer()->setBitmapTextMode(OF_BITMAPMODE_SCREEN);
        particleTexture.bind();
        drawShader.begin();
        particles->setUniforms(&drawShader);
        drawShader.setUniform1f("geomSize", geomSize);
        
        //drawShader.setUniformMatrix4f("modelViewProjectionMatrix", camera->getModelViewProjectionMatrix());
        drawShader.setUniformTexture("particleTexture", particleTexture, 1);

        drawShader.setUniform4f("particleColor",
                                particleColor.r,
                                particleColor.g,
                                particleColor.b,
                                particleColor.a);
        particles->mesh.drawWireframe();
        //particles->mesh.draw();
        drawShader.end();
        particleTexture.unbind();
    }
    
    
    
    
    
    
};