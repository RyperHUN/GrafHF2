#pragma once

#include "Materials.h"
#include "VectorMath.h"
#include "RayLogic_Hit.h"

struct Intersectable
{
	Material* material;
	virtual Hit intersect(const Ray& ray) = 0;
};

class Sphere : public Intersectable {
	vec3 center;
	float radius;
public:
	Sphere(float x, float y, float z, float r)
		: center(x, y, z), radius(r)
	{

	}
	
	//Hit intersect(const Ray& ray)
	//{
	//	Hit talalat;
	//	vec3 eye = ray._kozeppont;
	//	vec3 v = ray._nezetiIrany;
	//	//v = v * -1.0f;

	//	///TODO Rakerdezni
	//	//Kovetkezo feltetelezessel elve: eye = ray._kozeppont, v = ray._nezetiIrany
	//	float a = (dot(v, v));
	//	float b = 2 * (dot((eye - center), v));
	//	float c = dot((eye - center), (eye - center)) - radius*radius;
	//	//Masodfoku egyenlet parameterei
	//	float diszkriminans = b*b - 4 * a * c;
	//	if (diszkriminans < 0) // nincs talalat
	//		return talalat; //Defaultbol -1 tehat nincs talalat

	//	float t1 = (-b + sqrtf(diszkriminans)) / (2 * a);
	//	float t2 = (-b - sqrtf(diszkriminans)) / (2 * a);
	//	///TODO x1, x2 itt mi lesz???? - Talan a tavolsag a szemtol a metszespontal
	//	///TODO HIT.Position.
	//	talalat.material = this->material;
	//	//v = v * -1;
	//	if (t1 > t2 && t1 > 0)
	//	{
	//		talalat.t = t1;
	//		talalat.position = eye + v  * talalat.t;
	//		talalat.normal = (talalat.position - center) / radius;
	//		talalat.normal = talalat.normal.normalize();
	//		return talalat;
	//	}
	//	else if( t2 > 0)
	//	{
	//		talalat.t = t2;
	//		talalat.position = eye + v * talalat.t;
	//		talalat.normal = (talalat.position - center) / radius;
	//		talalat.normal = talalat.normal.normalize();
	//		return talalat;
	//	}

	//}
	
	Hit intersect(const Ray& ray)
	{
		vec3 eye = ray._kozeppont;
		vec3 v = ray._nezetiIrany;
		//v = v * -1.0f;

		float a = dot(v, v);
		float b = 2 * dot(eye - center, v);
		float c = dot(eye - center, eye - center) - radius* radius;

		float det = (b*b) - (4 * a *c);
		if (det < 0)
			return Hit();

		Hit hit;
		float t1 = (-b - sqrt(det)) / (2 * a);
		float t2 = (+b + sqrt(det)) / (2 * a);
		if (t1 > 0 && t1 > t2)
			hit.t = t1;
		else
		{
			if (t2 < 0) return Hit();
			hit.t = t2;
		}
		
		hit.position = eye + v*hit.t;
		if (hit.position.y > 0.0f)
			hit.normal.y = 0.0f; // csak hogy legyen break point amikor ütközik anormal gömbel
		hit.normal = (hit.position - center) / radius;
		hit.normal = hit.normal.normalize();
		hit.material = material;
		return hit;
	}

	/* Kopari
	Hit intersect(const Ray& ray)
	{
		Hit h;
		vec3 L = center - ray._kozeppont;
		float tca = dot(L, ray._nezetiIrany);
		if (tca < 0)
			h.t = -1;
		float d2 = L.Length() - tca * tca;
		if (d2 > radius*radius)
			h.t = -1;
		float thc = sqrt(radius*radius - d2);
		float x1 = tca - thc;
		float x2 = tca + thc;

		if (x2 < 0 && x1 >= 0) h.t = x2;
		else if (x1 < 0 && x2 >= 0) h.t = x2;
		else if (x1 >= 0 && x1 < x2) h.t = x1;
		else if (x2 >= 0 && x2 < x1) h.t = x2;
		else
		{
			h.t = -1; return h;
		}
		h.position = ray._kozeppont + ray._nezetiIrany * -1 * h.t;
		h.normal = (h.position - center) / radius;
		h.material = this->material;
		return h;
	}
	*/
	//Hit intersect(const Ray& ray)
	//{
	//	vec3 eye = ray._kozeppont;
	//	vec3 v = ray._nezetiIrany*(-1.0f);

	//	float a = dot(v, v);
	//	float b = 2 * dot(eye - center, v);
	//	float c = dot(eye - center, eye - center) - radius* radius;

	//	float det = b*b - 4 * a *c;
	//	if (det < 0)
	//		return Hit();

	//	Hit hit;
	//	float t = (-b - sqrt(det)) / (2 * a);
	//	if (t > 0)
	//		hit.t = t;
	//	else
	//	{
	//		t = (+b + sqrt(det)) / (2 * a);
	//		if (t < 0) return Hit();
	//		hit.t = t;
	//	}
	//	hit.position = eye + v*hit.t;
	//	hit.normal = (hit.position - center) / radius;
	//	hit.material = material;
	//	return hit;
	//}

};

class Ellipsoid : Intersectable
{
	vec3 center;
	vec3 size;
	Ellipsoid(vec3 center, vec3 size,Material* material)
		: center(center),size(size)
	{
		this->material = material;
	}
	Hit intersect(const Ray& ray)
	{
		float EPSILON = 0; ///TODO IDE LEHET MAS KENE
		vec3 O_C = ray._kozeppont - center;
		vec3 dir = ray._nezetiIrany;
		dir = dir.normalize();

		float a =
			((dir.x*dir.x) / (size.x*size.x))
			+ ((dir.y*dir.y) / (size.y*size.y))
			+ ((dir.z*dir.z) / (size.z*size.z));
		float b =
			((2.f*O_C.x*dir.x) / (size.x*size.x))
			+ ((2.f*O_C.y*dir.y) / (size.y*size.y))
			+ ((2.f*O_C.z*dir.z) / (size.z*size.z));
		float c =
			((O_C.x*O_C.x) / (size.x*size.x))
			+ ((O_C.y*O_C.y) / (size.y*size.y))
			+ ((O_C.z*O_C.z) / (size.z*size.z))
			- 1.f;

		float d = ((b*b) - (4.f*a*c));
		if (d < 0.f || a == 0.f || b == 0.f || c == 0.f)
			return Hit();

		d = sqrt(d);

		float t1 = (-b + d) / (2.f*a);
		float t2 = (-b - d) / (2.f*a);

		if (t1 <= EPSILON && t2 <= EPSILON) return Hit(); // both intersections are behind the ray origin
		bool back = (t1 <= EPSILON || t2 <= EPSILON); // If only one intersection (t>0) then we are inside the ellipsoid and the intersection is at the back of the ellipsoid
		float t = 0.f;
		if (t1 <= EPSILON)
			t = t2;
		else
			if (t2 <= EPSILON)
				t = t1;
			else
				t = (t1 < t2) ? t1 : t2;

		if (t < EPSILON) return Hit(); // Too close to intersection

		Hit talalat;
		talalat.position = ray._kozeppont + dir*t;
		talalat.t = t;
		vec3 normal = talalat.position - center;
		normal.x = 2.f*normal.x / (size.x*size.x);
		normal.y = 2.f*normal.y / (size.y*size.y);
		normal.z = 2.f*normal.z / (size.z*size.z);

		normal = (back) ? normal * -1.f : normal * 1.f;
		normal = normal.normalize();
		
		talalat.normal = normal;
		talalat.material = this->material;
		
		return talalat;
		///TODO
		/// http://cudaopencl.blogspot.hu/2012/12/ellipsoids-finally-added-to-ray-tracing.html
	}
};

class Plane : public Intersectable
{
public: 
	vec3 normal;
	vec3 position;
	Plane(vec3 position, vec3 normal,Material * material)
	{
		///Egyellore csak (0,0,0) al mûködik
		this->position = position;
		this->normal = normal;
		this->material = material;
	}
	/*
		Mi a francért nem jó???
	*/
	Hit intersect(const Ray& ray)
	{
		vec3 eye = ray._kozeppont;
		vec3 v = ray._nezetiIrany; // -1???
		v = v * (-1.0f);

		float denom = dot(normal, v);
		float t = -1;
		if (denom > 1e-6) 
		{
			vec3 p0l0 = position - eye;
			p0l0 = p0l0* -1;
			t = dot(p0l0, normal) / denom;
			if (t < 0)
				return Hit(); // Nincs talalat
			Hit talalat;
			talalat.t = t;
			talalat.position = eye + v*t * -1;  ///TODO Position nem mûködik
			talalat.normal = normal;  
			talalat.material = material;
			return talalat;
		}
		return Hit();
	}

};

///TODO teglalap vagy sik
