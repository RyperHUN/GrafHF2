#pragma once

#include "Types.h"

struct Material
{
	bool isReflect;
	bool isRefract;
	///TODO irni konstruktort ami alapbol mindent 0 ba rak
	Material()
	{
		isReflect = false;
		isRefract = false;
	}
	TYPES::Material materialType;
	vec3 F0;  // F0 = ( (n-1)^2 + k^2 ) / ( (n+1)^2 + k^2 )
			  //vec3 n;  //Femeknel meg uvegnel nem nulla egyebkent szinte mindig 0 meg ebbol adodoan az F0 konstans

	vec3 ka;
	//CSAK ROUGH MATERIALNAL AZ ALSOK
	vec3 kd, ks;  // Ks - anyag szine ! , Ks - spekularis resze, ami visszaverodik
	float shininess;
	//IDAIG
	vec3 color;

	bool isReflective() { return isReflect; }
	bool isRefractive() { return isRefract; }

	vec3 reflect(vec3 &inDir, vec3 &normal)
	{
		inDir = inDir.normalize();
		normal = normal.normalize();
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
			throw "vec3::reflect() - Csak normalizalt vektorral mukodik a visszaverodes";

		return inDir - normal * (dot(normal, inDir) * 2.0f);
	}
	vec3 refract(vec3 &inDir, vec3 &normal) { return vec3(0, 0, 0); }

	virtual void calcF0() {}

	vec3 Fresnel(vec3 inDir, vec3 normal)
	{
		float cosa = fabs(dot(normal, inDir));
		return F0 + (vec3(1, 1, 1) - F0) * pow(1 - cosa, 5);
	}
	//Ezis rough materialnak kell
	vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir,
		vec3 inRad)
	{
		vec3 reflRad(0, 0, 0);
		float cosTheta = dot(normal, lightDir);
		if (cosTheta < 0)
			return reflRad;
		reflRad = inRad * kd * cosTheta;
		vec3 halfway = (viewDir + lightDir).normalize();
		float cosDelta = dot(normal, halfway);
		if (cosDelta < 0)
			return reflRad;
		return reflRad + inRad * ks * pow(cosDelta, shininess);
	}
};

//Reflection mindig kell!
//Refraction nem mindig pl: Arany ezust NEM, de viznel igen
//Itt a szín a Fresnel egyenletbõl lesz!!!!!!!!!!!!!!!!!!!!!!

//Ennek nem kene diffuz meg spekularis komponens!
class SmoothMaterial : public Material {
	//vec3   F0;	// - Base class ba van!
	vec3 n;	// n
	vec3 k; // Ezt igazabol nem kotelezo eltarolni
			///TODO ka attributum megadna a szinet ha ambiens feny megvilagitja
public:
	SmoothMaterial(vec3 nToresmutato, vec3 kKioltasiTenyezo, bool isReflect, bool isRefract)
	{
		this->materialType = TYPES::Smooth;
		this->isReflect = isReflect;
		this->isRefract = isRefract;
		this->n = nToresmutato;
		this->k = kKioltasiTenyezo;
		calcF0();
	}
	vec3 reflect(vec3 inDir, vec3 normal) {
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
		{
			inDir = inDir.normalize();
			normal = normal.normalize();
		}

		return inDir - normal * dot(normal, inDir) * 2.0f;
	}
	///TODO egyellore csak 1 n el mûködik
	vec3 refract(vec3 inDir, vec3 normal) {
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
		{
			inDir = inDir.normalize();
			normal = normal.normalize();
		}
		float ior = n.x;
		float cosa = -dot(normal, inDir);
		if (cosa < 0) 
		{ 
			cosa = -cosa;
			normal = normal * -1.0f;
			ior = 1 / ior;
		}
		float disc = 1 - (1 - cosa * cosa) / ior / ior;
		if (disc < 0) return reflect(inDir, normal);
		return inDir / ior + normal * (cosa / ior - sqrtf(disc));
	}

	vec3 Fresnel(vec3 inDir, vec3 normal) {
		inDir = inDir.normalize();
		normal = normal.normalize();

		float cosa = fabs(dot(normal, inDir));
		return F0 + (vec3(1, 1, 1) - F0) * pow(1 - cosa, 5);
	}
	//A megadott adatokkal szamolja ki
	void calcF0()
	{
		float r = (powf(n.x - 1, 2) + powf(k.x, 2)) / (powf(n.x + 1, 2) + powf(k.x, 2));
		float g = (powf(n.y - 1, 2) + powf(k.y, 2)) / (powf(n.y + 1, 2) + powf(k.y, 2));
		float b = (powf(n.z - 1, 2) + powf(k.z, 2)) / (powf(n.z + 1, 2) + powf(k.z, 2));
		F0 = vec3(r, g, b);
	}
};

///TODO
//Szin Phong-Bling modellbol lesz 
//ka hozzadodik a komponens, akkoris ha árnyékban van
//diffuz és spekulárishoz pedig kellenek shadowRayek
class RoughMaterial : public Material {
	float n, k;
	
public:
	RoughMaterial(vec3 ka,vec3 kd, vec3 ks, float shininess, bool isReflect, bool isRefract)
	{
		this->materialType = TYPES::Rough;
		this->isReflect = isReflect;
		this->isRefract = isRefract;
		n = 0;
		k = 0;
		this->kd = kd;
		this->ks= ks;
		this->ka = ka;

		this->shininess = shininess;
	}
	vec3 shade(vec3 normal, vec3 viewDir, vec3 lightDir,
		vec3 inRad)
	{
		vec3 reflRad(0, 0, 0);
		float cosTheta = dot(normal, lightDir);
		if (cosTheta < 0)
			return reflRad;
		reflRad = inRad * kd * cosTheta;
		vec3 halfway = (viewDir + lightDir).normalize();
		float cosDelta = dot(normal, halfway);
		if (cosDelta < 0)
			return reflRad;
		return reflRad + inRad * ks * pow(cosDelta, shininess);
	}
};