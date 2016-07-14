#pragma once
#include "ofMain.h"
#include "BaseParticleSystem.h"

class MyParticleSystem : public BaseParticleSystem
{
public:
    
    
    ofTexture particleTexture;

    MyParticleSystem()
    {
        magnitudeFactor = 500.f;
        radius = 200.f;
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
        
        // initial positions
        // use new to allocate 4,000,000 floats on the heap rather than
        // the stack
        
        
        vector<float> positions;
        positions.reserve(w * h * 4);
        
        for (unsigned y = 0; y < h; ++y)
        {
            for (unsigned x = 0; x < w; ++x)
            {
                unsigned idx = y * w + x;
                positions[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
                positions[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
                positions[idx * 4 + 2] = 0.f; // particle z
                positions[idx * 4 + 3] = 0.f; // dummy
            }
        }
        particles->loadDataTexture(GpuParticles::POSITION, &positions[0]);
        
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
                              
                              out vec2 texCoordVarying;
                              
                              void main()
                              {
                                  texCoordVarying = texcoord;
                                  gl_Position = modelViewProjectionMatrix * vec4(texture(particles0, texCoordVarying).xyz, 1.0);
                              });
        drawFrag += STRINGIFY(
                              
                              
                              uniform vec4 globalColor;
                              in vec2 texCoordVarying;
                              uniform sampler2DRect particleTexture;

                              out vec4 fragColor;
                              
                              
                              void main()
                              {
                                  fragColor = texture(particleTexture, texCoordVarying)*globalColor;
                              });
        
        compileShaders();
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
        particleTexture.bind();

        drawShader.begin();
        ofColor globalColor(ofColor::red);
        ofSetColor(globalColor);
        particles->setUniforms(&drawShader);
        drawShader.setUniformTexture("particleTexture", particleTexture, 1);

        particles->mesh.draw();
        
        drawShader.end();
        particleTexture.unbind();
    }
    

    
};