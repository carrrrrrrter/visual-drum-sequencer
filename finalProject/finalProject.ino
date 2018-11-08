int ledPinArray[8] = {0, 1, 2, 3, 7, 8, 9, 10};
int stepButtonPins[8] = {23, 22, 21, 20, 19, 18, 17, 16};
int kickChannel = 39;
int snareChannel = 38;
int hihatChannel = 37;
int crashChannel = 36;
int midiNotes[4] = {40, 42, 45, 47};
int currentStep = 0;
int channelDisplayed = 0;
unsigned long lastStepTime = 0;
boolean lastButtonState[8] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
boolean buttonState[8] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};
boolean lastButtonState2 = LOW;
boolean buttonState2 = LOW;
boolean lastButtonState3 = LOW;
boolean buttonState3 = LOW;
boolean lastButtonState4 = LOW;
boolean buttonState4 = LOW;
boolean lastButtonState5 = LOW;
boolean buttonState5 = LOW;
boolean on[4][8] = {
  {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH},
  {HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW},
  {LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH},
  {HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW}
};

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(ledPinArray[i], OUTPUT);
    pinMode(stepButtonPins[i], INPUT);
  }
  pinMode(kickChannel, INPUT);
  pinMode(snareChannel, INPUT);
  pinMode(hihatChannel, INPUT);
  pinMode(crashChannel, INPUT);

  //pinMode(nextChannelButtonPin, INPUT);
  //pinMode(prevChannelButtonPin, INPUT);
  //pinMode(switchPin, INPUT);
}

void loop() {
  sequence();
  checkButtons();
  setLeds();
}

void sequence() {
  int tempo = analogRead(A22);

  if (millis() > lastStepTime + tempo) {

    //previous step off
    digitalWrite(ledPinArray[currentStep], LOW);

    incrementUp();
    lastStepTime = millis();

    digitalWrite(ledPinArray[currentStep], HIGH);

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

      lastButtonState2 = buttonState2;
      buttonState2 = digitalRead(kickChannel);
      lastButtonState3 = buttonState3;
      buttonState3 = digitalRead(snareChannel);
      lastButtonState4 = buttonState4;
      buttonState4 = digitalRead(hihatChannel);
      lastButtonState5 = buttonState5;
      buttonState5 = digitalRead(crashChannel);

      if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
        if (on[channelDisplayed][i] == false)
          on[channelDisplayed][i] = true;
        else if (on[channelDisplayed][i] == true)
          on[channelDisplayed][i] = false;
      }

      if (buttonState2 == HIGH && lastButtonState2 == LOW) {
        channelDisplayed = 0;
      }
      if (buttonState3 == HIGH && lastButtonState3 == LOW) {
        channelDisplayed = 1;
      }
      if (buttonState4 == HIGH && lastButtonState4 == LOW) {
        channelDisplayed = 2;
      }
      if (buttonState5 == HIGH && lastButtonState5 == LOW) {
        channelDisplayed = 3;
      }


      if (channelDisplayed == 7)
        channelDisplayed = 0;
      if (channelDisplayed < 0)
        channelDisplayed = 7;
    }
  }
}

void setLeds() {
  for (int i = 0; i < 8; i++) {
    if (on[channelDisplayed][i] == true) {
      digitalWrite(ledPinArray[i], HIGH);
      digitalWrite(ledPinArray[currentStep], HIGH);

    }
    else if (on[channelDisplayed][i] == false) {
      digitalWrite(ledPinArray[i], LOW);
      //      digitalWrite(ledPinArray[currentStep], HIGH);
    }
  }
}

void incrementUp() {
  currentStep++;
  if (currentStep > 7)
    currentStep = 0;
}
