#define GL3_PROTOTYPES 1
#define GLEW_STATIC
#include <GL/glew.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
//#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

#include <SDL2/SDL.h>

#include <iostream>

#include "shader.hpp"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TIME_UNIT_TO_SECONDS 1000

SDL_Window* window = nullptr;

// based on tutorial at 
// http://headerphile.com/sdl2/opengl-part-1-sdl-opengl-awesome/

SDL_GLContext gl_context;

GLuint shader_program = 0;

GLuint g_VAO = 0;
GLuint g_VBO = 0;
GLuint g_EBO = 0;

// vertex shader source string
// const GLchar* vertex_shader_src[] = {
//     "#version 330 core\n"
//     "layout (location = 0) in vec3 position;\n"
//     //"in vec3 position;\n" 
//     "uniform float u_time;\n"
//     "out vec3 v_pos;\n"
//     "void main(void) {\n"
//     "   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
//     "   v_pos = position.xyz;\n"
//     "}\n"
// };

// fragment shader source string
// const GLchar* fragment_shader_src[] = {
//     "#version 330 core\n"
//     "precision highp float;\n"
//     "in vec3 v_pos;\n"
//     "uniform float u_time;\n"
//     "out vec4 color\n;"
//     "void main(void) {\n"
//     "   vec3 adjust = v_pos;\n"
//     "   adjust.x = sin(adjust.x + u_time);\n"
//     "   adjust.y = cos(adjust.y + u_time);\n"
//     "   color = vec4(sqrt(adjust), 1.0);\n"
//     "   //color = vec4(sqrt(v_pos), 1.0);\n"
//     "}\n"
// };

// fragment shader source string
// const GLchar* fragment_shader_src[] = {
//     "#version 330 core\n"
//     "precision highp float;\n"
//     "in vec3 v_pos;\n"
//     "uniform float u_time;\n"
//     "out vec4 color\n;"
//     "\n"
//     "float generate(vec2 p)\n"
//     "{\n"
//     "   return sin(u_time) + sin((u_time * p.y * p.x) + cos(p.x) * .01);\n"
//     "}\n"
//     "\n"
//     "void main(void)\n"
//     "{\n"
//     "   vec2 p = v_pos.xy;\n"
//     "\n"
//     "   vec3 c = mix(vec3(.11, .45, .59), vec3(.11, .45, .59), vec3(.11, .45, .59));\n"
//     "\n"
//     "   float z = generate(4. * p);\n"
//     "   if (z > 0.) {\n"
//     "       c = vec3(z, z, z);\n"
//     "   }\n"
//     "   color = vec4(sqrt(c), 1.);\n"
//     "}\n"
// };

// const GLchar* fragment_shader_src[] = {
//     "#version 330 core\n"
//     "precision highp float;\n"
//     "in vec3 v_pos;\n"
//     "uniform float u_time;\n"
//     "out vec4 color\n;"
//     "\n"
//     "void main(void)\n"
//     "{\n"
//     "   color = vec4(sqrt(sin(v_pos + u_time)), 1.);\n"
//     "}\n"
// };

// vertex shader source string
const GLchar* vertex_shader_src[] = {
    R""(
    #version 330 core
    layout (location = 0) in vec3 position; 
    out vec3 v_pos;
    uniform float u_time;
    
    void main(void) {
       gl_Position = vec4(position, 1.0);
       v_pos = position;
    }
    )""
};

const GLchar* fragment_shader_src[] = {
    R""(
    #version 330 core
    precision highp float;
    in vec3 v_pos;
    out vec4 color;
    uniform float u_time;
    
    void main(void)
    {
        color = vec4(sqrt(sin(v_pos + u_time)), 1.);
    }
    )""
};

int ignore_mouse_movement(void* unused, SDL_Event* event)
{
	return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

int main(/*int argc, char* argv[]*/)
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize" << std::endl;
        return EXIT_FAILURE;
    }
    
    // disable the cursor
    SDL_ShowCursor(SDL_DISABLE);
    // ignore mouse movement events
    SDL_SetEventFilter(ignore_mouse_movement, nullptr);
    
    // openGL initialization ///////////////////////////////////////////////////
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // create the window
    if (!(window = SDL_CreateWindow("openGL TEST",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN /*| SDL_WINDOW_ALLOW_HIGHDPI*/)))
    {
        std::cout << "Window could not be created" << std::endl;
        return EXIT_FAILURE;
    }
    
	gl_context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
    glewInit();
	
	SDL_GL_SetSwapInterval(1);
	
	glEnable(GL_DEPTH_TEST);
    glDepthRange(0, 1);
    glDepthFunc(GL_LEQUAL);
	
	//gl_init();
	
    // VERTICES ////////////////////////////////////////////////////////////////

    const GLfloat FACTOR = 1.0;
        
    // VERTEX BUFFER OBJECT DATA
    GLfloat vertex_data[] = {
        -FACTOR,  FACTOR, 0.0f,  // Top Left
        -FACTOR, -FACTOR, 0.0f,  // Bottom Left 
         FACTOR, -FACTOR, 0.0f,  // Bottom Right
         FACTOR,  FACTOR, 0.0f,  // Top Right
    };
    
    GLuint index_data[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    // VERTEX ARRAY OBJECT, VERTEX BUFFER OBJECT, ELEMENT BUFFER OBJECT
    
    glGenVertexArrays(1, &g_VAO);
    glGenBuffers(1, &g_VBO);
    glGenBuffers(1, &g_EBO);
    glBindVertexArray(g_VAO);
    
    // copy vertices buffer
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    // copy element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
    
    // set attribute pointers for vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//     glVertexAttribPointer(
//         glGetAttribLocation(shader_program, "position"), 
//         3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0
//     );

    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    
    Shader prog_shader("shaders/basic_a.vrts", "shaders/basic_a.frgs");
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    // MAIN RUN LOOP
    bool keep_running = true;
    SDL_Event event;
    Uint32 start_time = SDL_GetTicks();
    while (keep_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                keep_running = false;
            }
        }
        
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        prog_shader.use();
        
        GLfloat elapsed = (SDL_GetTicks() - start_time) / (GLfloat)TIME_UNIT_TO_SECONDS;
        GLuint time_addr = glGetUniformLocation(prog_shader.program(), "u_time");
        glUniform1f(time_addr, elapsed);
        
        glBindVertexArray(g_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
                
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteVertexArrays(1, &g_VAO);
    glDeleteBuffers(1, &g_VBO);
    glDeleteBuffers(1, &g_EBO);
    glDeleteProgram(shader_program);
    
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
