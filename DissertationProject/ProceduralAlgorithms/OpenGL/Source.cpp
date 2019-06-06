#include "Includes.h"
#include "Perlin.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


void PerlinTerrain(GLuint);
void DiamondTerrain(GLuint);
void SquareStep(int, int, int);
void DiamondStep(int, int, int);

//Timer
Timer timer;

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1280, 720, 0.001, 1000.0 };

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(1.5f, 3.0f, 4.0f));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

const int TerrainWidth = 64;
const int TerrainHeight = 64;
int selection = 1;
float TerrainValues[TerrainWidth * TerrainHeight * 4];
float TerrainColour[TerrainWidth * TerrainHeight * 4];
int DiamondArray[TerrainWidth+1][TerrainHeight+1];
int ArrayIndex = 0;
int Terrsize = TerrainHeight * TerrainWidth;
int seed;
vector<int> indices;
Perlin p;
int main()
{
#pragma region windowsetup
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "Real-Time Rendering: DEMO 1", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
#pragma endregion

#pragma	region ShaderSetup
	////	Shaders - Textures - Models	////

	GLuint phongShader;
	GLuint cubeShader;
	GLuint pyramidShader;

	// build and compile our shader program
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Phong-texture.vs"),
		string("Resources\\Shaders\\Phong-texture.fs"),
		&phongShader);

	// build and compile our shader program
	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Cube_shader.vs"),
		string("Resources\\Shaders\\Cube_shader.fs"),
		&cubeShader);

	glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Pyramid_shader.vs"),
		string("Resources\\Shaders\\Pyramid_shader.fs"),
		&pyramidShader);
#pragma endregion


	//Rendering settings
	glfwSwapInterval(1);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling

	GLuint terrainVAO;
	GLuint textureTerrain;

	glGenVertexArrays(1, &terrainVAO);
	DiamondTerrain(terrainVAO);
	PerlinTerrain(terrainVAO);
	cout << "Current Seed: " << seed << endl;
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		// Clear the screen
		glClearColor(1, 1, 1, 1);
		//glClearColor(0.905f, 0.901f, 0.901f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			PerlinTerrain(terrainVAO);
			cout << "Current Seed: " << seed << endl;

			//cout << "Pressed" << endl;
		}
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			selection = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			selection = 2;
		}

		glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(1.0));
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		
		glm::mat4 viewProjection = projection * view;

		glUseProgram(pyramidShader);
		glUniformMatrix4fv(glGetUniformLocation(pyramidShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(pyramidShader, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));

		glBindVertexArray(terrainVAO);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLE_STRIP, (TerrainHeight*TerrainWidth)+(TerrainWidth-1)*(TerrainHeight - 2), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);

		glUseProgram(0);

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void PerlinTerrain(GLuint terrainVAO)
{
	Perlin p;
	seed = rand() % 255 + 1;
	p.CreateNoise(seed = rand() % 255 + 1, (TerrainHeight * TerrainWidth));
	int count = 0;
	for (int i = 0; i < TerrainHeight; i++)
	{
		for (int j = 0; j < TerrainWidth; j++)
		{
			float x = (float)j / ((float)TerrainWidth);
			float y = (float)i / ((float)TerrainHeight);

			float n = p.Noise(1 * x, 1 * y, 0.8);

			TerrainValues[count] = x;
			TerrainColour[count] = x;
			count++;

			if (selection == 0)
			{
				TerrainValues[count] = DiamondArray[i][j];
				TerrainColour[count] = DiamondArray[i][j];
				count++;
			}
			if (selection == 1)
			{
				TerrainValues[count] = n;
				TerrainColour[count] = n;
				count++;
			}

			TerrainValues[count] = y;
			TerrainColour[count] = y;
			count++;
			TerrainValues[count] = 1.0f;
			TerrainColour[count] = 1.0f;
			count++;
		}
	}

	for (int i = 0; i < TerrainHeight - 1; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < TerrainWidth; j++)
			{
				indices.push_back(j + i * TerrainWidth);
				indices.push_back(j + (i + 1) * TerrainWidth);
			}
		}
		else
		{
			for (int j = TerrainWidth - 1; j > 0; j--)
			{
				indices.push_back(j + (i + 1) * TerrainWidth);
				indices.push_back(j - 1 + i * TerrainWidth);
			}
		}
	}

	// Per-vertex colours (RGBA) floating point values

	GLuint terrainVertexBuffer;
	GLuint terrainColourBuffer;
	GLuint terrainIndexBuffer;

	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainValues), TerrainValues, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &terrainColourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainColourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainColour), TerrainColour, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, 0, nullptr);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	
	glGenBuffers(1, &terrainIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void DiamondTerrain(GLuint TerrainVAO)
{
	int half = Terrsize;
	if (half < 1)
		return;
	
	for (int i = half; i < TerrainHeight; i += Terrsize)
		for (int j = half; j < TerrainWidth; j += Terrsize)
			SquareStep(j % TerrainHeight, i % TerrainWidth, half);

	int Columns = 0;
	for (int i = 0; i < TerrainHeight; i += half)
	{
		Columns++;
		if (Columns % 2 == 1)
			for (int j = half; j < TerrainWidth; j += half)
				DiamondStep(j % TerrainHeight, i % TerrainWidth, half);
		else
			for (int j = 0; j < TerrainWidth; j += half)
				DiamondStep(j % TerrainHeight, i % TerrainWidth, half);
	}

	Terrsize /= 2;
	DiamondTerrain(TerrainVAO);

}

void SquareStep(int x, int z, int half)
{
	int count = 0;
	float average = 0.0f;

	if (x - half > 0 && z - half >= 0)
	{
		average += DiamondArray[x - half][z - half];
		count++;
	}
	if (x - half >= 0 && z + half < TerrainHeight)
	{
		average += DiamondArray[x - half][z + half];
		count++;

	}
	if (x + half < TerrainWidth && z - half >= 0)
	{
		average += DiamondArray[x + half][z - half];
		count++;

	}
	if (x + half < TerrainWidth && z + half < TerrainHeight);
	{
		average += DiamondArray[x + half][z + half];
		count++;
	}
	average += rand() % half + 1;
	average /= count;
	DiamondArray[x][z] = round(average);
}

void DiamondStep(int x, int z, int half)
{
	int count = 0;
	float average = 0.0f;

	if (x - half >= 0)
	{
		average += DiamondArray[x - half][z];
		//average += (x - half);
		//average += (z);
		count++;
	}
	if (x + half < TerrainHeight)
	{
		average += DiamondArray[x + half][z];
		count++;
	}
	if (z - half >= 0)
	{
		average += DiamondArray[x][z - half];
		count++;
	}
	if (z + half < TerrainWidth);
	{
		average += DiamondArray[x][z + half];
		count++;
	}

	average += rand() % half + 1;
	average /= count;
	DiamondArray[x][z] = (int)average;
}

#pragma region Control

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	 if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

#pragma endregion