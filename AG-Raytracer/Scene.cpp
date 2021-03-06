﻿#include "template.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Scene::Scene()
{
	camera = new Camera();
	//sceneBounds = new AABB(vec3(-100, -100, -100), vec3(100, 100, 100));

#if TRI_SCENE
	lights[0] = new Light(vec3(0, 0, 1), vec3(100, 100, 100));
	//lights[1] = new Light(vec3(-2, 0, 0), vec3(50, 50, 50));

	primitives[0] = new Triangle(vec3(-3, 0, 8), vec3(-3, 2, 8), vec3(-1, 0, 8));
	primitives[0]->material = Material(vec3(0, 0, 1), Material::MaterialKind::DIFFUSE);

	primitives[1] = new Triangle(vec3(0, 0, 8), vec3(0, 2, 8), vec3(2, 0, 8));
	primitives[1]->material = Material(vec3(0, 1, 0), Material::MaterialKind::DIFFUSE);

	primitives[2] = new Triangle(vec3(3, 0, 8), vec3(3, 2, 8), vec3(5, 0, 8));
	primitives[2]->material = Material(vec3(0, 1, 1), Material::MaterialKind::DIFFUSE);

	primitives[3] = new Triangle(vec3(-3, 2.5, 8), vec3(-3, 4.5, 8), vec3(-1, 2.5, 8));
	primitives[3]->material = Material(vec3(1, 0, 0), Material::MaterialKind::DIFFUSE);

	primitives[4] = new Triangle(vec3(0, 2.5, 8), vec3(0, 4.5, 8), vec3(2, 2.5, 8));
	primitives[4]->material = Material(vec3(1, 0, 1), Material::MaterialKind::DIFFUSE);

	primitives[5] = new Triangle(vec3(3, 2.5, 8), vec3(3, 4.5, 8), vec3(5, 2.5, 8));
	primitives[5]->material = Material(vec3(1, 1, 0), Material::MaterialKind::DIFFUSE);

	primitives[6] = new Triangle(vec3(-3, -2.5, 8), vec3(-3, -0.5, 8), vec3(-1, -2.5, 8));
	primitives[6]->material = Material(vec3(1, 1, 1), Material::MaterialKind::DIFFUSE);

	primitives[7] = new Triangle(vec3(0, -2.5, 8), vec3(0, -0.5, 8), vec3(2, -2.5, 8));
	primitives[7]->material = Material(vec3(1, 1, 1), Material::MaterialKind::DIFFUSE);

	primitives[8] = new Triangle(vec3(3, -2.5, 8), vec3(3, -0.5, 8), vec3(5, -2.5, 8));
	primitives[8]->material = Material(vec3(1, 1, 1), Material::MaterialKind::DIFFUSE);

#elif OBJ_LOAD

	lights[0] = new Light(vec3(-3, 2, -3), vec3(70, 70, 70));
	lights[1] = new Light(vec3(3, -3, -5), vec3(70, 70, 70));

#if BUNNY_LOAD
	string inputfile = "bunny.obj";
#elif SUZANNE_LOAD
	string inputfile = "suzanne.obj";
#elif f16_LOAD
	string inputfile = "f16.obj";
#endif

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	// Loop over shapes

	int counter = 0;

	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			int fv = shapes[s].mesh.num_face_vertices[f];

			vec3 vertices[3];
			vec3 normal;
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				float vx = attrib.vertices[3 * idx.vertex_index + 0];
				float vy = -attrib.vertices[3 * idx.vertex_index + 1];
				float vz = -attrib.vertices[3 * idx.vertex_index + 2];
				float nx = attrib.normals[3 * idx.normal_index + 0];
				float ny = attrib.normals[3 * idx.normal_index + 1];
				float nz = attrib.normals[3 * idx.normal_index + 2];

				vertices[v] = vec3(vx, vy, vz);
				normal += vec3(nx, ny, nz);
			}
			index_offset += fv;

			Triangle* triangle = new Triangle(vertices[0], vertices[1], vertices[2]);
			//triangle->normal = vec3(normal.x / 3, normal.y / 3, normal.z / 3);
			primitives[counter] = triangle;

			counter++;
		}
	}

	Triangle* tri1 = new Triangle(vec3(8, -8, 8), vec3(-8, -8, 8), vec3(-8, 8, 8));
	tri1->material = Material(vec3(1, 1, 1), Material::MaterialKind::DIFFUSE);
	primitives[counter++] = tri1;

	Triangle* tri2 = new Triangle(vec3(8, -8, 8), vec3(-8, 8, 8), vec3(8, 8, 8));
	tri2->material = Material(vec3(1, 1, 1), Material::MaterialKind::DIFFUSE);
	primitives[counter++] = tri2;

#endif
	sceneBounds = this->CalculateSceneBounds();
	bvh = new BVH(primitives, sizeof(this->primitives) / sizeof(this->primitives[0]));
}

AABB* Scene::CalculateSceneBounds()
{
	float maxX = -INFINITY;
	float maxY = -INFINITY;
	float maxZ = -INFINITY;

	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;

	for (int i = 0; i < sizeof(this->primitives) / sizeof(this->primitives[0]); i++) {
		AABB* currentAABB = primitives[i]->boundingBox;
		if (currentAABB->max.x >= maxX)
			maxX = currentAABB->max.x;
		if (currentAABB->max.y >= maxY)
			maxY = currentAABB->max.y;
		if (currentAABB->max.z >= maxZ)
			maxZ = currentAABB->max.z;
		if (currentAABB->min.x <= minX)
			minX = currentAABB->min.x;
		if (currentAABB->min.y <= minY)
			minY = currentAABB->min.y;
		if (currentAABB->min.z <= minZ)
			minZ = currentAABB->min.z;
	}

	return new AABB(vec3(minX, minY, minZ), vec3(maxX, maxY, maxZ));
}