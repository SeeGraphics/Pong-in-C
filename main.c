#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

// Function to generate a random direction vector and normalize it
Vector2 GetRandomNormalizedDirection()
{
    Vector2 direction;
    direction.x = GetRandomValue(-100, 100) / 100.0f; // Random float between -1 and 1
    direction.y = GetRandomValue(-50, 50) / 100.0f;   // Smaller Y range for better gameplay

    // Handle the case where we get (0,0) - very unlikely but safe
    if (direction.x == 0 && direction.y == 0)
    {
        direction.x = 1.0f;
    }

    // Normalize the vector to length 1
    direction = Vector2Normalize(direction);
    return direction;
}

int main()
{
    const int window_width = 1000;
    const int window_height = 600;

    // game states
    typedef enum
    {
        GAME_PLAYING,
        GAME_SCORED,
        GAME_GUI_OPEN
    } GameState;

    // game difficulties
    typedef enum
    {
        EASY = 120,
        NORMAL = 250,
        EXPERT = 400
    } GameDifficulty;

    GameDifficulty gameDifficulty;
    GameState gameState = GAME_GUI_OPEN;

    // Player
    int playerSpeed = 400;
    Vector2 playerSize = {10, 120};
    Vector2 playerPos = {window_width - 20, (window_height / 2) - (playerSize.y / 2)};

    // Enemy
    int enemySpeed;
    Vector2 enemySize = {10, 120};
    Vector2 enemyPos = {10, (window_height / 2) - (playerSize.y / 2)};

    // Pong ball
    int pongSpeed = 450;
    int pongSize = 10;
    Vector2 pongDirection = GetRandomNormalizedDirection();
    Vector2 pongVel = Vector2Scale(pongDirection, pongSpeed);
    Vector2 pongPos = {window_width / 2, window_height / 2};

    // score
    int playerScore = 0;
    int enemyScore = 0;
    float scoreTimer = 0.0f;

    InitWindow(window_width, window_height, "1 Player Pong");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    while (!WindowShouldClose())
    {
        // update
        float dt = GetFrameTime();

        // begin drawing for all future elements
        BeginDrawing();
        ClearBackground(BLACK);

        // SELECT DIFFICULTY GUI
        if (gameState == GAME_GUI_OPEN)
        {
        }

        // GAME PLAYING
        if (gameState == GAME_PLAYING)
        {
            // ESC opens MAIN MENU
            if (IsKeyPressed(KEY_ESCAPE))
                gameState = GAME_GUI_OPEN;

            // draw elements
            DrawCircle(pongPos.x, pongPos.y, pongSize, WHITE);
            DrawRectangle(playerPos.x, playerPos.y, playerSize.x, playerSize.y, WHITE);
            DrawRectangle(enemyPos.x, enemyPos.y, enemySize.x, enemySize.y, WHITE);
            DrawText(TextFormat("%d | %d", enemyScore, playerScore), window_width / 2 - 30, 30, 30, WHITE);

            // move pong ball
            pongPos.x += pongVel.x * dt;
            pongPos.y += pongVel.y * dt;

            // player movement
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            {
                playerPos.y -= playerSpeed * dt;
            }
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            {
                playerPos.y += playerSpeed * dt;
            }

            // enemy movement
            if (pongVel.x < 0)
            {
                float enemyCenter = enemyPos.y + enemySize.y / 2;
                float deadZone = 25.0f; // Adjust this value to reduce jitter

                if (pongPos.y < enemyCenter - deadZone)
                {
                    enemyPos.y -= enemySpeed * dt;
                }
                else if (pongPos.y > enemyCenter + deadZone)
                {
                    enemyPos.y += enemySpeed * dt;
                }
                // If ball is within dead zone, don't move
            }

            // player(s) boundaries
            playerPos.y = Clamp(playerPos.y, 0, window_height - playerSize.y);

            // Pong bouncing
            if (pongPos.y - pongSize <= 0 || pongPos.y + pongSize >= window_height)
            {
                pongVel.y = -pongVel.y;
            }

            // Pong collision with paddles
            // Check if ball is hitting the player paddle (right side)
            if (pongPos.x + pongSize >= playerPos.x &&
                pongPos.x - pongSize <= playerPos.x + playerSize.x &&
                pongPos.y + pongSize >= playerPos.y &&
                pongPos.y - pongSize <= playerPos.y + playerSize.y)
            {
                pongVel.x = -pongVel.x;
            }

            // Check if ball is hitting the enemy paddle (left side)
            else if (pongPos.x - pongSize <= enemyPos.x + enemySize.x &&
                     pongPos.x + pongSize >= enemyPos.x &&
                     pongPos.y + pongSize >= enemyPos.y &&
                     pongPos.y - pongSize <= enemyPos.y + enemySize.y)
            {
                pongVel.x = -pongVel.x;
            }

            // score system
            if (pongPos.x - pongSize <= 0)
            {
                playerScore += 1;
                scoreTimer = 2.0f;
                gameState = GAME_SCORED;

                // reset positions
                pongPos.x = window_width / 2;
                pongPos.y = window_height / 2;
                playerPos.x = window_width - 20;
                playerPos.y = (window_height / 2) - (playerSize.y / 2);
                enemyPos.x = 10;
                enemyPos.y = (window_height / 2) - (playerSize.y / 2);

                pongVel.x = 0;
                pongVel.y = 0;
            }
            if (pongPos.x + pongSize >= window_width)
            {
                enemyScore += 1;
                scoreTimer = 2.0f;
                gameState = GAME_SCORED;

                // reset positions
                pongPos.x = window_width / 2;
                pongPos.y = window_height / 2;
                playerPos.x = window_width - 20;
                playerPos.y = (window_height / 2) - (playerSize.y / 2);
                enemyPos.x = 10;
                enemyPos.y = (window_height / 2) - (playerSize.y / 2);

                pongVel.x = 0;
                pongVel.y = 0;
            }
        }

        // POINT SCORED
        else if (gameState == GAME_SCORED)
        {
            // draw elements
            DrawText(TextFormat("%d | %d", enemyScore, playerScore), window_width / 2 - 30, 30, 30, WHITE);
            DrawText(TextFormat("%.1f", scoreTimer), window_width / 2 - 30, 200, 50, WHITE);
            DrawRectangle(playerPos.x, playerPos.y, playerSize.x, playerSize.y, WHITE);
            DrawRectangle(enemyPos.x, enemyPos.y, enemySize.x, enemySize.y, WHITE);

            // flashing pong ball
            if (fmod(scoreTimer, 1.0f) < 0.5f)
            {
                DrawCircle(pongPos.x, pongPos.y, pongSize, WHITE);
            }

            scoreTimer -= dt;

            // reset ball
            if (scoreTimer <= 0)
            {
                gameState = GAME_PLAYING;
                pongDirection = GetRandomNormalizedDirection();
                pongVel = Vector2Scale(pongDirection, pongSpeed);
            }
        }

        if (gameState == GAME_GUI_OPEN)
        {
            // reset ball to prevent buggy points or showing old position
            pongPos.x = window_width / 2;
            pongPos.y = window_height / 2;

            Vector2 mousePos = GetMousePosition();

            // "PONG" title
            const char *titleText = "PONG";
            int titleSize = 90;
            int titleWidth = MeasureText(titleText, titleSize);

            // Scrolling text effect
            float scrollSpeed = 30.0f;
            int spacing = 20; // Gap between each PONG
            int totalWidth = titleWidth + spacing;
            float scrollX = fmod(GetTime() * scrollSpeed, totalWidth);

            for (int i = -1; i < 10; i++)
            {
                float x = scrollX + (i * totalWidth);
                DrawText(titleText, x, 50, titleSize, WHITE);
            }

            // Difficulties
            // Easy button
            const char *easyText = "EASY";
            int easySize = 48;
            Color easyColor = WHITE;
            int easyWidth = MeasureText(easyText, easySize);
            int easyX = (window_width - easyWidth) / 2;
            int easyY = 200;
            Rectangle easyRect = {easyX, easyY, easyWidth, easySize};

            // Normal button
            const char *normalText = "NORMAL";
            int normalSize = 48;
            Color normalColor = WHITE;
            int normalWidth = MeasureText(normalText, normalSize);
            int normalX = (window_width - normalWidth) / 2;
            int normalY = 280;
            Rectangle normalRect = {normalX, normalY, normalWidth, normalSize};

            // Expert button
            const char *expertText = "EXPERT";
            int expertSize = 48;
            Color expertColor = WHITE;
            int expertWidth = MeasureText(expertText, expertSize);
            int expertX = (window_width - expertWidth) / 2;
            int expertY = 360;
            Rectangle expertRect = {expertX, expertY, expertWidth, expertSize};

            // Hover animation
            if (CheckCollisionPointRec(mousePos, easyRect))
            {
                easyColor = DARKGRAY;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            }

            if (CheckCollisionPointRec(mousePos, normalRect))
            {
                normalColor = DARKGRAY;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            }

            if (CheckCollisionPointRec(mousePos, expertRect))
            {
                expertColor = DARKGRAY;
                SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
            }

            if (!CheckCollisionPointRec(mousePos, expertRect) && !CheckCollisionPointRec(mousePos, normalRect) && !CheckCollisionPointRec(mousePos, easyRect))
            {
                SetMouseCursor(MOUSE_CURSOR_DEFAULT);
            }

            // Draw buttons
            DrawText(easyText, easyX, easyY, easySize, easyColor);
            DrawText(normalText, normalX, normalY, normalSize, normalColor);
            DrawText(expertText, expertX, expertY, expertSize, expertColor);

            // Check for clicks
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (CheckCollisionPointRec(mousePos, easyRect))
                {
                    gameDifficulty = EASY;
                    gameState = GAME_PLAYING;
                    enemySpeed = gameDifficulty;
                    pongDirection = GetRandomNormalizedDirection();
                    pongVel = Vector2Scale(pongDirection, pongSpeed);
                }
                else if (CheckCollisionPointRec(mousePos, normalRect))
                {
                    gameDifficulty = NORMAL;
                    gameState = GAME_PLAYING;
                    enemySpeed = gameDifficulty;
                    pongDirection = GetRandomNormalizedDirection();
                    pongVel = Vector2Scale(pongDirection, pongSpeed);
                }
                else if (CheckCollisionPointRec(mousePos, expertRect))
                {
                    gameDifficulty = EXPERT;
                    gameState = GAME_PLAYING;
                    enemySpeed = gameDifficulty;
                    pongDirection = GetRandomNormalizedDirection();
                    pongVel = Vector2Scale(pongDirection, pongSpeed);
                }
            }

            // subtitle
            const char *subtitleText = "select difficulty to play";
            int subtitleSize = 40;
            int subtitleWidth = MeasureText(subtitleText, subtitleSize);

            if (fmod(GetTime(), 1.0f) < 0.5f) // Flash every 1 second
            {
                DrawText(subtitleText, (window_width - subtitleWidth) / 2, window_height - 100, subtitleSize, LIGHTGRAY);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
