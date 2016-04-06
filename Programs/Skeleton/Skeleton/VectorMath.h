#pragma once

int sgn(float val) {
	return ((0.0f) < val) - (val < (0.0f));
}

struct vec4 {
	float v[4];

	vec4(float x = 0, float y = 0, float z = 0, float w = 1) {
		v[0] = x; v[1] = y; v[2] = z; v[3] = w;
	}
};

struct vec3
{
	float x, y, z;

	vec3(float x0 = 0, float y0 = 0, float z0 = 0) { x = x0; y = y0; z = z0; }
	vec3 operator*(float a) { return vec3(x * a, y * a, z * a); }

	vec3 operator+(const vec3& v) const {
		return vec3(x + v.x, y + v.y, z + v.z);
	}
	vec3 operator-(const vec3& v) const {
		return vec3(x - v.x, y - v.y, z - v.z);
	}
	vec3 operator*(const vec3& v) const {
		return vec3(x * v.x, y * v.y, z * v.z);
	}
	vec3 operator/(const vec3& v) const {
		return vec3(x / v.x, y / v.y, z / v.z);
	}
	vec3 operator/(const float oszto)
	{
		if (oszto == 0)
			throw "vec3::operator/() - 0 val osztas";
		return vec3(x / oszto, y / oszto, z / oszto);
	}
	float Length() { return sqrtf(x * x + y * y + z * z); }
	vec3 normalize()
	{
		float meret = Length();
		return vec3(x / meret, y / meret, z / meret);
	}
};

float dot(const vec3& v1, const vec3& v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

vec3 cross(const vec3& v1, const vec3& v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y,
		(v1.z * v2.x) - (v1.x * v2.z),
		v1.x * v2.y - v1.y * v2.x);
}
//FONTOS CSAK EGYSEG VEKTOROKKAL MUKODIK
//Visszaverõdési irány határozza meg!
//vec3 reflect(vec3 inDir, vec3 normal)
//{
//if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
//	throw "vec3::reflect() - Csak normalizalt vektorral mukodik a visszaverodes";
//	return inDir - normal * dot(normal, inDir) *2.0f;
//}

//FONTOS CSAK EGYSEG VEKTOROKKAL MUKODIK
//Törési irányt határozza meg!
///TODO SmoothMaterial class-ba belerakni.
//vec3 refract(vec3 inDir, vec3 normal) {
//	if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
//		throw "vec3::refract() - Csak normalizalt vektorral mukodik a visszaverodes";
//	float ior = n;
//	float cosa = -dot(normal, inDir);
//	if (cosa < 0) { cosa = -cosa; normal = -normal; ior = 1 / n; }
//	float disc = 1 - (1 - cosa * cosa) / ior / ior;
//	if (disc < 0) return reflect(inDir, normal);
//	return inDir / ior + normal * (cosa / ior - sqrt(disc));
//}