#pragma once
#include <vector>
#include "AABB.h"
#include "Object.h"
struct BVHNode{
	AABB box;
	BVHNode *left;
	BVHNode *right;
	Object* obj;
	
	BVHNode():left(NULL), right(NULL),obj(NULL){}
	~BVHNode(){
		delete left;
		delete right;
	}
};

