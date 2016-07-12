/*
 *  GpuParticles.h
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#pragma once

#include "ofMain.h"

class GpuParticles;
class GpuParticlesListener
{
public:
    virtual void onParticlesUpdate(GpuParticles* particles)=0;
    virtual void onParticlesDraw(GpuParticles* particles)=0;
    
};

/**
 * For now, only uses RGBA and TEXTURE_RECTANGLE to make usage simpler
 */
class GpuParticles
{
public:
    static const unsigned FLOATS_PER_TEXEL = 4;
    
    // you don't have to use these but makes
    // code more readable
    enum DataTextureIndex
    {
        POSITION,
        VELOCITY
    };
    
    
    GpuParticlesListener* updateListener;
    GpuParticlesListener* drawListener;
    const string UPDATE_SHADER_NAME = "update";
    const string DRAW_SHADER_NAME = "draw";
    
    ofFbo fbos[2];
    ofVboMesh mesh;
    ofVboMesh quadMesh;
    ofShader updateShader, drawShader;
    unsigned currentReadFbo;
    unsigned textureLocation;
    unsigned width, height, numFloats;
    
    GpuParticles()
    {
        updateListener = NULL;
        drawListener = NULL;
        currentReadFbo = 0;
        textureLocation = 0;
    }
    
    void init(unsigned width, unsigned height,
              ofPrimitiveMode primitive = OF_PRIMITIVE_POINTS,
              bool loadDefaultShaders = false,
              int numDataTextures = 2)
    {
        this->width = width;
        this->height = height;
        numFloats = width * height * FLOATS_PER_TEXEL;
        
        // fbos
        ofFbo::Settings s;
        s.internalformat = GL_RGBA32F_ARB;
        s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        s.minFilter = GL_NEAREST;
        s.maxFilter = GL_NEAREST;
        s.wrapModeHorizontal = GL_CLAMP;
        s.wrapModeVertical = GL_CLAMP;
        s.width = width;
        s.height = height;
        s.numColorbuffers = numDataTextures;
        for (unsigned i = 0; i < 2; ++i)
        {
            fbos[i].allocate(s);
        }
        
        // mesh
        mesh.clear();
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                mesh.addVertex(ofVec3f(200.f * x / (float)width - 100.f, 200.f * y / (float)height - 100.f, -500.f));
                mesh.addTexCoord(ofVec2f(x, y));
            }
        }
        mesh.setMode(primitive);
        
        quadMesh.addVertex(ofVec3f(-1.f, -1.f, 0.f));
        quadMesh.addVertex(ofVec3f(1.f, -1.f, 0.f));
        quadMesh.addVertex(ofVec3f(1.f, 1.f, 0.f));
        quadMesh.addVertex(ofVec3f(-1.f, 1.f, 0.f));
        
        quadMesh.addTexCoord(ofVec2f(0.f, 0.f));
        quadMesh.addTexCoord(ofVec2f(width, 0.f));
        quadMesh.addTexCoord(ofVec2f(width, height));
        quadMesh.addTexCoord(ofVec2f(0.f, height));
        
        quadMesh.addIndex(0);
        quadMesh.addIndex(1);
        quadMesh.addIndex(2);
        quadMesh.addIndex(0);
        quadMesh.addIndex(2);
        quadMesh.addIndex(3);
        
        quadMesh.setMode(OF_PRIMITIVE_TRIANGLES);
        
        // shaders
        if (loadDefaultShaders)
        {
            updateShader.load(UPDATE_SHADER_NAME);
            drawShader.load(DRAW_SHADER_NAME);
        }
    }
#if 0
    void loadShaders(const string& updateShaderName, const string& drawShaderName)
    {
        updateShader.load(updateShaderName);
        drawShader.load(drawShaderName);
    }
#endif
#define STRINGIFY(A) #A
    void loadShaders()
    {
        string HEADER = "#version 330\n";
        string updateVert = HEADER;
        string updateFrag = HEADER;
        string drawVert = HEADER;
        string drawFrag = HEADER;
        
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
                              uniform sampler2DRect tex0;
                              in vec2 texCoordVarying;
                              
                              out vec4 fragColor;
                              
                              
                              void main()
                              {
                                  //fragColor = texture(tex0, texCoordVarying)*globalColor;
                                  fragColor = texture(tex0, texCoordVarying)*vec4(1.0, 1.0, 1.0, 0.2);
                                  //fragColor = globalColor;
                                  
                              });
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
        
        fbos[1 - currentReadFbo].begin(false);
        glPushAttrib(GL_ENABLE_BIT);
        // we set up no camera model and ignore the modelview and projection matrices
        // in the vertex shader, we make a viewport large enought to ensure the shader
        // is executed for each pixel
        glViewport(0, 0, width, height);
        glDisable(GL_BLEND);
        fbos[1 - currentReadFbo].activateAllDrawBuffers();
        
        updateShader.begin();
        
        if(updateListener)
        {
            updateListener->onParticlesUpdate(this);
        }
        setUniforms(updateShader);
        quadMesh.draw();
        updateShader.end();
        glPopAttrib();
        
        fbos[1 - currentReadFbo].end();
        
        currentReadFbo = 1 - currentReadFbo;
    }
    
    void draw()
    {

        drawShader.begin();
        if(drawListener)
        {
            drawListener->onParticlesDraw(this);
        }
        setUniforms(drawShader);
        mesh.draw();
        drawShader.end();

    }
    
    void setUniforms(ofShader& shader)
    {
        for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
        {
            ostringstream oss;
            oss << "particles" << ofToString(i);
            //ofLogVerbose() << "oss.str(): " << oss.str();
            
            shader.setUniformTexture(oss.str().c_str(), fbos[currentReadFbo].getTexture(i), i + textureLocation);
        }
    }
    
    void loadDataTexture(int idx, float* data,
                         int x = 0, int y = 0, int width = 0, int height = 0)
    {
        if (idx < fbos[currentReadFbo].getNumTextures())
        {
            if (!width) width = this->width;
            if (!height) height = this->height;
            fbos[currentReadFbo].getTexture(idx).bind();
            glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
            fbos[currentReadFbo].getTexture(idx).unbind();
        }
        else ofLogError() << "Trying to load data from array into non-existent buffer.";
    }
    
    void zeroDataTexture(int idx,
                         int x = 0, int y = 0, int width = 0, int height = 0)
    {
        if (!width) width = this->width;
        if (!height) height = this->height;
        float* zeroes = new float[width * height * FLOATS_PER_TEXEL];
        memset(zeroes, 0, sizeof(float) * width * height * FLOATS_PER_TEXEL);
        loadDataTexture(idx, zeroes, x, y, width, height);
        delete[] zeroes;
    }
   
};

