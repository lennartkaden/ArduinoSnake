#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "pitches.h"

// Joystick
#define btn 1  //5
#define j_x A2
#define j_y A3

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define EDGE 4

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Snake
#define SNAKE_MAX_LEN 100

char snake_pos[SNAKE_MAX_LEN][2];
int snake_current_len = 1;

// Food
char food_pos[2];
bool food_eaten = true;

// Score
int score = 0;

// Sound
#define buzzer_pin 0
#define S_COLLECT 0
#define S_TEST 1
char s_lens[] = { 2, 3 };
int s_collect[2][2] = { { NOTE_C3, NOTE_G1 }, { 40, 60 } };
int s_test[3][3] = { { NOTE_C3, NOTE_G1, NOTE_G3 }, { 40, 60, 100 } };
bool playing = false;
char current_note = 0;
char sound_len;
u_long start_time_playing;
int** current_sound;
char current_sound_max_notes;

void play_sound(int** sound_to_play, char s_id) {
  Serial.println("set sound");
  current_sound = sound_to_play;
  current_note = 0;
  current_sound_max_notes = s_lens[s_id];
  playing = true;
  start_time_playing = millis();
  Serial.println(current_sound[0][current_note]);
  tone(buzzer_pin, current_sound[0][current_note]);
}

void sound() {
  if(current_note>1) return;
  if (!playing) return;
  if (millis() - start_time_playing <= current_sound[1][2]) return;
  start_time_playing = millis();
  current_note += 1;
  tone(buzzer_pin, current_sound[0][current_note]);
  Serial.println(current_sound[0][current_note]);
  if (current_note >= current_sound_max_notes) {
    playing = false;
    noTone(buzzer_pin);
  }
}

void random_food_pos() {
  food_pos[0] = random(EDGE + 2, SCREEN_WIDTH - 2);
  food_pos[1] = random(EDGE + 2, SCREEN_HEIGHT - 2);
}

bool snake_on_food() {
  for (int i = 0; i < snake_current_len; i++) {
    if (snake_pos[i][0] == food_pos[0] && snake_pos[i][1] == food_pos[1]) return true;
    if (snake_pos[i][0] == food_pos[0] + 1 && snake_pos[i][1] == food_pos[1]) return true;
    if (snake_pos[i][0] == food_pos[0] && snake_pos[i][1] == food_pos[1] + 1) return true;
    if (snake_pos[i][0] == food_pos[0] + 1 && snake_pos[i][1] == food_pos[1] + 1) return true;
  }
  return false;
}

enum joystick_states {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  NONE
};
int snake_dir = LEFT;

enum menu_res {
  PLAY = 1,
  SETTINGS = 2
};

void setup() {
  Serial.begin(9600);

  // Joystick
  pinMode(btn, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);  // Pause for 2 seconds

  display.clearDisplay();

  display.drawPixel(10, 10, SSD1306_WHITE);

  display.display();
  delay(2000);

  //play_sound((int**)s_collect, S_COLLECT);
}

bool jBtnDown() {
  return digitalRead(btn);
}

int jState() {
  if (analogRead(j_x) > 900)
    return RIGHT;
  else if (analogRead(j_x) < 100)
    return LEFT;
  else if (analogRead(j_y) > 900)
    return DOWN;
  else if (analogRead(j_y) < 100)
    return UP;
  else return NONE;
}

void settings() {
  while (jBtnDown())
    ;
  while (!jBtnDown()) {
    display.clearDisplay();

    display.setCursor(16, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("SETTINGS"));

    display.display();

    delay(10);
  }
}

void reset_game() {
  snake_current_len = 1;
  for (int i = 0; i < SNAKE_MAX_LEN; i++) {
    snake_pos[0][0] = 0;
    snake_pos[0][1] = 0;
  }
  snake_pos[0][0] = SCREEN_WIDTH / 2;
  snake_pos[0][1] = SCREEN_HEIGHT / 2;
  snake_dir = LEFT;
  food_eaten = true;
  score = -1;
}

void game() {
  while (jBtnDown())
    ;
  while (1) {
    // Input Step
    int js = jState();
    if (js != NONE && !(snake_dir == UP && js == DOWN) && !(snake_dir == DOWN && js == UP) && !(snake_dir == LEFT && js == RIGHT) && !(snake_dir == RIGHT && js == LEFT))
      snake_dir = js;

    // Logic Step
    for (int i = snake_current_len - 1; i > 0; i--) {
      snake_pos[i][0] = snake_pos[i - 1][0];
      snake_pos[i][1] = snake_pos[i - 1][1];
    }
    switch (snake_dir) {
      case UP:
        snake_pos[0][1] -= 1;
        break;
      case DOWN:
        snake_pos[0][1] += 1;
        break;
      case LEFT:
        snake_pos[0][0] -= 1;
        break;
      case RIGHT:
        snake_pos[0][0] += 1;
        break;
    }

    // Check Food
    if (snake_on_food()) food_eaten = true;
    if (food_eaten) {
      food_eaten = false;
      if (snake_current_len < SNAKE_MAX_LEN)
        snake_current_len++;
      score += 1;
      random_food_pos();
      while (snake_on_food()) {
        random_food_pos();
      }
    }

    // Check Gameover
    bool gameover = false;
    for (int i = 1; i < snake_current_len; i++) {
      if (snake_pos[0][0] == snake_pos[i][0] && snake_pos[0][1] == snake_pos[i][1])
        gameover = true;
      if (snake_pos[0][0] >= SCREEN_WIDTH - EDGE || snake_pos[0][1] >= SCREEN_HEIGHT - EDGE || snake_pos[0][0] <= EDGE || snake_pos[0][1] <= EDGE) gameover = true;
    }

    // Draw Step
    // Clear
    display.clearDisplay();

    // Draw Edge
    display.fillRect(0, 0, display.width(), display.height(), SSD1306_INVERSE);
    display.fillRect(2, 2, display.width() - EDGE, display.height() - EDGE, SSD1306_INVERSE);

    // Draw Food
    display.drawPixel(food_pos[0], food_pos[1], SSD1306_WHITE);
    display.drawPixel(food_pos[0] + 1, food_pos[1], SSD1306_WHITE);
    display.drawPixel(food_pos[0], food_pos[1] + 1, SSD1306_WHITE);
    display.drawPixel(food_pos[0] + 1, food_pos[1] + 1, SSD1306_WHITE);

    // Draw Snake
    for (int i = 0; i < snake_current_len; i++) {
      display.drawPixel(snake_pos[i][0], snake_pos[i][1], SSD1306_WHITE);
    }

    // Display
    display.display();

    delay(50);

    if (gameover) break;
  }
  for (int counter = 0; counter <= score; counter++) {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.println(F("GAME OVER"));
    display.println();

    display.setTextSize(1);
    display.setCursor(30, display.getCursorY());
    display.println(F("Your Score"));

    display.setTextSize(2);
    if (score < 10)
      display.setCursor(SCREEN_WIDTH / 2 - 12, display.getCursorY());
    if (score >= 10 && score < 100)
      display.setCursor(SCREEN_WIDTH / 2 - (12 * 2), display.getCursorY());
    if (score >= 100)
      display.setCursor(SCREEN_WIDTH / 2 - (12 * 3), display.getCursorY());
    display.println(counter);

    display.display();
    delay(10);
  }
  while (!jBtnDown())
    ;
}

int menu() {
  bool color = false;
  byte pos = 0;
  bool used = false;
  while (1) {
    //sound();
    display.clearDisplay();

    display.setCursor(25, 0);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    if (color && pos == 0) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println(F("SNAKE"));

    display.println();
    display.setTextColor(SSD1306_WHITE);
    if (color && pos == 1) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println(F("PLAY"));

    display.setTextColor(SSD1306_WHITE);
    if (color && pos == 2) display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.println(F("SETTINGS"));

    display.display();
    long unsigned int start_time = millis();
    while (millis() - start_time < 300) {
      if (jState() == UP && pos > 0 && !used) {
        pos--;
        used = true;
      }
      if (jState() == DOWN && pos < 2 && !used) {
        pos++;
        used = true;
      }
      if (jState() == NONE) used = false;
      if (jBtnDown() && pos > 0) return pos;
    }
    color = !color;
  }
}

void loop() {
  switch (menu()) {
    case PLAY:
      reset_game();
      game();
      break;
    case SETTINGS:
      settings();
      break;
  }
}
