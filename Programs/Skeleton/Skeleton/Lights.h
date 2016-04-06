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
	AmbientLight(vec3 NewLightColor)
	{
		lightType = TYPES::Ambient;
		LightColor = NewLightColor;
	}
	vec3 getDecrasedColor(vec3 intersectPos)
	{
		float tavolsagNegyzet = getSquareDist(intersectPos);

		vec3 csokkentettColor(LightColor.x / tavolsagNegyzet, LightColor.y / tavolsagNegyzet, LightColor.z / tavolsagNegyzet);
		return csokkentettColor;

	}
};

struct DirectionLight : public Light
{
	vec3 lightDirection;
	float mekkoraRadianbanLatszik;
	DirectionLight(vec3 NewLightColor, vec3 dir, vec3 pos)
	{
		lightType = TYPES::Direction;
		lightDirection = dir;
		LightColor = NewLightColor;
	}
	vec3 getDecrasedColor(vec3 intersectPos)
	{
		float tavolsagNegyzet = getSquareDist(intersectPos);

		vec3 csokkentettColor(LightColor.x / tavolsagNegyzet, LightColor.y / tavolsagNegyzet, LightColor.z / tavolsagNegyzet);
		return csokkentettColor;

	}
};