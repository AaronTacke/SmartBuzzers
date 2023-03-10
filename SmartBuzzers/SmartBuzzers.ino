#define MAX_PLAYERS 8
#define ANALOG_THRESHOLD 500


// Pins for buttons of buzzers in correct order
const uint8_t buttons[] = {A2, A1, A6, A4, A7, A5, A3, A0};

// Pins for lights of buzzers in correct order
const uint8_t lights[] = {11, 6, 3, 2, 4, 7, 8, 5};

// Settings for different gamemodes
const long timer[] = {86400000, 1000, 3000, 5000, 10000, 20000, 30000, 60000};
uint8_t chosenTimer = 0;
uint8_t playerCount = 0;


/* 
 * Sets lights from 1 to 8, 0 means no light
 */
void setLight(uint8_t light){
  static uint8_t oldLight;
  
  // Turn off previous light
  if(oldLight > 0) pinMode(lights[oldLight-1], INPUT);
  
  // Turn on chosen light
  if(light > 0){
    oldLight = light;
    pinMode(lights[oldLight-1], OUTPUT);
    digitalWrite(lights[oldLight-1], LOW);
  }
}


/*
 * Blink players light until timeout or press
 */
void select_player(long millis_time, uint8_t player){
  if(player == 0) return;
  
  // Calculate end time in milliseconds
  long endTime = millis() + millis_time;

  // While time is left
  long timeLeft = endTime - millis();
  while(timeLeft > 0){
    // Blinking speed depending on time left
    int light_time = min(sqrt(timeLeft)*5,666);

    // Blink while waiting for pressed button
    setLight(player);
    if(pressed_timeout(light_time,player)) endTime = millis();
    setLight(0);
    if(pressed_timeout(light_time/2,player)) endTime = millis();

    timeLeft = endTime - millis();
  }
}


/*
 * Animate specified button and then select player (wait for press)
 */
void player_lost(uint8_t player){
  if(player == 0) return;
  const uint8_t LONG_TIMER_INDEX = 0;

  // Wait a second for suspense
  setLight(0);
  delay(1000);

  // Blink 2 seconds
  for(uint8_t i = 0; i < 5; i++){
    setLight(player);
    delay(200);
    setLight(0);
    delay(200);
  }
  
  // Wait for player to continue by pressing buzzer
  select_player(timer[LONG_TIMER_INDEX], player);
}


/* 
 * Returns number of pressed buzzer, 0 if none is pressed 
 */
uint8_t pressed(){
  static long buttonsDebounceTime[] = {0,0,0,0,0,0,0,0};
  static bool buttonState[] = {false, false, false, false, false, false, false, false};
  static const long debounceTime = 30;

  long currentTime = millis();
  uint8_t pressed = 0;

  // Reset unpressed buttons after debounce time
  for(uint8_t i = 1; i <= MAX_PLAYERS; i++){
    if(currentTime - buttonsDebounceTime[i-1] > debounceTime
        && analogRead(buttons[i-1]) < ANALOG_THRESHOLD && buttonState[i-1]){
      buttonState[i-1] = false;
      buttonsDebounceTime[i-1] = currentTime;
    }
  }

  // Choose pressed button after debounce time
  for(uint8_t i = 1; i <= MAX_PLAYERS; i++){
    if(currentTime - buttonsDebounceTime[i-1] > debounceTime
        && analogRead(buttons[i-1]) > ANALOG_THRESHOLD && !buttonState[i-1]){
      buttonState[i-1] = true;
      buttonsDebounceTime[i-1] = currentTime;
      return i;
    }
  }
  return 0;
}


/* 
 * Checks whether specified player presses button until timeout 
 */
bool pressed_timeout(long millis_timeout, uint8_t player){
  long endTime = millis() + millis_timeout;
  while(millis()<endTime) if(pressed() == player) return true;
  return false;
}


/* 
 * Do setup if necessary
 */
void setup() {}


/*
 * Start Gamemode
 */
void loop() {
  switch(pressed()){
    case 1: gamemode_first();
    case 2: gamemode_order();
    case 3: gamemode_last();
    case 4: gamemode_potato();
    case 5: gamemode_random_potato();
    case 6: gamemode_follow();
    case 7: gamemode_random();
    case 8: gamemode_states();
  }
}


/* 
 * 1st Gamemode: The first player to press the button is chosen
 */
void gamemode_first(){
  // Choose time to answer
  while(chosenTimer == 0) chosenTimer = pressed();

  // Always select player that presses the button
  while(true) select_player(timer[chosenTimer-1], pressed());
}


/* 
 * 2nd Gamemode: The players are chosen in the order in which they pressed
 */
void gamemode_order(){
  // Queue stores order of button presses
  uint8_t order[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  while(true){
    // Activate light for current head
    setLight(order[0]);
    uint8_t button = pressed();
    
    // Remove head if pressed
    if(button == order[0]) for(uint8_t i = 0; i < 8; i++) order[i] = order[i+1];

    // Append press to queue (if not contained yet)
    else for(uint8_t i = 0; i < 8; i++){
      if(order[i] == 0) order[i] = button;
      if(order[i] == button) i = 8;
    }
  }
}


/* 
 * 3rd Gamemode: The last player to press the button is chosen
 */
void gamemode_last(){
  // Choose number of players
  while(playerCount == 0) playerCount = pressed();

  while(true){
    bool alreadyPressed[] = {false, false, false, false, false, false, false, false};
    uint8_t counter = 0;

    // Until only one player is left
    while(counter < playerCount - 1){
      
      // Subtly blink for already pressed players
      uint8_t lightIndex = millis() / 100 % MAX_PLAYERS;
      setLight(alreadyPressed[lightIndex] * (lightIndex + 1));

      // Check if new player pressed the button
      uint8_t button = pressed();
      if(button > 0 && !alreadyPressed[button-1]){
        alreadyPressed[button-1] = true;
        counter++;
      }
    }
    
    // Last player loses
    for(uint8_t i = 0; i < playerCount; i++){
      if(!alreadyPressed[i]) player_lost(i + 1);
    }
  }
}


/*
 * 4th Gamemode: Players pass hot potato until it explodes
 */
void gamemode_potato(){
  // Choose number of players
  while(playerCount == 0) playerCount = pressed();

  while(true){
    // Play with random starter for random amount of time
    int player = random(0, playerCount);
    long endTime = millis() + random(5000,15000);
    while(millis() < endTime){

      // Activate light for active player
      setLight((player % playerCount) + 1);

      // If player presses the button, the it's the next players turn
      uint8_t button = pressed();
      if(button == ((player % playerCount) + 1)) player++;
      else if(button > 0) {
        // If wrong player presses button, he loses directly
        player = button - 1;
        endTime = millis();
      }
      delay(1);
    }
    
    // If the time is up, the active player loses
    player_lost((player % playerCount) + 1);
  }
}


/*
 * 5th Gamemode: Players pass hot potato until it explodes
 */
void gamemode_random_potato(){
  // Choose number of players
  while(playerCount == 0) playerCount = pressed();

  while(true){
    // Play with random starter for random amount of time
    int player = random(0, playerCount);
    long endTime = millis() + random(5000,15000);
    while(millis() < endTime){
      
      // Activate light for active player
      setLight((player % playerCount) + 1);
      
      // If player presses the button, another random player is selected
      uint8_t button = pressed();
      if(button == ((player % playerCount) + 1)) 
        while(button == ((player % playerCount) + 1)) 
          player = random(0, playerCount);
      else if(button > 0) {
        // If wrong player presses button, he loses directly
        player = button - 1;
        endTime = millis();
      }
      delay(1);
    }
    
    // If the time is up, the active player loses
    player_lost((player % playerCount) + 1);
  }
}


/*
 * 6th Gamemode: 2 potatos follow each other until reached
 */
void gamemode_follow(){
  // Choose number of players - at least 4
  while(playerCount < 4) playerCount = pressed();

  while(true){
    // 2 random start positions at least 1 apart
    int player1 = random(0, playerCount);
    int player2 = player1 + random(2, playerCount - 2);

    // Until both point to the same buzzer (one catches up)
    while(player1 % playerCount != player2 % playerCount){

      // Blink both active buzzers
      bool playerLight = millis() / 50 % 2 == 0;
      setLight(playerLight * ((player1 % playerCount) + 1) 
        + (!playerLight) * ((player2 % playerCount) + 1));

      // If active player presses the button, it's the next players turn
      uint8_t button = pressed();
      if(button == ((player1 % playerCount) + 1)) player1++;
      else if(button == ((player2 % playerCount) + 1)) player2++;
      else if(button > 0) {
        // If wrong player presses button, he loses directly
        player1 = button - 1;
        player2 = button - 1;
      }
      delay(1);
    }

    // If another player catches up, one loses
    player_lost((player1 % playerCount) + 1);
  }
}


/*
 * 7th Gamemode: A random player is selected
 */
void gamemode_random(){
  // Choose number of players
  while(playerCount == 0) playerCount = pressed();

  while(true){
    // Start at a random position for a random amount of time
    int player = random(0, playerCount);
    int freq = random(200,300);
    int threshold = random(-500,-300);
    while(freq > threshold){
      
      // Activate light for active player
      setLight((player % playerCount) + 1);
      player++;
      
      // Starts slow, gets fast, ends slow
      freq -= 10;
      delay(max(abs(freq), 100));
    }
    // Player loses at position where it stops
    player_lost((player % playerCount) + 1);
  }
}


/*
 * 8th Gamemode: Boolean state holder
 */
void gamemode_states(){
  bool states[] = {false, false, false, false, false, false, false, false};
  while(true){
      
    // Subtly blink for active states
    uint8_t lightIndex = millis() / 10 % MAX_PLAYERS;
    setLight(states[lightIndex] * (lightIndex + 1));

    // Toggle state when button pressed
    uint8_t button = pressed();
    if(button > 0) states[button-1] = !states[button-1];
  }
}
