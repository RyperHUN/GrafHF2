#pragma once

#include "Types.h"



struct Light
{
	TYPES::Light lightType; //Ambiens vagy irány fényforrás
	vec3 position;
	//float Lout; // Nem biztos hogy float
	float intensity;
	vec3 LightColor;
	vec3 getLightDir(); ///TODO
	vec3 getInRad(); ///TODO
	float getSquareDist(vec3 intersectPos)
	{
		vec3 tavolsagVec = position - intersectPos;
		float tavolsag = tavolsagVec.Length();
		return powf(tavolsag, 2);
	}
	Light() {}
};

struct AmbientLight : public Light
{
	AmbientLight(vec3 NewLightColor,float intensity)
	{
		lightType = TYPES::Ambient;
		LightColor = NewLightColor;
		this->intensity = intensity;
	}
};

struct DirectionLight : public Light
{
	vec3 lightDirection;
	float mekkoraRadianbanLatszik;
	DirectionLight(vec3 NewLightColor, vec3 dir, vec3 pos,float intensity)
	{
		lightType = TYPES::Direction;
		this->position = pos;
		lightDirection = dir;
		LightColor = NewLightColor;
		this->intensity = intensity;
	}
	float getInRad()
	{
		return intensity;
	}
};

float magic_intensity = 0.2f;

DirectionLight nap(vec3(1, 1, 1), vec3(0, 0, 0), vec3(10, 10, 1),magic_intensity*10);

AmbientLight ambiensFeny(vec3(1.0f, 1.0f, 1.0f),magic_intensity); // Igy elmeletileg feher lesz