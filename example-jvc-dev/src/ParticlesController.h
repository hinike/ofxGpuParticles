#pragma once

#include "ofMain.h"
#include "GpuParticles.h"

class ParticlesController : public GpuParticlesListener
{
public:
    
    GpuParticles* particles;
    float magnitudeFactor;
    float radiusSquared;
    ofVec3f gravity;
    
    ofTexture particleTexture;
    
    ParticlesController()
    {
        magnitudeFactor = 500.f;
        radiusSquared = 40000.f;
        gravity.set(0.0, -1000.5, 0.0);
        particles = NULL;
        ofLoadImage(particleTexture, "of.png");

    }
    void createParticles(int w=100, int h=1000)
    {
        if(particles)
        {
            delete particles;
        }
        particles = new GpuParticles();
        particles->loadShaders();
        //"shaders330/update", "shaders330/draw");
        particles->init(w, h);
        
        
        // initial positions
        // use new to allocate 4,000,000 floats on the heap rather than
        // the stack
        float* particlePosns = new float[w * h * 4];
        for (unsigned y = 0; y < h; ++y)
        {
            for (unsigned x = 0; x < w; ++x)
            {
                unsigned idx = y * w + x;
                particlePosns[idx * 4] = 400.f * x / (float)w - 200.f; // particle x
                particlePosns[idx * 4 + 1] = 400.f * y / (float)h - 200.f; // particle y
                particlePosns[idx * 4 + 2] = 0.f; // particle z
                particlePosns[idx * 4 + 3] = 0.f; // dummy
            }
        }
        particles->loadDataTexture(GpuParticles::POSITION, particlePosns);
        delete[] particlePosns;
        
        // initial velocities
       particles->zeroDataTexture(GpuParticles::VELOCITY);
        
        // listen for update event to set additonal update uniforms
        particles->updateListener = this;
        particles->drawListener = this;

    
    }
    void setup()
    {

        
       
    }
    
    void update()
    {
        if(!particles) return;
        
        particles->update();
        
    }
    void onParticlesDraw(GpuParticles* particleSystem)
    {
        ofColor particleColor(ofColor::red);
        ofSetColor(particleColor);
        particleTexture.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    
    void draw()
    {
        if(!particles) return;
        
        ofPushStyle();
        particles->draw();
        ofPopStyle();
    }
    
    // set any update uniforms in this function
    void onParticlesUpdate(GpuParticles* particleSystem)
    {
        ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY() , 0.f);
        particleSystem->updateShader.setUniform3fv("mouse", mouse.getPtr());
        particleSystem->updateShader.setUniform3fv("gravity", gravity.getPtr());
        particleSystem->updateShader.setUniform1f("elapsed", ofGetLastFrameTime());
        particleSystem->updateShader.setUniform1f("radiusSquared", radiusSquared);
        particleSystem->updateShader.setUniform1f("magnitudeFactor", magnitudeFactor);
        
        
    }
};