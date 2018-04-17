#include <stdio.h>
#include "WObject.h"

WorldObject::WorldObject()
{
}

// CREATE A NEW OBJECT WITH A SHAPE, LOCATION, ROTATION, MESH LOCATION, AND SCALE
WorldObject::WorldObject(std::shared_ptr<Shape> shap, Eigen::Vector3f tran, Eigen::Vector3f rot, Eigen::Vector3f mesh, Eigen::Vector3f scale)
{
	translation = tran;
	rotation = rot;
	meshLocation = mesh;
	meshScale = scale;
	shape = shap;
	mat = Material();
	mins = translation + meshLocation - .5 * scale;
	maxs = translation + meshLocation + .5 * scale;
}

// DRAW THIS OBJECT
void WorldObject::draw(std::shared_ptr<MatrixStack> mStack, std::shared_ptr<Program> prog)
{
	mStack->pushMatrix();
	mStack->translate(translation);
	mStack->rotate(rotation[0], Eigen::Vector3f(1,0,0));
	mStack->rotate(rotation[1], Eigen::Vector3f(0,1,0));
	mStack->rotate(rotation[2], Eigen::Vector3f(0,0,1));
	mStack->pushMatrix();
	mStack->translate(meshLocation);
	mStack->scale(meshScale);
	glUniform3f(prog->getUniform("ka"), mat.getka(0), mat.getka(1), mat.getka(2));
	glUniform3f(prog->getUniform("kd"), mat.getkd(0), mat.getkd(1), mat.getkd(2));
	glUniform3f(prog->getUniform("ks"), mat.getks(0), mat.getks(1), mat.getks(2));
	glUniform1f(prog->getUniform("s"), mat.gets());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, mStack->topMatrix().data());
	shape->draw(prog);
	mStack->popMatrix();
	mStack->popMatrix();
}

// SET THE MATERIAL PARAMETERS FOR THIS OBJECT
void WorldObject::setMaterial(Eigen::Vector3f newKa, Eigen::Vector3f newKd, Eigen::Vector3f newKs, float newS, float newAlpha)
{
	mat.set(newKa, newKd, newKs, newS, newAlpha);
}

// DETECT whether or not the balloon is on top of this object
bool WorldObject::inBounds(Eigen::Vector3f pos)
{
	bool result = false;
	if (pos[0] >= mins[0] && pos[0] <= maxs[0])
	{
		if (pos[2] >= mins[2] && pos[2] <= maxs[2])
		{
			result = true;
		}
	}
	return result;
}

// RETURN the height of this object assuming it is a unit object initially
float WorldObject::getHeight()
{
	return meshScale[1];
}

// DETECT collisions with the given sphere
Eigen::Vector2f WorldObject::block(Eigen::Vector3f pos, float rad)
{
	Eigen::Vector2f result = Eigen::Vector2f(0.0f, 0.0f);
	if (pos[1] - rad < maxs[1])
	{
		Eigen::Vector3f checkPoint = translation + meshLocation;
		checkPoint[1] = pos[1];
		checkPoint = checkPoint - pos;
		checkPoint.normalize();
		checkPoint = pos + rad * checkPoint;
		if (checkPoint[0] >= mins[0] && checkPoint[0] <= maxs[0])
		{
			if (checkPoint[2] >= mins[2] && checkPoint[2] <= maxs[2])
			{
				if (std::abs(pos[2] - checkPoint[2]) < std::abs(pos[0] - checkPoint[0]))
				{
					result[0] = pos[0] - checkPoint[0];
				}
				else
				{
					result[1] = pos[2] - checkPoint[2];
				}
			}
		}
	}
	return result;
}