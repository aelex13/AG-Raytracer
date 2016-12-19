﻿#pragma once

class BVHNode
{
public:
	AABB bounds;
	int leftFirst;
	int count;

	//void Traverse(Ray ray, BVHNode** pool);
	//void BVHNode::IntersectPrimitives(Ray ray, BVHNode** pool);

	void Subdivide(BVHNode** pool, Primitive** primitives, glm::uint& poolPtr);
	void Partition(BVHNode** pool, Primitive** primitives, glm::uint& poolPtr);

	bool IsLeaf();


	BVHNode() {};

	BVHNode(AABB bounds, int leftFirst, int count)
	{
		this->bounds = bounds;
		this->leftFirst = leftFirst;
		this->count = count;
	}
};