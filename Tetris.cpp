#include "Tetris.h"

using namespace std;
using namespace std::chrono_literals;
    //constructor
    Tetris::Tetris()
    {
        fieldWidth = 12;
        fieldHeight = 18;
        pfield = nullptr;

        ScreenWidth = 80;
        ScreenHeight = 30;
        fieldOffsetX = (ScreenWidth - (fieldWidth + 20)) / 2;
        fieldOffsetY = (ScreenHeight - fieldHeight) / 2;

        CurrentPiece = 0;
        NextPiece = 0;

        CurrentRotation = 0;
        rotateHold = false;

        CurrentX = fieldWidth / 2;
        CurrentY = 0;
        NextX = fieldOffsetX + fieldWidth + 6;
        NextY = fieldOffsetY + 8;

        HeldPiece = -1;
        HasHeld = false;
        holdKeyHeld = false;
        HoldX = fieldOffsetX + fieldWidth + 6;
        HoldY = fieldOffsetY + 14;

        HardDropKeyHeld = false;

        GameOver = false;
        score = 0;
        PieceCount = 0;
        Speed = 20;
        SpeedCounter = 0;
        Level = 1;
        LinesCleared = 0;

        screen = nullptr;
        hConsole = nullptr;
        dwBytesWritten = 0;
    }
    
    //deconstructor
    Tetris::~Tetris()
    {
        delete [] pfield;
        delete [] screen;
    }
    
    //rotate function
    int Tetris::rotate(int px, int py, int rotation)
    {
            // r is how many times the player rotates a given piece; i.e. case 0 being r = 0 
        switch(rotation % 4)
        {
            //formulas found via rotating a 4x4 matrix of x and y
            case 0: return py*4 + px; // 0 degrees
            case 1: return 12 + py - (px * 4); // 90 degrees
            case 2: return 15 -(py * 4) - px; // 180 degrees
            case 3: return 3 - py + (px * 4); // 270 degrees
        }
        return 0;
    }
    
    //checks if piece can fit in a given spot
    bool Tetris::DoesPieceFit(int piece, int rotation, int posx, int posy)
    {
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++)
            {
                //get index into piece
                int pieceIndex = rotate(px, py, rotation);

                //get index into field
                int fieldIndex = (posy + py) * fieldWidth + (posx + px);

                if(posx + px >= 0 && posx + px < fieldWidth)
                {
                    if(posy + py >= 0 && posy + py < fieldHeight)
                    {
                        if(tetromino[piece][pieceIndex] == L'X' && pfield[fieldIndex] != 0)
                        {
                            return false; // fails as there is a collision
                        }
                    }
                }
            }
        }
        return true;
    }
    
    //sets up tetris pieces
    void Tetris::setupTetrominoes()
    {
        //These will be the general shapes of the tetris pieces
        tetromino[0].append(L"..X."); // L prefix tells the program its a wstring 
        tetromino[0].append(L"..X.");
        tetromino[0].append(L"..X.");
        tetromino[0].append(L"..X.");

        tetromino[1].append(L"..X.");
        tetromino[1].append(L".XX.");
        tetromino[1].append(L".X..");
        tetromino[1].append(L"....");

        tetromino[2].append(L".X..");
        tetromino[2].append(L".XX.");
        tetromino[2].append(L"..X.");
        tetromino[2].append(L"....");

        tetromino[3].append(L"....");
        tetromino[3].append(L".XX.");
        tetromino[3].append(L".XX.");
        tetromino[3].append(L"....");

        tetromino[4].append(L"..X.");
        tetromino[4].append(L".XX.");
        tetromino[4].append(L"..X.");
        tetromino[4].append(L"....");

        tetromino[5].append(L"....");
        tetromino[5].append(L".XX.");
        tetromino[5].append(L"..X.");
        tetromino[5].append(L"..X.");

        tetromino[6].append(L"....");
        tetromino[6].append(L".XX.");
        tetromino[6].append(L".X..");
        tetromino[6].append(L".X..");
    }
    
    //sets up tetris field
    void Tetris::setupField()
    {
        pfield = new unsigned char[fieldWidth*fieldHeight]; // Create play field 
        for(int x = 0; x < fieldWidth; x++)// boundary for the field
        { 
            for(int y = 0; y < fieldHeight; y++)
            {
                if(x == 0 || x == fieldWidth - 1 || y == fieldHeight - 1)
                {
                    pfield[y*fieldWidth + x] = 9;
                }
                else
                {
                    pfield[y*fieldWidth + x] = 0;
                }
            }
        }
    }
    
    //handles user input
    void Tetris::handleInput()
    {
        //Force Quit
        if(GetAsyncKeyState(VK_ESCAPE))
        {
            GameOver = true;
        }

        //Left
        if(GetAsyncKeyState(VK_LEFT))
        {
            if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX - 1, CurrentY))
            {
                CurrentX--;
            }
        }

        //Right
        if(GetAsyncKeyState(VK_RIGHT))
        {
            if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX + 1, CurrentY))
            {
                CurrentX++;
            }
        }

        //DOWN
        if(GetAsyncKeyState(VK_DOWN))
        {
            if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
            {
                CurrentY++;
            }
        }

        //Rotation + hold
        if(GetAsyncKeyState(VK_UP))
        {
            if(!rotateHold){
                if(DoesPieceFit(CurrentPiece, CurrentRotation + 1, CurrentX, CurrentY))
                {
                    CurrentRotation++;
                }                
            }
            rotateHold = true;
        }
        else
        {
            rotateHold = false;
        }

        //Hold Piece
        if(GetAsyncKeyState('C'))
        {
            if(!holdKeyHeld && !HasHeld)
            {
                if(HeldPiece == -1)//if holding nothing, store held piece and replace it with the next, then load the next piece
                {
                    HeldPiece = CurrentPiece;
                    CurrentPiece = NextPiece;
                    NextPiece = rand() % 7;
                }
                else //if holding something, swap the current and held pieces
                {
                    int temp = HeldPiece;
                    HeldPiece = CurrentPiece;
                    CurrentPiece = temp;
                }
                CurrentRotation = 0;
                CurrentX = fieldWidth / 2;
                CurrentY = 0;
                HasHeld = true;
            }
            holdKeyHeld = true;
        }
        else
        {
            holdKeyHeld = false;
        }

        //Hard Drop Piece
        if(GetAsyncKeyState(VK_SPACE))
        {
            if(!HardDropKeyHeld)
            {
                int dropY = CurrentY;
                while(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, dropY + 1))
                {
                    dropY++;
                }
                CurrentY = dropY;
                SpeedCounter = Speed - 1;
            }
            HardDropKeyHeld = true;
        }
        else
        {
            HardDropKeyHeld = false;
        }
    }
    
    //handles game states(gravity, lines, game difficulty, )
    void Tetris::update()
    {
        vLines.clear();
        SpeedCounter++;
        bool ForceDown = (SpeedCounter == Speed);

        if(ForceDown)
        {
            SpeedCounter = 0;
            if(DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY + 1))
            {
                CurrentY++;
            }
            else
            {
                //Lock the current piece in the field (look through tetromino 2D array and translate the piece into the field)
                for(int px = 0; px < 4; px++)
                {
                    for(int py = 0; py < 4; py++)
                    {
                        if(tetromino[CurrentPiece][rotate(px, py, CurrentRotation)] == L'X')
                        {
                            pfield[(CurrentY + py) * fieldWidth + (CurrentX + px)] = CurrentPiece + 1;
                        }
                    }
                }

                //increases difficulty (artificially)//line detection for the field
                for(int py = 0; py < 4; py++)
                {
                    if(CurrentY + py < fieldHeight -1)
                    {
                        bool bLine = true;
                        for(int px = 1; px < fieldWidth - 1;px++)
                        {
                            bLine &= (pfield[(CurrentY + py)*fieldWidth + px]) != 0;
                        }
                        if(bLine)
                        {
                            //Remove lines then turn to =
                            for(int px = 1; px < fieldWidth - 1; px++)
                            {
                                pfield[(CurrentY+ py)*fieldWidth + px] = 8;
                            }
                            vLines.push_back(CurrentY + py);
                        }
                    }   
                }

                LinesCleared += vLines.size();

                int newLevel = (LinesCleared / 5) + 1;
                if(newLevel > Level){
                    Level = newLevel;
                    Speed = 20 - ( Level -1) * 2;
                    if(Speed < 5){
                        Speed = 5;
                    }
                }

                //Score calculations
                score += 25;
                if(!vLines.empty()){
                    switch(vLines.size())
                    {
                        case 1: score += 100; break;
                        case 2: score += 300; break;
                        case 3: score += 500; break;
                        case 4: score += 800; break;
                    }
                }
                //Next piece after 
                CurrentPiece = NextPiece;
                NextPiece = rand() % 7;

                HasHeld = false;
                CurrentRotation = 0;
                CurrentX = fieldWidth / 2;
                CurrentY = 0;
                
                //Detects a GameOver
                GameOver = !DoesPieceFit(CurrentPiece, CurrentRotation, CurrentX, CurrentY);
            } 
          
        }
        
    }

    //renders what the user should be seeing
    void Tetris::render()
    {
        //Draw Field
        for(int x = 0; x < fieldWidth; x++)
        {
            for(int y = 0; y < fieldHeight; y++)
            {
                screen[(y + fieldOffsetY)*ScreenWidth + (x + fieldOffsetX)] = L" ABCDEFG=#"[pfield[y*fieldWidth + x]]; //sets output. 0(" ") empty space, (ABCDEFG) are the 7 pieces, 
                                                                                                //8(=) is when you make a line, 9(#) meaning borders
            }
        }

        //Draw Ghost Piece
        int ghostY = CurrentY;
        while(DoesPieceFit(CurrentPiece, CurrentRotation,CurrentX, ghostY + 1))
        {
            ghostY++;
        }
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++){
                if(tetromino[CurrentPiece][rotate(px, py, CurrentRotation)] == L'X')
                {
                    screen[(ghostY + py + fieldOffsetY)*ScreenWidth + (CurrentX + px + fieldOffsetX)] = L'.';
                }
            }
        }

        //Draw Current Piece
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++)
            {
                if(tetromino[CurrentPiece][rotate(px, py,CurrentRotation)] == L'X')
                {
                    screen[(CurrentY + py + fieldOffsetY)*ScreenWidth + (CurrentX + px + fieldOffsetX)] = CurrentPiece + 65;
                }
            } 
        }

        //Clear New Piece
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++)
            {
                screen[(NextY + py)*ScreenWidth + (NextX + px)] = L' ';
            } 
        }

        //Draw Next Piece
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++)
            {
                if(tetromino[NextPiece][rotate(px, py,0)] == L'X')
                {
                    screen[(NextY + py)*ScreenWidth + (NextX + px)] = NextPiece + 65;
                }
            } 
        }

        //Clear Held Piece
        for(int px = 0; px < 4; px++)
        {
            for(int py = 0; py < 4; py++)
            {
                screen[(HoldY + py)*ScreenWidth + (HoldX + px)] = L' ';
            } 
        }        

        //Draw Held Piece
        if(HeldPiece != -1)
        {
            for(int px = 0; px < 4; px++)
                {
                    for(int py = 0; py < 4; py++)
                    {
                        if(tetromino[HeldPiece][rotate(px, py, 0)] == L'X')
                        {
                            screen[(HoldY + py)*ScreenWidth + (HoldX + px)] = HeldPiece + 65;
                        }
                    }
                }
        }    


        //Draws Score / Level / Lines
        swprintf_s(&screen[(fieldOffsetY + 0) * ScreenWidth + fieldOffsetX + fieldWidth + 6], 16, L"SCORE: %8d", score);
        swprintf_s(&screen[(fieldOffsetY + 1) * ScreenWidth + fieldOffsetX + fieldWidth + 6], 16, L"LEVEL: %8d", Level);
        swprintf_s(&screen[(fieldOffsetY + 2) * ScreenWidth + fieldOffsetX + fieldWidth + 6], 16, L"LINES: %8d", LinesCleared);
        swprintf_s(&screen[(fieldOffsetY + 3) * ScreenWidth + fieldOffsetX + fieldWidth + 6], 16, L"SPEED: %8d", Speed); // debug

		if (!vLines.empty())
		{
			// Display Frame (cheekily to draw lines)
			WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay a bit

			for (auto &v : vLines)
				for (int px = 1; px < fieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pfield[py * fieldWidth + px] = pfield[(py - 1) * fieldWidth + px];
					pfield[px] = 0;
				}

			vLines.clear();
		}        
        //Display Current Frame
        WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth * ScreenHeight, {0,0}, &dwBytesWritten); 
            
    }

    //sets up the game
    void Tetris::initialize()
    {
        setupTetrominoes();
        setupField();
        CurrentPiece = rand() % 7;
        NextPiece = rand() % 7;

        screen = new wchar_t[ScreenWidth*ScreenHeight];

        for(int i = 0; i < ScreenWidth * ScreenHeight; i++)
        {
            screen[i] = L' ';
        }

        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        SMALL_RECT windowSize = {0, 0, (SHORT)(ScreenWidth - 1), (SHORT)(ScreenHeight - 1)};
        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);

        
        COORD bufferSize = {(SHORT)ScreenWidth, (SHORT)ScreenHeight};
        SetConsoleScreenBufferSize(hConsole, bufferSize);

        SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
        system("cls");
    }

    //runs the game
    void Tetris::run()
    {
        while(!GameOver){
            //Timing
            this_thread::sleep_for(50ms);

            //Input
            handleInput();

            //Game Logic
            update();

            //Rendering
            render();

        }
        cout << "Game Over!! Score: " << score <<endl;
        system("pause");
    }