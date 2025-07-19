
//! BUG: Some Cell dont open
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
using namespace std;

const int WIDTH = 10;
const int HEIGHT = 10;
const int FLAG = 200;
const int EMPTY = 0;
const int UNREVEALED = -2;
const int BOMB = -1;
const int BOMBCOUNT = 10;
int findCount = 0;
int flagCount = 0;
bool gameOver = false;
int cursorX = 0, cursorY = 0;
bool firstCell = true;
int grid[HEIGHT][WIDTH];     // برای وضعیت قابل نمایش به کاربر
int gridMain[HEIGHT][WIDTH]; // وضعیت واقعی بازی (مین‌ها و اعداد)

void revealAllBomb()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (gridMain[y][x] == BOMB)
            {
                grid[y][x] = BOMB;
            }
        }
    }
}

void displayGrid()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == cursorX && y == cursorY)
            {
                cout << "[*]";
            }
            else if (grid[y][x] == FLAG)
            {
                cout << "[F]";
            }
            else if (grid[y][x] == EMPTY)
            {
                cout << "[ ]";
            }
            else if (grid[y][x] >= 1 && grid[y][x] <= 8)
            {
                cout << "[" << grid[y][x] << "]";
            }
            else if (grid[y][x] == BOMB)
            {
                cout << "[X]";
            }
            else
            {
                cout << "[-]";
            }
        }
        cout << endl;
    }
}
void displayGridMain()
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (gridMain[y][x] == BOMB)
            {
                cout << "[X]";
            }
            else
            {
                cout << "[" << gridMain[y][x] << "]";
            }
        }
        cout << endl;
    }
}
void displayGridReal()
{

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            cout << "[" << grid[y][x] << "]";
        }
        cout << endl;
    }
}
void initGrid()
{
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            grid[y][x] = UNREVEALED;
}

void setBomb(int grid[][WIDTH], int cursorX, int cursorY)
{
    srand(time(0));
    int bombCount = 0;
    while (bombCount < BOMBCOUNT) // تعداد مین‌ها را اینجا می‌توان تنظیم کرد
    {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        if (abs(x - cursorX) <= 1 && abs(y - cursorY) <= 1)
            continue;

        if (grid[y][x] != BOMB)
        {
            grid[y][x] = BOMB;
            bombCount++;
        }
    }
}

void calculateNumbers(int grid[][WIDTH])
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (grid[y][x] == BOMB)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && grid[ny][nx] != BOMB)
                            grid[ny][nx]++;
                    }
                }
            }
        }
    }
}

void setFlag(int x, int y)
{
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

void revealCell(int x, int y)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    // if (grid[y][x] != UNREVEALED)
    //     return;

    if (gridMain[y][x] == BOMB)
    {

        gameOver = true;
    }

    queue<pair<int, int>> q;
    q.push({x, y});

    while (!q.empty())
    {
        auto [cx, cy] = q.front();
        q.pop();

        if (grid[cx][cy] != UNREVEALED)

            continue;

        if (gridMain[cy][cx] == 0)
        {
            grid[cy][cx] = gridMain[cy][cx];

            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int nx = cx + dx;
                    int ny = cy + dy;
                    if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
                    {
                        if (grid[ny][nx] == UNREVEALED)
                        {

                            if (gridMain[ny][nx] == 0)
                                q.push({nx, ny});
                            else if (gridMain[ny][nx] > 0)
                                grid[ny][nx] = gridMain[ny][nx];
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

int main()
{

    if (firstCell)
    {
        initGrid();
        displayGrid();
    }

    while (!gameOver)
    {
        if (findCount == BOMBCOUNT)
        {
            cout << "You found all the bombs!" << endl;
            gameOver = true;
            break;
        }
        int key = _getch();
        switch (key)
        {
        case 72:
            if (cursorY > 0)
                cursorY--;
            break; // بالا
        case 80:
            if (cursorY < HEIGHT - 1)
                cursorY++;
            break; // پایین
        case 75:
            if (cursorX > 0)
                cursorX--;
            break; // چپ
        case 77:
            if (cursorX < WIDTH - 1)
                cursorX++;
            break; // راست
        case 32:
            setFlag(cursorX, cursorY);
            break; // اسپیس = پرچم
        case 13:
            if (firstCell)
            {
                setBomb(gridMain, cursorX, cursorY);
                calculateNumbers(gridMain);
                revealCell(cursorX, cursorY);
                firstCell = false;
                break;
            }
            revealCell(cursorX, cursorY);
            break; // اینتر = باز کردن
        case 27:
            return 0; // ESC
        }
        displayGrid();
        cout << "====================" << endl;
        displayGridMain();
        cout << "====================" << endl;
        displayGridReal();
        cout << "====================" << endl;
        cout << "Flags: " << flagCount << "  Finds: " << findCount << endl;
    }
    revealAllBomb();
    displayGrid();
    cout << "====================" << endl;
    cout << "Flags: " << flagCount << "  Finds: " << findCount << endl;
    exit(0);
}
