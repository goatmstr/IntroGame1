/**
* Author: Hongbin Pan
* Assignment: Simple 2D Scene
* Date due: 2024-02-17, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex.glsl",
		   F_SHADER_PATH[] = "shaders/fragment.glsl";

const int TRIANGLE_RED = 1.0,
TRIANGLE_BLUE = 0.4,
TRIANGLE_GREEN = 0.4,
TRIANGLE_OPACITY = 1.0;

const float MILLISECONDS_IN_SECOND = 1000.0;
const float DEGREES_PER_SECOND = 90.0f;

ShaderProgram g_program;
glm::mat4 g_view_matrix,
g_model_matrix,
g_other_model_matrix,
g_projection_matrix,
g_trans_matrix;

float g_model_x = 0.0f;
float g_other_model_x = 0.0f;
float g_other_model_rotate = 0.0f;
float g_previous_ticks = 0.0f;

GLuint g_player_texture_id;
GLuint g_weapon_texture_id;
const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0; // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0; // this value MUST be zero

const char PLAYER_SPRITE_FILEPATH[] = "assets/mario.jpg";
const char WEAPON_SPRITE_FILEPATH[] = "assets/sword.jpg";

SDL_Window* displayWindow;
bool gameIsRunning = true;

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
	g_program.SetModelMatrix(object_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint load_texture(const char* filepath)
{
	// STEP 1: Loading the image file
	int width, height, number_of_components;
	unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

	if (image == NULL)
	{
		LOG("Unable to load image. Make sure the path is correct.");
		assert(false);
	}

	// STEP 2: Generating and binding a texture ID to our image
	GLuint textureID;
	glGenTextures(NUMBER_OF_TEXTURES, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	// STEP 3: Setting our texture filter parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// STEP 4: Releasing our file from memory and returning our texture id
	stbi_image_free(image);

	return textureID;
}

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hello, World!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
	g_program.Load(V_SHADER_PATH, F_SHADER_PATH);

	g_model_matrix = glm::mat4(1.0f);
	g_other_model_matrix = glm::mat4(1.0f);
	g_other_model_matrix = glm::translate(g_other_model_matrix, glm::vec3(0.5f, 0.5f, 0.0f));

	g_view_matrix = glm::mat4(1.0f);
	g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
	g_trans_matrix = g_model_matrix;
	g_program.SetProjectionMatrix(g_projection_matrix);
	g_program.SetViewMatrix(g_view_matrix);

	g_program.SetColor(TRIANGLE_RED, TRIANGLE_BLUE, TRIANGLE_GREEN, TRIANGLE_OPACITY);

	glUseProgram(g_program.programID);

	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);

	g_player_texture_id = load_texture(PLAYER_SPRITE_FILEPATH);
	g_weapon_texture_id = load_texture(WEAPON_SPRITE_FILEPATH);
	// END OF INITALIZE

	SDL_Event event;
	while (gameIsRunning) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				gameIsRunning = false;
			}
		}
		float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
		float delta_time = ticks - g_previous_ticks;
		g_previous_ticks = ticks;

		g_model_x += 1.0f * delta_time;
		g_other_model_rotate += DEGREES_PER_SECOND * delta_time;

		g_model_matrix = glm::mat4(1.0f);

		g_model_matrix = glm::translate(g_model_matrix, glm::vec3(g_model_x, 0.0f, 0.0f));
		g_other_model_matrix = glm::translate(g_model_matrix, glm::vec3(0.5f , 0.5f, 0.0f));
		g_other_model_matrix = glm::rotate(g_other_model_matrix, glm::radians(g_other_model_rotate), glm::vec3(0.0f, 0.0f, 1.0f));


		// START OF RENDER
		glClear(GL_COLOR_BUFFER_BIT);

		float vertices[] =
		{
			 0.5f, -0.5f,
			 0.0f,  0.5f,
			-0.5f, -0.5f
		};

		glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(g_program.positionAttribute);

		draw_object(g_model_matrix, g_player_texture_id);
		draw_object(g_other_model_matrix, g_weapon_texture_id);

		glDisableVertexAttribArray(g_program.positionAttribute);

		SDL_GL_SwapWindow(displayWindow);
		// END OF RENDER
	}

	



	SDL_Quit();
	return 0;
}