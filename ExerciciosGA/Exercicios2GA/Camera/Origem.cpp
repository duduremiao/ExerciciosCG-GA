#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../Common/include/Shader.h"

float deltaTime = 0.0f;  // Tempo entre frames
float lastFrame = 0.0f;  // Tempo do último frame

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void do_movement();

int setupGeometry();

const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX = false, rotateY = false, rotateZ = false;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
float cameraSpeed = 0.05;

// EulerAngles


bool keys[1024];
// Variáveis adicionadas

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicios Camera", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    Shader shader("../../Common/shaders/camera.vs", "../../Common/shaders/camera.fs");

    GLuint VAO = setupGeometry();

    glUseProgram(shader.ID);

    glm::mat4 model = glm::mat4(1); //matriz identidade;
    GLint modelLoc = glGetUniformLocation(shader.ID, "model");
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //Matriz de view -- posição e orientação da câmera
    glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    GLint viewLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    //Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    GLint projLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Projeção paralela ortográfica
    glm::mat4 orthoProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
    GLint orthoProjLoc = glGetUniformLocation(shader.ID, "orthoProjection");
    glUniformMatrix4fv(orthoProjLoc, 1, GL_FALSE, glm::value_ptr(orthoProjection));

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        float angle = (GLfloat)glfwGetTime();

        model = glm::mat4(1);

        if (rotateX)
        {
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (rotateY)
        {
            model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (rotateZ)
        {
            model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Atualizando a posição e orientação da câmera
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 42);

        glDrawArrays(GL_POINTS, 0, 42);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    //Frente do cubo
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        cameraPos = glm::vec3(0.0, 0.0, 3.0);
        cameraFront = glm::vec3(0.0, 0.0, -1.0);
    }

    //Traseira do cubo
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        cameraPos = glm::vec3(0.0, 0.0, -3.0);
        cameraFront = glm::vec3(0.0, 0.0, 1.0);
    }

    //Esquerda do cubo
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraPos = glm::vec3(-3.0f, 0.0f, 0.0f);
        cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    //Direita do cubo
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraPos = glm::vec3(3.0f, 0.0f, 0.0f);
        cameraFront = glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    //Topo do cubo
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
        cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
        cameraUp = glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    float cameraSpeed = 0.05;

    if (key == GLFW_KEY_W)
    {
        cameraPos += cameraFront * cameraSpeed;
    }
    if (key == GLFW_KEY_S)
    {
        cameraPos -= cameraFront * cameraSpeed;
    }
    if (key == GLFW_KEY_A)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (key == GLFW_KEY_D)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
}
// Declaração das variáveis usadas para o controle do mouse
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool firstMouse = true;
GLfloat yaw = -90.0f; // Note que você deve declarar 'yaw' e 'pitch' aqui
GLfloat pitch = 0.0f;
GLfloat fov = 45.0f;


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Corrija a ordem aqui para inverter o movimento do mouse

    lastX = xpos;
    lastY = ypos;

    // Aplique as transformações de 'yaw' e 'pitch' aqui com base em 'xoffset' e 'yoffset'
    const GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Certifique-se de limitar o ângulo de inclinação (pitch) entre -89 e 89 graus
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int setupGeometry()
{
    GLfloat vertices[] = {
        // Base da pirâmide: 2 triângulos
        -0.5, -0.5, -0.5, 0.0, 0.0, 1.0, // ponto A (azul)
        -0.5, -0.5, 0.5, 0.0, 1.0, 0.0, // ponto B (verde)
        0.5, -0.5, -0.5, 1.0, 0.0, 0.0, // ponto C (vermelho)

        0.5, -0.5, 0.5, 1.0, 1.0, 0.0, // ponto D (amarelo)
        -0.5, -0.5, 0.5, 0.0, 1.0, 0.0, // ponto B (verde)
        0.5, -0.5, -0.5, 1.0, 0.0, 0.0, // ponto C (vermelho)


    };

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
