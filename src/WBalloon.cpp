#include <stdio.h>
#include <math.h>
#include "WBalloon.h"

WBalloon::WBalloon()
{
	center = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	moveTo = center;
	velocity = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	radius = 1.0f;
	floor = 0.0f;
	gravity << 0.0f, 0.0f, 0.0f;
	t = 0.0f;
	h = 0.01f;
	m = 1.0f;
	stiff = 0.01f;
	mat = Material();
}

WBalloon::WBalloon(Eigen::Vector3f cent, float rad, int verticalCuts, int horizontalCuts, Eigen::Vector3f color)
{
	center = cent;
	moveTo = center;
	velocity = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
	radius = rad;
	floor = 0.0f;
	gravity << 0.0f, -10.0f, 0.0f;
	t = 0.0f;
	h = 0.01f;
	m = 1.0f;
	stiff = 0.01f;
	mat = Material(color, Eigen::Vector3f(0.7f, 0.7f, 0.7f), Eigen::Vector3f(1.0f, 0.9f, 0.8f), 100.0f, 0.8f);

	init(horizontalCuts, verticalCuts);
}

void WBalloon::init(int rows, int cols)
{
	// CREATE south pole (vert 0)
	vertices.push_back(Eigen::Vector3f(center[0], center[1] - radius, center[2]));
	expected.push_back(vertices[0] - center);

	float theta, phi;
	Eigen::Vector3f pos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);

	// CREATE the sphere
	for (int row = 0; row < rows; row++)
	{
		theta = M_PI * ((rows + 1) - (row + 1)) / (rows + 1);
		for (int col = 0; col < cols; col++)
		{
			phi = 2 * M_PI * col / cols;

			pos[0] = radius * sin(theta) * sin(phi);
			pos[1] = radius * cos(theta);
			pos[2] = radius * sin(theta) * cos(phi);
			expected.push_back(pos);
			vertices.push_back(center + pos);
		}
	}

	// CREATE north pole (vert n-1)
	vertices.push_back(Eigen::Vector3f(center[0], center[1] + radius, center[2]));
	expected.push_back(vertices[vertices.size() - 1] - center);

	// CONNECT the south pole to the appropriate points
	for (int col = 1; col < cols; col++)
	{
		indBuf.push_back(0);
		indBuf.push_back(col + 1);
		indBuf.push_back(col);
	}
	indBuf.push_back(0);
	indBuf.push_back(1);
	indBuf.push_back(cols);

	// CONNECT all of the inner vertices
	for (int row = 0; row < rows - 1; row++)
	{
		for (int col = 0; col < cols - 1; col++)
		{
			// UPPER left triangle
			indBuf.push_back(row * cols + col + 1);
			indBuf.push_back((row + 1) * cols + col + 2);
			indBuf.push_back((row + 1) * cols + col + 1);

			// Bottom right triangle
			indBuf.push_back(row * cols + col + 1);
			indBuf.push_back(row * cols + col + 2);
			indBuf.push_back((row + 1) * cols + col + 2);
		}
		// UPPER left triangle
		indBuf.push_back((row + 1) * cols);
		indBuf.push_back((row + 1) * cols + 1);
		indBuf.push_back((row + 2) * cols);

		// Bottom right triangle
		indBuf.push_back((row + 1) * cols);
		indBuf.push_back(row * cols + 1);
		indBuf.push_back((row + 1) * cols + 1);
	}


	// CONNECT the north pole to the appropriate points
	unsigned int np = vertices.size() - 1;
	for (int col = np - 2; col >= np - cols; col--)
	{
		indBuf.push_back(col);
		indBuf.push_back(col + 1);
		indBuf.push_back(np);
	}
	indBuf.push_back(np - 1);
	indBuf.push_back(np - cols);
	indBuf.push_back(np);

	// CREATE the knot
	for (int cir = 0; cir < (int)(cols / 2); cir++)
	{
		vertices.push_back(Eigen::Vector3f(center[0] + .07f * cos(2 * M_PI * cir / (int)(cols / 2)), center[1] + radius + .2, center[2] + .07f * sin(2 * M_PI * cir / (int)(cols / 2))));
		expected.push_back(vertices[vertices.size() - 1] - center);
	}

	for (int knoPos = np + 1; knoPos < vertices.size() - 1; knoPos++)
	{
		indBuf.push_back(np);
		indBuf.push_back(knoPos + 1);
		indBuf.push_back(knoPos);
	}
	indBuf.push_back(np);
	indBuf.push_back(np + 1);
	indBuf.push_back(vertices.size() - 1);

	// INITIALIZE the other vectors
	normals = std::vector<Eigen::Vector3f> (vertices.size(), Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	velocities = std::vector<Eigen::Vector3f> (vertices.size(), Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	posBuf = std::vector<float> (vertices.size() * 3, 0.0f);
	norBuf = std::vector<float> (normals.size() * 3, 0.0f);

	// FILL the position buffer and calculate normals
	vtoPB();
	recalcNorms();

	// CREATE the GPU Buffers
	GLuint tmp[5];
	glGenBuffers(5, tmp);
	bufIDs["aPos"] = tmp[0];
	bufIDs["aNor"] = tmp[1];
	bufIDs["bPos"] = tmp[2];
	bufIDs["bNor"] = tmp[3];
	bufIDs["bInd"] = tmp[4];

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	assert(norBuf.size() == posBuf.size());
}

// FILL the normal buffer from the normal vector
void WBalloon::ntoNB()
{
	for (int tcount = 0; tcount < norBuf.size(); tcount += 3)
	{
		norBuf[tcount] = normals[(int)tcount / 3][0];
		norBuf[tcount + 1] = normals[(int)tcount / 3][1];
		norBuf[tcount + 2] = normals[(int)tcount / 3][2];
	}
}

// FILL the position buffer from the vertex vector
void WBalloon::vtoPB()
{
	for (int tcount = 0; tcount < posBuf.size(); tcount += 3)
	{
		posBuf[tcount] = vertices[(int)tcount / 3][0];
		posBuf[tcount + 1] = vertices[(int)tcount / 3][1];
		posBuf[tcount + 2] = vertices[(int)tcount / 3][2];
	}
}

// CALCULATE the normals for each vertex
void WBalloon::recalcNorms()
{
	Eigen::Vector3f a;
	Eigen::Vector3f b;
	Eigen::Vector3f result;

	for (int i = 0; i < normals.size(); i++)
	{
		normals[i] -= normals[i];
	}

	for (int tcount = 0; tcount < indBuf.size(); tcount += 3)
	{
		a = vertices[indBuf[tcount + 1]] - vertices[indBuf[tcount]];
		b = vertices[indBuf[tcount + 2]] - vertices[indBuf[tcount]];
		result = a.cross(b);
		normals[indBuf[tcount]] += result;
		normals[indBuf[tcount + 1]] += result;
		normals[indBuf[tcount + 2]] += result;
	}

	for (int j = 0; j < normals.size(); j++)
	{
		normals[j].normalize();
	}

	ntoNB();
}

// MOVE the balloon by one step
void WBalloon::step(Eigen::Vector2f block)
{
	// Gravity downwards
	Eigen::Vector3f f;
	float d;

	// MOVE the balloon downward if it is not on the floor
	if (center[1] > floor + radius)
	{
		moveTo[1] += h * gravity[1];
	}
	else
	{
		moveTo[1] = floor + radius;
		center[1] = floor + radius;
	}

	// MOVE the balloon if there was a collision
	if (block[0] != 0.0f)
	{
		moveTo[0] = center[0] + block[0];
	}
	if (block[1] != 0.0f)
	{
		moveTo[2] = center[2] + block[1];
	}

	// MOVE the center of the balloon
	velocity += (moveTo - center) / h - velocity * .9;
	center += h * velocity;

	// ROTATE the expected positions for each vertex
	Eigen::Vector3f ax = h * velocity;
	float angle = ax.norm() / radius;
	ax = ax.cross(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
	if (ax.norm() > 0.0f && angle > 0.0f)
	{
		ax.normalize();
		rotatePoints(ax, -angle);
	}

	// MOVE each vertex
	for (int tcount = 0; tcount < velocities.size(); tcount ++)
	{
		// Gravity
		f = m * gravity;
		
		// Damping
		d = randFloat(-0.8f, 7.5f);
		f -= d * velocities[tcount];

		// Update velocity
		velocities[tcount] += (stiff * ((center + expected[tcount]) - vertices[tcount]) / h) + (h / m) * f;
		
		// Update position
		vertices[tcount] += h * velocities[tcount];
		
		// Stop vertices at the floor
		if (vertices[tcount][1] < floor)
		{
			vertices[tcount][1] = floor;
		}
	}

	t += h;

	// FILL the position and normal buffers
	vtoPB();
	recalcNorms();
}

// ROTATE the expected positions the given amount around the given axis
void WBalloon::rotatePoints(Eigen::Vector3f axis, float rotation)
{
	Eigen::Matrix3f rot = Eigen::AngleAxisf(rotation, axis).toRotationMatrix();
	for (int vert = 0; vert < expected.size(); vert++)
	{
		expected[vert] = rot * expected[vert];
	}
}

// Compute a random float between l and r
float WBalloon::randFloat(float min, float max)
{
	float r = rand() / (float)RAND_MAX;
	return (1.0f - r) * min + r * max;
}

// SET the floor that the balloon is on
void WBalloon::setFloor(float y)
{
	floor = y;
}

// MOVE the balloon's center
void WBalloon::move(Eigen::Vector3f direction, float distance)
{
	moveTo += distance * direction;
}

// DRAW the balloon
void WBalloon::draw(std::shared_ptr<MatrixStack> mStack, std::shared_ptr<Program> prog)
{
	glUniform3f(prog->getUniform("ka"), mat.getka(0), mat.getka(1), mat.getka(2));
	glUniform3f(prog->getUniform("kd"), mat.getkd(0), mat.getkd(1), mat.getkd(2));
	glUniform3f(prog->getUniform("ks"), mat.getks(0), mat.getks(1), mat.getks(2));
	glUniform1f(prog->getUniform("s"), mat.gets());
	glUniform1f(prog->getUniform("alpha"), mat.getalpha());
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, mStack->topMatrix().data());

	// Bind position buffer
	int h_pos = prog->getAttribute("aPos");
	glEnableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

	// Bind normal buffer
	int h_nor = prog->getAttribute("aNor");
	glEnableVertexAttribArray(h_nor);
	glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bNor"]);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);

	// Draw
	glDrawElements(GL_TRIANGLES, (int)indBuf.size(), GL_UNSIGNED_INT, (void *)0);

	// Disable and unbind
	glDisableVertexAttribArray(h_nor);
	glDisableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// RETURN the balloon's position
Eigen::Vector3f WBalloon::getPosition()
{
	return moveTo;
}

// RETURN the balloon's radius
float WBalloon::getRadius()
{
	return radius;
}