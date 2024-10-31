//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_02_indexare.cpp |
// ======================================
// 
//	Program ce deseneaza un patrat, folosindu-se tehnicile MODERN OpenGL;
//	ELEMENTE DE NOUTATE:
//	 - folosirea indexarii varfurilor: elemente asociate (matrice, buffer);
//	 - desenarea se face folosind functia glDrawElements();
//
//
// 
//	Biblioteci

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

/// 1. Identificatorii obiectelor de tip OpenGL;
GLuint
	VaoId,
	VboId,
	EboId,
	ProgramId,
	myMatrixLocation,
	codColLocation;

///	1. Dimensiunile ferestrei de afisare;
GLfloat winWidth = 600, winHeight = 600;

//	Variabile catre matricile de transformare;
glm::mat4 myMatrix, resizeMatrix;

/// 1. Variabile pentru proiectia ortogonala;
float xMin = -95.0f, xMax = 95.0f, yMin = -95.0f, yMax = 95.0f;

/// 1. Variabila ce determina schimbarea culorii pixelilor din shader;
int codCol;

/// 1S. Valoarea lui PI;
const float PI = 3.141592;

/// 1S. Valoarea N aleasa pt. cele 2 cercuri concentrice 
/// (cercul mare are raza R = 27, iar cercul mic are raza r = 12);
const int N = 8;

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("04_02_Shader.vert", "04_02_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	/// 1. Coordonatele varfurilor;
	static const GLfloat Vertices[] =
	{
		/// 1. Alegerea valorilor potrivite pentru coordonatele varfurilor celor 2 poligoane (patrate) alipite;
		-30.0f, 45.f, 0.f, 1.f,  /// stanga-jos (indice 0)
		0.f, 45.f, 0.f, 1.f,    /// mijloc-jos (indice 1)
		0.f, 75.f, 0.f, 1.f,     /// mijloc-sus (indice 2)
		-30.f, 75.f, 0.f, 1.f,   /// stanga-sus (indice 3)
		30.f, 45.f, 0.f, 1.f,   /// dreapta-jos (indice 4)
		30.f, 75.f, 0.f, 1.f,     /// dreapta-sus (indice 5)
		/// 1S. Construirea unui trapez isoscel, trasand si una dintre diagonalele sale;
		-5.f, 12.f, 0.f, 1.f, /// stanga-jos (indice 6)
		5.f, 12.f, 0.f, 1.f, /// dreapta-jos (indice 7)
		11.f, 27.f, 0.f, 1.f, /// dreapta-sus (indice 8)
		-11.f, 27.f, 0.f, 1.f /// stanga-sus (indice 9)
	};

	/// 1. Culorile ca atribute ale varfurilor;
	static const GLfloat Colors[] =
	{
		/// 1. Prima figura (poligonul format din cele 2 patrate alipite)
		1.f, 0.f, 0.f, 1.f, /// Red (indice 0)
		0.f, 1.f, 0.f, 1.f, /// Green (indice 1)
		0.f, 0.f, 1.f, 1.f, /// Blue (indice 2)
		1.f, 0.f, 1.f, 1.f, /// Purple (Magenta) (indice 3)
		1.f, 0.f, 0.f, 1.f, /// Red (indice 4)
		1.f, 0.f, 1.f, 1.f  /// Purple (Magenta) (indice 5)
	};
	
	///	1. Indicii care determina ordinea de parcurgere a varfurilor (10 varfuri si 16 indici in total);
	static const GLuint Indices[] =
	{
		/// 1. Prima figura (poligonul format din cele 2 patrate alipite)
		0, 1, 2, 3, 0, 2, 5, 4, 1, 5,
		/// 1S. A 2-a figura (trapezul isoscel, trasand si una dintre diagonalele sale)
		6, 7, 8, 9, 6, 8
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer comun pentru VARFURI - COORDONATE si CULORI;
	glGenBuffers(1, &VboId);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors) + sizeof(Vertices), NULL, GL_STATIC_DRAW);	//	Definirea bufferului, dimensiunea lui = dimensiunea(COLORS + VERTICES)
	//	Spatiul bufferului este impartit intre zona de COORDONATE si cea de VARFURI:
	//	 - prima sectiune incepe de la 0, are dimensiunea VERTICES si continele datele despre COORDONATE;
	//	 - a doua sectiune incepe de la sizeof(Vertices) - deci la finalul primei sectiuni, are dimensiunea COLORS si contine datele despre CULOARE;
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);				//	COORDONATELE;
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);		//	CULORILE;
	
	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(Vertices));
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

	//  Stergerea bufferelor pentru VARFURI(Coordonate + Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	//	Dreptunghiul "decupat"; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;
	/// 1. Prima figura (poligonul format din cele 2 patrate alipite)
	codCol = 0; /// 1. Culorile din programul principal (.cpp);
	glUniform1i(codColLocation, codCol);
	/// 1. Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glLineWidth(6); /// 1. Latimea laturilor ce vor fi desenate
	/// 1. Primul poligon (format din alte 2 patrate alipite) este translatat doar in stanga cu -45.0f 
	glm::mat4 matTransl = glm::translate(glm::mat4(1.0f), glm::vec3(-45.0f, 0.0f, 0.0f)); 
	/// 1. Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = myMatrix * matTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	/// 1. Urmatoarele 2 poligoane (fiecare fiind format din alte 2 patrate alipite) sunt translatate doar in dreapta 
	/// cu 60.0f fata de pozitia ultimei matrici de transformare; 
	matTransl = glm::translate(glm::mat4(1.0f), glm::vec3(60.0f, 0.0f, 0.0f));
	for (int c = 0; c < 2; c++)
	{
		/// 1. Desenarea primelor 10 primitive (cele 2 patrate alipite) in functie de ordinea de parcurgere a varfurilor din GLuint Indices[] = {...};
		glDrawElements(GL_LINE_STRIP, 10, GL_UNSIGNED_INT, (void*)(0)); 
		/// 1. Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
		myMatrix = myMatrix * matTransl;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	}
	/// 1. Desenarea primelor 6 primitive (doar primul patrat) in functie de ordinea de parcurgere a varfurilor din GLuint Indices[] = {...};
	glDrawElements(GL_LINE_STRIP, 6, GL_UNSIGNED_INT, (void*)(0));


	/// 1S. A 2-a figura (trapezul isoscel, trasand si una dintre diagonalele sale)
	float angle = PI / 4.f; // unghi de (360 / 8 = 180 / 4 = PI / 4 = 45) grade (sens trigonometric);
	/// Rotatie cu unghiul "angle" in jurul axei 'z' (sens trigonometric)
	glm::mat4 matRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	/// Rotatie cu unghiul "-angle / 2.f = -22.5 grade" in jurul axei 'z' (sens orar)
	glm::mat4 init_matRot = glm::rotate(glm::mat4(1.0f), -angle / 2.f, glm::vec3(0.0, 0.0, 1.0));
	codCol = 3; /// 1S. Black;
	glUniform1i(codColLocation, codCol);
	glLineWidth(3); /// 1S. Latimea laturilor ce vor fi desenate
	/// 1S. Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = resizeMatrix * init_matRot;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int i = 0; i < N; i++)
	{
		/// 1S. Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
		/// Rotim (in sens trigonometric) matricea curenta cu unghiul "angle", avand centrul in origine O;
		myMatrix = myMatrix * matRot;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		/// 1S. Desenarea ultimelor 6 primitive (trapezul isoscel, trasand una dintre diagonalele sale) in functie de ordinea de parcurgere a varfurilor din GLuint Indices[] = {...}; 
		/// "unsigned int" occupies '4' bytes
		glDrawElements(GL_LINE_STRIP, 6, GL_UNSIGNED_INT, (void*)(4 * 10));
	}

	/*
	//	Desenarea primitivelor
	//	Functia glDrawElements primeste 4 argumente:
	//	 - arg1 = modul de desenare;
	//	 - arg2 = numarul de varfuri;
	//	 - arg3 = tipul de date al indicilor;
	//	 - arg4 = pointer spre indici (EBO): pozitia de start a indicilor;
	///   glDrawElements(GL_LINES, 10, GL_UNSIGNED_INT, (void*)(0));
	*/

	glFlush();								//  Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Indexarea varfurilor - OpenGL <<nou>>");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

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


