#pragma once

#include "Materials.h"
#include "VectorMath.h"
#include "RayLogic_Hit.h"
#include <iostream>



struct Intersectable
{
	Material* material;
	virtual Hit intersect(const Ray& ray) = 0;
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

		float denom = dot(normal, v); //Nagyjaból 1 irányba néz a vektor! akkor JÓ!
		float t = -1;
		if (denom > 1e-6) 
		{
			vec3 p0l0 = position - eye;
			p0l0 = p0l0* -1;
			t = dot(p0l0, normal) / denom; // Ez nemtudom miért adja meg a távolságot.
			if (t < 0)
				return Hit(); // Nincs talalat
			Hit talalat;
			talalat.t = t;
			talalat.position = eye + v*t * -1;  
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
			vec3 normal = cross((r2 - r1), (r3 - r1)); //Normálvektor irány a körüljárási iránytól függ!
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
		if (material->isWater)
		{
			if (v.y > 0)
				return Hit();
		}
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
			if(bestHit.t != -1)
				if (t > bestHit.t) // Ha mar van jobb talalat
					continue;

			vec3 p = eye + v*t;

			vec3 crossa = cross((r2 - r1), (p - r1));
			float teszt = dot(crossa, normal);
			if (teszt > 0)
			{
				vec3 oldal = (r3 - r2);
				vec3 toPoint = (p - r2);
				crossa = cross(oldal,toPoint );
				teszt = dot(crossa, normal);
				if (teszt > 0)
				{
					crossa = cross((r4 - r3), (p - r3));
					teszt = dot(crossa, normal);
					if (teszt > 0)
					{
						crossa = cross((r1 - r4), (p - r4));
						teszt = dot(crossa, normal);
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
const float HULLAMNAGYSAGA = 0.5f;
class Water : public Intersectable
{

public:
	Rectanglef* VizHullamTeteje;
	Rectanglef* VizHullamAlja;
	Plane* plane;
	float HullamY;
	float x0, x1, z0, z1;
	Water(Rectanglef* VizHullamTeteje, Rectanglef* VizHullamAlja,Plane* plane,Material* material,vec3 balPos,vec3 jobbPos)
	{
		this->VizHullamAlja = VizHullamAlja;
		this->VizHullamTeteje = VizHullamTeteje;
		this->material = material;
		this->plane = plane;
		this->x0 = balPos.x;
		this->x1 = jobbPos.x;
		this->z0 = balPos.z;
		this->z1 = jobbPos.z;
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

		float X = p0x + vx*t;
		float Z = p0z + vz*t;
		float Y = (p0y + vy*t);

		///Valahogy ha nem az origóba van a hullám akkor ki kéne belõle vonni
		///Ez mar mukodik
		//float kiszamolt = HULLAMNAGYSAGA*((sin(X*X + Z*Z) / (1 + X*X + Z*Z))) - Y;
			float kiszamol_max1 = HULLAMNAGYSAGA*(sin((X - x1)*(X - x1) + (Z - z1)*(Z - z1)) / (1 + (X - x1)*(X - x1) + (Z - z1)*(Z - z1))) - Y;
			float kiszamol_max2 = HULLAMNAGYSAGA*(sin((X - x0)*(X - x0) + (Z - z0)*(Z - z0)) / (1 + (X - x0)*(X - x0) + (Z - z0)*(Z - z0))) - Y;
			float maxer = max(kiszamol_max1, kiszamol_max2);
		//Csak egy hullam
		/*return HULLAMNAGYSAGA*cos(sqrt((2*X - x0)*(2*X - x0) + (2*Z - z0)*(2*Z - z0))) - Y;*/
		return maxer;
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
				//std::cout << "Iter kesz" << i << ": x = " << c << std::endl; ///TODO Kitorol csak debugra van itt
				return c;
				break;
			}
			i++;
		}
		return c;
	}
	Hit intersect(const Ray& ray)
	{
		if (material->isWater)
		{
			if (ray._nezetiIrany.y > 0) //Igy ha alulrol jon a sugar atengedi ( Medence aljáról így látjuk a medencét ) 
				return Hit();
		}
		//Elmeletileg ezzel a sorral csak azt jelenitene meg ami kivan vágva a síkból.
		Hit sikTalal = plane->intersect(ray);
		if (sikTalal.t > 0)  // Ha nincs talalat a sikkal, akkor a "MEDENCEBE " vagyok
			return Hit();
		Hit felso = VizHullamTeteje->intersect(ray);
		Hit also  = VizHullamAlja->intersect(ray);
		if (felso.t > 0 && also.t > 0)
		{
			Hit talalat;
			float fa = f(also.t,ray);
			float fb = f(felso.t,ray);
			if (fa* fb < 0)
			{
				float t = Reg(0.00001f, 100, felso.t, also.t, ray);
				talalat.t = t;

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

				float kiszamol_max1 = HULLAMNAGYSAGA*(sin((X - x1)*(X - x1) + (Z - z1)*(Z - z1)) / (1 + (X - x1)*(X - x1) + (Z - z1)*(Z - z1))) - Y;
				float kiszamol_max2 = HULLAMNAGYSAGA*(sin((X - x0)*(X - x0) + (Z - z0)*(Z - z0)) / (1 + (X - x0)*(X - x0) + (Z - z0)*(Z - z0))) - Y;
				float normalx;
				float normalZ;
				float normalY;
				//Csak 1 hullam
				/*normalx = (-2 * HULLAMNAGYSAGA* (x0 + 2 * X) * sin(sqrtf((x0 + 2 * X)*(x0 + 2 * X) + 4 * (z0 / 2 + Z)*(z0 / 2 + Z)))) / sqrt((x0 + 2 * X)*(x0 + 2 * X) + 4 * (z0 / 2 + Z)*(z0 / 2 + Z));
				normalZ = (-4 * HULLAMNAGYSAGA * (-z0 / 2 + Z)*sin(sqrtf((-x0 + 2 * X)*(-x0 + 2 * X) + 4 * (-z0 / 2 + Z)*(-z0 / 2 + Z)))) / sqrt((-x0 + 2 * X)*(-x0 + 2 * X) + 4 * (-z0 / 2 + Z)*(-z0 / 2 + Z));
				normalY = 1.0f;*/

				if (kiszamol_max1 > kiszamol_max2)
				{
					normalx = ((-x1 + X)* (HULLAMNAGYSAGA * 2 * (1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)) *cos((-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)) - HULLAMNAGYSAGA * 2 * sin((-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)))) / ((1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1))*(1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)));
					normalZ = ((Z - z1) * (HULLAMNAGYSAGA * 2 * (1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1))* cos((-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)) - HULLAMNAGYSAGA * 2 * sin((-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)))) / ((1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1))*(1 + (-x1 + X)*(-x1 + X) + (Z - z1)*(Z - z1)));
					normalY = 1.0f;
				}
				else
				{
					normalx = ((-x0 + X)* (HULLAMNAGYSAGA * 2 * (1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)) *cos((-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)) - HULLAMNAGYSAGA * 2 * sin((-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)))) / ((1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0))*(1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)));
					normalZ = ((Z - z0) * (HULLAMNAGYSAGA * 2 * (1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0))* cos((-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)) - HULLAMNAGYSAGA * 2 * sin((-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)))) / ((1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0))*(1 + (-x0 + X)*(-x0 + X) + (Z - z0)*(Z - z0)));
					normalY = 1.0f;
				}
				normalx = normalx * -1.0f;
				normalZ = normalZ * -1.0f;
				
				talalat.normal = vec3(normalx, normalY, normalZ);
				talalat.normal = talalat.normal.normalize();
				talalat.position = vec3(X, Y, Z);
			}
			else
				throw "Elrontott szamok Regula falsi";

			
			
			talalat.material = material;
			return talalat;
			//float normal = 0.2f*cos(t);

		}
		
		return Hit();
		//if (f(a) * f(b) < 0)
		//	Reg(aerr, maxitr, a, b);

	}

};