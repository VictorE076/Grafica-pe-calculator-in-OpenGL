#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;

//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId,
VboId,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrScaleLocation,
matrTranslLocation,
matrRotlLocation,
codColLocation;

//	Dimensiunile ferestrei de afisare;
GLfloat winWidth = 900, winHeight = 900;

//	Variabile catre matricile de transformare;
glm::mat4 myMatrix, resizeMatrix, matrTransl1, matrTransl2, matrScale, matrRot;

//	Variabila ce determina schimbarea culorii pixelilor in shader;
int codCol;

//	Unghiul de rotire al patratului;
float angle = 0;	

//	Variabile pentru proiectia ortogonala;
float xMin = -600.f, xMax = 600.f, yMin = -600.f, yMax = 600.f;

//  Valoarea lui PI (180 de grade);
const float PI = 3.141592;

///  float tx = 0; float ty = 0;			//	Coordonatele de translatie ale patratului pe Ox si Oy;
///  int direction_Ox = 1; /// Directia de deplasare pe axa Ox ('1' -> dreapta, '-1' -> stanga);

// !!! Numarul maxim de varfuri din desen;
// const int maxNumVertices = 30000;

// Numarul total de varfuri ce contureaza river-ul;
int totalNumVerticesRiver;

// Numarul total de varfuri ce contureza wave-ul;
int totalNumVerticesWave;


///
// (-1) * (Radical de ordinul 3 din X):
float RiverGraphFunction(const float x) {
	return (x >= 0.f) ? (-1.f) * glm::pow(x, 1 / 3.f) : glm::pow(-x, 1 / 3.f);
}

// Radical din |X|:
float WaveGraphFunction(const float x) {
	return (1.f) * glm::sqrt(glm::abs(x));
}
/// 


class VBO {

	void AddVertex2D(const float x, const float y) {
		Vertices[counterCoordVertices++] = x, Vertices[counterCoordVertices++] = y,
			Vertices[counterCoordVertices++] = 0.f, Vertices[counterCoordVertices++] = 1.f;
	}

	void AddColorforVertex2D(const float r, const float g, const float b) {
		Colors[counterCoordColors++] = r, Colors[counterCoordColors++] = g, Colors[counterCoordColors++] = b,
			Colors[counterCoordColors++] = 1.f;
	}

public:
	GLfloat* Vertices; // 1. Coordonatele varfurilor:
	GLfloat* Colors; // 2. Culorile punctelor din colturi:
	int counterCoordVertices;
	int counterCoordColors;

	VBO(const int maxNumVertices) : counterCoordVertices(0), counterCoordColors(0)
	{
		Vertices = new GLfloat[4 * maxNumVertices];
		Colors = new GLfloat[4 * maxNumVertices];
	}
	
	void AdaugaCer() {
		AddVertex2D(-600.f, 0.f);
		AddVertex2D(600.f, 0.f);
		AddVertex2D(600.f, 600.f);
		AddVertex2D(-600.f, 600.f);
	}
	void AdaugaCuloriCer() {
		AddColorforVertex2D(0.3f, 0.6f, 0.9f);
		AddColorforVertex2D(0.3f, 0.6f, 0.9f);
		AddColorforVertex2D(0.53f, 0.81f, 0.92f);
		AddColorforVertex2D(0.53f, 0.81f, 0.92f);
	}

	void AdaugaIarba() {
		AddVertex2D(-600.f, -600.f);
		AddVertex2D(600.f, -600.f);
		AddVertex2D(600.f, 0.f);
		AddVertex2D(-600.f, 0.f);
	}

	void AdaugaLiniaOrizontului() {
		AddVertex2D(-600.f, 0.f);
		AddVertex2D(600.f, 0.f);
	}

	void AdaugaCasa() {
		// Fundatia
		AddVertex2D(-200.f, -70.f);
		AddVertex2D(200.f, -70.f);
		AddVertex2D(200.f, 150.f);
		AddVertex2D(-200.f, 150.f);

		// Acoperisul
		AddVertex2D(-230.f, 150.f);
		AddVertex2D(230.f, 150.f);
		AddVertex2D(0.f, 230.f);

		// Usa
		AddVertex2D(-50.f, -70.f);
		AddVertex2D(50.f, -70.f);
		AddVertex2D(50.f, 100.f);
		AddVertex2D(-50.f, 100.f);

		// Clanta usii
		AddVertex2D(30.f, 0.f);

		// Fereastra (stanga/dreapta)
		AddVertex2D(-170.f, 10.f);
		AddVertex2D(-80.f, 10.f);
		AddVertex2D(-80.f, 100.f);
		AddVertex2D(-170.f, 100.f);

		AddVertex2D(-125.f, 10.f);
		AddVertex2D(-125.f, 100.f);
		AddVertex2D(-80.f, 55.f);
		AddVertex2D(-170.f, 55.f);
	}

	void AdaugaCopac() {
		// Tulpina
		AddVertex2D(300.f, -130.f);
		AddVertex2D(360.f, -130.f);
		AddVertex2D(360.f, 90.f);
		AddVertex2D(300.f, 90.f);
		// Coroana
		AddVertex2D(330.f, 90.f);
	}

	void AdaugaRiver() {
		for (float x = xMin; x <= xMax; x += 0.05f)
		{
			const float y = 20.f * RiverGraphFunction(x) - 355.f;
			AddVertex2D(x, y);

			totalNumVerticesRiver++;
		}
	}

	void AdaugaWave() {
		for (float x = xMin; x <= xMin + 20.f; x += 0.05f)
		{
			const float y = 5.f * WaveGraphFunction(x + xMax) - 200.f;
			AddVertex2D(x, y);

			totalNumVerticesWave++;
		}
	}


	~VBO() {
		delete[] Vertices;
		delete[] Colors;
	}
};


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("DayNightCycle.vert", "DayNightCycle.frag");
	glUseProgram(ProgramId);
}



//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
static void CreateVBO(void)
{
	VBO vbo(30000); // Max number of vertices

	// 1. Coordonatele varfurilor:
	// Cerul
	vbo.AdaugaCer();

	// Iarba
	vbo.AdaugaIarba();

	// Linia orizontului
	vbo.AdaugaLiniaOrizontului();

	// Casa
	vbo.AdaugaCasa();

	// Copac
	vbo.AdaugaCopac();

	// River
	vbo.AdaugaRiver();

	// Wave
	vbo.AdaugaWave();


	// 2. Culorile punctelor din colturi:
	// Cerul (Clear Sky)
	vbo.AdaugaCuloriCer();


	//  Transmiterea datelor prin buffere;
	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);
	//  Se creeaza un buffer pentru VARFURI;
	glGenBuffers(1, &VboId);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, vbo.counterCoordVertices * sizeof(GLfloat), vbo.Vertices, GL_STATIC_DRAW);      //  Punctele sunt "copiate" in bufferul curent;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//  Se creeaza un buffer pentru CULOARE;
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, vbo.counterCoordColors * sizeof(GLfloat), vbo.Colors, GL_STATIC_DRAW);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	//  Stergerea bufferelor pentru varfuri, culori;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);
	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

///// Functii utile:
void changeColorShader(const int val) {
	codCol = val;
	glUniform1i(codColLocation, codCol);
}
///// 

class Draw {

	int DeseneazaCopac(int cp) {
		// Tulpina
		changeColorShader(1); // Brown
		glDrawArrays(GL_QUADS, cp, 4);
		cp += 4;
		// Coroana
		glPointSize(115.f);
		changeColorShader(6); // Dark Green
		glDrawArrays(GL_POINTS, cp++, 1);
		return cp;
	}

	int DeseneazaWave(int cp, const int codCol) {
		glPointSize(4);
		changeColorShader(codCol); // Wave Color
		glDrawArrays(GL_POINTS, cp, totalNumVerticesWave);
		cp += totalNumVerticesWave;
		return cp;
	}

	void TranslateWave2D(const float x, const float y) {
		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	}

public:
	int DeseneazaCerul(int cp) {
		changeColorShader(0); // Clear Sky
		glDrawArrays(GL_QUADS, cp, 4);
		cp += 4;
		return cp;
	}

	int DeseneazaIarba(int cp) {
		changeColorShader(5); // Bright Green
		glDrawArrays(GL_QUADS, cp, 4);
		cp += 4;
		return cp;
	}

	int DeseneazaLiniaOrizontului(int cp) {
		glLineWidth(2.75f);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINES, cp, 2);
		cp += 2;
		return cp;
	}

	int DeseneazaCasa(int cp) {
		// Fundatia
		changeColorShader(4); // Grey
		glDrawArrays(GL_QUADS, cp, 4);
		glLineWidth(2.f);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINE_LOOP, cp, 4); // contur
		cp += 4;

		// Acoperisul
		changeColorShader(1); // Brown
		glDrawArrays(GL_TRIANGLES, cp, 3);
		glLineWidth(2.f);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINE_LOOP, cp, 3); // contur
		cp += 3;

		// Usa
		changeColorShader(1); // Brown
		glDrawArrays(GL_QUADS, cp, 4);
		glLineWidth(2.f);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINE_LOOP, cp, 4); // contur
		cp += 4;

		// Clanta usii
		glPointSize(8.f);
		changeColorShader(-1); // Black
		glDrawArrays(GL_POINTS, cp++, 1);

		// Fereastra (stanga)
		changeColorShader(3); // Cyan
		glDrawArrays(GL_QUADS, cp, 4);
		glLineWidth(1.5f);
		changeColorShader(-2); // White
		glDrawArrays(GL_LINES, cp + 4, 2);
		glDrawArrays(GL_LINES, cp + 6, 2);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINE_LOOP, cp, 4); // contur

		myMatrix = myMatrix * glm::translate(glm::mat4(1.f), glm::vec3(250.f, 0.f, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		// Fereastra (dreapta)
		changeColorShader(3); // Cyan
		glDrawArrays(GL_QUADS, cp, 4);
		glLineWidth(1.5f);
		changeColorShader(-2); // White
		glDrawArrays(GL_LINES, cp + 4, 2);
		glDrawArrays(GL_LINES, cp + 6, 2);
		changeColorShader(-1); // Black
		glDrawArrays(GL_LINE_LOOP, cp, 4); // contur

		myMatrix = resizeMatrix;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		cp += 8;
		return cp;
	}

	int DeseneazaCopaci(int cp) {
		
		// Copacul 1
		const int currentPos = DeseneazaCopac(cp);

		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.f), glm::vec3(150.f, -200.f, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		

		// Copacul 2
		DeseneazaCopac(cp);

		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.f), glm::vec3(-770.f, -460.f, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		// Copacul 3
		DeseneazaCopac(cp);

		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.f), glm::vec3(-670.f, 80.f, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		// Copacul 4
		DeseneazaCopac(cp);

		myMatrix = resizeMatrix * glm::translate(glm::mat4(1.f), glm::vec3(-845.f, 15.f, 0.f));
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		// Copacul 5
		DeseneazaCopac(cp);

		myMatrix = resizeMatrix;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		
		return currentPos;
	}

	int DeseneazaRiver(int cp) {
		glPointSize(65);
		changeColorShader(7); // Water Color
		glDrawArrays(GL_POINTS, cp, totalNumVerticesRiver);
		cp += totalNumVerticesRiver;
		return cp;
	}

	int DeseneazaAllWaves(int cp) {

		srand(time(NULL));

		int randBlack_or_White = (-1) * (1 + rand() % 2);
		const int currentPos = DeseneazaWave(cp, randBlack_or_White); // Black or White (Random choice)
		for (float x = 25.f; x <= 2 * xMax; x += 25.f)
		{
			const float y = 20.f * RiverGraphFunction(x + xMin) - 170.f;
			TranslateWave2D(x, y);
			randBlack_or_White = (-1) * (1 + rand() % 2);
			DeseneazaWave(cp, randBlack_or_White); // Black or White (Random choice)
		}

		myMatrix = resizeMatrix;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

		return currentPos;
	}
};


//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului (alb);
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	//	Dreptunghiul "decupat"; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

///  "RenderFunction" este functia de desenare a graficii pe ecran 
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;
	glEnable(GL_POINT_SMOOTH);

	/*
	angle += 0.08f; /// Patratul se rostogoleste continuu (pana la inchiderea ferestrei sau a programului)

	/// Schimbarea directiei de deplasare atunci cand sunt atise / depasite variabilele xMin si xMax pentru proiectia ortogonala.
	/// Translatia patratului porneste din origine (initializare globala: tx = ty = 0 ).
	if (tx - 50.0f <= xMin)
		direction_Ox = 1; /// Am ajuns la "capatul stang", deci schimbam directia de deplasare a patratului spre DREAPTA.

	if (tx + 50.0f >= xMax)
		direction_Ox = -1; /// Am ajuns la "capatul drept", deci schimbam directia de deplasare a patratului spre STANGA.

	tx += 4.0f * direction_Ox; /// Patratul se deplaseaza continuu (pana la inchiderea ferestrei sau a programului)

	///	Matrici pentru transformari;
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	///
	*/

	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	Draw draw;
	int currentPos = 0;
	// Cerul
	currentPos = draw.DeseneazaCerul(currentPos);

	// Iarba
	currentPos = draw.DeseneazaIarba(currentPos);

	// Linia orizontului
	currentPos = draw.DeseneazaLiniaOrizontului(currentPos);

	// Casa
	currentPos = draw.DeseneazaCasa(currentPos);
	
	// Copaci
	currentPos = draw.DeseneazaCopaci(currentPos);

	// River
	currentPos = draw.DeseneazaRiver(currentPos);

	// Waves
	currentPos = draw.DeseneazaAllWaves(currentPos);

	
	glDisable(GL_POINT_SMOOTH);
	glutSwapBuffers();	//	Inlocuieste imaginea deseneata in fereastra cu cea randata; 
	glFlush();			//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);					//	Se folosesc 2 buffere (unul pentru afisare si unul pentru randare => animatii cursive) si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Utilizarea tastaturii - OpenGL <<nou>>");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();							//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);		//  Desenarea scenei in fereastra;

	///
	glutIdleFunc(RenderFunction);			///	!!! Asigura rularea continua a randarii;
	///

	glutCloseFunc(Cleanup);					//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	///  cout << (-1.f) * glm::pow((- 25.3f), 1.f / 3.f);

	glutMainLoop();

	return 0;
}
