#include "GL/glew.h" 
#include "GL/glfw3.h" 

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#include <iostream>


unsigned int Wwidth0 = 800, Wheight0 = 800;


const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec2 aTexCoord;\n"
                                 "out vec2 TexCoord;\n"
                                 "uniform mat4 projection;\n"
                                 "uniform mat4 modeltrans;\n"
                                 "void main()\n"
                                 "{\n"
                                 "  TexCoord = aTexCoord;\n"
                                 "  gl_Position = projection * modeltrans * vec4(aPos, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "in vec2 TexCoord;\n"
                                   "uniform sampler2D texture1;\n"
                                   "uniform sampler2D texture2;\n"
                                   "uniform float alpha1;\n"
                                   "uniform float alpha2;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "    vec4 color1 = texture(texture1, TexCoord) * alpha1;\n"
                                   "    vec4 color2 = texture(texture2, TexCoord) * alpha2;\n"
                                   "    FragColor = color1 + color2 * (1.0 - alpha1);\n"
                                   "}\n\0";

unsigned int shaderProgram;
unsigned int texture1, texture2;


void InitMyShaders()
{
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

   
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

  
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
  
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

float xmin = -2.0f, xmax = 2.0f, ymin = -2.0f, ymax = 2.0f, zmin = -2.0f, zmax = 2.0f;


float vertices[] = {
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.0f,  0.0f, 0.0f
};

unsigned int VAO, VBO;

void SetupVerticesData()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/pollock2.jpg", &width, &height, &nrChannels, 0);
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

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("textures/monalisa.jpg", &width, &height, &nrChannels, 0);
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
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawFace(float alpha1, float alpha2)
{
    glBindVertexArray(VAO);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0); // Set texture1 to texture unit 0
    glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1); // Set texture2 to texture unit 1
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha1"), alpha1); // Set alpha1 value
    glUniform1f(glGetUniformLocation(shaderProgram, "alpha2"), alpha2); // Set alpha2 value
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1); // Bind first texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2); // Bind second texture
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void mydisplay(float angle, float alpha1, float alpha2)
{
    glm::mat4 myIdentitymatrix = glm::mat4(1.0f);
    glm::mat4 mymodelmatrix = glm::rotate(myIdentitymatrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 1.0f));
    int transform_matrix_location = glGetUniformLocation(shaderProgram, "modeltrans");
    glUniformMatrix4fv(transform_matrix_location, 1, GL_FALSE, glm::value_ptr(mymodelmatrix));

    drawFace(alpha1, alpha2);
}

int main(void)
{
    // start GL context and O/S window using the GLFW helper library
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(Wwidth0, Wheight0, "Exercise about texture blending", NULL, NULL);
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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_DEPTH_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        float time = glfwGetTime();
        float angle = time * 50;
        float alpha1 = 0.4f; // Set alpha1 value between 0.0 and 1.0
        float alpha2 = 0.8f; // Set alpha2 value between 0.0 and 1.0
      
        mydisplay(angle, alpha1, alpha2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // close GL context and any other GLFW resources
    glfwTerminate();
    return 0;
}




