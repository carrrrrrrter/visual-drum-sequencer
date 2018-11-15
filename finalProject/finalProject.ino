#include <LiquidCrystal.h>

LiquidCrystal lcd(27, 28, 29, 30, 31, 32);
//Initialize 8-step pins, and 8 LED pins
int ledPinArray[8] = {0, 1, 2, 3, 7, 8, 9, 10};
int stepButtonPins[8] = {23, 22, 21, 20, 19, 18, 17, 16};

//Initialize pins for 4 channels
int kickChannel = 39;
int snareChannel = 38;
int hihatChannel = 37;
int crashChannel = 36;

//Initialize LEDs for channels
int kickLED = 11;
int snareLED = 12;
int hihatLED = 24;
int crashLED = 25;

int midiNotes[4] = {40, 42, 45, 47};
int currentStep = 0;
int channelDisplayed = 0;
unsigned long lastStepTime = 0;

boolean lastButtonState[8] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
boolean buttonState[8] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

boolean lastKickState = LOW;
boolean kickState = LOW;
boolean lastSnareState = LOW;
boolean snareState = LOW;
boolean lastHihatState = LOW;
boolean hihatState = LOW;
boolean lastCrashState = LOW;
boolean crashState = LOW;

//Set default sequence for all channels
boolean on[4][8] = {
  {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH},
  {HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW},
  {LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},
  {HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW}
};

void setup() {
  lcd.begin(16, 2);
  for (int i = 0; i < 8; i++) {
    pinMode(ledPinArray[i], OUTPUT);
    pinMode(stepButtonPins[i], INPUT);
  }
  pinMode(kickChannel, INPUT);
  pinMode(snareChannel, INPUT);
  pinMode(hihatChannel, INPUT);
  pinMode(crashChannel, INPUT);
  pinMode(kickLED, OUTPUT);
  pinMode(snareLED, OUTPUT);
  pinMode(hihatLED, OUTPUT);
  pinMode(crashLED, OUTPUT);
}

void loop() {
  displayChannel();
  displayTempo();
  sequence();
  checkButtons();
  setLeds();
}

void displayTempo() {
  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");
  lcd.setCursor(7, 1);
  int tempo = analogRead(A22);
  int newTempo = map(tempo, 0, 1023, 200, 0);
  lcd.print(newTempo);
  if (newTempo < 100) {
    lcd.print(newTempo);
    lcd.setCursor(9, 1);
    lcd.print("           ");
  }
}

void sequence() {
  //Pot to set tempo
  int tempo = analogRead(A22);
  if (millis() > lastStepTime + tempo) {

    //previous step off
    //    digitalWrite(ledPinArray[currentStep], LOW);
    incrementUp();
    lastStepTime = millis();

    //    digitalWrite(ledPinArray[currentStep], HIGH);

    for (int i = 0; i < 8; i++) {
      if (on[i][currentStep] == true) {
        usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}

void checkButtons() {
  if (channelDisplayed <= 3) {
    for (int i = 0; i < 8; i++) {
      lastButtonState[i] = buttonState[i];
      buttonState[i] = digitalRead(stepButtonPins[i]);

      lastKickState = kickState;
      kickState = digitalRead(kickChannel);
      lastSnareState = snareState;
      snareState = digitalRead(snareChannel);
      lastHihatState = hihatState;
      hihatState = digitalRead(hihatChannel);
      lastCrashState = crashState;
      crashState = digitalRead(crashChannel);

      if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
        if (on[channelDisplayed][i] == false)
          on[channelDisplayed][i] = true;
        else if (on[channelDisplayed][i] == true)
          on[channelDisplayed][i] = false;
      }
      if (kickState == HIGH && lastKickState == LOW) {
        channelDisplayed = 0;
      }
      if (snareState == HIGH && lastSnareState == LOW) {
        channelDisplayed = 1;
      }
      if (hihatState == HIGH && lastHihatState == LOW) {
        channelDisplayed = 2;
      }
      if (crashState == HIGH && lastCrashState == LOW) {
        channelDisplayed = 3;
      }

      //Reset to beginning of sequence
      if (channelDisplayed == 7)
        channelDisplayed = 0;
    }
  }
}

void setLeds() {
  for (int i = 0; i < 8; i++) {
    if (on[channelDisplayed][i] == true || i == currentStep) {
      digitalWrite(ledPinArray[i], HIGH);
      //      digitalWrite(ledPinArray[currentStep], HIGH);

    }
    else if (on[channelDisplayed][i] == false) {
      digitalWrite(ledPinArray[i], LOW);
      //      digitalWrite(ledPinArray[currentStep], HIGH);
    }
  }
}

//Step through sequence
void incrementUp() {
  currentStep++;
  if (currentStep > 7)
    currentStep = 0;
}

void displayChannel() {
  lcd.setCursor(0, 0);
  if (channelDisplayed == 0) {
    lcd.print("Kick  ");
    digitalWrite(kickLED, HIGH);
  }
  if (channelDisplayed == 1) {
    lcd.print("Snare ");
    digitalWrite(snareLED, HIGH);
  }
  if (channelDisplayed == 2) {
    lcd.print("Hi-Hat ");
    digitalWrite(hihatLED, HIGH);
  }
  if (channelDisplayed == 3) {
    lcd.print("Crash ");
    digitalWrite(crashLED, HIGH);
  }
}
