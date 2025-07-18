#include <iostream>
#include <conio.h> // برای getch()
#include <windows.h>
#include <vector>
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for time()
using namespace std;
// empty point = 100
// flag = 200
// number 1-9
// bomb -1;
// defult point = 0
const int WIDTH = 10;
const int HEIGHT = 10;
int cursorX = 0, cursorY = 0;
char grid[HEIGHT][WIDTH];
int gridMain[HEIGHT][WIDTH];
void displayGrid()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == cursorX && y == cursorY)
            {
                if (gridMain[y][x] == 200)
                    cout << "[F]";
                else
                    cout << "[*]";
            }
            else if (gridMain[y][x] == 100)
            {

                cout << "[-]";
            }
            else if (gridMain[y][x] < 0)
            {

                cout << "[X]";
            }
            else if (gridMain[y][x] >= 1 && gridMain[y][x] <= 9)
            {
                cout << "[" << gridMain[y][x] << "]";
            }
            else if (gridMain[y][x] == 200)
            {
                cout << "[F]";
            }
            else if (gridMain[y][x] == 0)
            {
                cout << "[ ]";
            }
        }
        cout << endl;
    }
}
void initGrid()
{
    cout << "Initializing grid..." << endl;
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {

            gridMain[y][x] = 0;
        }
    }
}
void setBomb(int grid[][WIDTH])
{
    srand(time(0));
    int bombCount = 0;
    while (bombCount < 5)
    {
        int x = rand() % (WIDTH + 1);
        int y = rand() % (WIDTH + 1);
        if (grid[y][x] == 0)
        {
            grid[y][x] = -20; // Set bomb
            cout << "Bomb placed at: (" << x << ", " << y << ")" << endl;
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
            if (grid[y][x] < 0)
            {
                // Increment numbers around the bomb
                for (int dy = -1; dy <= 1; dy++)
                {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        if (dy == 0 && dx == 0)
                            continue; // Skip the bomb itself
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT && grid[ny][nx] >= 0)
                        {

                            grid[ny][nx]++;
                        }
                    }
                }
            }
        }
    }
}
void setFlag(int x, int y, int grid[][WIDTH])
{
    if (grid[y][x] == 9)
    {
        grid[y][x] = 10; // Set flag
    }
    else if (grid[y][x] == 10)
    {
        grid[y][x] = 9; // Remove flag
    }
}
void displayGridMain()
{
    system("cls");
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            cout << gridMain[y][x] << " ";
        }
        cout << endl;
    }
}
int main()
{
    initGrid();
    setBomb(gridMain);
    calculateNumbers(gridMain);
    displayGrid();
    // displayGridMain();
    while (true)
    {
        int key = _getch(); // بدون نیاز به Enter

        switch (key)
        {
        case 72: // بالا (arrow up)
            if (cursorY > 0)
                cursorY--;
            break;
        case 80: // پایین (arrow down)
            if (cursorY < HEIGHT - 1)
                cursorY++;
            break;
        case 75: // چپ (arrow left)
            if (cursorX > 0)
                cursorX--;
            break;
        case 77: // راست (arrow right)
            if (cursorX < WIDTH - 1)
                cursorX++;
            break;
        case 32:
            setFlag(cursorX, cursorY, gridMain);
            break;
        case 27: // ESC برای خروج
            return 0;
        }
        displayGrid();
    }
}
