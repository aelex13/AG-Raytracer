﻿#pragma once
#include "Ray.h"
#include "template.h"
#include "Scene.h"

class Renderer
{
public:
	Surface* renderSurface;
	Scene* scene;
	Pixel buffer[SCRHEIGHT][SCRWIDTH];
	Renderer(Scene* scene, Surface* renderSurface);

	void Render();
	vec3 Trace(Ray* ray, int x, int y);
	vec3 DirectIllumination(vec3 intersectionPoint, vec3 direction, vec3 normal, Light* lightSource, Material material);
	vec3 Refract(vec3 intersectionPoint, Primitive* hit);
	vec3 RefractInToOut(Ray* innerRay);
};
