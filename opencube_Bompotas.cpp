#include "GL/glew.h" // include GLEW and new version of GL on Windows
#include "GL/glfw3.h" // GLFW helper library

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // for texture loading

#include <iostream>

// window size
unsigned int Wwidth0 = 800, Wheight0 = 800;

// Shaders
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "layout (location = 2) in vec3 aColor;\n"
                                 "out vec2 TexCoord;\n"
                                 "out vec3 vertexColor;\n"
                                 "uniform mat4 projection;\n"
                                 "uniform mat4 modeltrans;\n"
                                 "void main()\n"
                                 "{\n"
                                 "  TexCoord = aTexCoord;\n"
                                 "  vertexColor = aColor;\n"
                                 "  gl_Position = projection * modeltrans * vec4(aPos, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "in vec2 TexCoord;\n"
                                   "in vec3 vertexColor;\n"
                                   "uniform sampler2D ourTexture;\n"
                                   "uniform bool useTextureFlag;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    if (useTextureFlag)\n"
                                   "    {\n"
                                   "        FragColor = texture(ourTexture, TexCoord);\n"
                                   "    }\n"
                                   "    else\n"
                                   "    {\n"
                                   "        FragColor = vec4(vertexColor, 1.0);\n"
                                   "    }\n"
                                   "}\n\0";


unsigned int shaderProgram;
unsigned int texture1, texture2;

// Function to initialize shaders
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

float xmin = -2.0f, xmax = 2.0f, ymin = -2.0f, ymax = 2.0f, zmin = -2.0f, zmax = 2.0f;

// Vertices and colors for each side of the cube
float vertices_front[] = {
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f
};

float colors_front[] = {
    1.0f, 0.0f, 0.0f, // red
    1.0f, 0.0f, 0.0f, // red
    1.0f, 0.0f, 0.0f, // red
    1.0f, 0.0f, 0.0f, // red
    1.0f, 0.0f, 0.0f, // red
    1.0f, 0.0f, 0.0f  // red
};

float vertices_back[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f
};

float colors_back[] = {
    0.0f, 1.0f, 0.0f, // green
    0.0f, 1.0f, 0.0f, // green
    0.0f, 1.0f, 0.0f, // green
    0.0f, 1.0f, 0.0f, // green
    0.0f, 1.0f, 0.0f, // green
    0.0f, 1.0f, 0.0f  // green
};

float vertices_left[] = {
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f
};

float colors_left[] = {
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 0.0f, 1.0f, // blue
    0.0f, 0.0f, 1.0f  // blue
};

float vertices_right[] = {
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f
};

float colors_right[] = {
    1.0f, 1.0f, 0.0f, // yellow
    1.0f, 1.0f, 0.0f, // yellow
    1.0f, 1.0f, 0.0f, // yellow
    1.0f, 1.0f, 0.0f, // yellow
    1.0f, 1.0f, 0.0f, // yellow
    1.0f, 1.0f, 0.0f  // yellow
};

unsigned int VAOs[4], VBOs[4], VBOCs[4];

void SetupVerticesData()
{
    glGenVertexArrays(4, VAOs);
    glGenBuffers(4, VBOs);
    glGenBuffers(4, VBOCs);

    // Setup for front face
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_front), vertices_front, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors_front), colors_front, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // Setup for back face
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_back), vertices_back, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors_back), colors_back, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // Setup for left face
    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_left), vertices_left, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors_left), colors_left, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // Setup for right face
    glBindVertexArray(VAOs[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_right), vertices_right, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCs[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors_right), colors_right, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    // Load and create textures 
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/pollock.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    data = stbi_load("textures/pollock2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void myInit()
{
    glClearColor(0.2, 0.2, 0.4, 0.0);
    glUseProgram(shaderProgram);
    float windowaspectratio = 1.0f * Wwidth0 / Wheight0;
    xmin = ymin * windowaspectratio; xmax = ymax * windowaspectratio;
    glm::mat4 myprojectionmatrix = glm::ortho(xmin, xmax, ymin, ymax, zmin, zmax);
    // inform GLSL
    int transform_matrix_location = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(transform_matrix_location, 1, GL_FALSE, glm::value_ptr(myprojectionmatrix));
    //glEnable(GL_CULL_FACE);
    // Specify which faces to cull (GL_BACK, GL_FRONT, or GL_FRONT_AND_BACK)
    //glCullFace(GL_FRONT_AND_BACK); // 
}

void drawFace(unsigned int VAO, bool useTexture, int textureChoice)
{
    glBindVertexArray(VAO);
    if (useTexture)
    {
        glUniform1i(glGetUniformLocation(shaderProgram, "useTextureFlag"), 1);  // Set useTextureFlag to true
        if (textureChoice == 1)
            glBindTexture(GL_TEXTURE_2D, texture1);  // Bind first texture
        else
            glBindTexture(GL_TEXTURE_2D, texture2);  // Bind second texture
    }
    else
    {
        glUniform1i(glGetUniformLocation(shaderProgram, "useTextureFlag"), 0);  // Set useTextureFlag to false
        glBindTexture(GL_TEXTURE_2D, 0);  // Ensure no texture is bound
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void mydisplay(float angle)
{
    glEnable(GL_CULL_FACE);

    glm::mat4 myIdentitymatrix = glm::mat4(1.0f);
    float x = 1.0f; // sin(angle / 5);
    glm::mat4 mymodelmatrix = glm::rotate(myIdentitymatrix, glm::radians(angle), glm::vec3(1.0f, x, 1.0f));
    int transform_matrix_location = glGetUniformLocation(shaderProgram, "modeltrans");
    glUniformMatrix4fv(transform_matrix_location, 1, GL_FALSE, glm::value_ptr(mymodelmatrix));

    // Draw front face
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    drawFace(VAOs[0], true, 1);

    // Draw back face
    glCullFace(GL_FRONT);
    glPolygonMode(GL_BACK, GL_FILL);
    drawFace(VAOs[0], false, 1);

    // Draw left face
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    drawFace(VAOs[2], true, 2);

    // Draw back face
    glCullFace(GL_FRONT);
    glPolygonMode(GL_BACK, GL_FILL);
    drawFace(VAOs[2], false, 2);

    // Draw right face
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    drawFace(VAOs[3], false, 2);

    // Draw back face
    glCullFace(GL_FRONT);
    glPolygonMode(GL_BACK, GL_FILL);
    drawFace(VAOs[3], true, 2);

    // Draw back face
    glCullFace(GL_FRONT);
    glPolygonMode(GL_BACK, GL_FILL);
    drawFace(VAOs[1], true, 1);

    // Draw front face
    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT, GL_FILL);
    drawFace(VAOs[1], false, 1);

    glDisable(GL_CULL_FACE);
}

int main(void)
{
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(Wwidth0, Wheight0, "My 3D object", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    InitMyShaders();
    SetupVerticesData();
    myInit();
    glEnable(GL_CULL_FACE);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        float time = glfwGetTime();
        float angle = time * 190;
      
        mydisplay(angle);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}

