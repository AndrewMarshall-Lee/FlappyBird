#ifndef UNICODE
#define UNICODE
#endif


#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <winuser.h>
#include <algorithm>
//Console Variables
int nScreenWidth = 120;
int nScreenHeight = 40;

//player position, only y will change
int PlayerX = nScreenWidth / 4;
float PlayerY = nScreenHeight / 2.0;




int main()
{
    //create buffer screen
    wchar_t* screen = new wchar_t[nScreenHeight * nScreenWidth];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    bool bGameOver = false;
    bool bGameStart = false;
    bool Space = false;
    bool Scored = false;
    bool Collided = false;
    bool Restart = false;

    float fVy = 0.0f;
    float fElapsedTime;
    float Scroll = 25.0f;

    float fGrav = 25.0f;
    float fVyMax = 20.0f;

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    float Wally[] = { 1.5f * nScreenHeight / 3.0f, 2.0f * nScreenHeight / 3.0f, nScreenHeight / 3.0f };
    float Wallx[] = { 3.0f * nScreenWidth / 3.0f, 4.0f * nScreenWidth / 3.0f, 5.0f * nScreenWidth / 3.0f };

    int WallWidth = 2;
    int GapHeight = 10;
    int Score = 0;



    while (!bGameOver)
    {
        //We need to control time to get a consistent acceleration
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        fElapsedTime = elapsedTime.count();

        //Space to start game and flap, but only flaps when going down
        Space = (0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) != 0;
        if (Space)
        {
            if (!bGameStart)
                bGameStart = true;
            if (fVy < 0)
            {
                fVy = fVy;
            }
            else
            {
                fVy = -15.0;
            }
        }
        if (bGameStart)
        {
            //apply gravity
            fVy += fElapsedTime * fGrav;
            //Scroll to the left
            for (int i = 0; i < sizeof(Wallx) / sizeof(Wallx[0]); i++)
            {
                Wallx[i] -= Scroll * fElapsedTime;
                if (Wallx[i] < -WallWidth)
                {
                    Wallx[i] = nScreenWidth + WallWidth;
                    //Making it so the boundary of the gap doesn't go off the edge
                    Wally[i] = rand() % nScreenHeight;
                    while (Wally[i] < GapHeight / 2 || Wally[i] > nScreenHeight - (GapHeight / 2))
                        Wally[i] = rand() % nScreenHeight;

                }
                if (PlayerX == int(Wallx[i]) && !Scored)
                {
                    Score++;
                    Scored = true;
                }
                else
                {
                    Scored = false;
                }

            }
            //Set max fall speed
            if (fVy >= fVyMax)
            {
                fVy = fVyMax;
            }

        }

        PlayerY += fVy * fElapsedTime;


        //cout << fVy;
        //screen[nScreenWidth * nScreenHeight - 1] = '\0';


        //Draw Walls
        for (int x = 0; x < nScreenWidth; x++) {
            for (int y = 0; y < nScreenHeight; y++) {
                //each tile is drawn based on value in array and a set of characters, e.g. 0 = " ", 1 = "A"
                if ((Wallx[0] - WallWidth) < x && x < (Wallx[0] + WallWidth) && (y > Wally[0] + (GapHeight / 2) || y < Wally[0] - (GapHeight / 2)))
                    screen[y * nScreenWidth + x] = 0x2588;
                else if ((Wallx[1] - WallWidth) < x && x < (Wallx[1] + WallWidth) && (y > Wally[1] + (GapHeight / 2) || y < Wally[1] - (GapHeight / 2)))
                    screen[y * nScreenWidth + x] = 0x2588;
                else if ((Wallx[2] - WallWidth) < x && x < (Wallx[2] + WallWidth) && (y > Wally[2] + (GapHeight / 2) || y < Wally[2] - (GapHeight / 2)))
                    screen[y * nScreenWidth + x] = 0x2588;
                else
                    screen[y * nScreenWidth + x] = L" "[0];

                if (y == 0 || y == nScreenHeight - 1)
                    screen[y * nScreenWidth + x] = 0x2588;
            }

        }


        //Draw Bird
        //TO DO: IMPLEMENT COLLISION DETECTION, ADD SCORE.




        if (fVy > 0)
        {
            wsprintf(&screen[int(PlayerY) * nScreenWidth + PlayerX], L"\\\\");
            wsprintf(&screen[int(PlayerY + 1) * nScreenWidth + PlayerX], L"<\\\\=Q");
        }
        else
        {
            wsprintf(&screen[int(PlayerY) * nScreenWidth + PlayerX], L"<///=Q");
            wsprintf(&screen[int(PlayerY + 1) * nScreenWidth + PlayerX], L"///");
        }

        //Collision detection
        //bird length is 6 so just see if each corner touches
        for (int x = 0; x < 6; x++)
            if (screen[int(PlayerY) * nScreenWidth + (PlayerX + x)] == 0x2588 || screen[int(PlayerY + 1) * nScreenWidth + (PlayerX + x)] == 0x2588 ||
                screen[int(PlayerY - 1) * nScreenWidth + (PlayerX + x)] == 0x2588 || screen[int(PlayerY + 2) * nScreenWidth + (PlayerX + x)] == 0x2588)
            {
                Collided = true;

                if (fVy > 0)
                {
                    wsprintf(&screen[int(PlayerY) * nScreenWidth + PlayerX], L"\\\\");
                    wsprintf(&screen[int(PlayerY + 1) * nScreenWidth + PlayerX], L"<\\\\=X");
                }
                else
                {
                    wsprintf(&screen[int(PlayerY) * nScreenWidth + PlayerX], L"<///=X");
                    wsprintf(&screen[int(PlayerY + 1) * nScreenWidth + PlayerX], L"///");
                }


            }
        //Display Stats
        wsprintf(&screen[nScreenWidth + 20], L"y = %d, Vy = %d, Score = %d", int(PlayerY), int(fVy), Score);
        if (!bGameStart)
        {
            wsprintf(&screen[(nScreenHeight / 2 - 3) * nScreenWidth + 20], L"Press Space to begin!!");
        }
        // swprintf_s(screen, nScreenWidth + 20, L"[%3f, %3f, %3f, %3f]",Wallx[0], Wallx[1], Wallx[2], Wallx[3]);
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

        if (Collided)
        {
            bGameOver = true;
        }
    }



    while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
    return 0;
}
