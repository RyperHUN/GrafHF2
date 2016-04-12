#pragma once

#include "Materials.h"
#include "VectorMath.h"
#include "RayLogic_Hit.h"
#include <iostream>

const float HULLAMNAGYSAGA = 0.1f;

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

class Ellipsoid : public Intersectable
{
	vec3 center;
	vec3 size;
public:
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
	bool kivag;
	vector<Intersectable*> kivagniObjektumok;
	Plane(vec3 position, vec3 normal,Material * material)
	{
		///Egyellore csak (0,0,0) al mûködik
		this->position = position;
		this->normal = normal;
		this->material = material;
		this->kivag = false;
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
			if (kivag)
			{
				for (int i = 0; i < kivagniObjektumok.size(); i++)
				{
					Hit kivagTalalat = kivagniObjektumok[i]->intersect(ray);
					if (kivagTalalat.t > 0)
						return Hit();
				}
				return talalat; //Ha egyik objektumba sincs benne!
			}
			else
			{
				return talalat;
			}
		}
		return Hit();
	}

};

//Pontok megadasanal a koruljarasi irany a lenyeg. JOBBKEZSZABALY!
class Polygonf : public Intersectable
{
	vector<vec3> points;
	public:
	Polygonf(std::vector<vec3> points, Material* material)
	{
		this->points = points;
		this->material = material;
	}

	Hit intersect(const Ray& ray)
	{
		vec3 eye = ray._kozeppont;
		vec3 v = ray._nezetiIrany;

		Hit bestHit;
		for (int i = 0; i < points.size() / 3; i++)
		{
			vec3 r1 = points[i*3 + 0];
			vec3 r2 = points[i * 3 + 1];
			vec3 r3 = points[i * 3 + 2];
			vec3 normal = cross((r2 - r1), (r3 - r1)); ///Normálvektor irány mitõl függ??
			normal = normal.normalize();

			float t = dot((r1 - eye), normal) / dot(v, normal);

			if (t < 0)
				continue; // nincs talalat
			if (t < bestHit.t) // Ha mar van jobb talalat
				continue;

			vec3 p = eye + v*t;

			float teszt = dot(cross((r2 - r1), (p - r1)), normal);
			if (teszt > 0)
			{
				teszt = dot(cross((r3 - r2), (p - r2)), normal);
				if (teszt > 0)
				{
					teszt = dot(cross((r1 - r3), (p - r3)), normal);
					if (teszt > 0)
					{
						//Háromszögbe vagyunk
						Hit talalat;
						talalat.normal = normal.normalize();
						talalat.t = t;
						talalat.material = this->material;
						talalat.position = p;
						
						bestHit = talalat;
					}
				}
			}
		}
		return bestHit;
	}
	void eltol(vec3 eltolas)
	{
		mat4 eltolasMatrix;
		eltolasMatrix.eltolas(eltolas.x, eltolas.y, eltolas.z);
		for (int i = 0; i < points.size(); i++)
		{
			points[i] = eltolasMatrix * points[i];
		}
	}
	void skalaz(vec3 skalazo)
	{
		mat4 skalazMatrix;
		skalazMatrix.projekcio(skalazo.x, skalazo.y, skalazo.z);
		for (int i = 0; i < points.size(); i++)
		{
			points[i] = skalazMatrix * points[i];
		}
	}
};

class Rectanglef: public Intersectable
{
	
public:
	vector<vec3> points;
	Rectanglef(std::vector<vec3> points, Material* material)
	{
		this->points = points;
		this->material = material;
	}

	Hit intersect(const Ray& ray)
	{
		vec3 eye = ray._kozeppont;
		vec3 v = ray._nezetiIrany;

		Hit bestHit;
		for (int i = 0; i < points.size() / 4; i++)
		{
			vec3 r1 = points[i * 4 + 0];
			vec3 r2 = points[i * 4 + 1];
			vec3 r3 = points[i * 4 + 2];
			vec3 r4 = points[i * 4 + 3];
			vec3 normal = cross((r2 - r1), (r3 - r1));
			normal = normal.normalize();

			float t = dot((r1 - eye), normal) / dot(v, normal);

			if (t < 0)
				continue; // nincs talalat
			if (t < bestHit.t) // Ha mar van jobb talalat
				continue;

			vec3 p = eye + v*t;

			float teszt = dot(cross((r2 - r1), (p - r1)), normal);
			if (teszt > 0)
			{
				teszt = dot(cross((r3 - r2), (p - r2)), normal);
				if (teszt > 0)
				{
					teszt = dot(cross((r4 - r3), (p - r3)), normal);
					if (teszt > 0)
					{
						teszt = dot(cross((r1 - r4), (p - r4)), normal);
						if (teszt > 0)
						{
							//Háromszögbe vagyunk
							Hit talalat;
							talalat.normal = normal.normalize();
							talalat.t = t;
							talalat.material = this->material;
							talalat.position = p;

							bestHit = talalat;
						}
					}
				}
			}
		}
		return bestHit;
	}
	void eltol(vec3 eltolas)
	{
		mat4 eltolasMatrix;
		eltolasMatrix.eltolas(eltolas.x, eltolas.y, eltolas.z);
		for (int i = 0; i < points.size(); i++)
		{
			points[i] = eltolasMatrix * points[i];
		}
	}
	void skalaz(vec3 skalazo)
	{
		mat4 skalazMatrix;
		skalazMatrix.projekcio(skalazo.x, skalazo.y, skalazo.z);
		for (int i = 0; i < points.size(); i++)
		{
			points[i] = skalazMatrix * points[i];
		}
	}
};

class Water : public Intersectable
{

public:
	Rectanglef* VizHullamTeteje;
	Rectanglef* VizHullamAlja;
	float HullamY;
	Water(Rectanglef* VizHullamTeteje, Rectanglef* VizHullamAlja,Material* material)
	{
		this->VizHullamAlja = VizHullamAlja;
		this->VizHullamTeteje = VizHullamTeteje;
		this->material = material;
		//Igy azt a magassagot kapom meg, ami pont a 2 db SIK kozott van
		HullamY = VizHullamTeteje->points[0].y - HULLAMNAGYSAGA;
	}
	//Ez a hullam.
	float f(float t, const Ray& ray)
	{
		float p0x = ray._kozeppont.x;
		float p0y = ray._kozeppont.y;
		float p0z = ray._kozeppont.z;

		float vx = ray._nezetiIrany.x;
		float vy = ray._nezetiIrany.y;
		float vz = ray._nezetiIrany.z;

		float x0 = 0; // Hzllam kezdo pos
		float z0 = 0;

		float X = p0x + vx*t;
		float Z = p0z + vz*t;
		float Y = (p0y + vy*t);
		//return HULLAMNAGYSAGA*(cos(sqrt(powf((p0x + vx*t - x0),2) + powf((p0y + vy*t - z0), 2))) - p0z + vz*t) - HullamY;
		float kiszamolt = HULLAMNAGYSAGA*(cos(X + Z) - Y);
		kiszamolt = kiszamolt + HullamY;
		float kiszamolt2 = HULLAMNAGYSAGA*( (sin(X*X + Z*Z) / (1 + X*X + Z*Z) + sin((X - 5)*(X - 5) + Z*Z) / (1 + (X - 5)*(X - 5) + Z*Z) ) - Y);
		return kiszamolt2;
	}
	///TODO Regula falsit irni
	float Reg(double aerr, int maxitr, double a, double b, const Ray& ray)
	{
		int i = 0;
		double c = 0;
		double prev = 0;
		vec3 eredmeny;
		while (i <= maxitr)
		{
			if (f(a,ray) * f(b,ray) < 0)
			{
				c = a - ((b - a) / (f(b,ray) - f(a, ray)))*f(a, ray);
			}
			if (f(a, ray) * f(c, ray) < 0)
			{
				b = c; prev = a;
			}
			else
			{
				a = c; prev = b;
			}
			if (fabs(prev - c) < aerr)
			{
				std::cout << "Iter kesz" << i << ": x = " << c << std::endl; ///TODO Kitorol csak debugra van itt
				return c;
				break;
			}
			i++;
		}
	}
	Hit intersect(const Ray& ray)
	{
		Hit felso = VizHullamTeteje->intersect(ray);
		Hit also  = VizHullamAlja->intersect(ray);
		if (felso.t > 0 && also.t > 0)
		{
			Hit talalat;
			float fa = f(also.t,ray);
			float fb = f(felso.t,ray);
			if (fa* fb < 0)
				float t = Reg(0.0001f, 100, felso.t, also.t, ray);
			else
				return Hit();
				//throw "Elrontott szamok Regula falsi";
			//float normal = 0.2f*cos(t);

		}
		
		return Hit();
		//if (f(a) * f(b) < 0)
		//	Reg(aerr, maxitr, a, b);

	}

};