#include "Material.h"

Material::Material()
{
	ka = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	kd = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	ks = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	s = 0.0f;
	alpha = 1.0f;
}

Material::Material(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha)
{
	ka = newKa;
	kd = newKd;
	ks = newKs;
	s = newS;
	alpha = newAlpha;
}

void Material::set(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha)
{
	ka = newKa;
	kd = newKd;
	ks = newKs;
	s = newS;
	alpha = newAlpha;
}

float Material::getka(int xyz)
{
	return ka[xyz];
}

float Material::getkd(int xyz)
{
	return kd[xyz];
}

float Material::getks(int xyz)
{
	return ks[xyz];
}

float Material::gets()
{
	return s;
}

float Material::getalpha()
{
	return alpha;
}