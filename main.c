#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gfx/shader.h"

void ErrorCallback(int i,
    const char * err_str) {
    printf("GLFW Error: %s\n", err_str);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool wireframe = false;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    	if(!wireframe) {
    		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    	}
    	else {
    		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    	}
    	wireframe = !wireframe;
    }
}

int main() {

    const int WIDTH = 800;
    const int HEIGHT = 600;
    glfwSetErrorCallback(ErrorCallback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD!\n");
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	float vertices[] = {
     -0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
	};
	unsigned int indices[] = {  // note that we start from 0!
	    0, 1, 2,   // first triangle
	};
    shader *s;
    char *vertexShaderFile = "../shaders/shader.vs";
    char *fragmentShaderFile = "../shaders/shader.fs";
    create_shader(s, vertexShaderFile, fragmentShaderFile);
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, & VAO);
    glGenBuffers(1, & VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        useShader(s);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}