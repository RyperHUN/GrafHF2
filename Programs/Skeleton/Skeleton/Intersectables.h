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
	Hit intersect(const Ray& ray)
	{
		Hit talalat;

		///TODO Rakerdezni
		//Kovetkezo feltetelezessel elve: eye = ray._kozeppont, v = ray._nezetiIrany
		float a = (dot(ray._nezetiIrany, ray._nezetiIrany));
		float b = 2 * (dot((ray._kozeppont - center), ray._nezetiIrany));
		float c = dot((ray._kozeppont - center), (ray._kozeppont - center)) - powf(radius, 2);
		//Masodfoku egyenlet parameterei
		float diszkriminans = powf(b, 2) - 4 * a * c;
		if (diszkriminans < 0) // nincs talalat
			return talalat; //Defaultbol -1 tehat nincs talalat

							//Normál vektor beállítása!


		float t1 = (-b + sqrtf(diszkriminans)) / (2 * a);
		float t2 = (+b + sqrtf(diszkriminans)) / (2 * a);
		///TODO x1, x2 itt mi lesz???? - Talan a tavolsag a szemtol a metszespontal
		///TODO HIT.Position.
		talalat.material = this->material;
		if (t1 > t2 && t1 > 0)
		{
			talalat.t = t1;
			talalat.position = ray._kozeppont + ray._nezetiIrany * talalat.t;
			talalat.normal = (talalat.position - center) / radius;
			talalat.normal = talalat.normal.normalize();
			return talalat;
		}
		else if( t2 > 0)
		{
			talalat.t = t2;
			talalat.position = ray._kozeppont + ray._nezetiIrany * talalat.t;
			talalat.normal = (talalat.position - center) / radius;
			talalat.normal = talalat.normal.normalize();
			return talalat;
		}

	}
};

///TODO teglalap vagy sik
