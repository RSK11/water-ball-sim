#pragma  once
#ifndef __TPCamera__
#define __TPCamera__

#include <memory>

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class MatrixStack;

class TPCamera
{
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	
	TPCamera();
	TPCamera(Eigen::Vector3f pos, float dist);
	virtual ~TPCamera();
	void mouseMoved(float x, float y);
	void setAspect(float a) { aspect = a; };
	void applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const;
	void applyViewMatrix(std::shared_ptr<MatrixStack> MV) const;
	void updateTarget(Eigen::Vector3f tar);
	void updatePosition();
	Eigen::Vector3f getForward();
	Eigen::Vector3f getRight();
	
private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
	Eigen::Vector2f rotation;
	Eigen::Vector3f position;
	Eigen::Vector3f futurePosition;
	Eigen::Vector3f forward;
	Eigen::Vector3f target;
	Eigen::Vector2f mousePrev;
	float xfactor;
	float yfactor;
	float distance;
	float speed;
};

#endif
