#include <math.h>
#include <ezButton.h>

#define BUTTON_PIN_1 12  // The ESP32 pin GPIO25 connected to the button 1
#define BUTTON_PIN_2 33  // The ESP32 pin GPIO26 connected to the button 2
#define BUTTON_PIN_3 32  // The ESP32 pin GPIO27 connected to the button 3
#define LED_PIN 25

ezButton button1(BUTTON_PIN_1);  // create ezButton object for button 1
ezButton button2(BUTTON_PIN_2);  // create ezButton object for button 2
ezButton button3(BUTTON_PIN_3);  // create ezButton object for button 3

bool ledIsOn = false;
const int NUM_SLIDERS = 3;
const int analogInputs[NUM_SLIDERS] = { 14, 27, 26 };
const int MAX_VALUE = 1050;

int analogSliderValues[NUM_SLIDERS];
int newAnalogSliderValues[NUM_SLIDERS];

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }
  button1.setDebounceTime(100);  // set debounce time to 100 milliseconds
  button2.setDebounceTime(100);  // set debounce time to 100 milliseconds
  button3.setDebounceTime(100);  // set debounce time to 100 milliseconds
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(9600);
}

void loop() {
  button1.loop();
  button2.loop();
  button3.loop();

  if (updateSliderValues()) {  //Send data only if +- 1%
    sendSliderValues();
  }

  if (button1.isPressed()){
    Serial.println("The button 1 is pressed");
    if (!ledIsOn){
      digitalWrite(LED_PIN, HIGH);
      ledIsOn = true;
    }else{
      digitalWrite(LED_PIN, LOW);
      ledIsOn = false;
    }
  }

  if (button2.isPressed())
    Serial.println("The button 2 is pressed");

  if (button3.isPressed())
    Serial.println("The button 3 is pressed");

  // printSliderValues(); // For debug
  delay(10);
}

bool updateSliderValues() {
  bool update = false;
  for (int i = 0; i < NUM_SLIDERS; i++) {
    newAnalogSliderValues[i] = MAX_VALUE - roundf((analogRead(analogInputs[i]) * MAX_VALUE) / 4095);
    if (analogSliderValues[i] != 0) {
      if (newAnalogSliderValues[i] > analogSliderValues[i] + 10 || newAnalogSliderValues[i] < analogSliderValues[i] - 10) {
        if(newAnalogSliderValues[i] <= 10){
          newAnalogSliderValues[i] = 0;
        }else if (newAnalogSliderValues[i] >= MAX_VALUE - 10) {
          newAnalogSliderValues[i] = MAX_VALUE;
        }
        analogSliderValues[i] = newAnalogSliderValues[i];
        update = true;
      }
    } else {
      analogSliderValues[i] = newAnalogSliderValues[i];
    }
  }
  return update;
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }

  Serial.println(builtString);
}

void printSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogSliderValues[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}