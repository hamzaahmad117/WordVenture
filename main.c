#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "raygui.h"

typedef enum GameScreen
{
    TITLE,
    GAME,
} GameScreen;
#define MAX_LINE_LENGTH 1000
#define MAX_CLUES 100
#define MAX_INPUT_CHARS 1

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
#define COLS 8
#define ROWS 8

const int screenWidth = 800;
const int screenHeight = 900;

const int cellWidth = screenWidth / COLS;
const int cellHeight = (screenHeight - 300) / ROWS;

typedef struct Cell
{
    int i;
    int j;
    char letter;
    bool valid;
    char h;

} Cell;

Cell grid[ROWS][COLS];
void CellDraw(Cell);
void read_puzzle();
void read_hints();

int activeCellRow = -1;
int activeCellCol = -1;
int won = 0;

bool IndexIsValid(int indexI, int indexJ)
{

    return indexI >= 0 && indexI < COLS && indexJ >= 0 && indexJ < ROWS;
}

void CellReveal(int, int);
int main(void)
{
    // Initialization
    //-------------------------------------------------------------------------------------

    InitWindow(screenWidth, screenHeight, "CrossWord Puzzle");
    for (int i = 0; i < COLS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            grid[i][j] = (Cell){
                .i = i,
                .j = j,
                .letter = '-'};
        }
    }

    GameScreen currentScreen = TITLE;
    char filePath[50];
    char hintPath[50];
    // Buttons positions
    Rectangle easyButton = {screenWidth / 2 - 100, 300, 200, 50};
    Rectangle mediumButton = {screenWidth / 2 - 100, 400, 200, 50};
    Rectangle hardButton = {screenWidth / 2 - 100, 500, 200, 50};

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {

        char inputChar[MAX_INPUT_CHARS + 1] = "\0"; // NOTE: One extra space required for null terminator char '\0'
        int letterCount = 0;
        int framesCounter = 0;

        if (currentScreen == TITLE)
        {
            if (CheckCollisionPointRec(GetMousePosition(), easyButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                sprintf(filePath, "./easy.txt");
                sprintf(hintPath, "./easy_hints.txt");
                currentScreen = GAME;
                read_puzzle(filePath);
            }
            else if (CheckCollisionPointRec(GetMousePosition(), mediumButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                sprintf(filePath, "./medium.txt");
                sprintf(hintPath, "./medium_hints.txt");
                currentScreen = GAME;
                read_puzzle(filePath);
            }
            if (CheckCollisionPointRec(GetMousePosition(), hardButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                sprintf(filePath, "./hard.txt");
                sprintf(hintPath, "./hard_hints.txt");
                currentScreen = GAME;
                read_puzzle(filePath);
            }
            // Similar checks for mediumButton and hardButton
        }
        else if (currentScreen == GAME)
        {
            //----------------------------------------------------------------------------------
            // Display a text box for the active cell
            read_hints(hintPath);
            for (int i = 0; i < COLS; i++)
            {
                for (int j = 0; j < ROWS; j++)
                {
                    CellDraw(grid[i][j]);
                    if (grid[i][j].letter != '-' && !grid[i][j].valid)
                    {
                        DrawRectangleLines(grid[i][j].i * cellWidth, grid[i][j].j * cellHeight, cellWidth, cellHeight, BLACK);
                        Rectangle textBox = {grid[i][j].i * cellWidth, grid[i][j].j * cellHeight, cellWidth, cellHeight};

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), textBox))
                        {
                            activeCellCol = i;
                            activeCellRow = j;
                        }
                        if (i == activeCellCol && j == activeCellRow)
                        {
                            DrawRectangleRec(textBox, LIGHTGRAY);
                            // Get char pressed (unicode character) on the queue
                            int key = GetCharPressed();

                            // Check if more characters have been pressed on the same frame
                            while (key > 0)
                            {
                                // NOTE: Only allow keys in range [32..125]
                                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                                {
                                    inputChar[letterCount] = (char)key;
                                    inputChar[letterCount + 1] = '\0'; // Add null terminator at the end of the string.
                                    letterCount++;
                                }

                                key = GetCharPressed(); // Check next character in the queue
                            }

                            DrawText(inputChar, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

                            if (letterCount < MAX_INPUT_CHARS)
                            {
                                // Draw blinking underscore char
                                if (((framesCounter / 20) % 2) == 0)
                                    DrawText("_", (int)textBox.x + 8 + MeasureText(inputChar, 40), (int)textBox.y + 12, 40, MAROON);
                            }

                            // compare user input with cell letter
                            if (grid[i][j].letter == inputChar[0])
                            {
                                grid[i][j].valid = true;
                                // Convert the character to a string
                                char letter[2] = {grid[i][j].letter, '\0'};

                                // Calculate the position to draw the letter
                                int x = grid[i][j].i * cellWidth + cellWidth / 2;   // Center of the cell horizontally
                                int y = grid[i][j].j * cellHeight + cellHeight / 2; // Center of the cell vertically
                                // Use DrawText with the string at the calculated position
                                DrawText(letter, x, y, 20, MAROON);
                                CellDraw(grid[i][j]);
                            }
                        }
                    }
                }
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (currentScreen == TITLE)
        {
            DrawText("Crossword Puzzle", screenWidth / 2 - MeasureText("Crossword Puzzle", 40) / 2, 50, 40, BLACK);

            // Draw easy button with hover effect
            Color easyButtonColor = CheckCollisionPointRec(GetMousePosition(), easyButton) ? SKYBLUE : DARKBLUE;
            DrawRectangleRec(easyButton, easyButtonColor);
            DrawText("Easy", easyButton.x + 70, easyButton.y + 15, 20, BLACK);

            // Draw medium button with hover effect
            Color mediumButtonColor = CheckCollisionPointRec(GetMousePosition(), mediumButton) ? GREEN : DARKGREEN;
            DrawRectangleRec(mediumButton, mediumButtonColor);
            DrawText("Medium", mediumButton.x + 60, mediumButton.y + 15, 20, BLACK);

            // Draw hard button with hover effect
            Color hardButtonColor = CheckCollisionPointRec(GetMousePosition(), hardButton) ? RED : MAROON;
            DrawRectangleRec(hardButton, hardButtonColor);
            DrawText("Hard", hardButton.x + 70, hardButton.y + 15, 20, BLACK);
        }
        EndDrawing();
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //-------------------------------------------------------------------------------------

    return 0;
}

void CellDraw(Cell cell)
{
    int win=1;
    for(int i=0;i<ROWS;i++)
    {
        for (int j=0;j<COLS;j++){
            if(!grid[i][j].valid && grid[i][j].letter!='-')
            {
                win=0;
            }
        }
    }

    if (win==1)
    {
      DrawText(TextFormat("You Win!"), cell.i * cellWidth + 10, cell.j * cellHeight + 4, 20, MAROON);
    }
    else
    {

    if (cell.letter != '-')
    {
        DrawRectangle(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, WHITE);
        DrawRectangleLines(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, BLACK);
        if (cell.valid)
        {
            DrawRectangle(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, LIGHTGRAY);
            DrawText(TextFormat("%c", cell.letter), cell.i * cellWidth + 24, cell.j * cellHeight + 14, cellHeight - 12, BLACK);
            DrawRectangleLines(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, BLACK);
        }
        if (cell.h != '\0')
        {
            DrawText(TextFormat("%c", cell.h), cell.i * cellWidth + 10, cell.j * cellHeight + 4, 20, MAROON);
            DrawRectangleLines(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, BLACK);
        }
    }
    else
    {
        DrawRectangle(cell.i * cellWidth, cell.j * cellHeight, cellWidth, cellHeight, BLACK);
    }
    }


}

void read_puzzle(char filePath[])
{
    FILE *file = fopen(filePath, "r");

    // Check if the file was opened successfully
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Buffer to store each line
    char line[MAX_LINE_LENGTH];

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Remove the trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Variables to store the row, column, and the character
        int row, col;
        char ch;
        char hint = '\0';

        // Extract the row, column, and the character from the line
        if (sscanf(line, "%1d%1d%c%c", &row, &col, &ch, &hint) == 4)
        {
            // If sscanf successfully reads three values, print them
            printf("Row: %d, Column: %d, Character: %c, Hint: %c\n", row, col, ch, hint);
        }
        else if (sscanf(line, "%1d%1d%c", &row, &col, &ch) == 3)
        {
            printf("Row: %d, Column: %d, Character: %c\n", row, col, ch);
        }
        else
        {
            // If sscanf fails to read three values, print an error message
            fprintf(stderr, "Error: Invalid line format '%s'\n", line);
        }

        grid[col][row] = (Cell){
            .i = col,
            .j = row,
            .letter = ch,
            .h = hint};
    }
    // Close the file
    fclose(file);
}

void read_hints(char hintPath[])
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    int numbers[MAX_CLUES];
    char directions[MAX_CLUES][10];
    char clues[MAX_CLUES][100];
    int clueCount = 0;

    file = fopen(hintPath, "r");
        if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL && clueCount < MAX_CLUES)
    {
        char *token;

        // Get the first token (number)
        token = strtok(line, ",");
        if (token != NULL)
        {
            numbers[clueCount] = atoi(token);

            // Get the second token (direction)
            token = strtok(NULL, ",");
            if (token != NULL)
            {
                strcpy(directions[clueCount], token);

                // Get the third token (clue)
                token = strtok(NULL, ",");
                if (token != NULL)
                {
                    strcpy(clues[clueCount], token);
                }
            }
        }
        clueCount++;
    }

    fclose(file);
    if (clueCount > 5)
    {
        for (int i = 0; i < clueCount; i++)
        {
            DrawText(TextFormat("\n%d: %s -> %s", numbers[i], directions[i], clues[i]), 50, 600 + 35 * i, 20, BLACK);
        }
    }
    else
    {
        for (int i = 0; i < clueCount; i++)
        {
            DrawText(TextFormat("\n%d: %s -> %s", numbers[i], directions[i], clues[i]), 50, 600 + 60 * i, 20, BLACK);
        }
    }
}