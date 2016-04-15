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


#include "EventHandler_ErrorHandler.h"
#include "FullScreenTexturedQuad.h"

#include "VectorMath.h"
#include "Materials.h"
#include "Lights.h"
#include "Intersectables.h"
#include "RayLogic_Hit.h"


#include "Types.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...
using namespace std;
// OpenGL major and minor versions

//===================================================== INNENTOL VAN ERDEKES DOLOG =================================================//



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

vector<Intersectable*> objects;

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

void color_normalize(vec3 &outRadiance)
{
	if (outRadiance.x > 1.0f)
		outRadiance.x = 1.0f;
	if (outRadiance.y > 1.0f)
		outRadiance.y = 1.0f;
	if (outRadiance.z > 1.0f)
		outRadiance.z = 1.0f;

	if (outRadiance.x < 0.0f)
		outRadiance.x = 0.0f;
	if (outRadiance.y < 0.0f)
		outRadiance.y = 0.0f;
	if (outRadiance.z < 0.0f)
		outRadiance.z = 0.0f;
}


vec3 trace(Ray ray, int depth) {

	if(depth > 20)
		return ambiensFeny.LightColor;

	Hit hit = firstIntersect(ray); //Milyen objektum van legkozelebb
	if (hit.t < 0) 
		return ambiensFeny.LightColor; // nothing  //Ambiens fenyt fogja visszaadni.


	float kicsinyitesNagysaga = 10.0f;
	vec3 outRadiance = vec3(0, 0, 0);
	if (hit.material->materialType == TYPES::Rough)
	{
		outRadiance = hit.material->ka * ambiensFeny.LightColor;
		
		//for (each light source l) {
		vec3 Neps = hit.normal*(sgn(dot(ray._nezetiIrany * -1.0f, hit.normal)));
		Neps = Neps.normalize();
		Neps = Neps / kicsinyitesNagysaga;
		vec3 fenyIranya = nap.position - hit.position; // Ellentetes ez a nap fele mutat
		fenyIranya = fenyIranya.normalize();

		Ray shadowRay(hit.position + Neps, fenyIranya);  
		Hit shadowHit = firstIntersect(shadowRay);
		if (shadowHit.t < 0 || shadowHit.t >  (hit.position - nap.position).Length()  )
			outRadiance = outRadiance + hit.material->shade(hit.normal, ray._nezetiIrany* -1.0f, fenyIranya, nap.LightColor);
	}
	else if (hit.material->materialType == TYPES::Smooth)
	{
		///Todo megcsinalni hogy tukrozodjon az arany!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (hit.material->isReflective()) {
			vec3 reflectionDir = hit.material->reflect(ray._nezetiIrany, hit.normal);  // Beérkezési irány, Normál vektora a felületnek
			
			vec3 Neps = hit.normal*(sgn(dot(ray._nezetiIrany * -1.0f, hit.normal)));
			Neps = Neps.normalize();
			Neps = Neps / kicsinyitesNagysaga;
			vec3 visszaverPos = hit.position;
			Ray reflectedRay(visszaverPos + Neps, reflectionDir);
			vec3 fresnel = hit.material->Fresnel(ray._nezetiIrany, hit.normal);

			outRadiance = outRadiance + trace(reflectedRay,depth + 1) * fresnel;
		}
		///TODO ez majd viznek kell!
		if (hit.material->isRefractive()) {
			vec3 refractionDir = hit.material->refract(ray._nezetiIrany, hit.normal);
			vec3 Neps = hit.normal*(sgn(dot(ray._nezetiIrany * -1.0f, hit.normal)));
			Neps = Neps.normalize();
			Neps = Neps / kicsinyitesNagysaga;

			Ray refractedRay(hit.position - Neps, refractionDir);

			vec3 fresnel = hit.material->Fresnel(ray._nezetiIrany, hit.normal);
			fresnel = (vec3(1, 1, 1) - fresnel);
			outRadiance = outRadiance + trace(refractedRay,depth + 1)*fresnel;
		}
	}

	color_normalize(outRadiance);

	return outRadiance;
}



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
				vec3 vegsoSzin = vec3(0, 0, 0);
				if (x >= 230 && y >= windowHeight - 250)
					vegsoSzin = vec3(1, 0, 0);
				//else
				{
					vegsoSzin = trace(ray,0);
				}

				elkeszultKep[y * windowWidth + x] = vec4(vegsoSzin.x, vegsoSzin.y, vegsoSzin.z, 1);
			}
		}
		return elkeszultKep;
	}
	///TODO atnezni parametrizalni
	void setCamera()
	{
		//Old view
		//camera.eyePosition = vec3(0, 0, 1);
		//camera.planePosition = vec3(0, 0, -1);
		//camera.right = vec3(1, 0, 0);
		//Felso nezet
		//camera.eyePosition = vec3(0, 2.5f, 1.5f);
		//camera.planePosition = vec3(0, -0.5f, -0.5f);
		//camera.right = vec3(0.707104f, 0, 0);
		//Full felso
		camera.eyePosition = vec3(0, 3.0f, -2.0f);
		camera.planePosition = vec3(0, -1, 0);
		camera.right = vec3(0, 0, -1);
		camera.up = cross(camera.right,camera.planePosition);
	}
	void setLight()
	{
		ambiensFeny.position = vec3(0, 1, 0);
		nap.mekkoraRadianbanLatszik = 0.0174532925f;
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

	SmoothMaterial* aranyAnyaga = new SmoothMaterial(AranyN, AranyK,true, false);  ///TODO felszabaditani
	SmoothMaterial* ezustAnyaga = new SmoothMaterial(EzustN, EzustK, true, false);  ///TODO felszabaditani
	SmoothMaterial* vizAnyaga = new SmoothMaterial(vec3(1.33f, 1.33f, 1.33f), vec3(0, 0, 0), true, true);
	RoughMaterial* roughAnyag = new RoughMaterial(vec3(0.5f, 0.2f, 0.2f), vec3(0.5f, 0.3f, 0.1f), vec3(1, 1, 1), 15, false, false);///TODO felszabaditani
	RoughMaterial* fuAnyagaSik = new RoughMaterial(vec3(0.0f, 0.2f, 0.1f), vec3(0.0f, 0.3f, 0.1f), vec3(0.0f, 0.2f, 0.0f), 100, false, false);///TODO felszabaditani
	RoughMaterial* medenceAnyaga = new RoughMaterial(vec3(0.0f, 0.4f, 0.4f), vec3(0.0f, 0.2f, 0.2f), vec3(1, 1, 1), 25, false, false);///TODO felszabaditani

	Sphere* sphere = new Sphere(-0.8f, 0, -1, 0.5f); ///TODO felszabaditani.
	Sphere* sphere2 = new Sphere(+0.8f, 0, -1, 0.5f); ///TODO felszabaditani.
	Sphere* sphere3 = new Sphere(0.0f, 0, -2.0f, 0.3f); ///TODO felszabaditani.
	sphere->material = aranyAnyaga;
	sphere2->material = roughAnyag;
	sphere3->material = ezustAnyaga;
	Plane* plane = new Plane(vec3(0, 0, 0), vec3(0, 1, 0), fuAnyagaSik);

	vec3 aranypos(0.9f, 0.6f, -1.5f);
	vec3 ezustpos(-0.8f, 0.6f, -3.0f);
	Ellipsoid* ellipsoid = new Ellipsoid(vec3(-0.45f, 0.2f, -1.2f), vec3(0.7f, 0.7f, 0.7f), roughAnyag);
	Ellipsoid* ellipsoidArany = new Ellipsoid(aranypos, vec3(0.5f, 0.5f, 0.5f), aranyAnyaga);
	Ellipsoid* ellipsoidEzust = new Ellipsoid(ezustpos, vec3(0.5f, 0.5f, 0.5f), ezustAnyaga);
	Ellipsoid* ellipsoidViz = new Ellipsoid(vec3(-0.3f, 0.2f, -1.2f), vec3(0.7f, 0.7f, 0.7f), vizAnyaga);

	vector<vec3> medenceAlapjaPontok;
	{
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, -1.0f));  //alja
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, 0));
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, 0));
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, -1.0f));

		medenceAlapjaPontok.push_back(vec3(-1.0f, -0.5f, -1.0f));  //Hatulja
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, -1.0f));
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, -1.0f));
		medenceAlapjaPontok.push_back(vec3(1.0f, -0.5f, -1.0f));

		medenceAlapjaPontok.push_back(vec3(-1.0f, -0.5f, 0)); //Bal oldala
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, 0));
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, -1.0f));
		medenceAlapjaPontok.push_back(vec3(-1.0f, -0.5f, -1.0f));

		medenceAlapjaPontok.push_back(vec3(1.0f, -0.5f, 0)); //jobb oldala
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, 0));
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, -1.0f));
		medenceAlapjaPontok.push_back(vec3(1.0f, -0.5f, -1.0f));

		///TODO nem jo a normálvektorja
		medenceAlapjaPontok.push_back(vec3(-1.0f, -1.0f, 0));
		medenceAlapjaPontok.push_back(vec3(-1.0f, -0.5f, 0));  //Eleje
		medenceAlapjaPontok.push_back(vec3(1.0f, -0.5f, 0));
		medenceAlapjaPontok.push_back(vec3(1.0f, -1.0f, 0));
	}
	
	vec3 medenceSkalaz(1.2f, 1, 2.0f);
	vec3 medenceEltolas(0.2f, 0.0f, 0.2f);
	vec3 HullamSkalaz(3, 1, 4);
	vec3 HullamEltol(0, -HULLAMNAGYSAGA + 0.5f, 0);
	Rectanglef* medenceAlap = new Rectanglef(medenceAlapjaPontok, medenceAnyaga);
//	medenceAlap->eltol(medenceEltolas);
//	medenceAlap->skalaz(medenceSkalaz);
	medenceAlap->eltol(HullamEltol);
	medenceAlap->skalaz(HullamSkalaz);
	
	vector<vec3> medenceTetejePontok;
	{
		medenceTetejePontok.push_back(vec3(-1.0f, -0.5f, -1.0f));  //Teteje
		medenceTetejePontok.push_back(vec3(-1.0f, -0.5f, 0));
		medenceTetejePontok.push_back(vec3(1.0f, -0.5f, 0));
		medenceTetejePontok.push_back(vec3(1.0f, -0.5f, -1.0f));
	}
	Rectanglef* medenceTeteje = new Rectanglef(medenceTetejePontok, medenceAnyaga);
//	medenceTeteje->eltol(medenceEltolas);
//	medenceTeteje->skalaz(medenceSkalaz);
	medenceTeteje->eltol(HullamEltol);
	medenceTeteje->skalaz(HullamSkalaz);
	plane->kivag = true;
	plane->kivagniObjektumok.push_back(medenceTeteje);

	vector<vec3> vizPontok = medenceTetejePontok;
	Rectanglef* viz = new Rectanglef(vizPontok, vizAnyaga);
	viz->material->isWater = true;
	viz->eltol(HullamEltol);
	viz->skalaz(HullamSkalaz);
	///Medence keszites
	Rectanglef* VizHullamTeteje = new Rectanglef(medenceTetejePontok, roughAnyag);
	VizHullamTeteje->eltol(vec3(0, HULLAMNAGYSAGA + 0.5f, 0.5f));
	VizHullamTeteje->skalaz(HullamSkalaz + vec3(4,1,4));

	Rectanglef* VizHullamAlja = new Rectanglef(medenceTetejePontok, roughAnyag);
	VizHullamAlja->eltol(vec3(0, -HULLAMNAGYSAGA + 0.5f, 0));
	VizHullamAlja->skalaz(HullamSkalaz);

	Water* hullamzoViz = new Water(VizHullamTeteje, VizHullamAlja,plane, vizAnyaga,aranypos,ezustpos);
	hullamzoViz->material->isWater = true;
	objects.push_back(plane); ///TODO plane bol kivagni a medencet
	//objects.push_back(ellipsoid);
	//objects.push_back(ellipsoidViz);
	objects.push_back(ellipsoidArany);
	objects.push_back(ellipsoidEzust);
	objects.push_back(medenceAlap);
	objects.push_back(hullamzoViz);
	//objects.push_back(VizHullamAlja);
	//objects.push_back(VizHullamTeteje);
	//objects.push_back(viz);
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
	fullScreenTexturedQuad.shaderProgram = glCreateProgram();
	if (!fullScreenTexturedQuad.shaderProgram) {
		printf("Error in shader program creation\n");
		exit(1);
	}
	glAttachShader(fullScreenTexturedQuad.shaderProgram, vertexShader);
	glAttachShader(fullScreenTexturedQuad.shaderProgram, fragmentShader);

	// Connect Attrib Arrays to input variables of the vertex shader
	glBindAttribLocation(fullScreenTexturedQuad.shaderProgram, 0, "vertexPosition"); // vertexPosition gets values from Attrib Array 0

																					 // Connect the fragmentColor to the frame buffer memory
	glBindFragDataLocation(fullScreenTexturedQuad.shaderProgram, 0, "fragmentColor");	// fragmentColor goes to the frame buffer memory

																						// program packaging

	glLinkProgram(fullScreenTexturedQuad.shaderProgram);
	checkLinking(fullScreenTexturedQuad.shaderProgram);
	// make this program run
	glUseProgram(fullScreenTexturedQuad.shaderProgram);
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
