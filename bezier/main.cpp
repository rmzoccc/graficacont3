#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <GLFW/glfw3.h>
#include <stdlib.h>


using namespace std;

double mouseX, mouseY;
bool mouseReleased = false;

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		mouseReleased = true;
	}
}

class Point2d{
public:
	float x,y;
	Point2d(float x, float y) : x(x),y(y){}
	~Point2d(){}

	void draw(float size = 1){
		glPointSize(size);
		glBegin(GL_POINTS);
		glColor3f(1, 1, 1);
		glVertex3f(x, y, 1);
		glEnd();
	}

};

void drawPoint(vector<Point2d> points, float t){
	if(points.size() == 1){
		points[0].draw();
	}else{
		vector<Point2d> aux = points;
		aux.pop_back();
		for(int i=0; i<aux.size(); i++){
			float x = (1-t)*points[i].x + t*points[i+1].x;
			float y = (1-t)*points[i].y + t*points[i+1].y;
			aux[i] = Point2d(x,y);
		}
		drawPoint(aux, t);
	}
}

void drawCurve(vector<Point2d> points, int steps){
	if(points.size() < 3) return;
	for(int i=0; i<steps; i++){
		float t = float(i)/(steps-1);
		drawPoint(points, t);
	}
}

void drawPoints(vector<Point2d>& points){
	for(int i=0; i<points.size(); i++){
		points[i].draw(5);
	}
}

int main(void) {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	vector<Point2d> points;

	do{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();
		glOrtho(0, 640, 480, 0, 1, -1);


		if(mouseReleased){
			glfwGetCursorPos(window, &mouseX, &mouseY);
			Point2d p = Point2d(mouseX, mouseY);
			points.push_back(p);
			mouseReleased = false;
		}

		drawPoints(points);
		drawCurve(points, 100);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
       glfwWindowShouldClose(window) == 0);

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}