#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/ext.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define EPSILON_WEIGHT 100

#include "loadObj.h"
#include "glRandom.h"
#include "Obstacle.h"
#include "stb_image.h"

using namespace std;

enum class MovedDirection {
	stop,
	up,
	down,
	right,
	left
};

GLuint VAO[6], VBO[14], Textures[7];
GLuint programID;
LoadObjFile loadObj[4];
MovedDirection CurrMovDir;

int caveLevel = 1;
int doubleColor;

float transCharacterX, transCharacterY, transCharacterZ, rotateCharacterY;

bool isWalkingLeft, isWalkingRight, isWalkingUp, isWalkingDown, isCollisionW, isCollisionO, isSideView, isColor, isLine;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyBoard(int key, int x, int y);
GLvoid TimerFunction(GLint value);
GLuint InitShader();
string filetobuf(const char* file);
GLvoid InitBuffer();
void InitTexture();

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 100);
	glutInitWindowSize(800, 450);
	glutCreateWindow("Cave_Escape");

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		cerr << "Unable to initialize GLEW" << endl;
		exit(EXIT_FAILURE);
	}

	else
		cout << "GLEW Initialized\n";

	AddObstacleStageFirst();
	AddObstacleStageSecond();
	AddObstacleStageThird();
	AddObstacleStageFourth();

	isLine = true;
	isSideView = true;
	transCharacterX = -1.75f;
	transCharacterZ = -0.25f;
	rotateCharacterY = 90.f;

	InitShader();
	InitBuffer();
	InitTexture();
	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeyBoard);
	glutTimerFunc(25, TimerFunction, 1);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(programID);

	glm::vec3 lightPos = glm::vec3(1.0f, 2.0f, 2.0f);

	glm::vec3 cameraPos1 = glm::vec3(0.0f, 9.0f, 8.06f);
	glm::vec3 cameraDirection1 = glm::vec3(0.0f, 0.0f, -0.8f);
	glm::vec3 cameraUp1 = glm::vec3(0.0f, 1.0f, -0.8f);
	glm::mat4 view1 = glm::mat4(1.0f);

	glm::vec3 cameraPos2 = glm::vec3(0.0f, 11.3f, 0.0f);
	glm::vec3 cameraDirection2 = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUp2 = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::mat4 view2 = glm::mat4(1.0f);

	glm::mat4 projection = glm::mat4(1.0f);

	glm::mat4 normalMatrix = glm::mat4(1.0f);
	glm::mat4 rotateWallMatrix_Y = glm::mat4(1.0f);
	glm::mat4 transArmsMatrix_R = glm::mat4(1.0f);
	glm::mat4 rotateLimbMatrix1 = glm::mat4(1.0f);
	glm::mat4 rotateBasicCharacterMatrix = glm::mat4(1.0f);
	glm::mat4 rotateCharacterMatrix = glm::mat4(1.0f);
	glm::mat4 rotateSlipCharacterMatrix = glm::mat4(1.0f);
	glm::mat4 transCharacterMatrix = glm::mat4(1.0f);
	glm::mat4 transRightArmMatrix = glm::mat4(1.0f);
	glm::mat4 transLeftArmMatrix = glm::mat4(1.0f);
	glm::mat4 scalePlatMatrix = glm::mat4(1.0f);
	glm::mat4 scaleObtacleMatrix = glm::mat4(1.0f);
	glm::mat4 scaleCharacterMatrix = glm::mat4(1.0f);
	glm::mat4 scaleCaveMatrix = glm::mat4(1.0f);
	glm::mat4 transLineMatrix = glm::mat4(1.0f);

	rotateWallMatrix_Y = glm::rotate(rotateWallMatrix_Y, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	scaleCharacterMatrix = glm::scale(scaleCharacterMatrix, glm::vec3(0.08f, 0.08f, 0.08f));
	scalePlatMatrix = glm::scale(scalePlatMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
	scaleObtacleMatrix = glm::scale(scaleObtacleMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
	scaleCaveMatrix = glm::scale(scaleCaveMatrix, glm::vec3(0.066f, 0.066f, 0.066f));
	transRightArmMatrix = glm::translate(transRightArmMatrix, glm::vec3(-0.09f, 0.2f, 0.0f));
	transLeftArmMatrix = glm::translate(transLeftArmMatrix, glm::vec3(0.09f, 0.2f, 0.0f));
	transCharacterMatrix = glm::translate(transCharacterMatrix, glm::vec3(transCharacterX, transCharacterY, transCharacterZ));
	rotateCharacterMatrix = glm::rotate(rotateCharacterMatrix, glm::radians(rotateCharacterY), glm::vec3(0.0f, 1.0f, 0.0f));
	rotateBasicCharacterMatrix = glm::rotate(rotateBasicCharacterMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rotateSlipCharacterMatrix = glm::rotate(rotateSlipCharacterMatrix, glm::radians(75.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rotateLimbMatrix1 = glm::rotate(rotateLimbMatrix1, glm::radians(90.f), glm::vec3(1.0f, 0.0f, 0.0f));
	transArmsMatrix_R = glm::translate(transArmsMatrix_R, glm::vec3(0.0f, 0.2f, -0.45f));
	transLineMatrix = glm::translate(transLineMatrix, glm::vec3(0.0f, 0.01f, 0.0f));

	view1 = glm::lookAt(cameraPos1, cameraDirection1, cameraUp1);
	view2 = glm::lookAt(cameraPos2, cameraDirection2, cameraUp2);

	projection = glm::perspective(glm::radians(20.f), 800.f / 450.f, 0.1f, 100.0f);

	unsigned int transformLocation = glGetUniformLocation(programID, "modelTransform");
	unsigned int viewLocation = glGetUniformLocation(programID, "viewTransform");
	unsigned int projectionLocation = glGetUniformLocation(programID, "projectionTransform");
	unsigned int lightPosLocation = glGetUniformLocation(programID, "lightPos");
	unsigned int lightColorLocation = glGetUniformLocation(programID, "lightColor");
	unsigned int objectlightLocation = glGetUniformLocation(programID, "objectColor");
	unsigned int viewPosLocation = glGetUniformLocation(programID, "viewPos");

	//camera
	if (isSideView) {
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view1[0][0]);
		glUniform3fv(viewPosLocation, 1, &cameraPos1[0]);
	}
	else {
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view2[0][0]);
		glUniform3fv(viewPosLocation, 1, &cameraPos2[0]);
	}
	//projection
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	//light
	glUniform3fv(lightPosLocation, 1, &lightPos[0]);
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	
	//Create Walls
	glBindVertexArray(VAO[0]);
	glBindTexture(GL_TEXTURE_2D, Textures[2]);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(scalePlatMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//CAVE
	glBindVertexArray(VAO[4]);
	glBindTexture(GL_TEXTURE_2D, Textures[4]);
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(rotateWallMatrix_Y * scaleCaveMatrix));
	glDrawArrays(GL_TRIANGLES, 0, loadObj[3].outvertex.size());

	//Stop
	if (isCollisionW || isCollisionO || !(isWalkingRight || isWalkingLeft || isWalkingUp || isWalkingDown)) {
		//BODY
		glBindVertexArray(VAO[2]);
		glBindTexture(GL_TEXTURE_2D, Textures[0]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * rotateBasicCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[0].outvertex.size());

		//ARMS
		glBindVertexArray(VAO[3]);
		glBindTexture(GL_TEXTURE_2D, Textures[1]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * transRightArmMatrix * rotateBasicCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[1].outvertex.size());

		glBindVertexArray(VAO[3]);
		glBindTexture(GL_TEXTURE_2D, Textures[1]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * transLeftArmMatrix * rotateBasicCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[1].outvertex.size());
	}
	//Move
	else {
		//BODY
		glBindVertexArray(VAO[2]);
		glBindTexture(GL_TEXTURE_2D, Textures[0]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * rotateSlipCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[0].outvertex.size());

		//ARMS
		glBindVertexArray(VAO[3]);
		glBindTexture(GL_TEXTURE_2D, Textures[1]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * transArmsMatrix_R * rotateLimbMatrix1 * transRightArmMatrix * rotateBasicCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[1].outvertex.size());

		glBindVertexArray(VAO[3]);
		glBindTexture(GL_TEXTURE_2D, Textures[1]);
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transCharacterMatrix * rotateCharacterMatrix * transArmsMatrix_R * rotateLimbMatrix1 * transLeftArmMatrix * rotateBasicCharacterMatrix * scaleCharacterMatrix));
		glDrawArrays(GL_TRIANGLES, 0, loadObj[1].outvertex.size());
	}

	//Create Obstacles
	if (caveLevel == 1) {
		if (isLine) {
			glPointSize(10.0f);
			glBindVertexArray(VAO[5]);
			if (isColor)
				glBindTexture(GL_TEXTURE_2D, Textures[5]);
			else
				glBindTexture(GL_TEXTURE_2D, Textures[6]);
			glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transLineMatrix));
			glDrawArrays(GL_POINTS, 0, 23);
		}

		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_1[i][j]) {
					glBindVertexArray(VAO[1]);
					glm::mat4 transObtacleMatrix = glm::mat4(1.0f);
					transObtacleMatrix = glm::translate(transObtacleMatrix, glm::vec3(j * 0.5f - 1.75f, 0.25f, i * 0.5f - 1.75f));
					glBindTexture(GL_TEXTURE_2D, Textures[3]);
					glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transObtacleMatrix * scaleObtacleMatrix));
					glDrawArrays(GL_TRIANGLES, 0, loadObj[2].outvertex.size());
				}
			}
		}
	}
	else if (caveLevel == 2) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_2[i][j]) {
					glBindVertexArray(VAO[1]);
					glm::mat4 transObtacleMatrix = glm::mat4(1.0f);
					transObtacleMatrix = glm::translate(transObtacleMatrix, glm::vec3(j * 0.5f - 1.75f, 0.25f, i * 0.5f - 1.75f));
					glBindTexture(GL_TEXTURE_2D, Textures[3]);
					glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transObtacleMatrix * scaleObtacleMatrix));
					glDrawArrays(GL_TRIANGLES, 0, loadObj[2].outvertex.size());
				}
			}
		}
	}
	else if (caveLevel == 3) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_3[i][j]) {
					glBindVertexArray(VAO[1]);
					glm::mat4 transObtacleMatrix = glm::mat4(1.0f);
					transObtacleMatrix = glm::translate(transObtacleMatrix, glm::vec3(j * 0.5f - 1.75f, 0.25f, i * 0.5f - 1.75f));
					glBindTexture(GL_TEXTURE_2D, Textures[3]);
					glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transObtacleMatrix * scaleObtacleMatrix));
					glDrawArrays(GL_TRIANGLES, 0, loadObj[2].outvertex.size());
				}
			}
		}
	}
	else if (caveLevel == 4) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_4[i][j]) {
					glBindVertexArray(VAO[1]);
					glm::mat4 transObtacleMatrix = glm::mat4(1.0f);
					transObtacleMatrix = glm::translate(transObtacleMatrix, glm::vec3(j * 0.5f - 1.75f, 0.25f, i * 0.5f - 1.75f));
					glBindTexture(GL_TEXTURE_2D, Textures[3]);
					glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(transObtacleMatrix * scaleObtacleMatrix));
					glDrawArrays(GL_TRIANGLES, 0, loadObj[2].outvertex.size());
				}
			}
		}
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		caveLevel = 1;
		transCharacterX = -1.75f;
		transCharacterZ = -0.25f;
		rotateCharacterY = 90.f;
		isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
		CurrMovDir = MovedDirection::stop;
		break;
	case '2':
		caveLevel = 2;
		transCharacterX = -1.75f;
		transCharacterZ = -0.25f;
		rotateCharacterY = 90.f;
		isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
		CurrMovDir = MovedDirection::stop;
		break;
	case '3':
		caveLevel = 3;
		transCharacterX = -1.75f;
		transCharacterZ = -0.25f;
		rotateCharacterY = 90.f;
		isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
		CurrMovDir = MovedDirection::stop;
		break;
	case '4':
		caveLevel = 4;
		transCharacterX = -1.75f;
		transCharacterZ = -0.25f;
		rotateCharacterY = 90.f;
		isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
		CurrMovDir = MovedDirection::stop;
		break;

	case 'c':
	case 'C':
		if(isSideView)
			isSideView = false;
		else
			isSideView = true;
		break;

	case 't':
	case 'T':
		if (isLine)
			isLine = false;
		else
			isLine = true;
		break;

	case 'r':
	case 'R':
		transCharacterX = -1.75f;
		transCharacterZ = -0.25f;
		rotateCharacterY = 90.f;
		isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
		CurrMovDir = MovedDirection::stop;
		break;

	case 'q':
	case 'Q':
		exit(1);
		break;
	}

	glutPostRedisplay();
}

GLvoid SpecialKeyBoard(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		if (isCollisionO || !(isWalkingUp || isWalkingDown || isWalkingLeft || isWalkingRight)) {
			isWalkingUp = true;
			isWalkingDown = false;
			isWalkingLeft = false;
			isWalkingRight = false;
			rotateCharacterY = 180.0f;
		}
	}
	if (key == GLUT_KEY_DOWN) {
		if (isCollisionO || !(isWalkingUp || isWalkingDown || isWalkingLeft || isWalkingRight)) {
			isWalkingUp = false;
			isWalkingDown = true;
			isWalkingLeft = false;
			isWalkingRight = false;
			rotateCharacterY = 0.0f;
		}
	}
	if (key == GLUT_KEY_LEFT) {
		if (isCollisionO || !(isWalkingUp || isWalkingDown || isWalkingLeft || isWalkingRight)) {
			isWalkingUp = false;
			isWalkingDown = false;
			isWalkingLeft = true;
			isWalkingRight = false;
			rotateCharacterY = 270.0f;
		}
	}
	if (key == GLUT_KEY_RIGHT) {
		if (isCollisionO || !(isWalkingUp || isWalkingDown || isWalkingLeft || isWalkingRight)) {
			isWalkingUp = false;
			isWalkingDown = false;
			isWalkingLeft = false;
			isWalkingRight = true;
			rotateCharacterY = 90.0f;
		}
	}
	glutPostRedisplay();
}

GLvoid TimerFunction(GLint value)
{	
	//collision check
	if (caveLevel == 1) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_1[i][j]) {
					if (transCharacterX >= j * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterX <= j * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ >= i * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ <= i * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT) {

						if (!isCollisionO) {
							isCollisionO = true;

							if (isWalkingUp) CurrMovDir = MovedDirection::up;
							else if (isWalkingDown) CurrMovDir = MovedDirection::down;
							else if (isWalkingRight) CurrMovDir = MovedDirection::right;
							else if (isWalkingLeft) CurrMovDir = MovedDirection::left;

							isWalkingUp = isWalkingDown = isWalkingRight = isWalkingLeft = false;
						}

					}
				}
			}
		}
	}
	else if (caveLevel == 2) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_2[i][j]) {
					if (transCharacterX >= j * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterX <= j * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ >= i * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ <= i * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT) {

						if (!isCollisionO) {
							isCollisionO = true;

							if (isWalkingUp) CurrMovDir = MovedDirection::up;
							else if (isWalkingDown) CurrMovDir = MovedDirection::down;
							else if (isWalkingRight) CurrMovDir = MovedDirection::right;
							else if (isWalkingLeft) CurrMovDir = MovedDirection::left;

							isWalkingUp = isWalkingDown = isWalkingRight = isWalkingLeft = false;
						}

					}
				}
			}
		}
	}
	else if (caveLevel == 3) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_3[i][j]) {
					if (transCharacterX >= j * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterX <= j * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ >= i * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ <= i * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT) {

						if (!isCollisionO) {
							isCollisionO = true;

							if (isWalkingUp) CurrMovDir = MovedDirection::up;
							else if (isWalkingDown) CurrMovDir = MovedDirection::down;
							else if (isWalkingRight) CurrMovDir = MovedDirection::right;
							else if (isWalkingLeft) CurrMovDir = MovedDirection::left;

							isWalkingUp = isWalkingDown = isWalkingRight = isWalkingLeft = false;
						}

					}
				}
			}
		}
	}
	else if (caveLevel == 4) {
		for (int i = 0; i < BOARD_MAX_NUM; i++) {
			for (int j = 0; j < BOARD_MAX_NUM; j++) {
				if (obstacles_stage_4[i][j]) {
					if (transCharacterX >= j * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterX <= j * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ >= i * 0.5f - 2.25f + FLT_EPSILON * EPSILON_WEIGHT &&
						transCharacterZ <= i * 0.5f - 1.25f - FLT_EPSILON * EPSILON_WEIGHT) {

						if (!isCollisionO) {
							isCollisionO = true;

							if (isWalkingUp) CurrMovDir = MovedDirection::up;
							else if (isWalkingDown) CurrMovDir = MovedDirection::down;
							else if (isWalkingRight) CurrMovDir = MovedDirection::right;
							else if (isWalkingLeft) CurrMovDir = MovedDirection::left;

							isWalkingUp = isWalkingDown = isWalkingRight = isWalkingLeft = false;
						}

					}
				}
			}
		}
	}

	if (!isCollisionO) {
		if (isWalkingUp) {
			if (transCharacterZ <= -1.75f) {
				isCollisionW = true;
				isWalkingUp = false;
			}
			else {
				transCharacterZ -= 0.05;
				isCollisionW = false;
			}
		}
		else if (isWalkingDown) {
			if (transCharacterZ >= 1.75f) {
				isCollisionW = true;
				isWalkingDown = false;
			}
			else {
				transCharacterZ += 0.05;
				isCollisionW = false;
			}
		}
		else if (isWalkingLeft) {
			if (transCharacterX <= -1.75f) {
				isCollisionW = true;
				isWalkingLeft = false;
			}
			else {
				transCharacterX -= 0.05;
				isCollisionW = false;
			}
		}
		else if (isWalkingRight) {
			if (transCharacterZ >= 0.0f &&
				transCharacterZ <= 0.5f &&
				transCharacterX > 2.0f) {
				caveLevel++;
				if (caveLevel > 4) {
					cout << "Congratulation!!!" << "\n";
					exit(0);
				}
				transCharacterX = -1.75f;
				transCharacterZ = -0.25f;
				rotateCharacterY = 90.f;
				isWalkingLeft = isWalkingRight = isWalkingUp = isWalkingDown = isCollisionW = isCollisionO = false;
				CurrMovDir = MovedDirection::stop;

			}

			else if (transCharacterZ >= 0.0f &&
				transCharacterZ <= 0.5f &&
				transCharacterX >= 1.5f &&
				transCharacterX <= 2.0f) {

				transCharacterX += 0.05;
				isCollisionW = false;
			}

			else if (transCharacterX >= 1.75f) {
				isCollisionW = true;
				isWalkingRight = false;
			}
			else {
				transCharacterX += 0.05;
				isCollisionW = false;
			}
		}
	}
	else {
		if (CurrMovDir != MovedDirection::up && isWalkingUp) {
			if (CurrMovDir == MovedDirection::right)
				transCharacterX -= 0.05;
			else if (CurrMovDir == MovedDirection::left)
				transCharacterX += 0.05;
			transCharacterZ -= 0.05;
			CurrMovDir = MovedDirection::stop;

			isCollisionO = false;
		}
		else if (CurrMovDir != MovedDirection::down && isWalkingDown) {
			if (CurrMovDir == MovedDirection::right)
				transCharacterX -= 0.05;
			else if (CurrMovDir == MovedDirection::left)
				transCharacterX += 0.05;
			transCharacterZ += 0.05;
			CurrMovDir = MovedDirection::stop;

			isCollisionO = false;
		}
		else if (CurrMovDir != MovedDirection::right && isWalkingRight) {
			if(CurrMovDir == MovedDirection::up)
				transCharacterZ += 0.05;
			else if (CurrMovDir == MovedDirection::down)
				transCharacterZ -= 0.05;

			transCharacterX += 0.05;
			CurrMovDir = MovedDirection::stop;

			isCollisionO = false;
		}
		else if (CurrMovDir != MovedDirection::left && isWalkingLeft) {
			if (CurrMovDir == MovedDirection::up)
				transCharacterZ += 0.05;
			else if (CurrMovDir == MovedDirection::down)
				transCharacterZ -= 0.05;

			transCharacterX -= 0.05;
			CurrMovDir = MovedDirection::stop;

			isCollisionO = false;
		}
	}

	if (++doubleColor % 20 == 0) {
		isColor = true;
	}
	else if (doubleColor % 10 == 0){
		isColor = false;
	}

	glutPostRedisplay();

	glutTimerFunc(25, TimerFunction, 1);
}

GLuint InitShader()
{
	string sVertexShaderSource = filetobuf("vertex.glvs");
	string sFragmentShaderSource = filetobuf("frag.glfs");

	const char* vertexShaderSource = sVertexShaderSource.c_str();
	const char* fragmentShaderSource = sFragmentShaderSource.c_str();

	GLint result;
	GLchar errorLog[512];

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);

		cerr << "ERROR: vertex shader 컴파일실패\n" << errorLog << endl;

		return false;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);

		cerr << "ERROR: fragment shader 컴파일실패\n" << errorLog << endl;

		return false;
	}

	GLuint ShaderProgramID = glCreateProgram();

	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragmentShader);

	glLinkProgram(ShaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);

		cerr << "ERROR: shader program 연결실패\n" << errorLog << endl;

		return false;
	}

	return programID = ShaderProgramID;
}


string filetobuf(const char* file)
{
	ifstream in(file);
	string s;

	if (in.is_open())
	{
		in.seekg(0, ios::end);

		int size = in.tellg();

		s.resize(size);
		in.seekg(0, ios::beg);
		in.read(&s[0], size);

		return s;
	}
	else
	{
		cout << "파일을 찾을 수 없습니다!" << endl;
		exit(-1);
	}
}

GLvoid InitBuffer()
{
	float plat[] = {
		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f,	//0
		-1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 1.0f,	//1
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		-1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		1.0f, 0.0f, 1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 0.0f,	//3
		1.0f, 0.0f, -1.0f,		0.0f, 1.0f, 0.0f,	1.0f, 1.0f	//0
	};

	float line[] = { //23 VERTEX
		-1.75f, 0.0f, -0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//0
		-1.5f, 0.0f, -0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//0
		-1.25f, 0.0f, -0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//0
		-1.0f, 0.0f, -0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//0
		-0.75f, 0.0f, -0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//1
		-0.75f, 0.0f, 0.0f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//1
		-0.75f, 0.0f, 0.25f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//1
		-0.75f, 0.0f, 0.5f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//1
		-0.75f, 0.0f, 0.75f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		-0.5f, 0.0f, 0.75f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		-0.25f, 0.0f, 0.75f,		0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		0.0f, 0.0f, 0.75f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		0.25f, 0.0f, 0.75f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		0.5f, 0.0f, 0.75f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//2
		0.75f, 0.0f, 0.75f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//3
		0.75f, 0.0f, 0.5f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//3
		0.75f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		1.0f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		1.25f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		1.5f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		1.75f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		2.0f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f,	//4
		2.25f, 0.0f, 0.25f,			0.0f, 1.0f, 0.0f,	0.0f, 0.0f	//5
	};

	loadObj[0].loadObj("finn_body.obj");
	loadObj[1].loadObj("finn_arms.obj");
	loadObj[2].loadObj("cube.obj");
	loadObj[3].loadObj("cave.obj");

	glGenVertexArrays(6, VAO);
	glGenBuffers(14, VBO);

	//PLAT
	glBindVertexArray(VAO[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plat), plat, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//CUBE
	glBindVertexArray(VAO[1]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[2].outvertex.size() * sizeof(glm::vec3), &loadObj[2].outvertex[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[2].outnormal.size() * sizeof(glm::vec3), &loadObj[2].outnormal[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[2].outuv.size() * sizeof(glm::vec2), &loadObj[2].outuv[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);

	//BODY
	glBindVertexArray(VAO[2]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[0].outvertex.size() * sizeof(glm::vec3), &loadObj[0].outvertex[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[0].outnormal.size() * sizeof(glm::vec3), &loadObj[0].outnormal[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[0].outuv.size() * sizeof(glm::vec2), &loadObj[0].outuv[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);

	//ARMS
	glBindVertexArray(VAO[3]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[1].outvertex.size() * sizeof(glm::vec3), &loadObj[1].outvertex[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[1].outnormal.size() * sizeof(glm::vec3), &loadObj[1].outnormal[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[1].outuv.size() * sizeof(glm::vec2), &loadObj[1].outuv[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);

	//CAVE
	glBindVertexArray(VAO[4]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[10]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[3].outvertex.size() * sizeof(glm::vec3), &loadObj[3].outvertex[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[3].outnormal.size() * sizeof(glm::vec3), &loadObj[3].outnormal[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
	glBufferData(GL_ARRAY_BUFFER, loadObj[3].outuv.size() * sizeof(glm::vec2), &loadObj[3].outuv[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);

	//LINE
	glBindVertexArray(VAO[5]);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void InitTexture()
{
	int widthImage[7], heightImage[7], numberOfChannel[7];

	glGenTextures(7, Textures);

	glBindTexture(GL_TEXTURE_2D, Textures[0]);

	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* body = stbi_load("finn_body_texture.png", &widthImage[0], &heightImage[0], &numberOfChannel[0], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[0], heightImage[0], 0, GL_RGB, GL_UNSIGNED_BYTE, body);

	glBindTexture(GL_TEXTURE_2D, Textures[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* arms = stbi_load("finn_arms_texture.png", &widthImage[1], &heightImage[1], &numberOfChannel[1], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[1], heightImage[1], 0, GL_RGB, GL_UNSIGNED_BYTE, arms);

	glBindTexture(GL_TEXTURE_2D, Textures[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* plat = stbi_load("icePlat.png", &widthImage[2], &heightImage[2], &numberOfChannel[2], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[2], heightImage[2], 0, GL_RGB, GL_UNSIGNED_BYTE, plat);

	glBindTexture(GL_TEXTURE_2D, Textures[3]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* rock = stbi_load("rock.png", &widthImage[3], &heightImage[3], &numberOfChannel[3], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[3], heightImage[3], 0, GL_RGB, GL_UNSIGNED_BYTE, rock);

	glBindTexture(GL_TEXTURE_2D, Textures[4]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* cave = stbi_load("caveSkin.png", &widthImage[4], &heightImage[4], &numberOfChannel[4], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[4], heightImage[4], 0, GL_RGB, GL_UNSIGNED_BYTE, cave);

	glBindTexture(GL_TEXTURE_2D, Textures[5]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* line1 = stbi_load("red.png", &widthImage[5], &heightImage[5], &numberOfChannel[5], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[5], heightImage[5], 0, GL_RGB, GL_UNSIGNED_BYTE, line1);

	glBindTexture(GL_TEXTURE_2D, Textures[6]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* line2 = stbi_load("yellow.png", &widthImage[6], &heightImage[6], &numberOfChannel[6], 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImage[6], heightImage[6], 0, GL_RGB, GL_UNSIGNED_BYTE, line2);
}