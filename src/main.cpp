#include <GLFW/glfw3.h>
#include "load_frame.hpp"

// const char* videoPath = "C:/Users/snake/Downloads/Video/ash.mp4";
const char* videoPath ="C:/Users/snake/Downloads/Video/Jordan Smith - Chandelier, Full Blind Audition - YouTube.mkv";

int main() {
	GLFWwindow* window;

	if (!glfwInit()) {
		cout << "coud't init glfw" << endl;
		return 1;
	}

	window = glfwCreateWindow(640, 480, "Hello, World", NULL, NULL);
	if (!window) {
		cout << "coud't open window" << endl;
		return 1;
	}

	// Allocate frame buffer
	int frame_width, frame_height;
	uint8_t* frame_data;
	if (!load_frame(videoPath, &frame_width, &frame_height, &frame_data)) {
		cout << "you could't load frame" << endl;
		return 1;
	}

	glfwMakeContextCurrent(window);

	GLuint tex_handle;
	glGenTextures(1, &tex_handle);
	glBindTexture(GL_TEXTURE_2D, tex_handle);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data);
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set up orphographic projection
		int window_width, window_height;
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, window_width, window_height, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);

		// Render whatever you want
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, tex_handle);
		glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2i(200, 200);
			glTexCoord2d(1, 0); glVertex2i(200 + frame_width, 200);
			glTexCoord2d(1, 1); glVertex2i(200 + frame_width, 200 + frame_height);
			glTexCoord2d(0, 1); glVertex2i(200, 200 + frame_height);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		glfwSwapBuffers(window);
		//glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
		glfwWaitEvents();
	}

	return 0;
}