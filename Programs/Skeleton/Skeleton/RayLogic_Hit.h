#pragma once

#include "VectorMath.h"
#include "Materials.h"

struct Hit
{
	float t;
	vec3 position;
	vec3 normal;
	Material* material;
	Hit()
	{
		t = -1;
	}
};

struct Ray {
	vec3 _kozeppont; //eye al egyezik a kamerába
	vec3 _nezetiIrany;
	Ray(vec3 kozeppont, vec3 nezetiIrany)
	{
		_kozeppont = kozeppont;
		_nezetiIrany = nezetiIrany;
		_nezetiIrany = _nezetiIrany.normalize();
	}
	Ray() {}
};