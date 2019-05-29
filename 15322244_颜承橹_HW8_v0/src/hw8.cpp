#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "my_shader.h"
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
int cursorPosX = 0;
int cursorPosY = 0;
vector<glm::vec3> Points;
bool onDelete = false;
bool onClear = false;
bool animation = true;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void cursor_pos_callback(GLFWwindow* window, double x, double y);
void click_callback(GLFWwindow* window, int button, int action, int mods);
vector<glm::vec3> Bezier();
glm::vec3 standardize(int x, int y);
// timing
float currentTime = 0.0f;
float lastFrame = 0.0f;
bool flag = true;
int main() {
	glfwInit();
	// OpenGL version & mode setting
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window & context/viewpoint setting
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HW8", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	// shader init
	Shader shader("my_shader.vs", "my_shader.fs");
	// ======================================
	// Setup ImGui bindings
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, click_callback);
	unsigned int VBO;
	// generate VBO & bind to buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// create shader with Shader class
	shader.use();

	// create VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	float color[3] = { 1.0f, 1.0f, 1.0f };
	float color2[3] = { 1.0f, 0.0f, 0.0f };
	// ---- render loop ----
	while (!glfwWindowShouldClose(window)) {
		vector<vector<float>> triangle;
		// process input from keyboard/mouse/other
		processInput(window);
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// init ImGui
		{
			ImGui::ColorEdit3("line color", color, 0);
			ImGui::ColorEdit3("curve color", color2, 0);
			ImGui::Checkbox("animation", &animation);
			ImGui::Checkbox("clear", &onClear);
			if (onClear) {
				Points.clear();
				onClear = false;
			}
			//draw curve
			if (Points.size() >=3 ) {
				vector<glm::vec3> curve = Bezier();
				for (size_t i = 0; i < curve.size(); i++) {
					float point[] = { curve[i].x, curve[i].y, curve[i].z, color2[0], color2[1], color2[2] };
					glBindVertexArray(VAO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
					// position
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(0);
					//color
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
					glPointSize(2.0f);
					glDrawArrays(GL_POINTS, 0, 1);
				}
			}
			//draw Points
			for (size_t i = 0; i < Points.size(); i++) {
				float point[] = {
					Points[i].x, Points[i].y, Points[i].z, color[0], color[1], color[2],
				};
				glBindVertexArray(VAO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
				// position
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				//color
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
				glPointSize(4.0f);	
				glDrawArrays(GL_POINTS, 0, 1);
			}
			//draw line
			if(Points.size() > 1)
			for (size_t i = 0; i < Points.size() - 1; i++) {
				float line[] = {
					Points[i].x, Points[i].y, Points[i].z, color[0], color[1], color[2],
					Points[i+1].x, Points[i+1].y, Points[i+1].z, color[0], color[1], color[2],
				};
				unsigned int lineVAO = 1;
				glBindVertexArray(lineVAO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
				// position
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);
				//color
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);
				glBindVertexArray(lineVAO);
				glDrawArrays(GL_LINE_STRIP, 0, 2);
			}
			//animation line
			if (animation) {
				if (flag) {
					lastFrame = glfwGetTime();
					flag = false;
				}
				else {
					currentTime = glfwGetTime();
					float deltaTime = (currentTime - lastFrame) / 3;
					if (deltaTime >= 1) {
						//animation = false;
						flag = true;
					}
					else {
						vector<glm::vec3> TempPoints = Points;
						vector<glm::vec3> TempPoints2;
						int count = 5;
						while (TempPoints.size() > 2) {
							for (int i = 0; i < TempPoints.size() - 1; i ++) {
								glm::vec3 P0 = (1 - deltaTime) * TempPoints[i] + deltaTime * TempPoints[i + 1];
								TempPoints2.push_back(P0);
							}
							for (int i = 0; i < TempPoints2.size() - 1; i++) {
								float line[] = {
									TempPoints2[i].x, TempPoints2[i].y,TempPoints2[i].z, color[0], color[1], color[2],
									TempPoints2[i+1].x, TempPoints2[i + 1].y, TempPoints2[i + 1].z, color[0], color[1], color[2],
								};
								
								unsigned int lineVAO2 = count;
								count++;
								glBindVertexArray(lineVAO2);
								glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
								// position
								glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
								glEnableVertexAttribArray(0);
								//color
								glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
								glEnableVertexAttribArray(1);
								glBindVertexArray(lineVAO2);
								glDrawArrays(GL_LINE_STRIP, 0, 2);
							}
							TempPoints.clear();
							TempPoints = TempPoints2;
							TempPoints2.clear();
						}
					}
				}
				
				
			}
		}
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
	// release sources
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	cursorPosX = x;
	cursorPosY = y;
}

void click_callback(GLFWwindow* window, int button, int action, int mods) {
	bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !hovered) {
		glm::vec3 clickPos = standardize(cursorPosX, cursorPosY);
		Points.push_back(clickPos);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (!Points.empty())
			Points.pop_back();	
	}
}

vector<glm::vec3> Bezier() {
	vector<glm::vec3> result;
	int n = Points.size() - 1;
	for (float t = 0; t <= 1; t += 0.001) {
		glm::vec3 r(0,0,0);
		for (int i = 0; i <= n; i++) {
			float f1 = pow(t, i);
			float f2 = pow(1 - t, n - i);		
			float cn = 1;
			for (int j = 0; j < i; j++) {
				cn = cn * (n-j) / (j+1);
			}
			r = r + cn * f1 * f2 * Points[i];
		}
		result.push_back(r);
	}
	return result;
}

glm::vec3 standardize(int x, int y) {
	glm::vec3 result = glm::vec3((float(x) / float(WINDOW_WIDTH)*2.0) - 1, -((float(y) / float(WINDOW_HEIGHT) * 2) - 1), 0.0f);
	return result;
}

