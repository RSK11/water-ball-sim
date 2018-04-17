#pragma once
#ifndef _MATER_H_
#define _MATER_H_

#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Dense>

class Material
{
public:
	Material();
	Material(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha);
	void set(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha);
	float getka(int xyz);
	float getkd(int xyz);
	float getks(int xyz);
	float gets();
	float getalpha();
	
private:
	Eigen::Vector3f ka;
	Eigen::Vector3f kd;
	Eigen::Vector3f ks;
	float s;
	float alpha;
};

#endif