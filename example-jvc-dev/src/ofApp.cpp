#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup()
{
    ofBackground(0);
    ofSetFrameRate(60);
    
    doControlPanelDraw = false;
    
    particlesController.setup();
    
    controlPanel.particlesController = &particlesController;
    controlPanel.cam = &cam;

    
    
    //cam.disableMouseInput();
}

//--------------------------------------------------------------
void ofApp::update()
{
    particlesController.update();
}



//--------------------------------------------------------------
void ofApp::draw()
{
    cam.begin();
    
    particlesController.draw();
    cam.end();
    
    if(doControlPanelDraw)
    {
        controlPanel.draw();

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'g')
    {
        doControlPanelDraw = !doControlPanelDraw;
    }
    if(key == '1')
    {
        cam.reset();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}