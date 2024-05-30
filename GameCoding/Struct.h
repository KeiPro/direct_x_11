#pragma once
#include "Types.h"

struct Vertex
{
	Vec3 position;
	//Color color;
	Vec2 uv;
};


struct TransformData
{
	Vec3 offset;
	float dummy; // constant버퍼를 만들 때에는 16바이트로 만들어져야 하기 때문에 더미 float추가.
};