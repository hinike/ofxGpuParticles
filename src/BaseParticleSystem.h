#pragma once
#include "ofMain.h"
#include "GpuParticles.h"

#define STRINGIFY(A) #A
class BaseParticleSystem : public GpuParticlesListener
{
public:
    
    ofShader updateShader;
    ofShader drawShader;
    
    GpuParticles* particles;
    float magnitudeFactor;
    float radius;
    ofVec3f gravity;
    
    BaseParticleSystem()
    {
        magnitudeFactor = 0;
        radius = 0;
        particles = NULL;        
    }
    
    virtual void setup()=0;
    virtual void createParticles(int width, int height)=0;
    virtual void update()=0;
    virtual void draw()=0;
    virtual void loadShaders()=0;
    virtual void onParticlesUpdate()=0;
    //virtual void onParticlesDraw()=0;
    
};