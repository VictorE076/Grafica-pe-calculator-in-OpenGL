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
#include "SOIL.h"			/// 3. Biblioteca pentru texturare
#include <iostream>

/// 3.Identificatorii obiectelor de tip OpenGL;
GLuint
	VaoId,
	VboId,
	EboId,
	ProgramId,
	myMatrixLocation,
	viewLocation,
	projLocation,
	matrRotlLocation,
	codColLocation,
	texture;

/// 3. Dimensiunile ferestrei de afisare;
GLfloat winWidth = 1200, winHeight = 900;

///	3. Variabile catre matricile de transformare;
glm::mat4 myMatrix, resizeMatrix, matrTransl1, matrScale, matrTransl2, matrRot;

///	3. Elemente pentru matricea de vizualizare;
float obsX = 0.0, obsY = 0.0, obsZ = 800.f;
float refX = 0.0f, refY = 0.0f;
float vX = 0.0;
glm::mat4 view;

/// 3. Elemente pentru matricea de proiectie;
float width = 800, height = 600;
float xMin = -800.f, xMax = 800.f, yMin = -600.f, yMax = 600.f;
float zNear = 0, zFar = 1000, fov = 45; /// 3. fov = field of view;
glm::mat4 projection;

/// 3. Variabila ce determina schimbarea culorii pixelilor din shader;
int codCol;

/// 3. Valoarea lui PI;
const float PI = 3.141592;

/// 3. Functie care schimba codul culorii conform celor declarate in shader;
void changeColorShader(const int val) {
	codCol = val;
	glUniform1i(codColLocation, codCol);
}


/// 3. Functia de incarcare a texturilor in program;
void LoadTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura; 
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
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

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	/// 3. Atributele varfurilor - COORDONATE (locatia 0), CULORI (locatia 1), COORDONATE DE TEXTURARE (locatia 2);
	static const GLfloat Vertices[] =
	{
		/// Patrat
		/// Coordonate					 Culori            Coordonate de texturare		
		-60.0f, -60.0f,  0.0f,  1.0f,	 1.f, 0.f, 0.f,	   0.f, 0.f,							/// stanga-jos [Red] (indice 0)
		 60.0f, -60.0f,  0.0f,  1.0f,	 0.f, 1.f, 0.f,	   1.f, 0.f,							/// dreapta-jos [Green] (indice 1)
		 60.0f,  60.0f,  0.0f,  1.0f,	 0.f, 0.f, 1.f,	   1.f, 1.f,							/// dreapta-sus [Blue] (indice 2)
		-60.0f,  60.0f,  0.0f,  1.0f,	 1.f, 0.f, 1.f,	   0.f, 1.f								/// stanga-sus [Purple(Magenta)] (indice 3)
	};
	
	/// 3. Indicii care determina ordinea de parcurgere a varfurilor (4 varfuri si 6 indici in total);
	static const GLuint Indices[] =
	{
		0, 1, 2, 3, 0, 2  
	};

	/// 3. Transmiterea datelor prin buffere;
	//  Se creeaza / se leaga un VAO (Vertex Array Object);
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer pentru VARFURI - COORDONATE, CULORI si TEXTURARE;
	glGenBuffers(1, &VboId);													//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);										//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);									//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
	//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  culoare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	//  Se asociaza atributul (2 =  texturare) pentru shader;
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

/// 3. Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI (Coordonate, Culori, Textura), INDICI;
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului (alb);
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;

	/// 3. Incarcarea texturii:
	LoadTexture("text_smiley_face.png");

	CreateShaders(); // Initilizarea shaderelor;

	///	3. Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");

	///	3. Dreptunghiul "decupat"; 
	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);

	/// TRANSLATII:
	/// 3. Vectorul ce cuprinde valori fixate (100, 100, 0) ce corespund coordonatelor (x, y, z);
	glm::vec3 CoordTransl = glm::vec3(100.f, 100.f, 0.f);

	/// 3. Translatia 1: Centrul (punctul in planul 2D corespunzator vectorului declarat "CoordTransl") este translatat in origine;
	matrTransl1 = glm::translate(glm::mat4(1.0f), -CoordTransl);

	/// 3. Translatia 2: Inversa / Opusa translatiei 1;
	matrTransl2 = glm::translate(glm::mat4(1.0f), CoordTransl);

	/// SCALARE:
	/// 3. Vectorul (factorii folositi pentru scalare) ce cuprinde valori fixate (2.f, 0.5f, 1.f) ce corespund coordonatelor (x, y, z);
	glm::vec3 FactorsScale = glm::vec3(2.f, 0.5f, 1.f);

	/// 3. Scalarea care modifica "axaX -> 2 * axaX", "axaY -> axaY / 2", "axaZ -> 1 * axaZ = axaZ";
	matrScale = glm::scale(glm::mat4(1.0f), FactorsScale);
}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT); // Se curata ecranul OpenGL pentru a fi desenat noul continut;

	glLineWidth(5); /// 3. Latimea laturilor ce vor fi desenate;
	glEnable(GL_POINT_SMOOTH);
	glPointSize(6); /// 3. Dimensiunea punctelor ce vor fi desenate;
	
	/// 3. TRANSFORMARI:
	//	Pozitia observatorului;
	glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);
	//	Pozitia punctului de referinta;
	refX = obsX; refY = obsY;
	glm::vec3 refPoint = glm::vec3(refX, refY, -1.0f);
	//	Verticala din planul de vizualizare; 
	glm::vec3 vert = glm::vec3(vX, 1.0f, 0.0f);
	// Matricea de vizualizare
	view = glm::lookAt(obs, refPoint, vert);

	/// 3. Realizarea proiectiei;
	projection = glm::perspective(fov, GLfloat(width) / GLfloat(height), zNear, zFar); 

	///	3. Transmiterea variabilelor uniforme pentru MATRICEA DE TRANSFORMARE, PERSPECTIVA si PROIECTIE spre shadere;
	/// Matrice generica (initializata ca fiind matricea identica I, se pot testa alte valori)
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	/// 3. TEXTURARE
	// Activarea / legarea texturii active
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	//	Transmiterea variabilei uniforme pentru texturare spre shaderul de fragmente;
	glUniform1i(glGetUniformLocation(ProgramId, "myTexture"), 0);

	/// 3. Desenarea primitivelor: Descrierea functiei "glDrawElements()":
	/*
	//	Desenarea primitivelor
	//	Functia glDrawElements primeste 4 argumente:
	//	 - arg1 = modul de desenare;
	//	 - arg2 = numarul de varfuri;
	//	 - arg3 = tipul de date al indicilor;
	//	 - arg4 = pointer spre indici (EBO): pozitia de start a indicilor;
	*/

	changeColorShader(4); /// Brown
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

	/// 3.1. Aplicam intai scalarea si dupa translatia conform enuntului;
	myMatrix = glm::mat4(1.0f) * matrTransl2 * matrScale;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	/// Amestec de culori:
	changeColorShader(6); /// mix(red, blue, 0.75) (albastru mai pronuntat = 3/4, rosu mai putin pronuntat = 1/4)
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

	/// 3.2. Aplicam intai translatia si apoi scalarea conform enuntului;
	myMatrix = glm::mat4(1.0f) * matrScale * matrTransl2;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	/// Desen texturat:
	changeColorShader(7); /// mix(texture(myTexture, tex_Coord), ex_Color, 0.3) (textura mai pronuntata = 7 / 10, iar gradientul mai putin pronuntata = 3 / 10);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));


	glDisable(GL_POINT_SMOOTH);
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
	glutCreateWindow("Indexarea varfurilor - OpenGL <<nou>>");		//	Creeaza fereastra de vizualizare, indicand numele acesteia;

	//	Se initializeaza GLEW si se verifica suportul de extensii OpenGL modern disponibile pe sistemul gazda;
	//  Trebuie initializat inainte de desenare;

	glewInit();
	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	
	/// 3. Asigura rularea continua a randarii;
	glutIdleFunc(RenderFunction);

	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;

	//  Bucla principala de procesare a evenimentelor GLUT (functiile care incep cu glut: glutInit etc.) este pornita;
	//  Prelucreaza evenimentele si deseneaza fereastra OpenGL pana cand utilizatorul o inchide;

	glutMainLoop();

	return 0;
}


