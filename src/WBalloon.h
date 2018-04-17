#pragma once
#ifndef _WBALLOON_H_
#define _WBALLOON_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <vector>

#include "Program.h"
#include "MatrixStack.h"
#include "Material.h"

class WBalloon {
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<unsigned int> indBuf;
	std::vector<Eigen::Vector3f> vertices;
	std::vector<Eigen::Vector3f> expected;
	std::vector<Eigen::Vector3f> normals;
	std::vector<Eigen::Vector3f> velocities;
	Eigen::Vector3f center;
	Eigen::Vector3f velocity;
	Eigen::Vector3f moveTo;
	Eigen::Vector3f gravity;
	Material mat;
	float radius, floor, t, h, m, stiff;
	std::map<std::string,GLuint> bufIDs;

	void init(int rows, int cols);
	void rotatePoints(Eigen::Vector3f direction, float rotation);
	void recalcNorms();
	void ntoNB();
	void vtoPB();
	float randFloat(float min, float max);
public:
	WBalloon();
	WBalloon(Eigen::Vector3f cent, float rad, int verticalCuts, int horizontalCuts, Eigen::Vector3f color);
	void move(Eigen::Vector3f direction, float distance);
	void step(Eigen::Vector2f block);
	void draw(std::shared_ptr<MatrixStack> , std::shared_ptr<Program>);
	void setFloor(float y);
	Eigen::Vector3f getPosition();
	float getRadius();
};

#endif