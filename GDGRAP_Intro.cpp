#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <string>
#include <iostream>

float x_mod = 0;
float z_mod = -2.f;

void Key_Callback(GLFWwindow* window,
    int key, //keycode of press
    int scancode, //physical position of press
    int action, //press or release
    int mods) //modifier keys
{
    if (key == GLFW_KEY_D &&
        action == GLFW_PRESS) {
        //move bunny to right
        x_mod += 20.f;
    }

    if (key == GLFW_KEY_A &&
        action == GLFW_PRESS) {
        //move bunny to left
        x_mod -= 20.f;
    }

    if (key == GLFW_KEY_W &&
        action == GLFW_PRESS) {
        //move bunny further
        z_mod += 0.3f;
    }

    if (key == GLFW_KEY_S &&
        action == GLFW_PRESS) {
        //move bunny closer
        z_mod -= 0.3f;
    }
}

//glm::mat3 identity_matrix3 = glm::mat3(1.0f);
//glm::mat4 identity_matrix4 = glm::mat4(1.0f);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    float window_width = 800.0f;
    float window_height = 800.0f;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Uriel Pascual", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    /* glViewport(0, //min x
        0, //min y
        640, //x
        480); //y */

    glfwSetKeyCallback(window, Key_Callback);

    //load vert shader
    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    //load frag shader
    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    //add vertex shader
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLint isCompiled = 0;

    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        //glDeleteShader(fragShader); // Don't leak the shader.
    }

    //add frag shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);
        std::cout << &errorLog[0];
        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(fragShader); // Don't leak the shader.
    }

    //create shader program and attach compiled shaders
    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertShader);
    glAttachShader(shaderProg, fragShader);

    //finalize the compilation process
    glLinkProgram(shaderProg);

    std::string path = "3D/bunny.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    //get EBO indices array
    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    GLfloat vertices[]{
        //x   y     z
        0.f, 0.5f, 0.f, //0
        -0.5f, -0.5f, 0.f, //1
        0.5f, -0.5f, 0.f //2
    };

    GLuint indices[]{
        0, 1, 2
    };

    GLuint VAO, VBO, EBO;
    //Initialize VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //tell opengl to use shader for VAOs below
    glUseProgram(shaderProg);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //VBO
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(), //bytes
        &attributes.vertices[0],         // == attributes.vertices.data()
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, //0 pos, 1 tex, 2 norms
        3, //xyz
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GL_FLOAT),
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        &mesh_indices[0],
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    float x, y, z;
    x = y = z = 0.0f;
    z = -5.0f;
    glm::mat3 identity_matrix3 = glm::mat3(1.0f);
    glm::mat4 identity_matrix4 = glm::mat4(1.0f);

    float scale_x, scale_y, scale_z = 1.f;
    scale_x = scale_y = scale_z = 1.f;

    float axis_x, axis_y, axis_z, theta = 0.f;
    axis_x = axis_y = axis_z = theta = 0.f;
    axis_y = 1;

    /*glm::mat4 projectionMatrix = glm::ortho(
        -1.f, //left
        1.f, //right
        -1.f, //bottom
        1.f, //top
        -1.f, //znear
        1.f //zfar
    );*/

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.f), //fov
        window_height / window_width, //aspect ratio
        0.1f, //znear > 0
        100.0f //zfar
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        z = z_mod;
        theta = x_mod;

        glm::mat4 transformation_matrix = glm::translate(
            identity_matrix4,
            glm::vec3(x, y, z)
        );

        transformation_matrix = glm::scale(
            transformation_matrix,
            glm::vec3(scale_x, scale_y, scale_x)
        );

        transformation_matrix = glm::rotate(
            transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(axis_x, axis_y, axis_z))
        );

        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");

        glUniformMatrix4fv(projectionLoc, //address of transform variable
            1, //how many matrices to assign
            GL_FALSE, //transpose
            glm::value_ptr(projectionMatrix) //pointer to matrix
        );

        //get location of transform variable in shader
        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");

        //assign matrix
        glUniformMatrix4fv(transformLoc, //address of transform variable
            1, //how many matrices to assign
            GL_FALSE, //transpose
            glm::value_ptr(transformation_matrix) //pointer to matrix
        );


        //x_mod += 0.0005f;

        /*
        //get address of variable x in shader
        unsigned int xLoc = glGetUniformLocation(shaderProg, "x");
        //assign x by using its address
        glUniform1f(xLoc, x_mod);

        //get address of variable y in shader
        unsigned int yLoc = glGetUniformLocation(shaderProg, "y");
        //assign y by using its address
        glUniform1f(yLoc, y_mod); */

        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES,
            mesh_indices.size(),
            GL_UNSIGNED_INT,
            0);

        /*glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();*/


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}