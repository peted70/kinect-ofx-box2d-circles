#include "ofApp.h"
#include <winbase.h>

//--------------------------------------------------------------
void ofApp::setup()
{
	_kinect = KinectSensor::GetDefault();
	if (_kinect != nullptr)
	{
		_multiFrameReader = _kinect->OpenMultiSourceFrameReader(FrameSourceTypes::Color | FrameSourceTypes::Body);

		_colorFrameDescription = _kinect->ColorFrameSource->CreateFrameDescription(ColorImageFormat::Rgba);
		_coordinateMapper = _kinect->CoordinateMapper;

		// rgba is 4 bytes per pixel
		_bytesPerPixel = _colorFrameDescription->BytesPerPixel;

		// allocate space to put the pixels to be rendered
		_colorPixels = ref new Array<byte>(_colorFrameDescription->Width * _colorFrameDescription->Height * _bytesPerPixel);

		_bodies = ref new Vector<Body^>(_kinect->BodyFrameSource->BodyCount);
		_kinect->Open();

	}

	_box2dWorld.init();
	float w = ofGetWidth();
	float h = ofGetHeight();

	_box2dWorld.createBounds(0.0f, 0.0f, w, h);
	_box2dWorld.setGravity(0, 1.0f);
	_box2dWorld.createGround(0.0f, h - 300.0f, w, h - 300.0f);

	//////////////////////////////////////////////////////////////////////
	b2BodyDef bd;
	auto wallleft = _box2dWorld.world->CreateBody(&bd);

	b2EdgeShape shape;
	shape.Set(b2Vec2(0.0f / OFX_BOX2D_SCALE, 0.0f / OFX_BOX2D_SCALE), b2Vec2(10.0f / OFX_BOX2D_SCALE, h / OFX_BOX2D_SCALE));
	wallleft->CreateFixture(&shape, 0.0f);

	b2BodyDef bd2;
	auto wallRight = _box2dWorld.world->CreateBody(&bd2);

	b2EdgeShape shape2;
	shape2.Set(b2Vec2(w / OFX_BOX2D_SCALE, h / OFX_BOX2D_SCALE), b2Vec2((w - 10.0f) / OFX_BOX2D_SCALE, 0.0f / OFX_BOX2D_SCALE));
	wallRight->CreateFixture(&shape2, 0.0f);

	_box2dWorld.setFPS(30.0);
	_box2dWorld.registerGrabbing();

	float32 timeStep = 1 / 20.0;    
	int32 velocityIterations = 8;   
	int32 positionIterations = 3;   
	_box2dWorld.world->Step(timeStep, velocityIterations, positionIterations);

	ofSetVerticalSync(true);
	ofBackgroundHex(0xfdefc2);
	ofSetLogLevel(OF_LOG_NOTICE);

	for (int i = 0; i < 30; i++)
	{
		createCircle(ofRandom(30, 1800), 0);
	}
}

void ofApp::exit()
{
	_kinect->Close();
}

//--------------------------------------------------------------
void ofApp::update()
{
	_box2dWorld.update();
	// remove shapes offscreen
	ofRemove(_circles, ofxBox2dBaseShape::shouldRemoveOffScreen);

	// Pull the frame…
  {
        auto multiFrame = _multiFrameReader->AcquireLatestFrame();
        if (multiFrame != nullptr)
	    {
	        if (multiFrame->ColorFrameReference != nullptr)
		    {
		        auto colorFrame = multiFrame->ColorFrameReference->AcquireFrame();
		
			    if (colorFrame != nullptr)
			    {
			        colorFrame->CopyConvertedFrameDataToArray(_colorPixels, ColorImageFormat::Rgba);
				    _img.setFromPixels(_colorPixels->Data, colorFrame->FrameDescription->Width, colorFrame->FrameDescription->Height, ofImageType::OF_IMAGE_COLOR_ALPHA);
			        _colorFrameProcessed = true;
			    }
		    }

			if (multiFrame->BodyFrameReference != nullptr)
			{
				auto bodyFrame = multiFrame->BodyFrameReference->AcquireFrame();

				if (bodyFrame != nullptr)
				{
					bodyFrame->GetAndRefreshBodyData(_bodies);
					_bodiesUpdated = true;
				}
			}
	    }

		if (_bodiesUpdated)
		{
			for (auto body : _bodies)
			{
				if (body->IsTracked)
				{
					auto hl = body->Joints->Lookup(JointType::HandLeft);
					auto csp1 = _coordinateMapper->MapCameraPointToColorSpace(hl.Position);
					auto h2 = body->Joints->Lookup(JointType::HandRight);
					auto csp2 = _coordinateMapper->MapCameraPointToColorSpace(hl.Position);
					for (int i = 0; i<_circles.size(); i++)
					{
						_circles[i].get()->addAttractionPoint(ofVec2f(csp1.X, csp1.Y), 3.0f);
						_circles[i].get()->addAttractionPoint(ofVec2f(csp2.X, csp2.Y), 3.0f);
					}
				}
			}
		}
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(128);

	if (!_kinect->IsAvailable)
	{
		ofClear(255, 0, 0);
		ofSetColor(255, 255, 255);
		ofDrawBitmapString("No Kinect", 50, 10);
	}

	ofSetColor(255, 255, 255);
	_img.draw(0, 0);

	for (int i = 0; i<_circles.size(); i++)
	{
		_circles[i].get()->draw();
	}

	_box2dWorld.drawGround();
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'b') 
	{
		createCircle(mouseX, mouseY);
	}
}

void ofApp::createCircle(float x, float y)
{
	float w = ofRandom(10, 60);
	ofColor col(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255), 150);
	_circles.push_back(ofPtr<ofxBox2dCircle>(new CustomParticle(col)));
	_circles.back().get()->setPhysics(3.0, 0.83, 0.1);
	_circles.back().get()->setup(_box2dWorld.getWorld(), x, y, w);
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
