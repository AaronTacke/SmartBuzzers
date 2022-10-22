
#define max_players 8
#define analog_threshold 500


// Pins for buttons of buzzers in correct order
const uint8_t buttons[] = {A2, A1, A6, A4, A7, A5, A3, A0};


// Pins for lights of buzzers in correct order
const uint8_t lights[] = {11, 6, 3, 2, 4, 7, 8, 5};


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


const long timer[] = {86400000, 1000, 3000, 5000, 10000, 20000, 30000, 60000};
uint8_t chosenTimer = 0;


// Do setup if necessary
void setup() {
  Serial.begin(9600);
}


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

bool delay_until(int millis_time, uint8_t player){
  long endTime = millis() + millis_time;
  while(millis()<endTime){
    if(pressed() == player) return true;
  }
  return false;
}

void choose(long millis_time, uint8_t player){
  // Wait until every button is released
  while(pressed() != 0);
  // Calculate end time in internal milliseconds
  long endTime = millis() + millis_time;
  long timeLeft = endTime - millis();

  // While time is left (pressing buzzer changes endTime)
  while(timeLeft > 0){
    // Blinking speed depending on time left
    int light_time = min(sqrt(timeLeft)*5,666);
    setLight(player);
    // Change end time to now if player presses button
    if(delay_until(light_time,player)) endTime = millis();
    setLight(0);
    // Change end time to now if player presses button
    if(delay_until(light_time/2,player)) endTime = millis();
    // Calculate remaining time left
    timeLeft = endTime - millis();
  }
  // Wait until every button is released
  while(pressed() != 0);
}


// Gamemode: The first player to press the button is chosen
void gamemode_first(){
  // Wait until every button is released
  while(pressed() != 0); 
  // Choose time to answer
  while(chosenTimer == 0) chosenTimer = pressed();
  while(true){
    uint8_t first = pressed();
    while(first == 0) first = pressed();
    choose(timer[chosenTimer-1], first);
  }
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
  // Wait until every button is released
  while(pressed() != 0); 
  // At the beginning only player 4 is logged in
  uint8_t players[max_players];
  for(int i = 0; i < sizeof(players); i++) players[i] = 0;
  players[0] = 4;
  uint8_t playerCount = 1;

  // Add new players until Player 4 continues
  uint8_t newPlayer = pressed();
  while(newPlayer!=4){
    bool exist = (newPlayer==0);
    // Check if player exists
    for(int i = 0; i < sizeof(players); i++) if(players[i] == newPlayer) exist = true;

    // Add new player
    if(!exist){
      playerCount++;
      int i = 0;
      while(players[i] != 0) i++;
      players[i] = newPlayer;
    }

    // Show Players
    setLight(players[(millis()/100)%playerCount]);

    newPlayer = pressed();
  }

  setLight(0);

  activePlayerIndex = random(0,playerCount);
  
  endTime = millis() + random(5000, 15000);
  long timeLeft = endTime - millis();

  //TODO just copied yet:
  // While time is left (pressing buzzer changes endTime)
  while(timeLeft > 0){
    // Blinking speed depending on time left
    int light_time = min(sqrt(timeLeft)*5,666);
    setLight(players[activePlayerIndex]);
    // Change end time to now if player presses button
    if(delay_until(light_time,player)) endTime = millis();
    setLight(0);
    // Change end time to now if player presses button
    if(delay_until(light_time/2,player)) endTime = millis();
    // Calculate remaining time left
    timeLeft = endTime - millis();
  }
  
  
}


// Gamemode: A random player is chosen
void gamemode_random(){
  // TODO
}
