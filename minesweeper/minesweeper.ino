#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoQueue.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define CELL_SIZE 11
#define GRID_COLS 11
#define GRID_ROWS 6
int8_t grid[GRID_ROWS][GRID_COLS];
int8_t gridMain[GRID_ROWS][GRID_COLS];
struct Point
{
  byte x;
  byte y;
};
#define X_PIN A0
#define Y_PIN A1
#define SW_PIN 8

int redLED = 10;
// int greenLED = 11;
int yellowLED = 9;
int buzzerPin = 12;
int buttonPin = 11;

int findCount = 0;
int flagCount = 0;
bool gameOver = false;
bool firstCell = true;
const int FLAG = 120;
const int EMPTY = 0;
const int UNREVEALED = -2;
const int BOMB = -1;
const int BOMBCOUNT = 15;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int cursorX = 0;
int cursorY = 0;

void setup()
{
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(redLED, OUTPUT);

  pinMode(yellowLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  randomSeed(analogRead(A0));
  grid[5][2] = 4;
  grid[3][2] = 3;
  grid[3][6] = 7;
  pinMode(SW_PIN, INPUT_PULLUP);
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    Serial.println(F("OLED not found"));
    while (true)
      ;
  }

  display.clearDisplay();
  drawGrid();
  drawCursor();
  display.display();
}

void loop()
{
  Serial.println(freeMemory());
  if (gameOver)
  {
    digitalWrite(redLED, HIGH);

    Serial.println(F("Game Over"));
    revealAllBomb();
    drawGrid();
    playGameOverMelody();
    delay(2000);
    while (true)
    {

      // display.clearDisplay();
      // display.setCursor(0, 0);
      // display.setTextColor(SSD1306_WHITE);
      // display.setTextSize(1);
      // display.print("You Lose");
      // display.display();
    }
  }
  if (findCount == BOMBCOUNT)
  {
    playWinMelody();
    display.clearDisplay();
    while (true)
    {
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(1);
      display.print("You Win");
      display.display();
    }
  }
  int xVal = analogRead(X_PIN);
  int yVal = analogRead(Y_PIN);
  int button = digitalRead(SW_PIN);
  int buttonState = digitalRead(buttonPin);

  static unsigned long lastMoveTime = 0;
  unsigned long now = millis();
  if (firstCell)
  {
    initGrid();
    drawGrid();
  }
  if (now - lastMoveTime > 200)
  {

    // محور X
    if (xVal < 400)
    {
      playMoveBeep();
      moveCursor('L');
      lastMoveTime = now;
    }
    else if (xVal > 600)
    {
      playMoveBeep();

      moveCursor('R');
      lastMoveTime = now;
    }

    // محور Y
    if (yVal < 400)
    {
      playMoveBeep();

      moveCursor('U');
      lastMoveTime = now;
    }
    else if (yVal > 600)
    {
      playMoveBeep();

      moveCursor('D');
      lastMoveTime = now;
    }

    if (button == LOW)
    {
      playRevealBeep();
      digitalWrite(yellowLED, HIGH);
      delay(100);
      digitalWrite(yellowLED, LOW);
      if (firstCell)
      {
        setBomb(gridMain, cursorX, cursorY);
        calculateNumbers(gridMain);
        revealCell(cursorX, cursorY);

        firstCell = false;
        return;
      }
      revealCell(cursorX, cursorY);

      return;
    }
    if (buttonState == LOW)
    {
      setFlag(cursorX, cursorY);

      return;
    }
  }
}

void moveCursor(char dir)
{
  switch (dir)
  {
  case 'L':
    if (cursorX > 0)
      cursorX--;
    break;
  case 'R':
    if (cursorX < GRID_COLS - 1)
      cursorX++;
    break;
  case 'U':
    if (cursorY > 0)
      cursorY--;
    break;
  case 'D':
    if (cursorY < GRID_ROWS - 1)
      cursorY++;
    break;
  }

  display.clearDisplay();
  drawGrid();
  drawCursor();
  display.display();
}

void drawGrid()
{
  display.clearDisplay();

  for (int y = 0; y < GRID_ROWS; y++)
  {
    for (int x = 0; x < GRID_COLS; x++)
    {
      int px = x * CELL_SIZE;
      int py = y * CELL_SIZE;

      display.drawRect(px, py, CELL_SIZE, CELL_SIZE, SSD1306_WHITE);

      if (x == cursorX && y == cursorY)
      {
        display.fillRect(px + 2.5, py + 2.5, CELL_SIZE - 5, CELL_SIZE - 5, SSD1306_WHITE);
      }
      else
      {
        if (grid[y][x] == FLAG)
        {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("F");
        }
        else if (grid[y][x] == EMPTY)
        {
          // خالی = فقط قاب خانه
        }
        else if (grid[y][x] >= 1 && grid[y][x] <= 8)
        {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print(grid[y][x]);
        }
        else if (grid[y][x] == BOMB)
        {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("X");
        }
        else
        {
          display.setCursor(px + 2, py + 1);
          display.setTextColor(SSD1306_WHITE);
          display.setTextSize(1);
          display.print("-");
        }
      }
    }
  }

  display.display(); // به‌روزرسانی صفحه نمایش
}

void drawCursor()
{
  int x = cursorX * CELL_SIZE;
  int y = cursorY * CELL_SIZE;
  display.drawRect(x, y, CELL_SIZE, CELL_SIZE, SSD1306_INVERSE);
}

void initGrid()
{
  for (int y = 0; y < GRID_ROWS; y++)
    for (int x = 0; x < GRID_COLS; x++)
      grid[y][x] = UNREVEALED;
}

void setBomb(int8_t grid[][GRID_COLS], int cursorX, int cursorY)
{

  int bombCount = 0;
  while (bombCount < BOMBCOUNT) // تعداد مین‌ها را اینجا می‌توان تنظیم کرد
  {
    int x = random(0, GRID_COLS);
    int y = random(0, GRID_ROWS);
    if (abs(x - cursorX) <= 1 && abs(y - cursorY) <= 1)
      continue;

    if (grid[y][x] != BOMB)
    {
      Serial.print("Bomb placed at: ");
      Serial.println(x);
      Serial.println(y);
      grid[y][x] = BOMB;
      bombCount++;
    }
  }
}
void calculateNumbers(int8_t grid[GRID_ROWS][GRID_COLS])
{
  for (int y = 0; y < GRID_ROWS; y++)
  {
    for (int x = 0; x < GRID_COLS; x++)
    {
      if (grid[y][x] == BOMB)
      {
        for (int dy = -1; dy <= 1; dy++)
        {
          for (int dx = -1; dx <= 1; dx++)
          {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < GRID_COLS && ny >= 0 && ny < GRID_ROWS && grid[ny][nx] != BOMB)
              grid[ny][nx]++;
          }
        }
      }
    }
  }
}

void revealAllBomb()
{
  for (int y = 0; y < GRID_ROWS; y++)
  {
    for (int x = 0; x < GRID_COLS; x++)
    {
      if (gridMain[y][x] == BOMB)
      {
        grid[y][x] = BOMB;
      }
    }
  }
}
void revealCell(int x, int y)
{
  if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_ROWS)
    return;

  if (gridMain[y][x] == BOMB)
  {
    gameOver = true;
    return;
  }

  ArduinoQueue<Point> q(30);
  q.enqueue({x, y});

  while (!q.isEmpty())
  {
    Point p = q.dequeue();
    int cx = p.x;
    int cy = p.y;

    if (grid[cy][cx] != UNREVEALED)
      continue;

    if (gridMain[cy][cx] == 0)
    {
      grid[cy][cx] = 0;

      for (int dy = -1; dy <= 1; dy++)
      {
        for (int dx = -1; dx <= 1; dx++)
        {
          int nx = cx + dx;
          int ny = cy + dy;
          if (nx >= 0 && nx < GRID_COLS && ny >= 0 && ny < GRID_ROWS)
          {
            if (grid[ny][nx] == UNREVEALED)
            {
              if (gridMain[ny][nx] == 0)
              {
                q.enqueue({nx, ny});
              }
              else if (gridMain[ny][nx] > 0)
              {
                grid[ny][nx] = gridMain[ny][nx];
              }
            }
          }
        }
      }
    }
    else
    {
      grid[cy][cx] = gridMain[cy][cx];
    }
  }
}
void setFlag(int x, int y)
{
  playExplosionSound();
  if (grid[y][x] == UNREVEALED && flagCount < BOMBCOUNT)
  {
    flagCount++;
    grid[y][x] = FLAG;
    if (gridMain[y][x] == BOMB)
    {
      findCount++;
    }
  }

  else if (grid[y][x] == FLAG)
  {
    flagCount--;
    grid[y][x] = UNREVEALED;
    if (gridMain[y][x] == BOMB)
    {
      findCount--;
    }
  }
}
extern int __heap_start, *__brkval;
int freeMemory()
{
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
// Melody
void beep(int frequency, int duration)
{
  long period = 1000000L / frequency;
  long cycles = (long)frequency * duration / 1000;
  for (long i = 0; i < cycles; i++)
  {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(period / 2);
  }
}

void playRevealBeep()
{
  beep(600, 30);
}

void playMoveBeep()
{
  beep(400, 20);
}

void playExplosionSound()
{
  beep(1000, 100);
  beep(800, 100);
  beep(600, 100);
  beep(400, 100);
}

void playGameOverMelody()
{
  beep(330, 200);
  delay(50);
  beep(294, 200);
  delay(50);
  beep(262, 400);
}

void playWinMelody()
{
  beep(262, 150);
  delay(50);
  beep(330, 150);
  delay(50);
  beep(392, 150);
  delay(50);
  beep(523, 300);
}