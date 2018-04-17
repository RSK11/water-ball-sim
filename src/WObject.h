#pragma once
#ifndef _WOBJECT_H_
#define _WOBJECT_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

#include <vector>

#include "Shape.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Material.h"

class WorldObject {
	Eigen::Vector3f translation;
	Eigen::Vector3f rotation;
	Eigen::Vector3f meshLocation;
	Eigen::Vector3f meshScale;
	Eigen::Vector3f mins;
	Eigen::Vector3f maxs;
	std::shared_ptr<Shape> shape;
	Material mat;
public:
	WorldObject();
	WorldObject(std::shared_ptr<Shape>, Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f);
	void draw(std::shared_ptr<MatrixStack> , std::shared_ptr<Program>);
	void setMaterial(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha);
	bool inBounds(Eigen::Vector3f pos);
	float getHeight();
	Eigen::Vector2f block(Eigen::Vector3f pos, float rad);
};

#endif