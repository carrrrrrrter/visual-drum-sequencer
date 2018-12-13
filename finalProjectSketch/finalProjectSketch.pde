/*
Visualizer for Digital Electronics final project, drum sequencer
 - When you change to a new channel, color of sketch will change (Kick/red, snare/yellow,
 HiHat/green, crash/yellow)
 - Every time the kick hits, circles get bigger and then will reset once the screen is
 filled
 */
import processing.serial.*;

Serial myPort;
int size = 5;
int maxCount = 47;
int lastVal;
int x = 0;
int y = 0;


void setup() { 
  //Open serial port for Arduino communication
  myPort = new Serial(this, Serial.list()[4], 9600);
  size(800, 600);
  init();
}

void init() {
  fill(255, 0, 0);
  background(0);
  noStroke(); //for non-overlap of circles
  size = 5;
}

void init2() {
  fill (255, 0, 0);
}

void draw() {
  if (myPort.available() > 0) {

    colorChannels(myPort);

    for (int y=0; y<height; y+=33.34) {
      for (int x=0; x<width; x+=33.34) {
        ellipse(x, y, size, size);
        if (myPort.read() == 10) {
          size+=6;
        }
      }
    }
  }

  if (size >= maxCount) init();
}

void colorChannels(Serial myPort) {
  if (myPort.read() == 0) {
    fill(255, 0, 0); //red for kick
    lastVal = 0;
  }
  if (myPort.read() == 1) {
    fill(255, 255, 0); //yellow for snare
  }
  if (myPort.read() == 2) {
    fill(0, 0, 255); //blue for hihat
  }
  if (myPort.read() == 3) {
    fill(0, 255, 0); //green for crash
  }
}
