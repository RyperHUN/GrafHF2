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

	vec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	vec3(float x0, float y0, float z0) 
	{ x = x0; y = y0; z = z0; }
	vec3(const vec3&  v)
	{ x = v.x; y = v.y; z = v.z; }
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

// Utolsó oszlopba lesznek a dolgok tehát a Column Major formátumt használom + jobbrol kell vektorral szorozni
class mat4 {
public:
	float m[4][4];
	mat4(float m00 = 1, float m01 = 0, float m02 = 0, float m03 = 0,
		float m10 = 0, float m11 = 1, float m12 = 0, float m13 = 0,
		float m20 = 0, float m21 = 0, float m22 = 1, float m23 = 0,
		float m30 = 0, float m31 = 0, float m32 = 0, float m33 = 1) {
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	mat4 operator*(const mat4& right) {
		mat4 result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.m[i][j] = 0;
				for (int k = 0; k < 4; k++) result.m[i][j] += m[i][k] * right.m[k][j];
			}
		}
		return result;
	}
	void eltolas(float x = 0, float y = 0, float z = 0)
	{
		//Utolso oszlop beallitasa
		m[0][3] = x;
		m[1][3] = y;
		m[2][3] = z;
	}
	void projekcio(float x = 1, float y = 1, float z = 1)
	{
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
	}
	void forgatZ(float szogFok)
	{
		//Valamiért kell egy minusz különben az óramutatóval ellentétesen forgat
		float radian = -szogFok / 180 * M_PI;
		m[0][0] = cosf(radian);
		m[1][0] = sinf(radian);
		m[0][1] = -sinf(radian);
		m[1][1] = cosf(radian);
	}
	mat4 operator=(const mat4& right)
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = right.m[i][j];
			}
		}
		return (*this);
	}
	vec4 operator*(const vec4& right);
	operator float*() { return &m[0][0]; }
	vec3 operator*(const vec3& sima)
	{
		vec4 result;
		vec4 right;
		right.v[0] = sima.x;
		right.v[1] = sima.y;
		right.v[2] = sima.z;
		for (int i = 0; i < 4; i++) {
			result.v[i] = 0;
			for (int j = 0; j < 4; j++) {

				result.v[i] += m[i][j] * right.v[j];
			}
		}

		return vec3(result.v[0], result.v[1], result.v[2]);
	}
};