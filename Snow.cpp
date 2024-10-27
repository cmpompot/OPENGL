#include "GL/glew.h" // include GLEW and new version of GL on Windows
#include "GL/glfw3.h" // GLFW helper library

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <ctime>
#include <iostream>
#include <vector> 
#include <random>
int Wwidth0, Wheight0;

//Shaders
GLfloat toprec = 3.0f;
GLfloat rightrec = 7.0f;
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n" // Color attribute
"out vec3 vertexColor;\n" // Output color to fragment shader
"uniform mat4 projection;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = aColor;\n" // Pass color to fragment shader
"}\n\0";


const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   if (gl_PrimitiveID == 0) { // Check if primitive is part of the rectangle\n"
"       FragColor = vec4(vertexColor, 1.0f); // Output color from vertex shader for the rectangle\n"
"   } else { // Otherwise, it's part of the circle\n"
"       FragColor = vec4(1.0f); // Output white color for the circle\n"
"   }\n"
"}\n\0";

unsigned int shaderProgram;

void InitMyShaders()
{
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


//Objects in space (xmin,xmax,ymin,ymax)
float xmin = -10, xmax = 10.0, ymin = -10.0, ymax = 10.0;

float vertices[] = {
    // Position        // Color
    -rightrec, -toprec, 0.0f,  0.0f, 0.5f, 1.0f, // left-bottom, red
    -rightrec,  toprec, 0.0f,  0.0f, 0.5f, 1.0f, // left-top, green
     rightrec,  toprec, 0.0f,  0.0f, 0.5f, 1.0f, // right-top, blue
     rightrec, -toprec, 0.0f,  0.0f, 0.5f, 1.0f  // right-bottom, yellow
};
//-----------------------------------------------------

unsigned int VBO, VAO;

void SetupVerticesData()
{
    // Generate and bind the Vertex Array Object first
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate, bind, and set vertex buffer(s)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set up position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set up color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


float rectangleSpeed = 0.001f; // Speed of the rectangle movement
float rectanglePosX = 0.0f;   // Initial position of the rectangle

void UpdateRectanglePosition()
{
    // Update position based on direction and speed
    rectanglePosX += rectangleSpeed;

    // Check if rectangle hits the limits and change direction
    if (rectanglePosX >= (xmax-rightrec ) || rectanglePosX <= (xmin+rightrec )) {
        rectangleSpeed = -rectangleSpeed; // Change direction
    }
}

void myInit()
{
    glClearColor(0.2, 0.2, 0.3, 0.0);
    glUseProgram(shaderProgram);
    //single projection - preserve aspect ratio
    float windowaspectratio = 1.0f * Wwidth0 / Wheight0;
    windowaspectratio = 1.0;  //remove for correct result
    xmin *= windowaspectratio; xmax *= windowaspectratio;
    glm::mat4 myprojectionmatrix = glm::ortho(xmin, xmax, ymin, ymax);  //plane - znear, zfar is not included
    //inform GLSL
    int transform_matrix_location = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transform_matrix_location, 1, GL_FALSE, glm::value_ptr(myprojectionmatrix));
    //-----------------------------------------    
}
// Circle properties
const int num_segments = 100; // number of segments for the circle
std::random_device rd;
std::mt19937 gen(rd());
//std::uniform_real_distribution<float> radiusdistribution(0.1, 0.5);
//float randomrad = radiusdistribution(gen);
float circleRadius = 0.3f; // radius of the circle
float circleVertices[300]; // array to hold the circle's vertices
float circlePosY = toprec - circleRadius; // initial position of the circle


std::uniform_real_distribution<float> distribution(-rightrec,rightrec);
float randomOffset=distribution(gen);
//float randomOffset = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (2.0f * rightrec) - rightrec;
float circlePosX = rectanglePosX+randomOffset;

// Function to generate the circle's vertices
void generateCircleVertices() {
    int index = 0;
    float theta = 2.0f * 3.1415926f / float(num_segments);
    float cos = cosf(theta);
    float sin = sinf(theta);
    float t;

    float x = circleRadius;
    float y = 0;

    for (int ii = 0; ii < num_segments; ii++) {
        circleVertices[index++] = x;
        circleVertices[index++] = y;
        circleVertices[index++] = 0.0f;

        // apply rotation matrix
        t = x;
        x = cos * x - sin * y;
        y = sin * t + cos * y;
    }
}

unsigned int circleVBO, circleVAO;

void SetupCircleData()
{
    circlePosY = toprec - circleRadius;//rclePosY to start from the top of the rectangle

    // Generate and bind the Vertex Array Object first, 
    glGenVertexArrays(1, &circleVAO);
    glBindVertexArray(circleVAO);
    // Then generate and bind and set vertex buffer(s), and then configure vertex attributes(s).
    glGenBuffers(1, &circleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}


void UpdateCirclePosition()
{
    // Update position based on speed
    circlePosY -= 0.001f;

    // Check if circle hits the bottom of the rectangle and reset position
    if (circlePosY <= (-toprec + circleRadius))
    {
        circlePosY = toprec;
        float randomOffset = distribution(gen);
        circlePosX = rectanglePosX + randomOffset;
        std::uniform_real_distribution<float> radiusdistribution(0.02, 0.2);
        float randomrad = radiusdistribution(gen);
        circleRadius = randomrad;

        // Generate new vertices for the circle with the new radius
        generateCircleVertices();

        // Update the vertex buffer data with the new vertices
        glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    }

    if (circlePosX >= (rectanglePosX + rightrec))
    {
        circlePosX = rectanglePosX - rightrec;
    }
    else if (circlePosX <= (rectanglePosX - rightrec))
    {
        circlePosX = rectanglePosX + rightrec;
    }
}

int main(void) {
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "My Practice APP", NULL, NULL);
    if (!window) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION); // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glfwGetWindowSize(window, &Wwidth0, &Wheight0); // Retrieves the size of the content area of the specified window.
    printf("winow size %d x %d \n", Wwidth0, Wheight0);
    srand(static_cast<unsigned int>(time(0)));
    InitMyShaders();
    SetupVerticesData();
    generateCircleVertices(); // Generate the circle's vertices
    SetupCircleData(); // Setup the circle's VAO and VBO
    myInit();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Update rectangle position
        UpdateRectanglePosition();

        // Apply translation to the model matrix for the rectangle
        glm::mat4 rectangleModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(rectanglePosX, 0.0f, 0.0f));
        int modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(rectangleModelMatrix));

        // Draw rectangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_QUADS, 0, 4);

        // Update circle position
        UpdateCirclePosition();

        // Apply translation to the model matrix for the circle
        glm::mat4 circleModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(circlePosX, circlePosY, 0.0f));
        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(circleModelMatrix));

        // Draw circle
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, num_segments);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}


