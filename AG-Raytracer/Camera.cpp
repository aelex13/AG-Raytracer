﻿#include "template.h"
#include "Camera.h"
#include "Ray.h"
#include <random>

Camera::Camera()
{
	this->Init();
	this->d = 2;

	//position = vec3(transformMatrix[0][3], transformMatrix[1][3], transformMatrix[2][3]);
	//viewDirection = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	printf("\n viewDirection : %f, %f, %f, %f \n",
		viewDirection[0],
		viewDirection[1],
		viewDirection[2],
		viewDirection[3]
	);

	screenCenter = vec4(position.x, position.y, position.z, 0) + d*viewDirection;
	p0 = screenCenter + vec4(-1.0f, -1.0f, 0.0f, 1.0f);
	p1 = screenCenter + vec4(1.0f, -1.0f, 0.0f, 1.0f);
	p2 = screenCenter + vec4(-1.0f, 1.0f, 0.0f, 1.0f);

	printf("\n screenCenter : %f, %f, %f \n",
		screenCenter[0],
		screenCenter[1],
		screenCenter[2]
	);	
	printf("\n p0: %f, %f, %f \n",
		p0[0],
		p0[1],
		p0[2]
	);
	printf("\n p1: %f, %f, %f \n",
		p1[0],
		p1[1],
		p1[2]
	);
	printf("\n p2: %f, %f, %f \n",
		p2[0],
		p2[1],
		p2[2]
	);

}

void Camera::GenerateRays()
{
	float u, v = 0.0f;

	float ratio = SCRWIDTH / SCRHEIGHT;
	float width = 1.0f;
	float height = ratio;

	for (int y = 0; y < SCRHEIGHT; y++) {
		for (int x = 0; x < SCRWIDTH; x++)
		{
			u = (width / SCRWIDTH) * x;
			v = (height / SCRHEIGHT) * y;

			vec4 pointOnScreen = p0 + u*(p1 - p0) + v*(p2 - p0);

			vec3 position3d = position.xyz;
			vec3 pointOnScreen3d = pointOnScreen.xyz;
			
			vec3 direction = normalize(pointOnScreen3d - position3d);

			Ray* ray = new Ray(position.xyz, direction);

			if (y == SCRHEIGHT / 2 && x == SCRWIDTH / 2)
				printf("\n middle of the screen : %f, %f, %f \n", ray->direction[0], ray->direction[1], direction, ray->direction[2]);

			

			primaryRays[y * SCRWIDTH + x] = ray;
		}
	}
}

//TODO : Check if the order of the matrix multiplication is correct
void Camera::TransformCamera(mat4 transformMatrix) 
{
	this->transformMatrix *= transformMatrix;
}

void Camera::Init() 
{
	this->transformMatrix = mat4(
		0.0f, 0.0f, 0.0f, 0.0f,  // x0,y0,z0,w0
		0.0f, 0.0f, 0.0f, 0.0f,  // x1,y1,z1,w1
		0.0f, 0.0f, 0.0f, 0.0f,  // x2,y2,z2,w2 
		0.0f, 0.0f, 0.0f, 1.0f); // x3,y3,z3,w3
	this->scale			= vec3();
	this->rotation		= quat();
	this->position		= vec3();
	this->skew			= vec3();
	this->viewDirection	= vec4();
}

void Camera::UpdatePosition()
{
	decompose(transformMatrix, scale, rotation, position, skew, viewDirection);
	rotation = conjugate(rotation);
}