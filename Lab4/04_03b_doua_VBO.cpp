//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_03b_douaVBO.cpp |
// ======================================
// 
//	Program ce deseneaza un patrat, folosindu-se tehnicile MODERN OpenGL;
//	ELEMENTE DE NOUTATE:
//	 - folosirea a doua VBO ale unui singur VAO pentru a desena doua obiecte diferite (fiecare VAO contine informatiile referitoare la varfurile ce definesc un obiect);
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

/// 2. Identificatorii obiectelor de tip OpenGL;
GLuint
	VaoId,                               // Identificator pentru VAO
	VboId1,VboId2,                       // Cate doua buffere pt. coordonate, culori, indici;
	ColorBufferId1, ColorBufferId2,
	EboId1,EboId2,
	ProgramId,
	myMatrixLocation,
	codColLocation;

/// 2. Dimensiunile ferestrei de afisare;
GLfloat winWidth = 600, winHeight = 600;

//	Variabile catre matricile de transformare;
glm::mat4 myMatrix, resizeMatrix;

/// 2. Variabile pentru proiectia ortogonala;
float xMin = -100.0f, xMax = 100.0f, yMin = -100.0f, yMax = 100.0f;

/// 2. Variabila ce determina schimbarea culorii pixelilor din shader;
int codCol;

/// 2. Functie care schimba codul culorii conform celor declarate in shader;
void changeColorShader(const int val) {
	codCol = val;
	glUniform1i(codColLocation, codCol);
}

//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("04_02_Shader.vert", "04_02_Shader.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza VAO-ul care contine cele doua VBO
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
 
	/// 2. PRIMUL VBO - corespunde unui COPAC (tulpina si coroana):
	//  Coordonatele varfurilor;
	static const GLfloat Vertices1[] =
	{
		/// Tulpina
		-70.f, -70.f, 0.f, 1.f, /// ind 0
		-55.f, -70.f, 0.f, 1.f, /// ind 1
		-55.f, -25.f, 0.f, 1.f, /// ind 2
		-70.f, -25.f, 0.f, 1.f, /// ind 3
		/// Coroana
		-75.f, -25.f, 0.f, 1.f, /// ind 4
		-50.f, -25.f, 0.f, 1.f, /// ind 5
		-62.5f, 0.f, 0.f, 1.f  /// ind 6
	};
	/// 2. Culorile ca atribute ale varfurilor (Folosim doar culorile din shader);
	static const GLfloat Colors1[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f // White;
	};
	/// 2.Indicii care determina ordinea de parcurgere a varfurilor;
	static const GLuint Indices1[] =
	{
		0, 1, 2, 3, 4, 5, 6
	};

	/// 2. AL 2-lea VBO - corespunde unei CASE (un etaj, o usa, 2 ferestre, un acoperis):
	//  Coordonatele varfurilor;
	static const GLfloat Vertices2[] =
	{
		/// Fundatie (unicul etaj)
		-30.f, -70.f, 0.f, 1.f, /// ind 0
		60.f, -70.f, 0.f, 1.f, /// ind 1
		60.f, -20.f, 0.f, 1.f, /// ind 2
		-30.f, -20.f, 0.f, 1.f, /// ind 3
		/// Acoperis
		-40.f, -20.f, 0.f, 1.f, /// ind 4
		70.f, -20.f, 0.f, 1.f, /// ind 5
		15.f, 20.f, 0.f, 1.f, /// ind 6
		/// Usa
		5.f, -70.f, 0.0f, 1.f, /// ind 7
		25.f, -70.f, 0.f, 1.f, /// ind 8
		25.f, -35.f, 0.f, 1.f, /// ind 9
		5.f, -35.f, 0.f, 1.f, /// ind 10
		/// Clanta usii
		20.f, -55.f, 0.f, 1.f, /// ind 11
		/// Fereastra (stanga/dreapta)
		-20.f, -50.f, 0.f, 1.f, /// ind 12
		-5.f, -50.f, 0.f, 1.f, /// ind 13
		-5.f, -35.f, 0.f, 1.f, /// ind 14
		-20.f, -35.f, 0.f, 1.f /// ind 15
	};
	/// 2. Culorile ca atribute ale varfurilor (Folosim doar culorile din shader);
	static const GLfloat Colors2[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f // White;
	};
	/// 2. Indicii care determina ordinea de parcurgere a varfurilor;
	static const GLuint Indices2[] =
	{
		4, 5, 6, 0, 1, 2, 3, 7, 8, 9, 10, 11, 12, 13, 14, 15
	};

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - contine ambele VBO;
	glGenVertexArrays(1, &VaoId);         //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	// Transmiterea datelor prin buffere
	//  Se creeaza primul buffer pentru VARFURI;
	glGenBuffers(1, &VboId1);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices1), Vertices1, GL_STATIC_DRAW);      //  Punctele sunt "copiate" in bufferul curent;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
 
	//  Se creeaza al doilea buffer pentru VARFURI;
	glGenBuffers(1, &VboId2);                                                        //  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);                                           //  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2), Vertices2, GL_STATIC_DRAW);
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//  Se creeaza primul buffer pentru CULOARE;
	glGenBuffers(1, &ColorBufferId1);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors1), Colors1, GL_STATIC_DRAW);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
 
	//  Se creeaza al doilea buffer pentru CULOARE;
	glGenBuffers(1, &ColorBufferId2);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors2), Colors2, GL_STATIC_DRAW);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//	Se creeaza primul buffer pentru INDICI;
	glGenBuffers(1, &EboId1);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices1), Indices1, GL_STATIC_DRAW);

	//	Se creeaza al doilea buffer pentru INDICI;
	glGenBuffers(1, &EboId2);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);
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
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &ColorBufferId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &ColorBufferId2);
	glDeleteBuffers(1, &EboId2);

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

	//  Trecerea datelor de randare spre bufferul folosit de shadere;
	CreateVBO();		

	//  Initializarea shaderelor;
	CreateShaders();							
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	//	Dreptunghiul "decupat"; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;
	changeColorShader(0); /// 2. Culorile din programul principal (.cpp);

	//	Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = resizeMatrix;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	//	Desenarea primitivelor

	/// 2. Prima primitiva (COPAC cu tulpina si coroana)
	// 
	// Sunt "legate" bufferele pentru primul VBO (coordonate, culori, indici). La coordonate si culori este asociat vertex attribute-ul corespunzator
	// Coordonate
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// Culori
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// Indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	/// 2. Desenarea COPACULUI
	/// Tulpina
	changeColorShader(1); // Brown;
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void*)(0)); 
	/// Coroana
	changeColorShader(2); // Green;
	/// "unsigned int" occupies '4' bytes
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(4 * 4));
	

	/// 2. A 2-a primitiva (CASA cu un etaj, 2 ferestre, o usa si un acoperis)
	// 
	// Sunt "legate" bufferele pentru al doilea VBO (coordonate, culori, indici). La coordonate si culori este asociat vertex attribute-ul corespunzator
	// Coordonate
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// Culori
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId2);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	// Indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	/// 2. Desenarea CASEI
	/// Fundatia (unicul etaj)
	changeColorShader(5); // Grey;
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void*)(4 * 3));
	/// Acoperisul
	changeColorShader(1); // Brown;
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(0));
	/// Usa
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void*)(4 * 7));
	/// Clanta usii
	glEnable(GL_POINT_SMOOTH);
	glPointSize(6);
	changeColorShader(3); // Black;
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(4 * 11));
	glDisable(GL_POINT_SMOOTH);
	/// Fereastra din stanga
	changeColorShader(4); // Cyan;
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void*)(4 * 12));
	/// Translatam in dreapta prima fereastra desenata (adica cea din stanga usii) cu valoarea 55.0f
	glm::mat4 matTransl = glm::translate(glm::mat4(1.0f), glm::vec3(55.f, 0.f, 0.f));
	/// Transmiterea variabilei uniforme pentru MATRICEA DE TRANSFORMARE spre shadere;
	myMatrix = myMatrix * matTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	/// Fereastra din dreapta
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (void*)(4 * 12));

	glFlush(); // Asigura rularea tuturor comenzilor OpenGL apelate anterior;
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	//  Se initializeaza GLUT si contextul OpenGL si se configureaza fereastra si modul de afisare;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);					//	Modul de afisare al ferestrei, se foloseste un singur buffer de afisare si culori RGB;
	glutInitWindowSize(winWidth, winHeight);						//  Dimensiunea ferestrei;
	glutInitWindowPosition(100, 100);								//  Pozitia initiala a ferestrei;
	glutCreateWindow("Utilizarea a doua VBO (Vertex Buffer Objects)");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

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


