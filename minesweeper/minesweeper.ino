#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDRESS 0x3C

#define CELL_SIZE 11
#define GRID_COLS 11
#define GRID_ROWS 6
int grid[GRID_ROWS][GRID_COLS];
#define X_PIN A0
#define Y_PIN A1
#define SW_PIN 8


const int FLAG = 200;
const int EMPTY = 0;
const int UNREVEALED = -2;
const int BOMB = -1;
const int BOMBCOUNT = 10;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int cursorX = 0;
int cursorY = 0;

void setup() {
  grid[5][2] = 4;
grid[3][2] = 3;
grid[3][6] = 7;
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("OLED not found"));
    while (true);
  }

  display.clearDisplay();
  drawGrid();
  drawCursor();
  display.display();
}

void loop() {
  int xVal = analogRead(X_PIN);
  int yVal = analogRead(Y_PIN);
  int button = digitalRead(SW_PIN);

  static unsigned long lastMoveTime = 0;
  unsigned long now = millis();

  if (now - lastMoveTime > 200) {
    // محور X
    if (xVal < 400) {
      moveCursor('L');
      lastMoveTime = now;
    } else if (xVal > 600) {
      moveCursor('R');
      lastMoveTime = now;
    }

    // محور Y
    if (yVal < 400) {
      moveCursor('U');
      lastMoveTime = now;
    } else if (yVal > 600) {
      moveCursor('D');
      lastMoveTime = now;
    }

    if (button == LOW) {
      Serial.println("Button Pressed!");
      // اینجا می‌تونی کدی بذاری برای انتخاب خانه
    }
  }
}

void moveCursor(char dir) {
  switch (dir) {
    case 'L':
      if (cursorX > 0) cursorX--;
      break;
    case 'R':
      if (cursorX < GRID_COLS - 1) cursorX++;
      break;
    case 'U':
      if (cursorY > 0) cursorY--;
      break;
    case 'D':
      if (cursorY < GRID_ROWS - 1) cursorY++;
      break;
  }

  display.clearDisplay();
  drawGrid();
  drawCursor();
  display.display();
}

void drawGrid() {
  display.clearDisplay();

  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = 0; x < GRID_COLS; x++) {
      int px = x * CELL_SIZE;
      int py = y * CELL_SIZE;

      // رسم خانه
      display.drawRect(px, py, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);

      // انتخاب محتوای داخل خانه
      if (x == cursorX && y == cursorY) {
        // نشانگر روی این خانه است
        display.fillRect(px + 2.5, py + 2.5, CELL_SIZE - 5, CELL_SIZE - 5, SSD1306_WHITE);
      } else {
        if (grid[y][x] == FLAG) {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("F");
        } else if (grid[y][x] == EMPTY) {
          // خالی = فقط قاب خانه
        } else if (grid[y][x] >= 1 && grid[y][x] <= 8) {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print(grid[y][x]);
        } else if (grid[y][x] == BOMB) {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("X");
        } else {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("-");
        }
      }
    }
  }

  display.display();  // به‌روزرسانی صفحه نمایش
}


void drawCursor() {
  int x = cursorX * CELL_SIZE;
  int y = cursorY * CELL_SIZE;
  display.drawRect(x, y, CELL_SIZE, CELL_SIZE, SSD1306_INVERSE);
}
