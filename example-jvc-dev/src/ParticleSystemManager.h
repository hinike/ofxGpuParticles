#pragma once

#include "ofMain.h"
#include "GpuParticleSystem.h"
#include "TexturedParticleSystem.h"
#include "ParticleSystem3D.h"

class ParticleSystemManager
{
public:
    
    
    vector<BaseParticleSystem*>particleSystems;
    BaseParticleSystem* currentSystem;
    int currentIndex;
    ParticleSystemManager()
    {
        currentSystem = NULL;
        currentIndex = 0;
    }
    
    
    
    void setup()
    {

        GpuParticleSystem* particleSystem = new GpuParticleSystem();
        particleSystem->setup();
        particleSystem->createParticles(1000, 1000);
        particleSystems.push_back(particleSystem);
        
        TexturedParticleSystem* texturedParticleSystem = new TexturedParticleSystem();
        texturedParticleSystem->setup();
        texturedParticleSystem->createParticles(texturedParticleSystem->particleTexture.getWidth(), texturedParticleSystem->particleTexture.getHeight());
        particleSystems.push_back(texturedParticleSystem);
        
    
        ParticleSystem3D* particleSystem3d = new ParticleSystem3D();
        particleSystem3d->setup();
        particleSystem3d->createParticles(particleSystem3d->particleTexture.getWidth(), particleSystem3d->particleTexture.getHeight());
        particleSystems.push_back(particleSystem3d);
        
        currentIndex = 0;
        
        currentSystem = particleSystems[currentIndex];
    }
    
    void nextSystem()
    {
        if(currentIndex+1 < particleSystems.size())
        {
            currentIndex++;
        }else
        {
            currentIndex = 0;
        }
        currentSystem = particleSystems[currentIndex];
    }
    
    void update()
    {
        if(!currentSystem) return;
        
        currentSystem->update();
        
    }
    
    
    void draw()
    {
        if(!currentSystem) return;
        currentSystem->draw();
       
    }
    
    
};