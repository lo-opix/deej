#define LED_PIN 25

const int NUM_SLIDERS = 3;
const int analogInputs[NUM_SLIDERS] = { 26, 27, 14 };
const int NUM_BUTTONS = 3;
const int buttonInputs[NUM_BUTTONS] = { 12, 33, 32 };

int analogSliderValues[NUM_SLIDERS];
int newAnalogSliderValues[NUM_SLIDERS];
int buttonValues[NUM_BUTTONS];
int newButtonValues[NUM_BUTTONS];
const int MAX_VALUE = 1023; //1023 default

bool isDeviceOn = false;
bool isLedOn = false;
bool isLedFlashing = true;
float last_ping_time = millis();
float blink_time_delay = 500.0;
float blink_time = millis();

void setup() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonInputs[i], INPUT_PULLUP);
  }

  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  
  if(Serial.available()){// if there is data comming
    String message = Serial.readStringUntil('\n');
    if(message == "1"){
      last_ping_time = millis();
    }
  }

  if((millis() - last_ping_time) > 15000){
    isLedFlashing = true;
  } else{
    isLedFlashing = false;
    if (isDeviceOn){
      isLedOn = true;
      analogWrite(LED_PIN, 10);
    }
  }

  if(updateSliderValues()){
    sendSliderValues(); // Actually send data (all the time)
  }
  
  if (isLedFlashing && isDeviceOn && millis() - blink_time > blink_time_delay){
    Serial.println("lig");
    blink_time = millis();
    if (isLedOn){
      analogWrite(LED_PIN, 0);
      isLedOn = false;
    }else{
      analogWrite(LED_PIN, 10);
      isLedOn = true;
    }
  }

  delay(100);
}

bool updateSliderValues() {
  bool offStateSended = true;
  bool update = false; // Superior
  for (int i = 0; i < NUM_SLIDERS; i++) {
    newAnalogSliderValues[i] = MAX_VALUE - roundf((analogRead(analogInputs[i]) * MAX_VALUE) / 4095);
    if (analogSliderValues[i] != 0) {
      if (newAnalogSliderValues[i] > analogSliderValues[i] + 10 || newAnalogSliderValues[i] < analogSliderValues[i] - 10) {
        if (newAnalogSliderValues[i] <= 10) {
          newAnalogSliderValues[i] = 0;
        } else if (newAnalogSliderValues[i] >= MAX_VALUE - 10) {
          newAnalogSliderValues[i] = MAX_VALUE;
        }
        analogSliderValues[i] = newAnalogSliderValues[i];
        update = true;
      }
    } else {
      analogSliderValues[i] = newAnalogSliderValues[i];
    }
  }
  for (int i = 0; i < NUM_BUTTONS; i++) {
    newButtonValues[i] = digitalRead(buttonInputs[i]);

    if (newButtonValues[i] != buttonValues[i]) {
      buttonValues[i] = newButtonValues[i];
      if(buttonValues[i] == 0 && i == 0){
        if(isDeviceOn){
          isDeviceOn = false;
          isLedOn = false;
          analogWrite(LED_PIN, 0);
          offStateSended = false; // to send the off status
        }else{
          isDeviceOn = true;
          isLedOn = true;
          analogWrite(LED_PIN, 10);
        }
      }
      update = true;
    }
  }
  
  if(!isDeviceOn && offStateSended){
    update = false;
    offStateSended = false;
  }
  return update;
}

void sendSliderValues() {
  String builtString = String("");

  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += "s";
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }

  if (NUM_BUTTONS > 0) {
    builtString += String("|");
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    if(i != 0){
      builtString += "b";
      builtString += String((int)buttonValues[i]);
    }else{
      if(isLedOn){
        builtString += "on";
      }else{
        builtString += "off";
      }
    }

      if (i < NUM_BUTTONS - 1) {
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