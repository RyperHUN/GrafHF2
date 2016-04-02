//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2016-tol.
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk.
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kivéve
// - new operatort hivni a lefoglalt adat korrekt felszabaditasa nelkul
// - felesleges programsorokat a beadott programban hagyni
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : 
// Neptun : 
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(__APPLE__)
#include <GLUT/GLUT.h>
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#else
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
#endif
#include <GL/glew.h>		// must be downloaded 
#include <GL/freeglut.h>	// must be downloaded unless you have an Apple
#endif

#include <vector>
const unsigned int windowWidth = 600, windowHeight = 600;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
using namespace std;
// OpenGL major and minor versions
int majorVersion = 3, minorVersion = 0;

void getErrorInfo(unsigned int handle) {
	int logLen;
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 0) {
		char * log = new char[logLen];
		int written;
		glGetShaderInfoLog(handle, logLen, &written, log);
		printf("Shader log:\n%s", log);
		delete log;
	}
}

// check if shader could be compiled
void checkShader(unsigned int shader, char * message) {
	int OK;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &OK);
	if (!OK) {
		printf("%s!\n", message);
		getErrorInfo(shader);
	}
}

// check if shader could be linked
void checkLinking(unsigned int program) {
	int OK;
	glGetProgramiv(program, GL_LINK_STATUS, &OK);
	if (!OK) {
		printf("Failed to link shader program!\n");
		getErrorInfo(program);
	}
}

// vertex shader in GLSL
const char *vertexSource = R"(
	#version 130
    precision highp float;

		in vec2 vertexPosition;		// variable input from Attrib Array selected by glBindAttribLocation
	out vec2 texcoord;			// output attribute: texture coordinate

		void main() {
		texcoord = (vertexPosition + vec2(1, 1))/2;							// -1,1 to 0,1
		gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0, 1); 		// transform to clipping space
	}
)";

// fragment shader in GLSL
const char *fragmentSource = R"(
	#version 130
    precision highp float;

		uniform sampler2D textureUnit;
	in  vec2 texcoord;			// interpolated texture coordinates
	out vec4 fragmentColor;		// output that goes to the raster memory as told by glBindFragDataLocation

		void main() {
		fragmentColor = texture(textureUnit, texcoord); 
	}
)";
//===================================================== INNENTOL VAN ERDEKES DOLOG =================================================//


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
		        v1.x * v2.y - v1.y * v2.x );
}
//FONTOS CSAK EGYSEG VEKTOROKKAL MUKODIK
//Visszaverődési irány határozza meg!
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



namespace TYPES
{
	enum Light
	{
		Ambient, Direction
	};
	enum Material
	{
		Rough, Smooth
	};
}

struct Material 
{
	///TODO irni konstruktort ami alapbol mindent 0 ba rak
	TYPES::Material materialType;
	vec3 F0;  // F0 = ( (n-1)^2 + k^2 ) / ( (n+1)^2 + k^2 )
	//vec3 n;  //Femeknel meg uvegnel nem nulla egyebkent szinte mindig 0 meg ebbol adodoan az F0 konstans
	  
	//CSAK ROUGH MATERIALNAL AZ ALSOK
	vec3 kd,ks; 
	float shininess; 
	//IDAIG
	vec3 color;

	bool isReflective();
	bool isRefractive();
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
	

	vec3 reflect(vec3 inDir, vec3 normal) 
	{
		if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
			throw "vec3::reflect() - Csak normalizalt vektorral mukodik a visszaverodes";

		return inDir - normal * dot(normal, inDir) * 2.0f;
	}
	///TODO itt n már nem jó
	//vec3 refract(vec3 inDir, vec3 normal)
	//{
	//	if (inDir.Length() > 1.1f || normal.Length() > 1.1f)
	//		throw "vec3::refract() - Csak normalizalt vektorral mukodik a visszaverodes";
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
//Itt a szín a Fresnel egyenletből lesz!!!!!!!!!!!!!!!!!!!!!!

//Ennek nem kene diffuz meg spekularis komponens!
class SmoothMaterial : public Material{
	//vec3   F0;	// - Base class ba van!
	vec3 n;	// n
	vec3 k; // Ezt igazabol nem kotelezo eltarolni
public:
	SmoothMaterial(vec3 nToresmutato, vec3 kKioltasiTenyezo)
	{
		materialType = TYPES::Smooth;
		n = nToresmutato;
		k = kKioltasiTenyezo;
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

struct Ray {
	vec3 _kozeppont; //eye al egyezik a kamerába
	vec3 _nezetiIrany;
	Ray(vec3 kozeppont, vec3 nezetiIrany)
	{
		_kozeppont = kozeppont;
		_nezetiIrany = nezetiIrany;
	}
	Ray() {}
};


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
	DirectionLight(vec3 NewLightColor,vec3 dir,vec3 pos)
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

DirectionLight nap(vec3(1, 1, 1), vec3(0, 0, 0), vec3(20, 0, 0));
AmbientLight ambiensFeny(vec3(1, 1, 1)); // Igy elmeletileg feher lesz

struct Camera
{
	vec3 eyePosition; // Szem pozició, ahonnan a kamera néz???
	vec3 planePosition; //Megmondja merre nez a kamera, vagyis hogy mennyire van messze a "vaszon" amin a pixelek vannak
	vec3 up;
	vec3 right;

	//TODO X-Y koordinátát beadsz neki ( Pixel pont a képernyőn, 600x600 ason)
	//Es megmondja hogy merre nez a sugar!
	Ray GetRay(int X, int Y) ///TODO
	{
		vec3 p = planePosition + right*(2 * float(X) / windowWidth - 1) + up*(2 * float(Y) / windowHeight - 1);
		Ray sugar;
		sugar._nezetiIrany = p.normalize();
		sugar._kozeppont = eyePosition;
		
		return sugar;
	}
};
struct Hit;
struct Intersectable
{
	Material* material;
	virtual Hit intersect(const Ray& ray) = 0;
};
vector<Intersectable*> objects;
struct Hit
{
	float t;
	vec3 position;
	vec3 normal;
	Material* material;
	Hit()
	{
		t = -1;
	}
	//Hit(Hit& hit)
	//{
	//	t = hit.t;
	//	position = hit.position;
	//	normal = hit.normal;
	//	material = hit.material;
	//}
};

Hit firstIntersect(Ray ray) {
	Hit bestHit; ///TODO normál vektor kiszámítása
	for (unsigned i = 0; i < objects.size(); i++)
	{
		Intersectable * obj = objects[i];
		Hit hit = obj->intersect(ray); //  hit.t < 0 if no intersection
		if (hit.t > 0 && (bestHit.t < 0 || hit.t < bestHit.t)) 
			bestHit = hit;
	}
	return bestHit;
}

class Sphere : public Intersectable {
	vec3 center;
	float radius;
public:
	Sphere(float x, float y, float z, float r)
		: center(x, y, z), radius(r)
	{
		material = new Material(); ///TODO felszabaditani
		
		//CSAK ROUGH MATERIALNAL AZ ALSOK

	}
	Hit intersect(const Ray& ray)
	{
		Hit talalat;
		///TODO Rakerdezni
		//Kovetkezo feltetelezessel elve: eye = ray._kozeppont, v = ray._nezetiIrany
		float a = (dot(ray._nezetiIrany,ray._nezetiIrany));
		float b = 2*(dot((ray._kozeppont - center), ray._nezetiIrany));
		float c = dot((ray._kozeppont - center), (ray._kozeppont - center)) - powf(radius, 2);
		//Masodfoku egyenlet parameterei
		float diszkriminans = powf(b, 2) - 4 * a * c;
		if (diszkriminans < 0) // nincs talalat
			return talalat; //Defaultbol -1 tehat nincs talalat
		
		float x1 = (-b + sqrtf(diszkriminans)) / (2 * a);
		float x2 = (+b + sqrtf(diszkriminans)) / (2 * a);
		///TODO x1, x2 itt mi lesz???? - Talan a tavolsag a szemtol a metszespontal
		///TODO HIT.Position.
		talalat.material = this->material;
		if (x1 > x2)
		{
			talalat.t = x1;
			talalat.position = ray._kozeppont + ray._nezetiIrany * talalat.t;
			return talalat;
		}
		else
		{
			talalat.t = x2;
			talalat.position = ray._kozeppont + ray._nezetiIrany * talalat.t;
			return talalat;
		}

	}
};

vec3 trace(Ray ray) {

	Hit hit = firstIntersect(ray); //Milyen objektum van legkozelebb
	if (hit.t < 0)  ///TODO azzal a feltetelezessel elve hogy La = ambiensFeny szine
		return ambiensFeny.LightColor; // nothing  //Ambiens fenyt fogja visszaadni.
	vec3 outRadiance = hit.material->color * ambiensFeny.LightColor;
	///TODO Kovetkezo sorba be van epitve hogy gyengul a fenye a tavolsaggal!
	//vec3 outRadiance = hit.material->color * ambiensFeny.getDecrasedColor(hit.position);

	//for (each light source l) {
	//	///TODO ez csak arnyeknak kell
	//	///TODO ShadowRay nel a dian megfelelo iranyba menjen a dolog ( ez kell a shade nek is ) 
	//	//Ray shadowRay(r + N, Ll);  
	//	//Hit shadowHit = firstIntersect(shadowRay);
	//	//if (shadowHit.t < 0 || shadowHit.t > | r - yl | )
	///TODO shade fuggveny
			//outRadiance = outRadiance + hit.material->shade(N, V, Ll, Lel);
	//}
	return outRadiance;
}





struct vec4 {
	float v[4];

	vec4(float x = 0, float y = 0, float z = 0, float w = 1) {
		v[0] = x; v[1] = y; v[2] = z; v[3] = w;
	}
};


// handle of the shader program
unsigned int shaderProgram;

class FullScreenTexturedQuad {
	unsigned int vao, textureId;	// vertex array object id and texture id
public:
	//void Create(vec4 image[windowWidth * windowHeight]) {
	void Create(const vector<vec4>& image){
		glGenVertexArrays(1, &vao);	// create 1 vertex array object
		glBindVertexArray(vao);		// make it active

		unsigned int vbo;		// vertex buffer objects
		glGenBuffers(1, &vbo);	// Generate 1 vertex buffer objects

								// vertex coordinates: vbo[0] -> Attrib Array 0 -> vertexPosition of the vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it active, it is an array
		static float vertexCoords[] = { -1, -1,   1, -1,  -1, 1,
			1, -1,   1,  1,  -1, 1 };	// two triangles forming a quad
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoords), vertexCoords, GL_STATIC_DRAW);	   // copy to that part of the memory which is not modified 
																							   // Map Attribute Array 0 to the current bound vertex buffer (vbo[0])
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);     // stride and offset: it is tightly packed

																	  // Create objects by setting up their vertex data on the GPU
		glGenTextures(1, &textureId);  				// id generation
		glBindTexture(GL_TEXTURE_2D, textureId);    // binding

		int meret = image.size();
		vec4* copyableImage= new vec4[meret]; ///TODO felszabadit
		for (int i = 0; i < meret; i++)
		{
			copyableImage[i] = image[i];
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, copyableImage); // To GPU
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // sampling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		delete[] copyableImage;
	}

	void Draw() {
		glBindVertexArray(vao);	// make the vao and its vbos active playing the role of the data source
		int location = glGetUniformLocation(shaderProgram, "textureUnit");
		if (location >= 0) {
			glUniform1i(location, 0);		// texture sampling unit is TEXTURE0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureId);	// connect the texture to the sampler
		}
		glDrawArrays(GL_TRIANGLES, 0, 6);	// draw two triangles forming a quad
	}
};

// The virtual world: single quad
FullScreenTexturedQuad fullScreenTexturedQuad;

struct Scene
{
	Camera camera;
	void addObject(Intersectable* intersectable)
	{
		////TODO adja hozzá object tombhoz
	}
	vector<vec4> createImage()
	{
		setCamera();
		setLight();
		
		vector<vec4> elkeszultKep;
		elkeszultKep.resize(windowWidth * windowHeight);
		for (int x = 0; x < windowWidth; x++) {
			for (int y = 0; y < windowHeight; y++) {
				//elkeszultKep[y * windowWidth + x] = vec4(0, 0, 0, 0); // Legyen alapbol 0 vagyis fekete
				Ray ray = camera.GetRay(x, y);

				vec3 vegsoSzin = vec3(0,0,0);
				///TODO repeat
				{
					vegsoSzin = trace(ray);
				}

				elkeszultKep[y * windowWidth + x] = vec4(vegsoSzin.x, vegsoSzin.y, vegsoSzin.y, 1);
			}
		}
		return elkeszultKep;
		//for each pixel of the screen
		//{
		//	Final color = 0;
		//	Ray = { starting point, direction };
		//	Repeat
		//	{
		//		5
		//		for each object in the scene
		//		{
		//			determine closest ray object / intersection;
		//		}
		//			if intersection exists
		//			{
		//				for each light in the scene
		//				{
		//					if the light is not in shadow of another object
		//					{
		//						add this light contribution to computed color;
		//					}
		//				}
		//			}
		//		Final color = Final color + computed color * previous reflection factor;
		//		reflection factor = reflection factor * surface reflection property;
		//		increment depth;
		//	} until reflection factor is 0 or maximum depth is reached;
		//}
	}
	///TODO atnezni parametrizalni
	void setCamera()
	{
		camera.eyePosition = vec3(0, 0, 1);
		camera.planePosition = vec3(0, 0, -1);
		camera.right = vec3(1, 0, 0);
		camera.up = cross(camera.right,camera.planePosition);
	}
	void setLight()
	{
		ambiensFeny.LightColor = vec3(1, 1, 1);
		ambiensFeny.position = vec3(0, 1, 0);
	}
};
Scene scene;

// Initialization, create an OpenGL context
void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

	//Feladatban specifikalt adatok
	vec3 EzustN(0.14f, 0.16f, 0.13f);
	vec3 EzustK(4.1f, 2.3f, 3.1f);
	vec3 AranyN(0.17f, 0.35f, 1.5f);
	vec3 AranyK(3.1f, 2.7f, 1.9f);

	vec3 AranyColor(1, 0.8431372549f, 0); // Arany szine
	Sphere* sphere = new Sphere(0, 0, -1, 1); ///TODO felszabaditani.
	//sphere->material->calcF0(AranyN,AranyK);
	sphere->material->color = AranyColor;
	objects.push_back(sphere);

	vector<vec4> background;
	background.resize(windowWidth * windowHeight);
	background = scene.createImage();

	fullScreenTexturedQuad.Create(background);

	// Create vertex shader from string
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (!vertexShader) {
		printf("Error in vertex shader creation\n");
		exit(1);
	}
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkShader(vertexShader, "Vertex shader error");

	// Create fragment shader from string
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!fragmentShader) {
		printf("Error in fragment shader creation\n");
		exit(1);
	}
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader, "Fragment shader error");

	// Attach shaders to a single program
	shaderProgram = glCreateProgram();
	if (!shaderProgram) {
		printf("Error in shader program creation\n");
		exit(1);
	}
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Connect Attrib Arrays to input variables of the vertex shader
	glBindAttribLocation(shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0

															  // Connect the fragmentColor to the frame buffer memory
	glBindFragDataLocation(shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory

															// program packaging

	glLinkProgram(shaderProgram);
	checkLinking(shaderProgram);
	// make this program run
	glUseProgram(shaderProgram);
}

void onExit() {
	glDeleteProgram(shaderProgram);
	printf("exit");
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);							// background color 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
	fullScreenTexturedQuad.Draw();
	glutSwapBuffers();									// exchange the two buffers
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();         // if d, invalidate display, i.e. redraw
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {

}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON and GLUT_DOWN / GLUT_UP
	}
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}

///TODO KERDESEK
// SmoothMaterial classban n ( törésmutató ) az egy float, de a háziban mind a 3 spektrumra megvan adva az n értéke.
// A haziban megadott n/k értékek közül egyáltalán az egyik a törésmutató ( n ) a másik meg a k ( kioltási tényező ) PL: aranyozott (n/k az r,g,b hullámhosszain: 0.17/3.1) itt n[0] = 0.17, k[0] = 3.1???
// Honnan van meg egy anyag szine? Az aranyozott dolognak adjak egy olyan attributumot hogy color és az legyen aranyhoz hasonlit??? Egyik dian se láttam colort meg ő se emlitette
// Mi az a kd meg a ks??? Diffuz meg spekularis allando ha jól ertettemde ezt ki adja meg egy rücskös felületnél???
// Ha jól értettem a szemet valahova el kell rakni de itt már bebonyolódtam...

// Idaig modosithatod...
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
	glutInit(&argc, argv);
#if !defined(__APPLE__)
	glutInitContextVersion(majorVersion, minorVersion);
#endif
	glutInitWindowSize(windowWidth, windowHeight);				// Application window is initially of resolution 600x600
	glutInitWindowPosition(100, 100);							// Relative location of the application window
#if defined(__APPLE__)
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);  // 8 bit R,G,B,A + double buffer + depth buffer
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutCreateWindow(argv[0]);

#if !defined(__APPLE__)
	glewExperimental = true;	// magic
	glewInit();
#endif

	printf("GL Vendor    : %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer  : %s\n", glGetString(GL_RENDERER));
	printf("GL Version (string)  : %s\n", glGetString(GL_VERSION));
	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
	printf("GL Version (integer) : %d.%d\n", majorVersion, minorVersion);
	printf("GLSL Version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	onInitialization();

	glutDisplayFunc(onDisplay);                // Register event handlers
	glutMouseFunc(onMouse);
	glutIdleFunc(onIdle);
	glutKeyboardFunc(onKeyboard);
	glutKeyboardUpFunc(onKeyboardUp);
	glutMotionFunc(onMouseMotion);

	glutMainLoop();
	onExit();
	return 1;
}
