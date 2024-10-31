//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul III - 03_03_resize.cpp |
// ======================================
//
//	Program care arata modalitati de utilizare ale proiectiei ortogonale. Se vor desena un patrat si un sistem de axe, folosindu-se tehnicile MODERN OpenGL;
//	ELEMENTE DE NOUTATE:
//	- pentru a stabili o fereastra de "decupare" intr-o scena 2D putem folosi atat functia glm::ortho, cat si indicarea explicita a transformarilor;
//
//
//	Biblioteci

#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Define?te prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
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
#include <ctime>

//  Identificatorii obiectelor de tip OpenGL;
GLuint
	VaoId,
	VboId,
	ColorBufferId,
	ProgramId,
	myMatrixLocation,
	matrRotlLocation,
	codColLocation;
//	Dimensiunile ferestrei de afisare;
GLfloat
	winWidth = 900, winHeight = 600;
///	Variabile catre matricile de transformare;
glm::mat4
	myMatrix, resizeMatrix, matrTransl1, matrScale, matrTransl2, matrRot;

//	Variabila ce determina schimbarea culorii pixelilor in shader;
int codCol;

float PI = 3.141592; /// Valoarea lui PI;

///	Variabile pentru proiectia ortogonala;
float
	xMin = 0, xMax = 110, yMin = 0, yMax = 110; /// Dreptunghiul [0, 110] * [0, 110] 

float
	deltax = xMax - xMin, deltay = yMax - yMin, // lungimile laturilor dreptunghiului decupat
	xcenter = (xMin + xMax) * 0.5, ycenter = (yMin + yMax) * 0.5; // centrul dreptunghiului decupat


//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("03_03_Shader.vert", "03_03_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	/// Coordonatele varfurilor (xMin <= x <= xMax, yMin <= y <= yMax);
	GLfloat Vertices[] = {
		///	Cele 4 puncte din colturi (in functie de variabilele pentru proiectia ortogonala - xMin, xMax, yMin, yMax); 
		0.0f, 0.0f, 0.0f, 1.0f,       /// xMin, yMin
		110.0f,  0.0f, 0.0f, 1.0f,    /// xMax, yMin
		110.0f, 110.0f, 0.0f, 1.0f,   /// xMax, yMax
		0.0f, 110.0f, 0.0f, 1.0f,     /// xMin, yMax

		/*
		//  Varfuri pentru axe;
		// -400.0f, 0.0f, 0.0f, 1.0f,
		// 500.0f,  0.0f, 0.0f, 1.0f,
		// 0.0f, -200.0f, 0.0f, 1.0f,
		// 0.0f, 400.0f, 0.0f, 1.0f,
		
		//	Varfuri pentru dreptunghi;
		// -50.0f,  -50.0f, 0.0f, 1.0f,
		// 50.0f, -50.0f, 0.0f, 1.0f,
		// 50.0f,  50.0f, 0.0f, 1.0f,
		// -50.0f,  50.0f, 0.0f, 1.0f,
		*/

		/// Varfurile poligonului convex (P1)
		20.0f, 20.0f, 0.0f, 1.0f,
		40.0f, 20.0f, 0.0f, 1.0f,
		40.0f, 40.0f, 0.0f, 1.0f,
		20.0f, 30.0f, 0.0f, 1.0f,

		/// Varfurile poligonului concav (P2) -> 2 triunghiuri (unul peste altul)
		70.0f, 75.0f, 0.0f, 1.0f,
		80.0f, 75.0f, 0.0f, 1.0f,
		80.0f, 85.0f, 0.0f, 1.0f,

		75.0f, 80.0f, 0.0f, 1.0f, /// Punctul care ajuta la desenarea conturului (Varful cu unghiul > 180 grade)  

		70.0f, 85.0f, 0.0f, 1.0f,
		70.0f, 75.0f, 0.0f, 1.0f,
		80.0f, 75.0f, 0.0f, 1.0f
	};

	//	Culorile varfurilor din colturi;
	GLfloat Colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f, // Red
		0.0f, 1.0f, 0.0f, 1.0f, // Green
		0.0f, 0.0f, 1.0f, 1.0f, // Blue
		0.8f, 0.4f, 0.0f, 1.0f // Brown
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI;
	glGenBuffers(1, &VboId);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);      //  Punctele sunt "copiate" in bufferul curent;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//  Se creeaza un buffer pentru CULOARE;
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
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

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului (alb); 
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;
	// Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	codColLocation = glGetUniformLocation(ProgramId, "codCol");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

	/// Se aplica functia glm::ortho, in functie de valorile variabilelor pentru proiectia ortogonala; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

	/// Efectuam explicit compunerea transformarilor (Matricele de rotatie, de translatie si de scalare);
	/// Calcularea coordonatelor centrului intre poligoanele P1 si P2:
	glm::vec3 P1corner = glm::vec3(40.0f, 40.0f, 0.0f);
	glm::vec3 P2corner = glm::vec3(70.0f, 75.0f, 0.0f);
	glm::vec3 centerP1_P2 = (P1corner + P2corner) * 0.5f; /// Centrul intre poligoanele P1 si P2;

	/// Translatia 1: Centrul intre poligoanele P1 si P2 este translatat in origine;
	matrTransl1 = glm::translate(glm::mat4(1.0f), -centerP1_P2);

	/// Translatia 2: Inversa / Opusa translatiei 1;
	matrTransl2 = glm::translate(glm::mat4(1.0f), centerP1_P2);

	/// Rotatie, in sens trigonometric, cu unghiul "angle" in jurul axei 'z' (in plan 2D) 
	GLfloat angle = PI / 3.0f; /// unghi de 60 de grade;
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));

	/// Scalarea care modifica dimensiunile poligoanelor P1, respectiv P2 (axe oarecare)
	srand(time(NULL));
	/// Atat axaX, cat si axaY vor avea valori random cuprinse intre 0.5f si 1.5f
	GLfloat axaXrand = (5 + rand() % 11) * 0.1f;
	GLfloat axaYrand = (5 + rand() % 11) * 0.1f;
	matrScale = glm::scale(glm::mat4(1.0f), glm::vec3(axaXrand, axaYrand, 1.0));
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	glEnable(GL_POINT_SMOOTH);

	glPointSize(15.0);							//  Se seteaza dimensiunea punctelor si netezirea marginilor;
	glLineWidth(5.0);

	//	Matrice pentru elemente "fixe" - puncte, axe;
	//	Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = resizeMatrix;	
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]); 

	/// Desenarea celor 4 puncte din colturi (fundalul este desenat cu gradient)
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_QUADS, 0, 4);

	///	Desenarea poligonului convex P1;
	codCol = 3;	//  Yellow;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_POLYGON, 4, 4); /// desenul interior
	codCol = 1; //  Black;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_LINE_LOOP, 4, 4); /// conturul poligonului convex P1

	///	Desenarea poligonului concav P2 -> 2 triunghiuri (unul peste altul);
	codCol = 2;	//	Purple(Magenta);
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_TRIANGLES, 8, 3); /// desenul interior
	glDrawArrays(GL_TRIANGLES, 11, 3); /// desenul interior
	/// Conturul poligonului concav P2
	codCol = 1; // Black;
	glUniform1i(codColLocation, codCol);
	glDrawArrays(GL_LINE_STRIP, 8, 3);
	glDrawArrays(GL_LINES, 12, 2);
	glDrawArrays(GL_LINE_STRIP, 10, 3);

	/// Compunerea de transformari -> Inmultirea matricelor urmeaza sa se realizeze in programul principal (03_03_resize.cpp)
	/// A - Aplicand ROTATIE:
	codCol = 4; // Red;
	myMatrix = resizeMatrix * matrTransl2 * matrRot * matrTransl1;

	/// B - Aplicand SCALARE:
	///  codCol = 5; // Orange;
	///  myMatrix = resizeMatrix * matrTransl2 * matrScale * matrTransl1;
	
	/// Transmiterea variabilelor uniforme pentru MATRICEA DE TRANSFORMARE si COLORARE spre shadere;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniform1i(codColLocation, codCol);
	glLineWidth(5.5);

	/// Se va desena doar conturul poligoanelor pe care aplicam transformari (translatie, scalare, rotatie):
	glDrawArrays(GL_LINE_LOOP, 4, 4); /// conturul poligonului convex P1
	/// Conturul poligonului concav P2
	glDrawArrays(GL_LINE_STRIP, 8, 3);
	glDrawArrays(GL_LINES, 12, 2);
	glDrawArrays(GL_LINE_STRIP, 10, 3);

	glFlush();	//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Utilizare glm::ortho - OpenGL <<nou>>");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();

	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}

