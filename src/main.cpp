#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "bezier.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bspline", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader colorShader("colors.vs", "colors.fs");

	// construct bezier curve
	vector<glm::vec3> controlPoints;
	controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
	controlPoints.push_back(glm::vec3(-0.4f, -0.3f, 0.0f));
	controlPoints.push_back(glm::vec3(-0.3f, -0.2f, 0.0f));
	controlPoints.push_back(glm::vec3(0.2f, 0.1f, 0.0f));
	controlPoints.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	BezierCurve bezierCurve(controlPoints);

	glPointSize(10.0f);
	float ans;
	glGetFloatv(GL_POINT_SIZE, &ans);
	std::cout << ans << std::endl;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// Bind to default framebuffer and draw scene 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		colorShader.use();

		// draw bazier curve
		bezierCurve.Draw(colorShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//if (dragging == true)
	//{
	//	double xpos, ypos;
	//	glfwGetCursorPos(window, &xpos, &ypos);

	//	glm::vec4 Pos((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, lastPos.z, 1.0f);

	//	glm::vec4 WorldPos = view_inverse * projection_inverse * Pos;
	//	glm::vec4 WorldLastPos = view_inverse * projection_inverse * lastPos;
	//	glm::vec3 WorldDir3 = glm::vec3(WorldPos.x / WorldPos.w, WorldPos.y / WorldPos.w, WorldPos.z / WorldPos.w) - glm::vec3(WorldLastPos.x / WorldLastPos.w, WorldLastPos.y / WorldLastPos.w, WorldLastPos.z / WorldLastPos.w);
	//	sphere->update(WorldDir3);

	//	lastPos = Pos;
	//}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

//void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//{
//	if (button == GLFW_MOUSE_BUTTON_LEFT)
//	{
//		if (action == GLFW_PRESS)
//		{
//			double xpos, ypos;
//			glfwGetCursorPos(window, &xpos, &ypos);
//			std::cout << xpos << " " << ypos << std::endl;
//
//			unsigned char pixel[3];
//			float depth;
//			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//			glReadPixels(xpos, SCR_HEIGHT - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
//			glReadPixels(xpos, SCR_HEIGHT - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
//			glBindFramebuffer(GL_FRAMEBUFFER, 0);
//			lastPos = glm::vec4((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, depth * 2.0f - 1.0f, 1.0f);
//			// pick the sphere
//			if (pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 0)
//				dragging = true;
//		}
//		else if (action == GLFW_RELEASE)
//		{
//			dragging = false;
//		}
//	}
//}