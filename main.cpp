#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp> // For glm::ortho for text
#include <glm/gtc/type_ptr.hpp>         // For glm::value_ptr> for text

#include "prt.h"
#include "const.h"
#include "text_render.h"

// --------- Vertix shader source ---------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"uniform int uMode;\n"
"uniform vec2 uOffset;\n"
"uniform float uScale;\n"
"uniform float uAspect;\n"
"uniform mat4 uProjection;\n" //idk what is the projection but it has something to do with text
"void main()\n"
"{\n"
"   if (uMode == 0) {\n" // Circle mode
"       gl_Position = vec4((aPos.x * uScale + uOffset.x) / uAspect, aPos.y * uScale + uOffset.y, 0.0, 1.0);\n"
"   } else if (uMode == 1) {\n" // Line mode
"       gl_Position = vec4(aPos.x * uScale + uOffset.x, aPos.y * uScale + uOffset.y, 0.0, 1.0);\n"
"   } else if (uMode == 2) {\n" // Text mode 
"       gl_Position = uProjection * vec4(aPos + uOffset, 0.0, 1.0);\n"
"}\n"
"}\0";

// --------- Fragment shader source ---------
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 uColor;\n"
"void main()\n"
"{\n"
"   FragColor = uColor;\n"
"}\0";

// ---------- Callback ----------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// --------- Text test -----------
void drawSimpleText(const char* text, float x, float y) {
    char buffer[99999];
    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}


// ---------- Main ----------
int main() {
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  
    GLFWwindow* window = glfwCreateWindow(800, 800, "Particle Generator", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Vertex Shader Object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);     // Creating the shader object
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attaching the shader source
    glCompileShader(vertexShader);                              // Compiling the shader object

    // Fragment Shader Object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader Program Object
    GLuint shaderProgram = glCreateProgram();       // Creating shader program object 
    glAttachShader(shaderProgram, vertexShader);    // Attaching the shaders objects
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);                   // Linking the shaders

    // Delete Vertex and Fragment Shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // ----- Generate circle mesh -----
    std::vector<glm::vec2> circleVerts;
    const int segments = 50;
    circleVerts.push_back(glm::vec2(0.0f, 0.0f));
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * pi * i / segments;
        circleVerts.push_back(glm::vec2(cos(angle), sin(angle)));
    }

    // Particle VAO/VBO
    GLuint pVAO, pVBO;
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);
    glBindVertexArray(pVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVerts.size() * sizeof(glm::vec2), circleVerts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Create a single particle
    Particle ball(glm::vec2(0.0f, startY), 0.07f);

	// Line VAO/VBO
    float lineVertices[] = {
    -1.0f, 0.0f,
     1.0f, 0.0f
    };

    GLuint lineVAO, lineVBO;
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);
    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

	// Text VAO/VBO
    GLuint textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, 99999, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);

    glBindVertexArray(0);

    // Timing
    const float fixedDeltaTime = 1.0f / 120.0f;
    double accumulator = 0.0f;
    double lastTime = glfwGetTime();
	double fpsTime = 0.0f;

	// FPS Counter
    int frameCount = 0;

    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        
        double currentTime = glfwGetTime();
        
        // FPS Counter
        frameCount++;
        if (currentTime - fpsTime >= 1.0) {
            double fps = frameCount / (currentTime - fpsTime);
            std::cout << "FPS: " << fps
                << " | Physics steps: " << (1.0f / fixedDeltaTime)
                << " | Particles: 1" << std::endl;
            frameCount = 0;
            fpsTime = currentTime;
        }

        double frameTime = currentTime - lastTime;
        if (frameTime > 0.25f) frameTime = 0.25f; // avoid spiral of death on stalls
        lastTime = currentTime;

        accumulator += frameTime;

        while (accumulator >= fixedDeltaTime) {
            ball.update(fixedDeltaTime, floorY);
            accumulator -= fixedDeltaTime;
        }

        glClearColor(0.0f, 0.1f, 0.2f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);
		

		// Set aspect ratio for the shader
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;
        glUniform1f(glGetUniformLocation(shaderProgram, "uAspect"), aspect);

        /*
        // Draw start line for testing
        glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), 0.0f, startY);
        glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), 1.0f);
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram, "uMode"), 1);
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINES, 0, 2);
        */
		
        
        // Draw particle
        glUseProgram(shaderProgram);
        glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), ball.getPosition().x, ball.getPosition().y);
        glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), ball.getRadius());
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 0.4f, 0.0f, 1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram, "uMode"), 0);

        glBindVertexArray(pVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

        // Draw floor line
        glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), 0.0f, floorY);
        glUniform1f(glGetUniformLocation(shaderProgram, "uScale"), 1.0f);
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram, "uMode"), 1);
        glBindVertexArray(lineVAO);
        glDrawArrays(GL_LINES, 0, 2);
        
        // Text test (not working)
        
        // ===============================
        // 1. Generate text vertex buffer
        // ===============================
        static char textVertexBuffer[99999];  // Allocated once, reused
        int num_quads = stb_easy_font_print(0, 0, (char*)"Hello, text!", NULL, textVertexBuffer, sizeof(textVertexBuffer));

        // ===============================
        // 2. Upload vertices to GPU
        // ===============================
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, num_quads * 4 * 16, textVertexBuffer);

        // ===============================
        // 3. Setup orthographic projection
        // ===============================
        glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

        // ===============================
        // 4. Use your shader
        // ===============================
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(shaderProgram, "uMode"), 2);
        glUniform2f(glGetUniformLocation(shaderProgram, "uOffset"), 50.0f, 50.0f);
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);

        // ===============================
        // 5. Draw
        // ===============================
        glBindVertexArray(textVAO);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glBindVertexArray(0);



        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &pVAO);
    glDeleteBuffers(1, &pVBO);
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	glDeleteVertexArrays(1, &textVAO);
	glDeleteBuffers(1, &textVBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}