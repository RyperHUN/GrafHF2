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
	vec3 kd, ks;
	float shininess;
	//IDAIG
	vec3 color;

	bool isReflective() { return isReflect; }
	bool isRefractive() { return isRefract; }
	//void calcF0(float k)
	//{
	//	F0 = calcF0(n, k);
	//}
	//vec3 calcF0(float nToresmutato, float k)
	//{
	//	n = nToresmutato;
	//	float f0 = (powf(n - 1, 2) + powf(k, 2)) / (powf(n + 1, 2) + powf(k, 2));
	//	F0 = vec3(f0, f0, f0);
	//	return F0;
	//}


	vec3 reflect(vec3 &inDir, vec3 &normal)
	{
		inDir = inDir.normalize();
		normal = normal.normalize();
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
			throw "vec3::reflect() - Csak normalizalt vektorral mukodik a visszaverodes";

		return inDir - normal * (dot(normal, inDir) * 2.0f);
	}
	///TODO itt n már nem jó
	//vec3 refract(vec3 inDir, vec3 normal)
	//{
	//	if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
	//		throw "vec3::refract() - Csak normalizalt vektorral mukodik a visszaverodes";
	virtual void calcF0() {}
	virtual vec3 calcF0(const vec3& nToresmutato, const vec3& k) { return vec3(); }
	//	float ior = n;
	//	float cosa = -dot(normal, inDir);
	//	if (cosa < 0)
	//	{
	//		cosa = -cosa;
	//		//normal = -normal; // "-normal" kell ide
	//		normal = vec3(-normal.x, -normal.y, -normal.z);
	//		ior = 1 / n;
	//	}
	//	float disc = 1 - (1 - cosa * cosa) / ior / ior;
	//	if (disc < 0) return reflect(inDir, normal);
	//	return inDir / ior + normal * (cosa / ior - sqrt(disc));
	//}
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
	SmoothMaterial(vec3 nToresmutato, vec3 kKioltasiTenyezo)
	{
		materialType = TYPES::Smooth;
		n = nToresmutato;
		k = kKioltasiTenyezo;
		calcF0();
	}
	vec3 reflect(vec3 inDir, vec3 normal) {
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
			throw "vec3::reflect() - Csak normalizalt vektorral mukodik a visszaverodes";

		return inDir - normal * dot(normal, inDir) * 2.0f;
	}
	///TODO at kene irni hogy mukodjon vec3 n el! bár lehet nincs rá szükség vízhez kell majd
	//vec3 refract(vec3 inDir, vec3 normal) {
	//	if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
	//		throw "vec3::refract() - Csak normalizalt vektorral mukodik a visszaverodes";

	//	float ior = n;
	//	float cosa = -dot(normal, inDir);
	//	if (cosa < 0)
	//	{
	//		cosa = -cosa;
	//		//normal = -normal; // "-normal" kell ide
	//		normal = vec3(-normal.x, -normal.y, -normal.z);
	//		ior = 1 / n;
	//	}
	//	float disc = 1 - (1 - cosa * cosa) / ior / ior;
	//	if (disc < 0) return reflect(inDir, normal);
	//	return inDir / ior + normal * (cosa / ior - sqrt(disc));
	//}
	vec3 Fresnel(vec3 inDir, vec3 normal) {
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
	vec3 calcF0(const vec3& nToresmutato, const vec3& k) override
	{
		n = nToresmutato;
		this->k = k;
		float r = (powf(n.x - 1, 2) + powf(k.x, 2)) / (powf(n.x + 1, 2) + powf(k.x, 2));
		float g = (powf(n.y - 1, 2) + powf(k.y, 2)) / (powf(n.y + 1, 2) + powf(k.y, 2));
		float b = (powf(n.z - 1, 2) + powf(k.z, 2)) / (powf(n.z + 1, 2) + powf(k.z, 2));
		F0 = vec3(r, g, b);
		return F0;
	}
};

///TODO
//Szin Phong-Bling modellbol lesz 
//ka hozzadodik a komponens, akkoris ha árnyékban van
//diffuz és spekulárishoz pedig kellenek shadowRayek
class RoughMaterial {
	vec3 kd, ks;
	float  shininess;
public:
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