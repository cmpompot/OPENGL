//Bompotas Christos
// Opengl graphics
//AEM:4435
//Move the mouse to the squares and see them change position and colors.
#include "GL/glew.h" 
#include "GLFW/glfw3.h" 

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include <random>
#include <iostream>


float xmin = -10, xmax = 10.0, ymin = -10.0, ymax = 10.0;
float plevra = 1.0f;
std::random_device rd;
std::mt19937 gen(rd());
float generateRandomPos() {
    std::uniform_real_distribution<float> posdistribution(xmin+plevra, xmax-plevra);
    return posdistribution(gen);
}
float generateRandomColor() {
    std::uniform_real_distribution<float> colordistribution(0.0f, 1.0f);
    return colordistribution(gen);
}
int Wwidth0, Wheight0;

//Shaders

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection*vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"uniform vec3 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(color, 1.0);\n"
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




float initialX = generateRandomPos();
float initialY = generateRandomPos();
float vertices[] = {
        initialX-plevra, initialY-plevra, 0.0f, 
         initialX-plevra, initialY+plevra, 0.0f, 
         initialX+plevra,  initialY+plevra, 0.0f, 
         initialX+plevra,  initialY-plevra, 0.0f 
};
//-----------------------------------------------------


unsigned int VBO, VAO;

void SetupVerticesData()
{
  
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}



void myInit()
{
    glClearColor(0.2, 0.2, 0.3, 0.0);
    glUseProgram(shaderProgram);
   
    float windowAspectRatio = 1.0f * Wwidth0 / Wheight0;
      
    xmin *= windowAspectRatio; xmax *= windowAspectRatio;
    glm::mat4 myProjectionMatrix = glm::ortho(xmin, xmax, ymin, ymax);
   
    int transformMatrixLocation = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transformMatrixLocation, 1, GL_FALSE, glm::value_ptr(myProjectionMatrix));

    
    int colorLocation = glGetUniformLocation(shaderProgram, "color"); 
    glUniform3f(colorLocation, 0.0f, 1.0f, 1.0f); 
}


void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec3 color;
    // Get the cursor position relative to the window
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    // Convert cursor position to OpenGL coordinates
    float mouse_x = (2.0f * x) / Wwidth0 - 1.0f;
    float mouse_y = 1.0f - (2.0f * y) / Wheight0;

    // Calculate color based on mouse position
    
    if (mouse_x >= (initialX - plevra) / xmax && mouse_x <= (initialX + plevra) / xmax &&
        mouse_y >= (initialY - plevra) / xmax && mouse_y <= (initialY + plevra) / xmax) {
        // Regenerate the position of the square
        initialX = generateRandomPos();
        initialY = generateRandomPos();
        // Generate a new color
        color = glm::vec3(generateRandomColor(), generateRandomColor(), generateRandomColor());
        // Update vertices
        vertices[0] = initialX - plevra; 
        vertices[1] = initialY - plevra; 
        vertices[3] = initialX - plevra; 
        vertices[4] = initialY + plevra; 
        vertices[6] = initialX + plevra; 
        vertices[7] = initialY + plevra; 
        vertices[9] = initialX + plevra; 
        vertices[10] = initialY - plevra;
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glUseProgram(shaderProgram);

        
        int colorLocation = glGetUniformLocation(shaderProgram, "color");
        glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    }

  
}



    
int main(void) {
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

    glfwSetCursorPosCallback(window, cursor_pos_callback);

    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER); 
    const GLubyte* version = glGetString(GL_VERSION); 
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glfwGetWindowSize(window, &Wwidth0, &Wheight0); 
    printf("winow size %d x %d \n", Wwidth0, Wheight0);

    InitMyShaders();
    SetupVerticesData();
    myInit(); 

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindVertexArray(VAO);
        glDrawArrays(GL_QUADS, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
