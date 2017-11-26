
/*
Libraries used: 
Leo Zimmerman's ofxFilterbank
https://github.com/leozimmerman/ofxFilterbank



*/

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxFilterbank.h"
#include "ofxOsc.h"
#include "ofSerial.h"
#include "ofxMaxim.h"
#include "ofxDatGui.h"
#include <queue>

#define BANDWITH  1.0
#define BUFFER_SIZE 512
#define LIN2dB(x) (double)(20. * log10(x))
#define SR 44100
#define FRAMERATE 30

#define PORT 12345

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioIn(float * input, int bufferSize, int nChannels);
        void audioOut(float * input, int bufferSize, int nChannels);
        

        // ------ OSC receiver
        ofxOscReceiver receiver;

        // ------ Utils
        void playSequence();
        void shrinkNotes();
        double avg2(std::vector<int> const& v);
        int vectorRdm(vector<int> notes);
        int vectorPlusGros(vector<int> notes);

        //------- Mike Input
		ofSoundStream soundStreamIn;
		int inChan, outChan;
        ofxFilterbank filterBank;
		
        // ------ OfxMaxim
        struct Key {
            maxiOsc oscillator, modulator;
            maxiEnv env;
            double frequency, modulation, volume;

            Key(double _f, double _v){
                frequency = _f;
                volume = _v;
                env.setAttack(1000);
                env.setDecay(1);
                env.setSustain(1);
                env.setRelease(2000);
            }

            Key(double _f, double _v, double _m){
                frequency = _f;
                modulation = _m;
                volume = _v;
                env.setAttack(100);
                env.setDecay(10);
                env.setSustain(1);
                env.setRelease(500);
            }

            // Hardcoding the types, for now, i don't need all the possibilities.
            inline double play(unsigned int playType) {

                double ret;
                switch(playType){
                case 0:
                    ret = env.adsr(oscillator.square(frequency), env.trigger) * volume;
                    break;
                case 1:
                    ret = env.adsr(oscillator.sinewave(frequency) * modulator.sinewave(modulation), env.trigger) * volume;
                    break;
                default:
                    ret = env.adsr(oscillator.sinewave(frequency), env.trigger) * volume;

                }
                return ret;
            }

            inline void on() {

                env.trigger = 1;
            }
            inline void off() {
                env.trigger = 0;
            }
            void freq(double f){
                this->frequency = f;
            }
            void vol(double v){
                this->volume = v;
            }
            void release(int r){
                env.setRelease(r);
            }


        };

        maxiMix mix;

        deque<unique_ptr<Key>> keys;
        deque<unique_ptr<Key>> backGroundNoise;
        deque<unique_ptr<Key>> memoryNoise;
        double currentFrame, currentVolume, modulationIndex, modulationFrequency;
		double outputs[2], modSpeed;

		int	sampleRate;
		int bufferSize;
        int framerate = FRAMERATE;





        // ------
		bool sPressed = false;
        bool bPressed = false;

		// Stock all notes
        queue<vector<int>> notesInTime;
        vector <vector<int>> global;
		

	    bool isPlaying = false;
	    int myByte = 0;

};

#endif

