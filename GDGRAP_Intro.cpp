#include <GLFW/glfw3.h>
#include <math.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */   
        glClear(GL_COLOR_BUFFER_BIT);

        int v;
        float pent[5][2], ang, da = 6.2832 / 5.0;   // da is central angle between vertices in radians

        for (v = 0; v < 5; v++) {                  // Computes vertex coordinates.
            ang = v * da;
            pent[v][0] = cos(ang);
            pent[v][1] = sin(ang);
        }

        //glBegin(GL_TRIANGLES);
        //glVertex2f(-0.5f, -0.5f);
        //glVertex2f(0.0f, 0.5f);
        //glVertex2f(0.5f, -0.5f);
        glBegin(GL_LINE_LOOP);                                         // Draws pentagon.
        for (v = 0; v < 5; v++)  glVertex2fv(pent[v]);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}