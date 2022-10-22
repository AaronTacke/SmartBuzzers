

// Pins for buttons of buzzers in correct order
static const uint8_t buttons[] = {A0, A1, A2, A3, A4, A5, A6, A7};


// Pins for lights of buzzers in correct order
static const uint8_t lights[] = {2, 3, 4, 5, 6, 7, 8, 11};


// Sets lights from 1 to 8, 0 means no light
void setLight(uint8_t light){
  static uint8_t oldLight;
  
  // Turn off previous buzzer
  if(oldLight > 0) pinMode(lights[oldLight-1], INPUT);
  
  // Turn on chosen buzzer
  if(light > 0){
    oldLight = light;
    pinMode(lights[oldLight-1], OUTPUT);
    digitalWrite(lights[oldLight-1], LOW);
  }
}


// Choose Gamemode and count/order players if necessary
void setup() {
  // TODO
}

// Replaced by specific loop functions for different Gamemodes
void loop() {}
