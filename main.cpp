#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <limits>

// Define cloth parameters
const int numCols = 10;
const int numRows = 10;
const float clothWidth = 2.0f;
const float clothHeight = 2.0f;
const float clothVertexRadius = 0.05f;

// Define box parameters for collision detection
const float boxMinX = -1.0f;
const float boxMaxX = 1.0f;
const float boxMinY = -1.0f;
const float boxMaxY = 1.0f;
const float boxMinZ = -1.0f;
const float boxMaxZ = 1.0f;

// Define cloth vertex structure
struct ClothVertex {
    float x, y, z;
    float vx, vy, vz; // velocity
};

// Define cloth mesh
std::vector<ClothVertex> clothVertices;

// Initialize cloth mesh
void initCloth() {
    float dx = clothWidth / (numCols - 1);
    float dy = clothHeight / (numRows - 1);
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            ClothVertex v;
            v.x = j * dx;
            v.y = i * dy;
            v.z = 0.0f;
            v.vx = 0.0f;
            v.vy = 0.0f;
            v.vz = 0.0f;
            clothVertices.push_back(v);
        }
    }
}

// Update cloth simulation including collision handling
void updateCloth() {
    // Update cloth vertices here...

    // Handle collisions with the box
    for (int i = 0; i < clothVertices.size(); ++i) {
        float& vertexX = clothVertices[i].x;
        float& vertexY = clothVertices[i].y;
        float& vertexZ = clothVertices[i].z;
        if (vertexX < boxMinX || vertexX > boxMaxX ||
            vertexY < boxMinY || vertexY > boxMaxY ||
            vertexZ < boxMinZ || vertexZ > boxMaxZ) {
            // If vertex is outside the box, handle collision
            handleCollision(vertexX, vertexY, vertexZ);
        }
    }
}

// Detect collision between a cloth vertex and the box
bool detectCollision(float vertexX, float vertexY, float vertexZ) {
    return (vertexX >= boxMinX && vertexX <= boxMaxX &&
            vertexY >= boxMinY && vertexY <= boxMaxY &&
            vertexZ >= boxMinZ && vertexZ <= boxMaxZ);
}

// Handle collision response for a cloth vertex and the box
void handleCollision(float &vertexX, float &vertexY, float &vertexZ) {
    // Calculate collision normal (from closest box face to vertex)
    float normalX = 0.0f, normalY = 0.0f, normalZ = 0.0f;
    float minDist = std::numeric_limits<float>::max();

    // Check distances to each face of the box
    float distX = std::min(abs(vertexX - boxMinX), abs(vertexX - boxMaxX));
    float distY = std::min(abs(vertexY - boxMinY), abs(vertexY - boxMaxY));
    float distZ = std::min(abs(vertexZ - boxMinZ), abs(vertexZ - boxMaxZ));

    // Find the minimum distance and corresponding normal
    if (distX < minDist) {
        minDist = distX;
        normalX = (vertexX < 0) ? -1.0f : 1.0f;
    }
    if (distY < minDist) {
        minDist = distY;
        normalY = (vertexY < 0) ? -1.0f : 1.0f;
    }
    if (distZ < minDist) {
        minDist = distZ;
        normalZ = (vertexZ < 0) ? -1.0f : 1.0f;
    }

    // Move vertex outside of the box
    vertexX += normalX * minDist;
    vertexY += normalY * minDist;
    vertexZ += normalZ * minDist;
}

// Render cloth mesh
void renderCloth() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.5f, 0.5f, 1.0f); // Cloth color
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < numRows - 1; ++i) {
        for (int j = 0; j < numCols - 1; ++j) {
            // Triangle 1
            glVertex3f(clothVertices[i*numCols + j].x, clothVertices[i*numCols + j].y, clothVertices[i*numCols + j].z);
            glVertex3f(clothVertices[(i+1)*numCols + j].x, clothVertices[(i+1)*numCols + j].y, clothVertices[(i+1)*numCols + j].z);
            glVertex3f(clothVertices[i*numCols + (j+1)].x, clothVertices[i*numCols + (j+1)].y, clothVertices[i*numCols + (j+1)].z);
            // Triangle 2
            glVertex3f(clothVertices[(i+1)*numCols + j].x, clothVertices[(i+1)*numCols + j].y, clothVertices[(i+1)*numCols + j].z);
            glVertex3f(clothVertices[(i+1)*numCols + (j+1)].x, clothVertices[(i+1)*numCols + (j+1)].y, clothVertices[(i+1)*numCols + (j+1)].z);
            glVertex3f(clothVertices[i*numCols + (j+1)].x, clothVertices[i*numCols + (j+1)].y, clothVertices[i*numCols + (j+1)].z);
        }
    }
    glEnd();
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up camera
    gluLookAt(0.0f, 0.0f, 5.0f,   // eye position
              0.0f, 0.0f, 0.0f,   // look-at position
              0.0f, 1.0f, 0.0f);  // up direction

    // Update cloth simulation
    updateCloth();

    // Render cloth
    renderCloth();

    glutSwapBuffers();
}

// Reshape callback function
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f,
