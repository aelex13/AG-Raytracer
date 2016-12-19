﻿#pragma once

#define TRI_SCENE 1
#define TUNNEL_SCENE 0
#define MIRROR_SCENE 0
#define OBJ_LOAD 0

class Scene
{
public:
	
#if TRI_SCENE
	Primitive* primitives[9];
	Light* lights[1];
#elif TUNNEL_SCENE
	Primitive* primitives[8];
	Light* lights[2];
#elif MIRROR_SCENE
	Primitive* primitives[9];
	Light* lights[2];
#elif TINY_SCENE
	Primitive* primitives[1];
	Light* lights[2];
#elif OBJ_LOAD
	Primitive* primitives[12];
	Light* lights[1];
#endif

	Camera* camera;
	AABB* sceneBounds;
	BVH* bvh;
	Scene();

	// TODO : Method to calculate maximal bounding box given all internal primitives?
	AABB* CalculateSceneBounds();
};
