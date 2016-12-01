﻿#include "template.h"
#include "Renderer.h"

#define DEBUG 1

Renderer::Renderer(Scene* scene, Surface* renderSurface)
{
	this->scene = scene;
	this->renderSurface = renderSurface;
	this->scene->camera->GenerateRays();
}

void Renderer::Render() {

	for (int y = 0; y < SCRHEIGHT; y++) {
		for (int x = 0; x < SCRWIDTH; x++)
		{
			buffer[y][x] = Trace(this->scene->camera->primaryRays[y*SCRWIDTH + x], x, y);
		}
	}

	for (int y = 0; y < SCRHEIGHT; y++)
		for (int x = 0; x < SCRWIDTH; x++)
			this->renderSurface->Plot(x, y, this->buffer[y][x]);
}


Pixel Renderer::Trace(Ray* ray, int x, int y)
{
	float smallestT = INFINITY;
	Primitive* hit;

	for (int x = 0; x < sizeof(this->scene->primitives) / sizeof(this->scene->primitives[0]); x++)
	{
		if (this->scene->primitives[x]->CheckIntersection(ray) && smallestT > ray->t)
		{
			smallestT = ray->t;
			hit = this->scene->primitives[x];
		}

	}
	if (ray->t == INFINITY) {
		return 0xffffff;
	}
	else {
		vec3 intersectionPoint = ray->origin + ray->t*ray->direction;
		vec3 colorResult = vec3(0, 0, 0);

		for (int i = 0; i < sizeof(this->scene->lights) / sizeof(this->scene->lights[0]); i++)
			DirectIllumination(	
			intersectionPoint,
			glm::normalize(ray->origin - scene->lights[i]->position),
			hit->GetNormal(intersectionPoint),
			scene->lights[i],
			hit->material);
		//TODO: vec3 to Pixel conversion
		// First convert range
		colorResult *= 256.0f;
		// Then clamp
		int r = min((int)colorResult.x, 255);
		int g = min((int)colorResult.y, 255);
		int b = min((int)colorResult.z, 255);
		// Then merge
		return ((r << 16) + (g << 8) + (b));
	}
}

vec3 Renderer::DirectIllumination(vec3 intersectionPoint, vec3 direction, vec3 normal, Light* lightSource, Material material) {
	Ray* shadowRay = new Ray(intersectionPoint, direction);
	float lightIntensity = 0.0f;
	
	for (int x = 0; (x < sizeof(this->scene->primitives) / sizeof(this->scene->primitives[0])) && (shadowRay->t == INFINITY); x++)
	{
		this->scene->primitives[x]->CheckIntersection(shadowRay);
	}
	
	if (shadowRay->t == INFINITY) {
		delete shadowRay;
		return vec3(0.0f, 0.0f, 0.0f);
	} else {
		//float euclidianDistance = distance(intersectionPoint, lightSource->position);
		float euclidianDistance = shadowRay->t;
		delete shadowRay;

		// I = LightColor * N. L * 1/d^2 * BRDF/PI
		return	lightSource->intensity * 
				lightSource->color * 
				normal * direction * 
				(1/(euclidianDistance*euclidianDistance)) *
				(material.color / PI);
	}
}