#include "TPCamera.h"
#include "MatrixStack.h"
#include <stdio.h>
#include <iostream>

TPCamera::TPCamera() :
	aspect(1.0f),
	fovy(45.0f),
	znear(0.1f),
	zfar(1000.0f),
	rotation(0.0, 0.0),
	position(0.0f, 0.0f, -5.0f),
	target(0.0f, 0.0f, 0.0f),
	xfactor(0.4f),
	yfactor(0.001f),
	speed(0.001f)
{
}

TPCamera::TPCamera(Eigen::Vector3f pos, float dist)
{
	aspect = 1.0f;
	fovy = 45.0f;
	znear = 0.1f;
	zfar = 1000.0f;
	rotation = Eigen::Vector2f(90.0, -180.0);
	forward = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
	target = pos;
	position = pos;
	distance = dist;
	position[2] -= distance;
	futurePosition = position;
	xfactor = 0.5f;
	yfactor = 0.5f;
	speed = 0.1f;
}

TPCamera::~TPCamera()
{
}

// ROTATE the camera based on mouse movement
void TPCamera::mouseMoved(float x, float y)
{
	Eigen::Vector2f mouseCurr(x, y);
	Eigen::Vector2f dv = mouseCurr - mousePrev;
	rotation -= Eigen::Vector2f(dv[1] * yfactor, dv[0] * xfactor);
	rotation[0] = fmax(fmin(rotation[0], 90.0f), 1.0f);
	updatePosition();
	forward = target - position;
	forward[1] = 0.0f;
	forward.normalize();
	mousePrev = mouseCurr;
}

void TPCamera::applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->perspective(fovy, aspect, znear, zfar);
}

void TPCamera::applyViewMatrix(std::shared_ptr<MatrixStack> MV) const
{
	MV->lookAt(position, target, Eigen::Vector3f(0.0f, 1.0f, 0.0f));
}

// MOVE the camera based on given directions
void TPCamera::updateTarget(Eigen::Vector3f tar)
{
	target = tar;
	updatePosition();
}

void TPCamera::updatePosition()
{
	position[0] = target[0] + distance * sin(rotation[0]* M_PI / 180) * sin(rotation[1]* M_PI / 180);
	position[1] = target[1] + distance * cos(rotation[0] * M_PI / 180);
	position[2] = target[2] + distance * sin(rotation[0]* M_PI / 180) * cos(rotation[1] * M_PI / 180);
}

Eigen::Vector3f TPCamera::getForward()
{
	return forward;
}

Eigen::Vector3f TPCamera::getRight()
{
	return forward.cross(Eigen::Vector3f(0.0f,1.0f, 0.0f));
}
