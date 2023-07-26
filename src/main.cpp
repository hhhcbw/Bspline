#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "bezier.h"
#include "spline.h"
#include "bspline.h"
#include "shader.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
unsigned int framebuffer;

// position
glm::vec4 lastPos;

// dragging id
unsigned int draggingId = 255;

BasisCurve* basisCurve;

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
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

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
    Shader colorIdShader("colorsId.vs", "colorsId.fs");

    // construct curve
    vector<glm::vec3> controlPoints;
    controlPoints.push_back(glm::vec3(-0.6f, -0.7f, 0.0f));
    controlPoints.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    controlPoints.push_back(glm::vec3(-0.4f, -0.3f, 0.0f));
    controlPoints.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    controlPoints.push_back(glm::vec3(0.3f, 0.2f, 0.0f));
    controlPoints.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
    controlPoints.push_back(glm::vec3{0.6f, 0.7f, 0.0f});
    vector<float> knots{0, 0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, 1};
 //   knots.push_back(0);
 //   knots.push_back(0);
 //   knots.push_back(0);
 //   knots.push_back(0);
 //   knots.push_back(0.25);
 //   knots.push_back(0.5);
 //   knots.push_back(0.75);
 //   knots.push_back(1);
 //   knots.push_back(1);
 //   knots.push_back(1);
	//knots.push_back(1);
    vector<glm::vec3> arcCircleControlPoints;
    arcCircleControlPoints.push_back(glm::vec3(0.5, 0, 0));
    arcCircleControlPoints.push_back(glm::vec3(0.5, 0.5, 0));
    arcCircleControlPoints.push_back(glm::vec3(0, 0.5, 0));
    vector<float> weights{1, 1, 1, 3, 1, 1, 1};
    //weights.push_back(1);
    //weights.push_back(1);
    //weights.push_back(2);
    //basisCurve = new BasisCurve(controlPoints);
    //basisCurve = new BezierCurve(controlPoints);
    //basisCurve = new BezierCurve(arcCircleControlPoints, weights);
    //basisCurve = new SplineCurve(controlPoints);
    //basisCurve = new BsplineCurve(controlPoints, knots);
    basisCurve = new BsplineCurve(controlPoints, knots, weights);

    basisCurve->init();

    glPointSize(10.0f);
    float ans;
    glGetFloatv(GL_POINT_SIZE, &ans);
    std::cout << ans << std::endl;

    // framebuffer configuration
    // -------------------------
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,
                          GL_DEPTH24_STENCIL8,
                          SCR_WIDTH,
                          SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // 1. bind to framebuffer and render sphere to set sphere id
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // make sure we clear the framebuffer's content
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        colorIdShader.use();

        // render control points
        basisCurve->DrawControlPoints(colorIdShader);

        // 2. Bind back to default framebuffer and draw scene
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        //colorIdShader.use();
        colorShader.use();

        // draw curve
        basisCurve->Draw(colorShader);
        //basisCurve->DrawControlPoints(colorIdShader);

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

    if (draggingId < 255)
    {
        //std::cout << draggingId << std::endl;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        //glm::vec4 Pos((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, lastPos.z, 1.0f);
        //glm::vec4 WorldPos = view_inverse * projection_inverse * Pos;
        //glm::vec4 WorldLastPos = view_inverse * projection_inverse * lastPos;
        //glm::vec3 WorldDir3 = glm::vec3(WorldPos.x / WorldPos.w, WorldPos.y / WorldPos.w, WorldPos.z / WorldPos.w) -
        //    glm::vec3(WorldLastPos.x / WorldLastPos.w,
        //              WorldLastPos.y / WorldLastPos.w,
        //              WorldLastPos.z / WorldLastPos.w);

        glm::vec4 pos((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, lastPos.z, 1.0f);
        glm::vec3 dir3 = glm::vec3(pos.x - lastPos.x, pos.y - lastPos.y, pos.z - lastPos.z);
        basisCurve->Update(draggingId, dir3);

        lastPos = pos;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            //std::cout << xpos << " " << ypos << std::endl;

            unsigned char pixel[3];
            float depth;
            glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
            glReadPixels(xpos, SCR_HEIGHT - ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
            //glReadPixels(xpos, SCR_HEIGHT - ypos, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            //lastPos = glm::vec4((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, depth * 2.0f - 1.0f, 1.0f);
            lastPos =
                glm::vec4((float)xpos * 2.0f / SCR_WIDTH - 1.0f, 1.0f - (float)ypos * 2.0f / SCR_HEIGHT, 0.0f, 1.0f);
            // pick the control point
            draggingId = pixel[0];
        }
        else if (action == GLFW_RELEASE)
        {
            draggingId = 255;
        }
    }
}