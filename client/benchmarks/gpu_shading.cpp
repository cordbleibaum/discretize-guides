#include "gpu.h"

#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace benchfactor::benchmarks {
	double gpu_shading() {
		if (!glfwInit()) {
			std::cout << "Failed to initialize GLFW" << std::endl;
			return -1;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Benchmark", NULL, NULL);
		if (!window)
		{
			std::cout << "Failed to initialize window" << std::endl;
			return -1;
		}

		glfwMakeContextCurrent(window);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			return -1;
		}

		glViewport(0, 0, 800, 600);

		auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto duration = currentTime - startTime;

		long frames = 0;

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		float fullscreenTriangle[] = {
		-1.0f,  3.0f,  0.0f,
		3.0f, -1.0f,  0.0f,
		-1.0f, -1.0f,  0.0f
		};

		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), fullscreenTriangle, GL_STATIC_DRAW);

		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		const char* vertex_shader =
			"#version 400\n"
			"in vec3 vPosition;"
			"out vec3 fPosition;"
			"void main() {"
			"  fPosition = vPosition;"
			"  gl_Position = vec4(vPosition, 1.0);"
			"}";

		const char* fragment_shader =
			"#version 400\n"
			"in vec3 fPosition;"
			"out vec4 fragColour;"
			"uniform int frameCount;"
			"float random (in vec2 st) {"
			"		return fract(sin(dot(st.xy,"
			"			vec2(12.9898, 78.233)))"
			"			* 43758.5453123);"
			"}"
			"float noise(in vec2 st) {"
			"	vec2 i = floor(st);"
			"	vec2 f = fract(st);"
			"	float a = random(i);"
			"	float b = random(i + vec2(1.0, 0.0));"
			"	float c = random(i + vec2(0.0, 1.0));"
			"	float d = random(i + vec2(1.0, 1.0));"
			"	vec2 u = f * f * (3.0 - 2.0 * f);"
			"	return mix(a, b, u.x) +"
			"		(c - a) * u.y * (1.0 - u.x) +"
			"		(d - b) * u.x * u.y;"
			"}"
			"void main() {"
			"  vec2 position = vec2(fPosition.xy);"
			"  float n = 0;"
			"  for (int i = 5; i < 500; ++i) {"
			"    vec2 stepPosition = position*i;"
			"    stepPosition += vec2((frameCount+1000)*i*i/5000.0);"
			"    n += noise(stepPosition)/i;"
			"  }"
			"  n *= 0.25;"
			"  fragColour = vec4(vec3(n), 1.0);"
			"}";

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex_shader, NULL);
		glCompileShader(vs);
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment_shader, NULL);
		glCompileShader(fs);

		GLuint shader_programme = glCreateProgram();
		glAttachShader(shader_programme, fs);
		glAttachShader(shader_programme, vs);
		glLinkProgram(shader_programme);

		glfwSwapInterval(0);

		while (std::chrono::duration<double>(duration).count() < 5)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(shader_programme);

			GLint frameUniform = glGetUniformLocation(shader_programme, "frameCount");
			glUniform1i(frameUniform, frames);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glfwPollEvents();
			glfwSwapBuffers(window);

			currentTime = std::chrono::high_resolution_clock::now();
			duration = currentTime - startTime;
			frames++;
		}

		glfwDestroyWindow(window);
		glfwTerminate();

		return frames / std::chrono::duration<double>(duration).count();
	}
}