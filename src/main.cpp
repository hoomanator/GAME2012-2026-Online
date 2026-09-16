#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include<iostream>
#include<random>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

float randomFloat(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

int main(void)
{
    GLFWwindow* window;

	constexpr int SCREEN_WIDTH = 1200;
	constexpr int SCREEN_HEIGHT = 800;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello GAME3121", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

	/* Initialize GLAD */
    gladLoadGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glClearColor(1.0f, 1.0f, 0.0f, 1.0);


		//float x = randomFloat(0.0f, SCREEN_WIDTH);
		//float y = randomFloat(0.0f, SCREEN_HEIGHT);

        //glfwSetCursorPos(window, x, y);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const char* keyName = glfwGetKeyName(key, scancode);
	if (action == GLFW_PRESS) printf("Key pressed: %s\n", keyName);
	

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        printf("Activate Airship\n");


    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
        
}