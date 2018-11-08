int ledPinArray[4] = {0, 1, 2, 3};
int buttonPinArray[4] = {4, 5, 6, 7};
int nextChannelButtonPin = 8;
int prevChannelButtonPin = 9;
int switchPin = 22;
int potPin = A9;
int midiNotes[3] = {40, 42, 45};
int currentStep = 0;
int channelDisplayed = 0;
unsigned long lastStepTime = 0;
boolean lastButtonState[4] = {LOW, LOW, LOW, LOW};
boolean buttonState[4] = {LOW, LOW, LOW, LOW};
boolean lastButtonState2 = LOW;
boolean buttonState2 = LOW;
boolean lastButtonState3 = LOW;
boolean buttonState3 = LOW;
boolean lastButtonState4 = LOW;
boolean buttonState4 = LOW;
boolean on[4][8] = {
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}
};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(ledPinArray[i], OUTPUT);
    pinMode(buttonPinArray[i], INPUT);
  }
  pinMode(nextChannelButtonPin, INPUT);
  pinMode(prevChannelButtonPin, INPUT);
  pinMode(switchPin, INPUT);
}

void loop() {
  sequence();
  checkButtons();
  setLeds();
}

void sequence() {
  int tempo = analogRead(A9);

  if (millis() > lastStepTime + tempo) {

    //previous step off
    digitalWrite(ledPinArray[currentStep], LOW);

    //increment
    if (digitalRead(switchPin) == LOW) {
      incrementUp();
    }
    else {
      incrementDown();
    }
    lastStepTime = millis();

    digitalWrite(ledPinArray[currentStep], HIGH);

    for (int i = 0; i < 3; i++) {
      if (on[i][currentStep] == true) {
        usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}

void checkButtons() {
  if (channelDisplayed <= 2) {
    for (int i = 0; i < 4; i++) {
      lastButtonState[i] = buttonState[i];
      buttonState[i] = digitalRead(buttonPinArray[i]);
      lastButtonState2 = buttonState2;
      buttonState2 = digitalRead(nextChannelButtonPin);
      lastButtonState3 = buttonState3;
      buttonState3 = digitalRead(prevChannelButtonPin);

      if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
        if (on[channelDisplayed][i] == false)
          on[channelDisplayed][i] = true;
        else if (on[channelDisplayed][i] == true)
          on[channelDisplayed][i] = false;
      }

      if (buttonState2 == HIGH && lastButtonState2 == LOW)
        channelDisplayed++;
      if (buttonState3 == HIGH && lastButtonState3 == LOW)
        channelDisplayed--;

      if (channelDisplayed == 3)
        channelDisplayed = 0;
      if (channelDisplayed < 0)
        channelDisplayed = 2;
    }
  }
}

void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (on[channelDisplayed][i] == true) {
      digitalWrite(ledPinArray[i], HIGH);
      digitalWrite(ledPinArray[currentStep], HIGH);

    }
    else if (on[channelDisplayed][i] == false) {
      digitalWrite(ledPinArray[i], LOW);
      digitalWrite(ledPinArray[currentStep], HIGH);
    }
  }
}

void incrementUp() {
  currentStep++;
  if (currentStep > 7)
    currentStep = 0;
}

void incrementDown() {
  currentStep--;
  if (currentStep < 0)
    currentStep = 3;
}
