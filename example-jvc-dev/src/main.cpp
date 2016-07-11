#include "ofMain.h"
#include "ofApp.h"

int main()
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofGLFWWindowSettings settings;
    settings.setGLVersion(4, 1);
    settings.width = 1024;
    settings.height = 768;
    ofCreateWindow(settings);
    ofRunApp(new ofApp());
}
