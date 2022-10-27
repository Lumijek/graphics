#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <cglm/cglm.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gfx/shader.h"

static float mix = 0.2;
const int WIDTH = 800;
const int HEIGHT = 600;
vec3 cameraPos = {0.0f, 0.0f, 10.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

double fov = 45.0f;
const double sensitivity = 0.15f;

void processInput(GLFWwindow *window) {
    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W)) {
        vec3 step;
        glm_vec3_scale(cameraFront, cameraSpeed, step);
        glm_vec3_add(cameraPos, step, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
        vec3 step;
        glm_vec3_scale(cameraFront, cameraSpeed, step);
        glm_vec3_sub(cameraPos, step, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
        vec3 step;
        glm_vec3_cross(cameraFront, cameraUp, step);
        glm_vec3_normalize(step);
        glm_vec3_scale(step, cameraSpeed, step);
        glm_vec3_sub(cameraPos, step, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D)) {
        vec3 step;
        glm_vec3_cross(cameraFront, cameraUp, step);
        glm_vec3_normalize(step);
        glm_vec3_scale(step, cameraSpeed, step);
        glm_vec3_add(cameraPos, step, cameraPos);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        vec3 step;
        glm_vec3_scale(cameraUp, cameraSpeed, step);
        glm_vec3_add(cameraPos, step, cameraPos);
    }
    if(glfwGetKey(window, GLFW_KEY_R)) {
        cameraPos[0] = 0;
        cameraPos[1] = 0;
        cameraPos[2] = 10.0;
        cameraFront[0] = 0;
        cameraFront[1] = 0;
        cameraFront[2] = -1;
        cameraUp[0] = 0;
        cameraUp[1] = 1;
        cameraUp[2] = 0;
        lastX = WIDTH / 2;
        lastY = HEIGHT / 2;
        pitch = 0.0f;
        yaw = -90.0f;
        firstMouse = true;

    }
    //cameraPos[1] = 0;
}
void ErrorCallback(int i,
    const char * err_str) {
    printf("GLFW Error: %s\n", err_str);
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    float cameraSpeed = 30.0f * deltaTime;
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
    if (glfwGetKey(window, GLFW_KEY_UP)) {
        mix += 0.04;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        mix -= 0.04;
    }
}

void print_matrix(mat4 m) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw = fmod((yaw + xoffset), (GLfloat) 360.0f);
    pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = 89.0f;


    vec3 direction;
    direction[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    direction[1] = sin(glm_rad(pitch));
    direction[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    glm_vec3_normalize(direction);
    glm_vec3_copy(direction, cameraFront);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f; 
}

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

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
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    shader *s = malloc(sizeof(shader));
    char *vertexShaderFile = "../shaders/shader.vs";
    char *fragmentShaderFile = "../shaders/shader.fs";
    create_shader(s, vertexShaderFile, fragmentShaderFile);
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, & VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);


    stbi_set_flip_vertically_on_load(true);  

    int t_width, t_height, nrChannels;
    unsigned char *data = stbi_load("../textures/wood_container.png", &t_width, &t_height, &nrChannels, 0);
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Failed to load texture1!\n");
    }
    stbi_image_free(data);
    unsigned char *data2 = stbi_load("../textures/awesomeface.png", &t_width, &t_height, &nrChannels, 0);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Failed to load texture2!\n");
    }



    vec3 cubePositions[6][3] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {1.0f, 1.0f, 1.0f},
        {-4.0f, 2.0f, 6.0f}
    };
    stbi_image_free(data2);
    useShader(s);
    setInt(s, "texture1", 0);
    setInt(s, "texture2", 1);
    unsigned int transformLoc = glGetUniformLocation(s->id, "transform");
    unsigned int color = glGetUniformLocation(s->id, "color");
    //glfwSwapInterval(0); // disable v-sync

    glEnable(GL_DEPTH_TEST);  
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback); 

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glBindTexture(GL_TEXTURE_2D, 0);

    while (!glfwWindowShouldClose(window)) {
        //glfwGetCursorPos(window, &xpos, &ypos)




        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame; 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);
        useShader(s);
        setFloat(s, "alpha", mix);
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        //glm_rotate(model, glm_rad(t), (vec3) {1.0f, 1.0f, 1.0f});
        glm_rotate(model, (float)glfwGetTime() * glm_rad(50.0f), (vec3) {0.5f, 1.0f, 0.0f});  

        mat4 view = GLM_MAT4_IDENTITY_INIT;

        vec3 target;
        glm_vec3_add(cameraPos, cameraFront, target);
        glm_lookat(cameraPos, target, cameraUp, view);
        mat4 projection;
        glm_perspective(glm_rad(fov), 800.0f / 600.0f, 0.1f, 100.0f, projection);

        int modelLoc = glGetUniformLocation(s->id, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *) model);

        int viewLoc = glGetUniformLocation(s->id, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view[0]);

        int projectionLoc = glGetUniformLocation(s->id, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection[0]);

        glBindVertexArray(VAO);

        for(unsigned int i = 0; i < 6; i++)
        {
            if(i < 3) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);
            }
            else {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture2);
            }

            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, (float *) cubePositions[i]);
            float angle = 50.0f * glfwGetTime(); 
            //glm_rotate(model, glm_rad(angle), (vec3) {1.0f, 0.3f, 0.5f});
            int modelLoc = glGetUniformLocation(s->id, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *) model);
            glDrawArrays(GL_TRIANGLES, 0, 36);


        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    glfwTerminate();
    return 0;
}