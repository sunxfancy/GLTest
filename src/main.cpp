#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.xyz, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\n";

const char *geometryShaderSource = "#version 330 core\n"
    "layout(lines) in;\n"
    "layout(triangle_strip, max_vertices = 4) out;\n"
    "uniform float line_width;\n"
    "in vec3 ourColor[];\n"
    "out vec3 outColor;\n" 
    "out vec2 mvec;\n"
    "out vec2 mpos;\n"
    "void main()\n"
    "{\n"
    "   vec2 a = gl_in[0].gl_Position.xy;\n"
    "   vec2 b = gl_in[1].gl_Position.xy;\n"
    "   vec2 ab = b - a;\n"
    "   vec2 m = normalize(ab);\n"
    "   vec2 nl = vec2(m.y, -m.x);\n"
    "   vec2 nr = vec2(-m.y, m.x);\n"
    "   mvec = ab;\n"
    "   mpos = a;\n"
    "   gl_Position = vec4(nl * line_width + a, 0, 1.0); outColor = ourColor[0]; EmitVertex();\n"
    "   gl_Position = vec4(nr * line_width + a, 0, 1.0); outColor = ourColor[0]; EmitVertex();\n"
    "   gl_Position = vec4(nl * line_width + b, 0, 1.0); outColor = ourColor[1]; EmitVertex();\n"
    "   gl_Position = vec4(nr * line_width + b, 0, 1.0); outColor = ourColor[1]; EmitVertex();\n"
    "}\n";

const char *fragmentShaderSource = "#version 330 core\n"
    "in vec3 outColor;\n"
    "in vec2 mvec;\n"
    "in vec2 mpos;\n"
    "out vec4 FragColor;\n"
    "uniform float line_width;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(outColor.xyz, 1.0f);\n"
    "}\n";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLTest", NULL, NULL);
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


    // build and compile our shader program
    // ------------------------------------
    int success;
    char infoLog[512];
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // geometry shader
    int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShader);
    // check for shader compile errors
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);


    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, // left  
    //      0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f // right 
    // }; 


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float* vertices = new float[81 * 2 * 6 * 2]; 

    for (int i = 0; i <= 80; i++) {
        vertices[i*12] = -1.0f;
        vertices[i*12+1] = (((float)-i + 40.0f)) / 40.0f;
        vertices[i*12+2] = 0.0f;

        vertices[i*12+3] = 0.5f;
        vertices[i*12+4] = 0.5f;
        vertices[i*12+5] = 0.5f;

        vertices[i*12+6] = 1.0f;
        vertices[i*12+7] = (((float)-i + 40.0f)) / 40.0f;
        vertices[i*12+8] = 0.0f;

        vertices[i*12+9] = 0.5f;
        vertices[i*12+10] = 0.5f;
        vertices[i*12+11] = 0.5f;
    }

    int offset = 81 * 2 * 6;
    for (int i = 0; i <= 80; i++) {
        vertices[offset+i*12] = (((float)-i + 40.0f)) / 40.0f;
        vertices[offset+i*12+1] = -1.0f;
        vertices[offset+i*12+2] = 0.0f;

        vertices[offset+i*12+3] = 0.5f;
        vertices[offset+i*12+4] = 0.5f;
        vertices[offset+i*12+5] = 0.5f;

        vertices[offset+i*12+6] = (((float)-i + 40.0f)) / 40.0f;
        vertices[offset+i*12+7] = 1.0f;
        vertices[offset+i*12+8] = 0.0f;

        vertices[offset+i*12+9] = 0.5f;
        vertices[offset+i*12+10] = 0.5f;
        vertices[offset+i*12+11] = 0.5f;
    }


    // std::cout << std::setprecision(3);
    // for (int t = 0; t < 2; t++) {
    //     for (int i = 0; i < 6; i++) {
    //         std::cout << vertices[t * 6 + i] << ' ';
    //     }
    //     std::cout << std::endl;
    // }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 81 * 2 * 2 * 6, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 


    static std::chrono::high_resolution_clock::time_point frame_start;
    static std::chrono::high_resolution_clock::time_point frame_end;
    static int64_t total_frame_time_ns;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        frame_start = std::chrono::high_resolution_clock::now();
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "line_width"), 0.001f); 

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_LINES, 0, 81 * 4);
        // glBindVertexArray(0); // no need to unbind it every time 
 
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);

        glfwPollEvents();
        frame_end = std::chrono::high_resolution_clock::now();

        // measure performance
        total_frame_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count();

        std::stringstream title;
        title.precision(3);
        title.setf(std::ios_base::fixed, std::ios_base::floatfield);
        title << "OpenGL Test | fps=" << 1000.0f / (float)(1e-6 * total_frame_time_ns);
        glfwSetWindowTitle(window, title.str().c_str());
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}