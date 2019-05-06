#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <glad\glad.h>  
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "my_shader.h"
#include "camera.h"
#include <iostream>
using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 窗口大小
const unsigned int Window_width = 800;
const unsigned int Window_height = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float animationTime1 = 0.0f;
float animationTime2 = 0.0f;
float lastFrame = 0.0f;


bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	// 实例化GLFW窗口
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口对象
	GLFWwindow* window = glfwCreateWindow(Window_width, Window_height, "Learn OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 创建并绑定ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsClassic();
	glEnable(GL_DEPTH_TEST);

	//use shader
	//	Shader myShader("my_shader.vs", "my_shader.fs");
	Shader PhongShader("PhongShader.vs", "PhongShader.fs");
	Shader GouraudShader("GouraudShader.vs", "GouraudShader.fs");
	//初始化
	ImVec4 color1 = ImVec4(0.0f, 1.0f, 1.0f, 1.00f);
	ImVec4 color2 = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 color3 = ImVec4(1.0f, 0.0f, 1.0f, 1.00f);
	ImVec4 color4 = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
	ImVec4 color5 = ImVec4(1.0f, 1.0f, 0.0f, 1.00f);
	ImVec4 color6 = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
	bool ImGui = true;
	bool Phong = true;
	bool Gouraud = false;
	bool MovingLight = false;
	//	float ambient[3] = { 0,0,0 };
	//	float diffuse[3] = { 0,0,0 };
	//	float specular[3] = { 0,0,0 };
	float ambientStrength[1] = { 0.1 };
	float specularStrength[1] = { 0.5 };
	float specularPower[1] = { 32 };

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	//生成cubeVAO、VBO对象
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	// 绑定cubeVAO
	glBindVertexArray(cubeVAO);
	// 将目标绑定在指定缓冲对象上
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 把之前定义的顶点数据复制到缓冲的内存中
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	// 设定顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	int flag = 0;
	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		// 创建一个程序对象
		//myShader.use();

		// 创建ImGui
		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Begin("Menu", &ImGui, ImGuiWindowFlags_MenuBar);
		//ImGui::SliderFloat3("ambient", ambient, 0, 1);
		//ImGui::SliderFloat3("diffuse", diffuse, 0, 1);
		//ImGui::SliderFloat3("specular", specular, 0, 1);
		ImGui::Checkbox("Phong", &Phong);
		ImGui::Checkbox("Gouraud", &Gouraud);
		ImGui::Checkbox("MovingLight", &MovingLight);
		ImGui::SliderFloat("ambientStrength", ambientStrength, 0, 1);
		ImGui::SliderFloat("specularStrength", specularStrength, 0, 1);
		ImGui::SliderFloat("specularPower", specularPower, 2, 128);
		ImGui::End();

		// 渲染窗口颜色
		int view_width, view_height;
		glfwGetFramebufferSize(window, &view_width, &view_height);
		glViewport(1, 0, view_width, view_height);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		if (MovingLight) {
			//Bonus
			animationTime2 = glfwGetTime();
			if (animationTime2 - animationTime1 >= 2) {
				if (flag == 0) {
					flag = 1;
				}
				else {
					flag = 0;
				}
				animationTime1 = animationTime2;
			}
			else {
				if (flag == 0) {
					animationTime2 = animationTime2 - animationTime1;
				}
				else {
					animationTime2 = 2 - animationTime2 + animationTime1;
				}
			}
			lightPos = glm::vec3(1.2f + animationTime2, 1.0f, 2.0f);
		}

		if (Phong) {
			// 使用PhongShader
			PhongShader.use();
			PhongShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			PhongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			PhongShader.setVec3("lightPos", lightPos);
			PhongShader.setVec3("viewPos", camera.Position);
			PhongShader.setFloat("ambientStrength", ambientStrength[0]);
			PhongShader.setFloat("specularStrength", specularStrength[0]);
			PhongShader.setFloat("specularPower", specularPower[0]);

			//设置projection、view和model
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Window_width / (float)Window_height, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			PhongShader.setMat4("projection", projection);
			PhongShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			PhongShader.setMat4("model", model);
		}
		if (Gouraud) {
			
			// 使用PhongShader
			GouraudShader.use();
			GouraudShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			GouraudShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			GouraudShader.setVec3("lightPos", lightPos);
			GouraudShader.setVec3("viewPos", camera.Position);

			//设置projection、view和model
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)Window_width / (float)Window_height, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			PhongShader.setMat4("projection", projection);
			PhongShader.setMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			PhongShader.setMat4("model", model);
		}

		// 绘制立方体
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	// 释放VAO、VBO、EBO资源
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);

	// 释放ImGui资源
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();

	// 清除所有申请的glfw资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// 键盘输入
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)	
		glfwSetWindowShouldClose(window, true);		
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}

// 鼠标追踪
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}