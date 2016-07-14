#pragma once
#include "ofMain.h"
#include "ofxImGui.h"
#include "ParticleSystemManager.h"

class ControlPanel
{
public:
    
    ofxImGui gui;
    ParticleSystemManager* particleSystemManager;
    
    
    ofEasyCam* cam;
    bool camDisableMouseInput;
    
    bool showParticlesWindow;
    int particlesWidth;
    int particlesHeight;
    ControlPanel()
    {
        showParticlesWindow = true;
        particleSystemManager = NULL;
        camDisableMouseInput = false;
        particlesWidth = 1000;
        particlesHeight = 1000;
        gui.setup();
    }
    
    
    void draw()
    {
        if(!particleSystemManager->currentSystem) return;
        BaseParticleSystem* currentSystem = particleSystemManager->currentSystem;
        
        particlesWidth = currentSystem->particleWidth;
        particlesHeight = currentSystem->particleHeight;
        gui.begin();
        
        if(!ImGui::Begin("Particles", &showParticlesWindow))
        {
            ImGui::End();
        }else
        {
            if(ImGui::SliderInt("particlesWidth", &particlesWidth, 100, 5000))
            {
                currentSystem->createParticles(particlesWidth, particlesHeight);
            }
            
            if(ImGui::SliderInt("particlesHeight", &particlesHeight, 100, 5000))
            {
                currentSystem->createParticles(particlesWidth, particlesHeight);
            }
            
            
            ImGui::SliderFloat("magnitudeFactor", &currentSystem->magnitudeFactor, 0.0f, 1000.0f);
            ImGui::SliderFloat("radius", &currentSystem->radius, 0.0f, 400.0f);

            ImGui::SliderFloat("gravity X", &currentSystem->gravity.x, -1000.0f, 1000.0f);
            ImGui::SliderFloat("gravity Y", &currentSystem->gravity.y, -1000.0f, 1000.0f);
            ImGui::SliderFloat("gravity Z", &currentSystem->gravity.z, -1000.0f, 1000.0f);
            
            if(ImGui::Button("Reset Particles"))
            {
                currentSystem->createParticles(currentSystem->resetWidth, currentSystem->resetHeight);
            }
            
            if(ImGui::Button("reset Gravity"))
            {
                currentSystem->gravity.set(0, 0, 0);
            }
            
            
            if(ImGui::Button("Toggle Cam Mouse"))
            {
                camDisableMouseInput = !camDisableMouseInput;
                if(camDisableMouseInput)
                {
                    cam->disableMouseInput();
                    cam->reset();
                }else
                {
                    cam->enableMouseInput();
                }
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }
        gui.end();
        
    }
};

