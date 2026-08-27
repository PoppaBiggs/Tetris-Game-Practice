#ifndef TETRIS_H
#define TETRIS_H

#include <iostream>
#include <vector>
#define UNICODE
#define _UNICODE
#include <Windows.h>
#include <thread>
#include <chrono>


class Tetris
{
    private:
        //Playfield
        int fieldWidth;
        int fieldHeight;
        unsigned char* pfield;

        //Tetrominoes
        std::wstring tetromino[7];

        //Current Piece
        int CurrentPiece;
        int NextPiece;
        bool rotateHold;
        int CurrentRotation;
        int CurrentX;
        int CurrentY;
        int NextX;
        int NextY;
        int fieldOffsetX;
        int fieldOffsetY;

        //Holding Piece
        int HeldPiece;
        bool HasHeld;
        bool holdKeyHeld;
        int HoldX;
        int HoldY;

        //Hard Drop
        bool HardDropKeyHeld;

        //GameStats
        bool GameOver;
        int score;
        int PieceCount;
        int Speed;
        int SpeedCounter;
        std::vector <int> vLines;
        int Level;
        int LinesCleared;

        //Rendering or Screen
        int ScreenWidth;
        int ScreenHeight;
        wchar_t* screen;
        HANDLE hConsole;
        DWORD dwBytesWritten;

    private:
        //Helper functions 
        void setupTetrominoes();
        void setupField();
        void handleInput();
        void update();
        void render();

    public: 
        Tetris();
        ~Tetris();

        //Game Setup
        int rotate(int px, int py, int rotation);
        bool DoesPieceFit(int piece, int rotation, int posx, int posy);

        //Game Loop
        void initialize();
        void run();
};
#endif
