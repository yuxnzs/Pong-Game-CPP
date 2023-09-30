#include <raylib.h>
#include <iostream>

// Define colors
const Color blue = Color{152, 189, 249, 255};
const Color lightBlue = Color{170, 204, 249, 255};
const Color darkBlue = Color{131, 171, 240, 255};
const Color brown = Color{209, 192, 164, 255};

int playerScore = 0;
int aiScore = 0;
int displaySpeed = 7;

class Ball {
public:
    float x, y;
    int speedX, speedY;
    int radius;

    void Draw(Color color) {
        DrawCircle(x, y, radius, color); // (x, y, radius, color)
    }

    void Update() { // Update the ball's position
        x += speedX * -1; // Multiply by -1 to ensure the ball initially moves towards the AI
        y += speedY;

        if (y + radius >= GetScreenHeight() || y - radius <= 0) { // Using >= instead of == because the movement is 7 pixels each time, so it may not "precisely" collide with the boundary
            speedY *= -1;
        }

        // Check who gets the score
        if (x + radius >= GetScreenWidth()) { 
            aiScore++;
            ResetBall();
        }
        if (x - radius <= 0) {
            playerScore++;
            ResetBall();
        }
    }

    void ResetBall() {
        std::cout << speedX << " ";
        int newSpeed = speedX;
        if (playerScore >= 15 || aiScore >= 15) {
            newSpeed = 11;
            displaySpeed = newSpeed;
        } else if (playerScore >= 10 || aiScore >= 10) {
            newSpeed = 10;
            displaySpeed = newSpeed;
        } else if (playerScore >= 5 || aiScore >= 5) {
            newSpeed = 9;
            displaySpeed = newSpeed;
        } else if (playerScore >= 3 || aiScore >= 3) {
            newSpeed = 8;
            displaySpeed = newSpeed;
        }

        // Reset the ball to the middle of the window
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        // Randomised ball's moving direction after reset
        int speedChoice[2] {-1, 1}; 

        // Make sure the ball's 'x' direction always starts towards the AI, regardless of the score
        // speedX and speedY only update when the score reaches 3. Thus, add this if-else condition to ensure the ball's direction is randomized before score 3
        if (playerScore >= 3 || aiScore >= 3) {
            // Ensure the ball starts with the AI's direction 'after' reaching score 3
            speedX = newSpeed * speedChoice[1]; // Multiplying by 1 to maintain the direction towards the AI
        } else {
            // Ensure the ball starts with the AI's direction 'before' reaching score 3
            speedX = newSpeed * speedChoice[0]; // Multiplying by -1 to maintain the direction towards the AI
        }

        // Only randomize the y (up and down) direction
        speedY = newSpeed * speedChoice[GetRandomValue(0, 1)]; // [0] is -1 (up), [1] is 1 (down)
    }
};

class Paddle {
protected: // Accessible by this class and its derived classes
    void limitMovement() { // Ensure the paddle doesn't move outside of the window
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) { // Ensure paddle doesn't exceed the bottom boundary. y is the starting position, so adding height represents the full size of the paddle
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    virtual void Draw() {
        if (playerScore >= 15 || aiScore >= 15) {
            height = 150;
        } else if (playerScore >= 10 || aiScore >= 10) {
            height = 145;
        } else if (playerScore >= 5 || aiScore >= 5) {
            height = 130;
        } else if (playerScore >= 3 || aiScore >= 3) {
            height = 125;
        }
        /** (rec, roundness, segment, color) int segments: Used to specify the number of segments for generating rounded corners.
         * Higher values produce a smoother rounding effect but also increase the rendering cost */
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (playerScore >= 15 || aiScore >= 15) {
            speed = 25;
        } else if (playerScore >= 10 || aiScore >= 10) {
            speed = 20;
        } else if (playerScore >= 5 || aiScore >= 5) {
            speed = 15;
        } else if (playerScore >= 3 || aiScore >= 3) {
            speed = 12;
        }
        
        // When the key pressed, move the player's paddle
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }

        limitMovement();
    }
};

class AiPaddle : public Paddle {
public:
    void Update(int ball_y) { // Automatically moves based on whether the ball's center is above or below the paddle's center
        if (playerScore >= 15 || aiScore >= 15) {
            speed = 25;
        } else if (playerScore >= 10 || aiScore >= 10) {
            speed = 20;
        }
        
        if (y + height / 2 > ball_y) { // If the ball's center greater than paddle's center
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) { // If the ball's center smaller than or equal to the paddle's center
            y = y + speed;
        }

        limitMovement();
    }
};

// Declare objects
Ball ball, secondBall;
Paddle player;
AiPaddle ai;

int main () {
    const int SCREEN_WIDTH = 1366; // 1366
    const int SCREEN_HEIGHT = 768; // 768
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Game");
    SetTargetFPS(60);

    Image icon = LoadImage("./images/ping-pong.png");  // Replace with your icon path
    SetWindowIcon(icon);
    UnloadImage(icon);

    // Initialize the ball
    ball.radius = 20;
    ball.x = SCREEN_WIDTH / 2; // Starts in the middle of the window
    ball.y = SCREEN_HEIGHT / 2; // Starts in the middle of the window
    ball.speedX = 7; // Set x speed
    ball.speedY = 7; // Set y speed

    // Initialize the secondBall
    secondBall.radius = 20;
    secondBall.x = SCREEN_WIDTH / 2; // Starts in the middle of the window
    secondBall.y = SCREEN_HEIGHT / 2; // Starts in the middle of the window
    secondBall.speedX = 7; // Set x speed
    secondBall.speedY = 7; // Set y speed

    // Initialize the player
    player.width = 25;
    player.height = 120;
    player.x = SCREEN_WIDTH - player.width - 10;
    player.y = SCREEN_HEIGHT / 2 - player.height / 2;
    player.speed = 10;

    // Initialize the ai
    ai.width = 25;
    ai.height = 120;
    ai.x = 10; // This paddle is on the left side, which means there's no need to do calculation
    ai.y = SCREEN_HEIGHT / 2 - ai.height / 2;
    ai.speed = 7;
    
    while (!WindowShouldClose() && !IsKeyPressed(KEY_ENTER)) { // WindowShouldClose detects whether the Esc key or the close button has been clicked
        BeginDrawing();                                        // The loop only be true if both Enter and Esc not clicked

        // Calling the Update method
        ball.Update();
        if (playerScore >= 5 || aiScore >= 5) {
            secondBall.Update(); // When one's score is equal to or greater than 5
        }
        player.Update();
        ai.Update(ball.y);

        // Checking for the first ball's collisions. (Vector2 center, float radius, Rectangle rec)  
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})) {
            ball.speedX *= -1;
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{ai.x, ai.y, ai.width, ai.height})) {
            ball.speedX *= -1;
        }

        // Checking for the second ball's collisions
        if (CheckCollisionCircleRec(Vector2{secondBall.x, secondBall.y}, secondBall.radius, Rectangle{player.x, player.y, player.width, player.height})) {
            secondBall.speedX *= -1;
        }

        if (CheckCollisionCircleRec(Vector2{secondBall.x, secondBall.y}, secondBall.radius, Rectangle{ai.x, ai.y, ai.width, ai.height})) {
            secondBall.speedX *= -1;
        }

        if (playerScore >= 10 || aiScore >= 10) { // Switch to hard mode
            // Clear the background first, then draw the elements
            ClearBackground(WHITE); // If the score surpass a certain number, left will be cleared to white
            DrawText(TextFormat("%d", aiScore), SCREEN_WIDTH / 4 - 20, 20, 80, BLACK); // Change color based on the score
            DrawText(TextFormat("Speed: %d ; Hard Mode", displaySpeed), 10, 10, 20, BLACK);
        } else {
            ClearBackground(darkBlue);
            DrawText(TextFormat("%d", aiScore), SCREEN_WIDTH / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("Speed: %d ; Easy Mode", displaySpeed), 10, 10, 20, WHITE);
        }
        
        DrawRectangle(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, blue);
        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 150, lightBlue);
        DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, WHITE); // (startX, startY, endX, endY, color)
        
        ball.Draw(WHITE);
        
        if (playerScore >= 5 || aiScore >= 5) { 
            secondBall.Draw(WHITE);
        }

        // Left paddle
        ai.Draw();

        // Right paddle
        player.Draw();
        
        DrawText(TextFormat("%d", playerScore), 3 * SCREEN_WIDTH / 4 - 20, 20, 80, WHITE); // "3 *" to move to the right position
        

        EndDrawing();
    }

    CloseWindow();

    return 0;
}