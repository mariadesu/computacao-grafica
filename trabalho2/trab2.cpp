/**
 * trabalho-2.cpp
 *
 * This program implements the 3D version of the penguin game as specified in the "Trabalho (Etapa 2)" document.
 *
 * Key Features:
 * - 3D models for penguins and fish using GLUT solid shapes.
 * - Control scheme for 3D navigation (rotation and forward/backward movement).
 * - Four-viewport display (Top, Front, Side, and Free-roam perspective).
 * - Camera in each viewport follows the mother penguin.
 * - Game logic adapted for a 3D environment, including collision detection,
 * fish collection, and feeding mechanics.
 * - Optional: Simple lighting is enabled.
 */
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include <iostream>

#define PI 3.141592654

// --- Game State Variables ---
bool gameWon = false;
bool gameLost = false;
int babyEnergy = 60; // 1 minute of energy
int gameTime = 300; // 5 minutes total game time

// --- Mother Penguin State ---
float pinguimX = 0.0f, pinguimY = 0.5f, pinguimZ = 5.0f; // Start in front of the baby
float pinguimRotation = 0.0f; // Rotation angle around the Y-axis
bool carryingFish = false;
int score = 0;

// --- Baby Penguin State ---
const float babyX = 0.0f, babyY = 0.35f, babyZ = 0.0f;

// --- Fish State ---
struct Fish {
    float x, y, z;
    bool isCaught;
};
std::vector<Fish> fishes(4);

// --- Hole State ---
struct Hole {
    float x, z, radius;
};
std::vector<Hole> holes(3);


// --- Function Prototypes ---
void drawPenguin(bool isBaby);
void drawFish();
void setupViewports();
void update(int value);
void checkCollisions();
void spawnItems();

/**
 * @brief Draws a solid penguin model using spheres.
 * @param isBaby If true, draws a smaller version.
 */
void drawPenguin(bool isBaby) {
    glPushMatrix();
    float scale = isBaby ? 0.5f : 1.0f;
    glScalef(scale, scale, scale);

    // Body (black)
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glScalef(1.0f, 1.2f, 1.0f);
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();

    // Belly (white)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.3f);
    glScalef(0.7f, 0.9f, 0.7f);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // Head (black)
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();

    // Beak (orange)
    glColor3f(1.0f, 0.65f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.25f);
    glScalef(1.0, 1.0, 1.5);
    glutSolidCone(0.1, 0.3, 10, 2);
    glPopMatrix();

    // Eyes (white part)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-0.1f, 0.8f, 0.25f);
    glutSolidSphere(0.05, 10, 10);
    glTranslatef(0.2f, 0.0f, 0.0f);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();

    glPopMatrix(); // End scale
}

/**
 * @brief Draws a simple fish model using a scaled sphere (ellipsoid) and a cone.
 */
void drawFish() {
    glPushMatrix();
    glScalef(0.4f, 0.4f, 0.4f);
    // Body (ellipsoid)
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glScalef(1.5f, 0.8f, 0.5f);
    glutSolidSphere(0.5, 15, 15);
    glPopMatrix();
    // Tail (cone)
    glColor3f(0.9f, 0.5f, 0.2f);
    glPushMatrix();
    glTranslatef(-0.8f, 0.0f, 0.0f);
    glRotatef(90, 0, 1, 0);
    glutSolidCone(0.3, 0.5, 10, 2);
    glPopMatrix();
    glPopMatrix();
}

/**
 * @brief Initializes game state like item positions.
 */
void initGame() {
    spawnItems();
}

/**
 * @brief Respawns fish and holes at random locations on the ice plate.
 */
void spawnItems() {
    // Spawn fish
    for (auto& fish : fishes) {
        fish.x = (rand() % 200 - 100) / 10.0f; // -10 to 10
        fish.z = (rand() % 200 - 100) / 10.0f; // -10 to 10
        fish.y = 0.1f;
        fish.isCaught = false;
    }
    // Spawn holes
    for (auto& hole : holes) {
        hole.x = (rand() % 160 - 80) / 10.0f; // -8 to 8
        hole.z = (rand() % 160 - 80) / 10.0f; // -8 to 8
        hole.radius = 1.0f + (rand() % 5) / 10.0f; // 1.0 to 1.5
    }
}

/**
 * @brief Main display callback. Sets up viewports and draws the scene.
 */
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int W = glutGet(GLUT_WINDOW_WIDTH);
    int H = glutGet(GLUT_WINDOW_HEIGHT);

    // Viewport Definitions
    int viewports[4][4] = {
        {0, H / 2, W / 2, H / 2},     // Top-Left: Top View
        {W / 2, H / 2, W / 2, H / 2}, // Top-Right: Free Perspective
        {0, 0, W / 2, H / 2},         // Bottom-Left: Front View
        {W / 2, 0, W / 2, H / 2}      // Bottom-Right: Side View
    };

    // --- Draw Scene in Each Viewport ---
    for(int i = 0; i < 4; ++i) {
        glViewport(viewports[i][0], viewports[i][1], viewports[i][2], viewports[i][3]);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0, (double)viewports[i][2] / (double)viewports[i][3], 1.0, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Set camera for the current viewport, always looking at the penguin [cite: 141]
        switch(i) {
            case 0: // Top View [cite: 137]
                gluLookAt(pinguimX, 20.0, pinguimZ,  pinguimX, 0.0, pinguimZ,  0.0, 0.0, -1.0);
                break;
            case 1: // Free Perspective [cite: 140]
                gluLookAt(pinguimX + 5.0, pinguimY + 5.0, pinguimZ + 5.0,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
            case 2: // Front View [cite: 139]
                gluLookAt(pinguimX, pinguimY + 2.0, pinguimZ + 10.0,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
            case 3: // Side View [cite: 138]
                gluLookAt(pinguimX + 10.0, pinguimY + 2.0, pinguimZ,  pinguimX, pinguimY, pinguimZ,  0.0, 1.0, 0.0);
                break;
        }

        // --- Draw Scene Objects ---
        // Ice plate
        glColor3f(0.8f, 0.9f, 1.0f);
        glPushMatrix();
        glTranslatef(0.0f, -0.1f, 0.0f);
        glScalef(25.0f, 0.2f, 25.0f);
        glutSolidCube(1.0);
        glPopMatrix();

        // Holes
        glColor3f(0.1f, 0.1f, 0.4f); // Dark blue for holes
        for (const auto& hole : holes) {
            glPushMatrix();
            glTranslatef(hole.x, 0.01f, hole.z);
            glRotatef(90, 1, 0, 0);
            gluDisk(gluNewQuadric(), 0, hole.radius, 20, 1);
            glPopMatrix();
        }

        // Mother Penguin
        glPushMatrix();
        glTranslatef(pinguimX, pinguimY, pinguimZ);
        glRotatef(pinguimRotation, 0.0f, 1.0f, 0.0f);
        drawPenguin(false);
        // Draw fish in beak if carrying [cite: 122]
        if (carryingFish) {
            glPushMatrix();
            glTranslatef(0.0f, 0.5f, 0.8f);
            drawFish();
            glPopMatrix();
        }
        glPopMatrix();

        // Baby Penguin
        glPushMatrix();
        glTranslatef(babyX, babyY, babyZ);
        drawPenguin(true);
        glPopMatrix();

        // Fishes
        for (const auto& fish : fishes) {
            if (!fish.isCaught) {
                glPushMatrix();
                glTranslatef(fish.x, fish.y, fish.z);
                drawFish();
                glPopMatrix();
            }
        }
    }
    glutSwapBuffers();
}

/**
 * @brief Handles keyboard input for penguin movement.
 * @param key The special key pressed.
 */
void specialKeys(int key, int x, int y) {
    if (gameWon || gameLost) return;
    float moveSpeed = 0.3f;
    float rotationSpeed = 5.0f;

    switch (key) {
        case GLUT_KEY_UP: // Move forward [cite: 132]
            pinguimX += moveSpeed * sin(pinguimRotation * PI / 180.0f);
            pinguimZ += moveSpeed * cos(pinguimRotation * PI / 180.0f);
            break;
        case GLUT_KEY_DOWN: // Move backward [cite: 132]
            pinguimX -= moveSpeed * sin(pinguimRotation * PI / 180.0f);
            pinguimZ -= moveSpeed * cos(pinguimRotation * PI / 180.0f);
            break;
        case GLUT_KEY_LEFT: // Rotate left [cite: 133]
            pinguimRotation += rotationSpeed;
            break;
        case GLUT_KEY_RIGHT: // Rotate right [cite: 133]
            pinguimRotation -= rotationSpeed;
            break;
    }
    glutPostRedisplay();
}

/**
 * @brief Checks for collisions between the penguin and other game objects.
 */
void checkCollisions() {
    // Fish collision [cite: 121]
    if (!carryingFish) {
        for (auto& fish : fishes) {
            if (!fish.isCaught) {
                float dx = pinguimX - fish.x;
                float dz = pinguimZ - fish.z;
                if (sqrt(dx*dx + dz*dz) < 1.0f) {
                    fish.isCaught = true;
                    carryingFish = true;
                    break;
                }
            }
        }
    }

    // Baby penguin collision (feeding) [cite: 127]
    if (carryingFish) {
        float dx = pinguimX - babyX;
        float dz = pinguimZ - babyZ;
        if (sqrt(dx*dx + dz*dz) < 1.5f) {
            carryingFish = false;
            babyEnergy += 60; // Add 1 minute of energy [cite: 125]
            score++;
            spawnItems(); // Respawn items after successful delivery
        }
    }

    // Hole collision
    for (const auto& hole : holes) {
        float dx = pinguimX - hole.x;
        float dz = pinguimZ - hole.z;
        if (sqrt(dx*dx + dz*dz) < hole.radius) {
            // Reset penguin position
            pinguimX = 0.0f;
            pinguimZ = 5.0f;
            pinguimRotation = 0.0f;
        }
    }
}

/**
 * @brief Main game loop for updating state and checking for win/loss conditions.
 */
void update(int value) {
    if (gameWon || gameLost) return;

    // Update timers
    gameTime--;
    babyEnergy--;

    checkCollisions();

    // Check win/loss conditions
    if (babyEnergy <= 0) {
        gameLost = true;
        std::cout << "Game Over! The baby ran out of energy." << std::endl;
    } else if (gameTime <= 0) {
        gameWon = true;
        std::cout << "You Win! You survived for 5 minutes. Score: " << score << std::endl;
    }

    glutPostRedisplay();
    glutTimerFunc(1000, update, 0); // 1-second interval
}


/**
 * @brief Initializes OpenGL context (lighting, depth test, etc.).
 */
void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f); // Sky blue background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); // [cite: 144]
    glEnable(GL_LIGHT0);

    // Set up a simple white light
    GLfloat light_pos[] = { 10.0f, 20.0f, 10.0f, 1.0f };
    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glEnable(GL_COLOR_MATERIAL); // Use glColor to set material properties
    srand(time(0));
    initGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Trabalho 2: Pinguim 3D");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(1000, update, 0); // Start the game loop

    glutMainLoop();
    return 0;
}