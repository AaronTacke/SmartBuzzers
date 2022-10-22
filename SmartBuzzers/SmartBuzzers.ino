
#define max_players 8
#define analog_threshold 500


// Pins for buttons of buzzers in correct order
const uint8_t buttons[] = {A0, A1, A2, A3, A4, A5, A6, A7};


// Pins for lights of buzzers in correct order
const uint8_t lights[] = {2, 3, 4, 5, 6, 7, 8, 11};


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


// Returns number of pressed buzzer, 0 if none is pressed
uint8_t pressed(){
  for(int i = 1; i <= max_players; i++){
    if(analogRead(buttons[i-1])>analog_threshold) return i;
  }
  return 0;
}


// Do setup if necessary
void setup() {}


// Start Gamemode
void loop() {
  switch(pressed()){
    case 1:
      gamemode_first();
      break;
    case 2:
      gamemode_order();
      break;
    case 3:
      gamemode_last();
      break;
    case 4:
      gamemode_potato();
      break;
    case 5:
      gamemode_random();
      break;
    default:
      break;
  }
}


// Gamemode: The first player to press the button is chosen
void gamemode_first(){
  // TODO
}


// Gamemode: The players are chosen in the order in which they pressed
void gamemode_order(){
  // TODO
}


// Gamemode: The last player to press the button is chosen
void gamemode_last(){
  // TODO
}


// Gamemode: Players pass a hot potato until it explodes
void gamemode_potato(){
  // TODO
}


// Gamemode: A random player is chosen
void gamemode_random(){
  // TODO
}
