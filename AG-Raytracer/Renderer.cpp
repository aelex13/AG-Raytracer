﻿#include "template.h"

#define DEBUG 1
#define EPSILON 0.01f
#define INVPI 0.31830988618379067153776752674503f

#define Psurvival 0.8f

#define USEBVH 0

#define MAXRAYDEPTH 5
#define UseRR 0

Renderer::Renderer(Scene* scene, Surface* renderSurface)
{

	this->scene = scene;
	this->renderSurface = renderSurface;
	this->scene->camera->GenerateRays();
	this->frameCount = 0;
	this->pixelNumber = 1;
	this->numberOfLights = sizeof(this->scene->lights) / sizeof(this->scene->lights[0]);
}

//Int, so it can return the total pixel values summed
int Renderer::Render() {

	int pixelCount = 0;

	//Increase frameCount, which is used for averaging multiple frames of path tracing.
	frameCount++;

#pragma omp parallel for
	for (int y = 0; y < SCRHEIGHT; y++) {
		//printf("Current y:%i% \n", y);

#pragma omp parallel for
		for (int x = 0; x < SCRWIDTH; x++)
		{
			//if (x == 632 && y == 422)
			//	printf("test");
			vec3 colorResult;
			if (x < SCRWIDTH / 2)
				colorResult = Sample(this->scene->camera->primaryRays[y*SCRWIDTH + x], 0);
			else
				colorResult = BasicSample(this->scene->camera->primaryRays[y*SCRWIDTH + x], 0);

			// First convert range
			colorResult *= 256.0f;

			//Put our newly calculated values in the accumulator.
			accumulator[y][x].r += colorResult.r;
			accumulator[y][x].g += colorResult.g;
			accumulator[y][x].b += colorResult.b;

			//Now, we can use the values stored in the accumulator to calculate the correct pixel value to draw.
			//We first need to calculate correct float values, and then clamp.

			float r = accumulator[y][x].r / static_cast<float>(frameCount);
			float g = accumulator[y][x].g / static_cast<float>(frameCount);
			float b = accumulator[y][x].b / static_cast<float>(frameCount);

			if (r < 0 || g < 0 || b < 0)
				printf("dingen zijn <0");

			// Then clamp the newly calculated float values (they may be way above 255, when something is very bright for example).
			int nextR = min((int)r, 255);
			int nextG = min((int)g, 255);
			int nextB = min((int)b, 255);

			pixelCount += (int)r + (int)g + (int)b;//(int)(r + g + b); //NOT nextR + nextG + nextB;

			// Then merge it to the buffer.
			buffer[y][x] = (nextR << 16) + (nextG << 8) + (nextB);

			this->pixelNumber++;
		}
	}
	//#pragma omp parallel for
	for (int y = 0; y < SCRHEIGHT; y++)
		//#pragma omp parallel for
		for (int x = 0; x < SCRWIDTH; x++)
			this->renderSurface->Plot(x, y, this->buffer[y][x]);

	this->pixelNumber = 1;

	return pixelCount;
}

vec3 Renderer::Sample(Ray* ray, int depth, bool secondaryRay)
{
	//TODO: De RR later, en dan een count bijhouden van alle dode rays? Dan  (AmountOfRays - KilledOf) / AmountOfRays gebruiken om levende rays te scalen?
	/*
	Over Russian Roulette:

	Pak geen vaste waarde voor kans, maar pak (r+g+b)/3, en dit cappen tussen 0 en 1.
	Verder: je DirectLight moet je altijd doen, 100%, dus die ook ZEKER NIET delen door je kans.
	Je moet wel de RandomLights delen door die kans, maar dus NIET je totaal.
	*/
#if UseRR
	float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//glm::uint  seed1 = ((uint)frameCount + pixel * 15485867 + depth * 103183) * 57089;
	//float a = RandomFloat(&seed1);


	//Russian Roulette; check if we need to kill a ray.
	if (a > Psurvival && secondaryRay)
	{
		return vec3(0);
	}
#else

	if (depth > MAXRAYDEPTH)
	{
		return vec3(0);
	}
#endif

	// trace ray
	vec3 intersect = Trace(ray);

	// terminate if ray left the scene
	if (ray->t == INFINITY)
	{
		return vec3(0);
	}

	Entity* hit = ray->hit;

	if (hit->isLight)
	{
		//This is to check if the ray is an indirect illumination one.
		if (secondaryRay)
			return vec3(0);
		else
			return static_cast<Light*>(hit)->color;
	}

	Primitive* primitiveHit = static_cast<Primitive*>(hit);

	vec3 normal = primitiveHit->GetNormal(intersect);

	//TODO: CHECK this.
	normal = dot(normal, ray->direction) <= 0.0f ? normal : normal * (-1.0f);

	if (primitiveHit->material.materialKind == Material::MaterialKind::MIRROR)
	{
		// continue in fixed direction
		Ray r(intersect, reflect(ray->direction, normal));
		return primitiveHit->material.color * Sample(&r, depth + 1, false);
	}

	vec3 directIllumination = DirectSampleLights(intersect, normal, primitiveHit->material);

	// continue in random direction
	vec3 R = CosineWeightedDiffuseReflection(normal);

	//This random ray is used for the indirect lighting.
	Ray newRay = Ray(intersect + R * EPSILON, R);

	vec3 BRDFIndirect = primitiveHit->material.color * INVPI;

	//float PDF = 1 / (2 * PI); //dot(normal, R) / PI;//

	vec3 Ei = Sample(&newRay, depth + 1, true) * dot(normal, R) * 2.0f * PI; // irradiance
	vec3 indirectIllumination = BRDFIndirect * Ei;

#if UseRR
	return vec3(indirectIllumination.x / Psurvival, indirectIllumination.y / Psurvival, indirectIllumination.z / Psurvival) + directIllumination;

#else
	vec3 result = indirectIllumination + directIllumination;
	if (result.x < 0 || result.y < 0 || result.z < 0)
		printf("smaller 0");

	return indirectIllumination + directIllumination;

#endif
}

vec3 Renderer::DirectSampleLights(vec3 intersect, vec3 normal, Material material)
{
	int lightIndex = rand() % numberOfLights;
	Triangle* lightTri = this->scene->lights[lightIndex]->tri;

	float a = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (a + b > 1)
	{
		a = 1 - a;
		b = 1 - b;
	}

	vec3 point = lightTri->v0 + a*(lightTri->v1 - lightTri->v0) + b*(lightTri->v2 - lightTri->v0);

	vec3 L = point - intersect;
	float dist = length(L);
	L /= dist;

	float cos_o = dot(-L, lightTri->normal);

	float cos_i = dot(L, normal);

	if ((cos_o <= 0) || (cos_i <= 0)) return vec3(0);

	Ray r = Ray(intersect + EPSILON * L, L);

	r.hit = 0;

	//If our ray to a light hits something on its path towards the light, we can't see light, so we will return black.
	Trace(&r);

	if (r.hit)
		//TODO: Check of geraakte primitive de lightTri is. Dan optimalisatie: Shadowrays, midner werk, sneller
		if (!r.hit->isLight)
			//Als we licht geraakt hebben niks ertussen, dus prima.
			return vec3(0);

	Entity* hit = r.hit;
	Light* lightHit = static_cast<Light*>(hit);

	vec3 BRDF = material.color * INVPI;
	float solidAngle = (cos_o * this->scene->lights[lightIndex]->area) / (dist*dist);


	vec3 result = BRDF * (float)numberOfLights * this->scene->lights[lightIndex]->color * solidAngle * cos_i;
	if (result.x < 0 || result.y < 0 || result.z < 0)
		printf("smaller in direct illu 0");

	return result;
}

//This is the "old", slow, noisy sampling function. But it works, so it's useful for testing.
vec3 Renderer::BasicSample(Ray* ray, int depth)
{
	if (depth > MAXRAYDEPTH)
	{
		return vec3(0);
	}
	// trace ray
	vec3 intersect = Trace(ray);

	// terminate if ray left the scene
	if (ray->t == INFINITY)
	{
		return vec3(0);
	}

	Entity* hit = ray->hit;

	// terminate if we hit a light source
	if (hit->isLight)
	{
		//printf("%f% \n ", dot(static_cast<Light*>(hit)->tri->normal, ray->direction));
		if (dot(static_cast<Light*>(hit)->tri->normal, ray->direction) > 0)
			return vec3(0);
		else
			return static_cast<Light*>(hit)->color;
	}

	Primitive* primitiveHit = static_cast<Primitive*>(hit);

	vec3 normal = primitiveHit->GetNormal(intersect);

	// continue in random direction
	vec3 R = CosineWeightedDiffuseReflection(normal);
	Ray newRay = Ray(intersect + R * EPSILON, R);


	////Code for direct Illu only
	//newRay.hit = nullptr;
	//vec3 intersect2 = Trace(&newRay);
	//if (!newRay.hit)
	//	return vec3(0);
	//else
	//	if (!newRay.hit->isLight)
	//		return vec3(0);
	//	else
	//		if (dot(static_cast<Light*>(newRay.hit)->tri->normal, newRay.direction) > 0)
	//			return vec3(0);

	vec3 BRDF = primitiveHit->material.color * INVPI;

	vec3 Ei = BasicSample(&newRay, depth + 1) * dot(normal, R); // irradiance

	return PI * 2.0f * BRDF * Ei; //* dot(normal, R) * static_cast<Light*>(newRay.hit)->color;
}

#pragma region 
glm::uint Renderer::TauStep(int s1, int s2, int s3, glm::uint M, glm::uint* seed)
{
	glm::uint b = (((*seed << s1) ^ *seed) >> s2);
	*seed = (((*seed & M) << s3) ^ b);
	return *seed;
}


glm::uint Renderer::HQIRand(glm::uint* seed)
{
	uint z1 = TauStep(13, 19, 12, 429496729, seed);
	uint z2 = TauStep(2, 25, 4, 4294967288, seed);
	uint z3 = TauStep(3, 11, 17, 429496280, seed);
	uint z4 = 1664525 * *seed + 1013904223;
	return z1 ^ z2 ^ z3 ^ z4;
}


glm::uint Renderer::SeedRandom(glm::uint s)
{
	uint seed = s * 1099087573;
	seed = HQIRand(&seed);
	return seed;
}

glm::uint Renderer::RandomInt(glm::uint * seed)
{
	// Marsaglia Xor32; see http://excamera.com/sphinx/article-xorshift.html
	// also see https://github.com/WebDrake/xorshift/blob/master/xorshift.c for higher quality variants
	*seed ^= *seed << 13;
	*seed ^= *seed >> 17;
	*seed ^= *seed << 5;
	return *seed;
}


float Renderer::RandomFloat(glm::uint * seed)
{
	return RandomInt(seed) * 2.3283064365387e-10f;
}
#pragma endregion RandomStuff


vec3 Renderer::DiffuseReflection(vec3 normal)
{
	// based on SmallVCM / GIC
	float r1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX), r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	float term1 = 2 * PI * r1;
	float term2 = 2 * sqrt(r2 * (1 - r2));
	vec3 R = vec3(cos(term1) * term2, sin(term1) * term2, 1 - 2 * r2);
	if (R.z < 0) R.z = -R.z;


	vec3 w = normal;
	vec3 axis = fabs(w.x) > 0.1f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 u = normalize(cross(axis, w));
	vec3 v = cross(w, u);


	vec3 result = normalize(R.x * u + R.y * v + R.z*w);
	return result;
}

vec3 Renderer::CosineWeightedDiffuseReflection(vec3 normal)
{
	glm::uint  seed1 = ((uint)frameCount + pixelNumber * 6217) * 57089;
	glm::uint  seed2 = ((uint)frameCount + pixelNumber * 104729) * 19423;

	//NOTE: THE RANDOM WITH SEEDS YIELDS ENTIRELY DIFFERENT RESULTS. SOMETHING IS WEIRD/BROKEN.

	//float r1 = 2.0f * PI * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	//float r2 = 1;
	//while (r2 == 1)
	//	r2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	glm::uint seeed1 = SeedRandom(seed1);
	float r1 = 2.0f * PI * RandomFloat(&seeed1);

	glm::uint seeed2 = SeedRandom(seed2);
	float r2 = RandomFloat(&seeed2);

	float r2s = sqrt(r2);
	vec3 n = normal;
	vec3 axis = fabs(n.x) > 0.1f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);
	vec3 t = normalize(cross(axis, n));
	vec3 b = cross(n, t);

	vec3 new_dir = normalize(t  * cos(r1) * r2s + b* sin(r1) * r2s + n * sqrt(1.0f - r2));

	if (dot(new_dir, normal) < 0)
		printf("this should never happen but happens.");

	return new_dir;

	////float r0 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX), r1A = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	////float r2s = sqrt(r0);
	////float r1 = 2 * PI * r1A;

	//float x = r2s * cos(r1);
	//float y = r2s * sin(r1);

	//vec3 dir = vec3(x, y, sqrt(1 - r2));

	//vec3 axisSelf = abs(normal.x) > 0.1f ? vec3(0.0f, 1.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f);

	//vec3 tSelf = cross(axisSelf, normal);
	//vec3 bSelf = cross(normal, tSelf);

	//vec3 manual = normalize( dir.x * tSelf + dir.y * bSelf + dir.z * normal);

	//return manual;
}

vec3 Renderer::Trace(Ray* ray)
{
	float smallestT = INFINITY;

#if !USEBVH
	for (int x = 0; x < sizeof(this->scene->entities) / sizeof(this->scene->entities[0]); x++)
	{
		if (this->scene->entities[x]->CheckIntersection(ray) && smallestT > ray->t)
		{
			smallestT = ray->t;
			ray->hit = this->scene->entities[x];
		}
	}

#else 
	scene->bvh->Traverse(ray, scene->bvh->rootNode);
	smallestT = ray->t;

	//Lights are not included in the BVH.

	//TODO: Fix this hardcoding.
	for (int x = 12; x < 14; x++)
	{
		if (this->scene->entities[x]->CheckIntersection(ray) && smallestT > ray->t)
		{
			smallestT = ray->t;
			ray->hit = this->scene->entities[x];
		}
	}

	smallestT = ray->t;
#endif

	if (smallestT == INFINITY)
	{
		return vec3(0);
	}
	else
	{
		//Set t back, this is needed for the pathtracing code which checks if we need to return black (occlusion toward light)
		ray->t = smallestT;

		vec3 intersectionPoint = ray->origin + smallestT*ray->direction;
		//vec3 normal = hit->GetNormal(intersectionPoint);
		vec3 colorResult = vec3(0, 0, 0);

		return intersectionPoint;
	}
}