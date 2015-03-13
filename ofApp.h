#pragma once

#include "ofMain.h"
#include <queue>
#include "ofxBox2d.h"

using namespace WindowsPreview::Kinect;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

class CustomParticle : public ofxBox2dCircle
{
public:
	CustomParticle(ofColor color) : color(color) {}
	ofColor color;

	void draw()
	{
		ofPushMatrix();
		float radius = getRadius();
		ofTranslate(getPosition().x, getPosition().y, 0);
		ofSetColor(color.r, color.g, color.b, color.a);
		ofFill();
		ofCircle(0, 0, radius);
		ofPopMatrix();
	}
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void DrawLine(const ofColor& col, const ofPolyline& line);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void createCircle(float x, float y);

private:
	KinectSensor^ _kinect;
	MultiSourceFrameReader^ _multiFrameReader;
	ofTexture _tex;
	ofImage _img;
	Array<byte>^ _colorPixels;

	Vector<Body^>^ _bodies;
	bool _bodiesUpdated;

	CoordinateMapper^ _coordinateMapper;
	FrameDescription^ _colorFrameDescription;

	unsigned int _bytesPerPixel;
	bool _colorFrameProcessed;

	ofxBox2d _box2dWorld;
	vector<ofPtr<ofxBox2dCircle>> _circles;
};
