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
    
    
    string HEADER = "#version 330\n";
    string updateVert = HEADER;
    string updateFrag = HEADER;
    string drawVert = HEADER;
    string drawFrag = HEADER;
    int particleWidth = 0;
    int particleHeight = 0;
    
    int resetWidth =0;
    int resetHeight = 0;
    BaseParticleSystem()
    {
        magnitudeFactor = 0;
        radius = 0;
        particles = NULL;        
    }
    
    void compileShaders()
    {
        updateShader.setupShaderFromSource(GL_VERTEX_SHADER, updateVert);
        updateShader.setupShaderFromSource(GL_FRAGMENT_SHADER, updateFrag);
        updateShader.bindDefaults();
        updateShader.linkProgram();
        
        drawShader.setupShaderFromSource(GL_VERTEX_SHADER, drawVert);
        drawShader.setupShaderFromSource(GL_FRAGMENT_SHADER, drawFrag);
        drawShader.bindDefaults();
        drawShader.linkProgram();
    }
    
    void update()
    {
        particles->update();
    }
    
    void initParticles(int w, int h)
    {
        particleWidth = w;
        particleHeight = h;
        resetWidth = w;
        resetHeight = h;
        if(particles)
        {
            delete particles;
        }
        particles = new GpuParticles();
        particles->updateShader = &updateShader;
        particles->drawShader = &drawShader;
        particles->init(particleWidth, particleHeight);
    }
    
    virtual void setup()=0;
    virtual void createParticles(int width, int height)=0;
    virtual void draw()=0;
    virtual void loadShaders()=0;
    virtual void onParticlesUpdate()=0;
    
};