#pragma region
#include "stdafx.h"
#pragma endregion Include
#pragma region

#define PI 3.14159265359
typedef float point3[3]; 
GLfloat density = 0.03; //set the density to 0.3 which is

CCamera Camera;

GLuint * PlanetTexture;
GLuint * skyBoxTexture;

GLfloat light_position[] = {0.0, 0.0, 0.0, 1.0};    
GLfloat light_position1[] = {10.0, 0.0,-30.0, 1.0};  
GLfloat fogColor[4] = {0.6, 0.0, 0.0, 1.0}; //set the for
int numOfPoints =257;
float range = 2000.0f;
float flySpeed = 0.005f;
bool stop=false;

struct Triple{
        int x;
		int y;
		int z;
        Triple(int a,int b,int c)
        {
			x = a;
			y = b;
			z = c;
        }
		Triple()
        {
			x = 0;
			y = 0;
			z = 0;
        }
		void setVec(int a, int b,int c)
		{
			x = a;
			y = b;
			z = c;
		}
};
struct triple
{
	float x;
	float y;
	float z;
	triple(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}
	triple()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	triple operator-(triple other)
	{
		return triple(x - other.x,y - other.y,z - other.z);
	}
	triple operator-()
	{
		return triple(x,y,z);
	}
	triple operator/(float aa)
	{
		return triple(x/aa,y/aa,z/aa);
	}
	triple operator+=(triple other)
	{
		return triple(x + other.x,y + other.y,z + other.z);
	}
};
struct colorUB
{
	unsigned char x;
	unsigned char y;
	unsigned char z;
	colorUB(unsigned char a, unsigned char b, unsigned char c)
	{
		x = a;
		y = b;
		z = c;
	}
	colorUB()
	{
		x = 0;
		y = 0;
		z = 0;
	}
};
colorUB ** colr;


#pragma region
/*************************************************************************************/

 // Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie 
 // FileName, alokuje pamiêæ i zwraca wskaŸnik (pBits) do bufora w którym 
 // umieszczone s¹ dane. 
 // Ponadto udostêpnia szerokoœæ (ImWidth), wysokoœæ (ImHeight) obrazu
 // tekstury oraz dane opisuj¹ce format obrazu wed³ug specyfikacji OpenGL 
 // (ImComponents) i (ImFormat).
 // Jest to bardzo uproszczona wersja funkcji wczytuj¹cej dane z pliku TGA.
 // Dzia³a tylko dla obrazów wykorzystuj¹cych 8, 24, or 32 bitowy kolor.
 // Nie obs³uguje plików w formacie TGA kodowanych z kompresj¹ RLE.


/*************************************************************************************/


GLuint *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{

/*************************************************************************************/ 

 // Struktura dla nag³ówka pliku  TGA 


    #pragma pack(1)            
    typedef struct                        
    {
        GLbyte    idlength;              
        GLbyte    colormaptype;           
        GLbyte    datatypecode;             
        unsigned short    colormapstart;  
        unsigned short    colormaplength; 
        unsigned char     colormapdepth;   
        unsigned short    x_orgin;         
        unsigned short    y_orgin;         
        unsigned short    width;          
        unsigned short    height;         
        GLbyte    bitsperpixel;                   
        GLbyte    descriptor;             
    }TGAHEADER;
    #pragma pack(8)


    FILE *pFile;                    
    TGAHEADER tgaHeader;            
    unsigned long lImageSize;        
    short sDepth;                    
    GLuint    *pbitsperpixel = NULL; 

           
/*************************************************************************************/ 

 // Wartoœci domyœlne zwracane w przypadku b³êdu 

    *ImWidth = 0;                
    *ImHeight = 0;
    *ImFormat = GL_BGR_EXT;
    *ImComponents = GL_RGB8;
    
    fopen_s(&pFile, FileName, "rb");
    if(pFile == NULL)
      return NULL;

/*************************************************************************************/ 


 // Przeczytanie nag³ówka pliku 

    fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
                 

/*************************************************************************************/ 

// Odczytanie szerokoœci, wysokoœci i g³êbi obrazu 

    *ImWidth = tgaHeader.width;
    *ImHeight = tgaHeader.height;
    sDepth = tgaHeader.bitsperpixel / 8;


/*************************************************************************************/

// Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity) 
    
    if(tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
        return NULL;

/*************************************************************************************/ 

// Obliczenie rozmiaru bufora w pamiêci 


    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


/*************************************************************************************/    

// Alokacja pamiêci dla danych obrazu 


     pbitsperpixel = (GLuint*)malloc(lImageSize * sizeof(GLuint));
    
      if(pbitsperpixel == NULL)
        return NULL;

    if(fread(pbitsperpixel, lImageSize, sizeof(GLuint), pFile) != 1)
        {
        free(pbitsperpixel);
        return NULL;
        }
    

/*************************************************************************************/ 

// Ustawienie formatu OpenGL


    switch(sDepth)

        {

        case 3:     

            *ImFormat = GL_BGR_EXT;

            *ImComponents = GL_RGB8;

            break;

        case 4:

            *ImFormat = GL_BGRA_EXT;

            *ImComponents = GL_RGBA8;

            break;

        case 1:

            *ImFormat = GL_LUMINANCE;

            *ImComponents = GL_LUMINANCE8;

            break;

        };

      

    fclose(pFile);

        

    return pbitsperpixel;

}

/*************************************************************************************/

 

 
#pragma endregion Textura




typedef float point3[3]; 

static GLfloat viewer[]= {0.0, 0.0, 10.0}; 
static GLfloat theta = 0.0;   // k?t obrotu obiektu
static GLfloat theta2 = 0.0;
static GLfloat zoom = 1.0;
static GLfloat pix2angle;     // przelicznik pikseli na stopnie
static GLfloat pix2azymut;

static GLfloat elvation;
static GLfloat azymut;
static GLfloat elvation2 = 180;
static GLfloat azymut2;
static GLfloat R=28;
static GLfloat R2=34;

static GLint status = 0;      // stan klawiszy myszy 
static GLint status2 = 0;                              // 0 - nie naci?ni?to ?adnego klawisza
                              // 1 - naci?ni?ty zosta? lewy klawisz

static int x_pos_old=0;       // poprzednia pozycja kursora myszy
static int y_pos_old=0;
static int y_zoom=0;	

float xpos = -100, ypos = 0, zpos = 0, xrot = 0, yrot = 90,zrot = 0, angle=0.0;

float lastx, lasty;

static triple ** planetPoints;
static triple ** planetVector;
static triple ** planetPoints2;
static triple ** planetPoints3;
unsigned char ** all;
unsigned char ** all1;
unsigned char ** all2;

static int delta_x = 0;       // ró?nica pomi?dzy pozycj? bie??c?
                              // i poprzedni? kursora myszy
static int delta_y = 0;    
// inicjalizacja po?o?enia obserwatora 
#pragma endregion Zmienne
#pragma region 
triple normalizeT(triple un)
{
	float len = sqrt(un.x*un.x + un.y*un.y + un.z*un.z);
	return triple(un.x/len,un.y/len,un.z/len);
}
triple crossProduct(triple pq,triple pr)
{
	return triple(pq.y*pr.z - pq.z*pr.y, pq.z*pr.x - pq.x*pr.z,pq.x*pr.y - pq.y*pr.x);
}
triple countPlaneNormal(triple p,triple q,triple r)
{
	return normalizeT(crossProduct(q-p,r-p));
}


float xFun(float u,float v,char what)
{
	return (range +5*what)*0.01f*sinf(u)*cosf(v);
}
float yFun(float u,float v,char what)
{

	return (range +5*what)*0.01f*sinf(u)*sinf(v);
}
float zFun(float u,char what)	
{
	return (range +5*what)*0.01f*cos(u);
}
float xR(float a,float e,float r)
{
	return r*cosf(a)*cosf(e);
}
float yR(float e,float r)
{   
	return r*sin(e);
}
float zR(float a,float e,float r)
{
	return r*sinf(a)*cosf(e);
}

triple ** startupPoints(unsigned char ** dis)
{
	triple ** spherePoints = new triple *[numOfPoints];
	float u =0;
	float v =0;
	float thi = (float)(2*PI)/(float)(numOfPoints-2);
	float fy= (float)(PI)/(float)(numOfPoints-2);
	for(int i=0;i<numOfPoints-1;i++)
	{
		spherePoints[i] = new triple[numOfPoints-1];
		for(int j = 0;j<numOfPoints-1;j++)
		{
			spherePoints[i][j] =  triple(xFun(u,v,dis[i][j]),yFun(u,v,dis[i][j]),zFun(u,dis[i][j]));
			v+=fy;
		}
		u+=thi;
		v = 0;
	}
	return spherePoints;
}
unsigned char ** diamond_squareAlg(int s,int d,int fall, unsigned char b)
{//zwracam tablice wartoœci z zakresu 0-255
	int size = s;
    
	unsigned char ** PointsArray = new  unsigned char * [size];
	colr = new colorUB * [size];

	unsigned char cl1;   //Zmienne wykorzystane przy generacji nowych kolorów
	unsigned char cl2;
	unsigned char cl3;
	unsigned char cl4;
	unsigned char newcl1;
	unsigned char newcl2;
	unsigned char newcl3;
	unsigned char newcl4;
	unsigned char newcl5;
	unsigned char first = 127; //Kolor pierwszego wype³nienie tablicy
	int a; //zmienna wykorzystwana aby sprawdzaæ czy przkroczono zares 0-255
	int dis = d; //stopieñ randomizacji nowych kolorów
	int half_dis = dis/2;
	Triple * squareArray = new Triple[(size-1)*(size-1)]; // a zawieraj¹ca wszystkie aktualnie powsta³e kawadraty
	int squaresIndex = 1; //index aby siê nie zgubiæ w tablicy
	int lastSquare = 0;
	for(int i=0;i<size;i++)
	{
		PointsArray[i] = new  unsigned char[size];
		colr[i] = new colorUB[size];
		for(int j=0;j<size;j++) 
			{
				PointsArray[i][j] = b;
				colr[i][j] = colorUB(rand()%255,rand()%255,rand()%255);
			}
		
	}
	/////////////Ustawienie pocz¹tkowych punktów///////
	PointsArray[0][0] = b;
	PointsArray[0][size-1] = b;
	PointsArray[size-1][size-1] = b;
	PointsArray[size-1][0] = b;
	squareArray[0] = Triple(0,0,size);
    ///////////////////////////////////////////////////
	/////////////Wype³nianie tablicy//////////////////
	while(size>1)
	{
		int count=0;
		dis-=fall;
		if(dis<=0) dis = 1;
		half_dis = dis/2;
		for(int i = lastSquare;i<squaresIndex;i++)
		{
			if(i!=0)
			{
				if(squareArray[i].x + squareArray[i].z ==s-1)
				{
					cl2 = PointsArray[0][squareArray[i].y];
					cl4 = PointsArray[0][squareArray[i].y + squareArray[i].z];
				}
				else if(squareArray[i].y + squareArray[i].z ==s-1)
				{
					cl4 = PointsArray[0][squareArray[i].y + squareArray[i].z];
				}
				else
				{
					cl1 = PointsArray[squareArray[i].x][squareArray[i].y];
					cl2 = PointsArray[squareArray[i].x + squareArray[i].z][squareArray[i].y];
					cl3 = PointsArray[squareArray[i].x][squareArray[i].y + squareArray[i].z];
					cl4 = PointsArray[squareArray[i].x + squareArray[i].z][squareArray[i].y + squareArray[i].z];
				}
			}
			else //dziele rozmiar kwadratów na dwa , a jest on nieparzysty co powoduje b³¹d je¿eli nie ma tego if
			{
			cl1 = PointsArray[squareArray[i].x][squareArray[i].y];
		    cl2 = PointsArray[squareArray[i].x + squareArray[i].z-1][squareArray[i].y];
			cl3 = PointsArray[squareArray[i].x][squareArray[i].y + squareArray[i].z-1];
			cl4 = PointsArray[squareArray[i].x + squareArray[i].z-1][squareArray[i].y + squareArray[i].z-1];
			}
			//////////////////////////////////////////////Colors
			//Tworzenie nowych kolorów i sprawdzenie czy mieszcz¹ siê one w zakresie
			a = (cl1+ cl2 + cl3 + cl4)/4 + (rand()%dis) - half_dis;
			if(a>255) a = 255;
			if(a<0) a =0;
			newcl1 = a;
			a = (cl1+ cl3)/2 +(rand()%dis) -half_dis; 
			if(a>255) a = 255;
			if(a<0) a =0;
			newcl2 = a;
			a = (cl1+ cl2)/2+  (rand()%dis) -half_dis; 
			if(a>255) a = 255;
			if(a<0) a =0;
			newcl3 = a;
			a = (cl2+ cl4)/2+  (rand()%dis) - half_dis; 
			if(a>255) a = 255;
			if(a<0) a =0;
			newcl4 = a;
			a = (cl3 +  cl4)/2+  (rand()%dis) -half_dis;
			if(a>255) a = 255;
			if(a<0) a =0;
			newcl5 = a;

			
			//////////////////////////////////////////////
			//Diamond midpoint
			PointsArray[squareArray[i].x + squareArray[i].z/2][squareArray[i].y + squareArray[i].z/2] = newcl1;
			//Square
			PointsArray[squareArray[i].x][squareArray[i].y + squareArray[i].z/2] = newcl2;
			PointsArray[squareArray[i].x+ squareArray[i].z/2][squareArray[i].y] = newcl3;
			PointsArray[squareArray[i].x+ squareArray[i].z-1][squareArray[i].y+ squareArray[i].z/2] = newcl4;
			PointsArray[squareArray[i].x+ squareArray[i].z/2][squareArray[i].y + squareArray[i].z-1] = newcl5;
			
			if(size/2>1)
			{
			//Dodanie nowo utworzonych kwadratów do tablicy
			squareArray[squaresIndex + count] = Triple(squareArray[i].x,squareArray[i].y,size/2);
			squareArray[squaresIndex + count + 1] = Triple(squareArray[i].x,squareArray[i].y + squareArray[i].z/2,size/2);
			squareArray[squaresIndex + count + 2] = Triple(squareArray[i].x + squareArray[i].z/2,squareArray[i].y + squareArray[i].z/2,size/2);
			squareArray[squaresIndex + count + 3] = Triple(squareArray[i].x + squareArray[i].z/2,squareArray[i].y,size/2);
			}
			count+=4;
			lastSquare++;
		}
		size/=2;
		squaresIndex+=count;
	}
	return PointsArray;
	delete []squareArray;
}

/*
Normalne liczone sa natepujaco
->liczymy wetory normalne dla 4 p³aszczyz zawierajacych dany punkt
->srednia
->normalizacja
*/
triple ** processPlanetNormalVectors(triple ** a)
{
	triple ** vectors = new triple *[numOfPoints];
	for(int i=0;i<numOfPoints-1;i++)
	{
		vectors[i] = new triple[numOfPoints];
		for(int j = 0;j<numOfPoints-1;j++)
		{
			if(i>1 && j>1)
			{
			vectors[i][j] = countPlaneNormal(
				a[i][j],
				a[(i-1)%(numOfPoints-1)][(j+1)%(numOfPoints-1)],  ///1
				a[(i+1)%(numOfPoints-1)][(j+1)%(numOfPoints-1)]);
			vectors[i][j] += countPlaneNormal(
				a[i][j],
				a[(i+1)%(numOfPoints-1)][(j-1)%(numOfPoints-1)],  ///1
				a[(i+1)%(numOfPoints-1)][(j+1)%(numOfPoints-1)]);
			vectors[i][j] += countPlaneNormal(
				a[i][j],
				a[(i-1)%(numOfPoints-1)][(j-1)%(numOfPoints-1)],  ///1
				a[(i+1)%(numOfPoints-1)][(j-1)%(numOfPoints-1)]);
			vectors[i][j] += countPlaneNormal(
				a[i][j],
				a[(i-1)%(numOfPoints-1)][(j+1)%(numOfPoints-1)],  ///1
				a[(i-1)%(numOfPoints-1)][(j+1)%(numOfPoints-1)]);
			vectors[i][j] = vectors[i][j]/4;
		    vectors[i][j] = normalizeT(vectors[i][j]);
			}
			else
			{
				vectors[i][j] = countPlaneNormal(
				a[i][j],
				a[(i+1)%(numOfPoints-1)][(j)%(numOfPoints-1)],  ///1
				a[(i)%(numOfPoints-1)][(j+1)%(numOfPoints-1)]);
			}

			if((i>numOfPoints-1)/2) vectors[i][j] = -vectors[i][j];
		}
	}
	return vectors;
}

//Pobieranie odpowiednich wartosci punktów i normalych
//i przekazanie ich funkcji rusujacej
void SphereCreator(triple ** a)
{
glBegin(GL_TRIANGLES);
for(int i=0;i<numOfPoints-1;i++)
	 {
		 for(int j=0;j<numOfPoints-1;j++)
		 {
		 if(i<numOfPoints-2 && j<numOfPoints-2)
		 {
		 if(i<(numOfPoints-1)/2)
		 {
             glTexCoord2f((float)(i)/numOfPoints,(float)(j)/numOfPoints);

			 glNormal3f(planetVector[i][j].x,planetVector[i][j].y,planetVector[i][j].z);
			 glVertex3f(a[i][j].x  , a[i][j].y  , a[i][j].z);  
			

             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j)/numOfPoints);
			 glNormal3f(planetVector[i+1][j].x,planetVector[i+1][j].y,planetVector[i+1][j].z);
			 glVertex3f(a[i+1][j].x  , a[i+1][j].y  , a[i+1][j].z); 
			 
             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j+1)/numOfPoints);

			 glNormal3f(planetVector[i+1][j+1].x,planetVector[i+1][j+1].y,planetVector[i+1][j+1].z);
			 glVertex3f(a[i+1][j+1].x  , a[i+1][j+1].y , a[i+1][j+1].z); 
			 
             glTexCoord2f((float)(i)/numOfPoints,(float)(j)/numOfPoints);

			 glNormal3f(planetVector[i][j].x,planetVector[i][j].y,planetVector[i][j].z);
			 glVertex3f(a[i][j].x  , a[i][j].y , a[i][j].z); 
			 
             glTexCoord2f((float)(i)/numOfPoints,(float)(j+1)/numOfPoints);

			 glNormal3f(planetVector[i][j+1].x,planetVector[i][j+1].y,planetVector[i][j+1].z);
			 glVertex3f(a[i][j+1].x  , a[i][j+1].y  , a[i][j+1].z);

             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j+1)/numOfPoints);
			 
			 glNormal3f(planetVector[i+1][j+1].x,planetVector[i+1][j+1].y,planetVector[i+1][j+1].z);
			 glVertex3f(a[i+1][j+1].x , a[i+1][j+1].y , a[i+1][j+1].z);
		 }
		 else
			 {
             glTexCoord2f((float)(i)/numOfPoints,(float)(j)/numOfPoints);

			 glNormal3f(planetVector[i][j].x,planetVector[i][j].y,planetVector[i][j].z);
			 glVertex3f(a[i][j].x , a[i][j].y  , a[i][j].z);  
			 
             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j)/numOfPoints);

			 glNormal3f(planetVector[i+1][j].x,planetVector[i+1][j].y,planetVector[i+1][j].z);
			 glVertex3f(a[i+1][j].x  , a[i+1][j].y  , a[i+1][j].z); 

             glTexCoord2f((float)(i)/numOfPoints,(float)(j+1)/numOfPoints);

			 glNormal3f(planetVector[i][j+1].x,planetVector[i][j+1].y,planetVector[i][j+1].z);
			 glVertex3f(a[i][j+1].x  , a[i][j+1].y  , a[i][j+1].z);

             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j+1)/numOfPoints);

			 glNormal3f(planetVector[i+1][j+1].x,planetVector[i+1][j+1].y,planetVector[i+1][j+1].z);
			 glVertex3f(a[i+1][j+1].x  , a[i+1][j+1].y  , a[i+1][j+1].z);
			 
             glTexCoord2f((float)(i+1)/numOfPoints,(float)(j)/numOfPoints);

			 glNormal3f(planetVector[i+1][j].x,planetVector[i+1][j].y,planetVector[i+1][j].z);
			 glVertex3f(a[i+1][j].x  , a[i+1][j].y  , a[i+1][j].z); 
			 
             glTexCoord2f((float)(i)/numOfPoints,(float)(j+1)/numOfPoints);

			 glNormal3f(planetVector[i][j+1].x,planetVector[i][j+1].y,planetVector[i][j+1].z);
			 glVertex3f(a[i][j+1].x , a[i][j+1].y , a[i][j+1].z);

	 }              
	 }
		}
 }
	glEnd();

}


/*************************************************************************************/

// Funkcja okre?laj?ca co ma by? rysowane (zawsze wywo?ywana, gdy trzeba 
// przerysowa? scen?)
#pragma endregion Rysujace
#pragma region
void KeyDown(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:		//ESC
		PostQuitMessage(0);
		break;
	case '0':		//ESC
		stop = !stop;
		break;
	case '4':		
		Camera.RotateY(1.5);		
		break;
	case '6':	
		Camera.RotateY(-1.5);		
		break;
	case 'w':		
		Camera.MoveForward( -1 ) ;		
		break;
	case 's':		
		Camera.MoveForward( 1 ) ;		
		break;
	case '8':		
		Camera.RotateX(1.5);		
		break;
	case '2':		
		Camera.RotateX(-1.5);		
		break;
	case 'c':		
		Camera.StrafeRight(-0.1);	
		break;
	case 'v':		
		Camera.StrafeRight(0.1);		
		break;
	case 'f':
		Camera.MoveUpward(-0.03);		
		break;
	case 'r':
		Camera.MoveUpward(0.03);		
		break;
	case '1':
		Camera.RotateZ(-5.0);		
		break;
	case '3':
		Camera.RotateZ(5.0);		
		break;

	}
	glutPostRedisplay();
}
void spin()
{
	if(!stop)
		{
			Camera.MoveForward(-0.02);
			glutPostRedisplay(); //odœwie¿enie zawartoœci aktualnego okna
		}
	azymut+=0.01f;
	elvation+=0.001f;
	azymut2+=0.005f;
	elvation2+=0.01f;
	glutPostRedisplay();
}
#pragma endregion Input
#pragma region
void RenderScene(void)
{
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// Czyszczenie okna aktualnym kolorem czyszcz?cym

   if(elvation <= 0) 
	        elvation += 2*PI;
   if(elvation >= 2*PI) 
	    elvation -= 2*PI;
   if(azymut <= 0) 
        azymut += 2*PI;
   if(azymut >= 2*PI ) 
        azymut -= 2*PI;
   light_position[0] = xR(azymut,elvation,R);
   light_position[1] = yR(elvation,R);
   light_position[2] = zR(azymut,elvation,R);

   if(elvation2 <= 0) 
	        elvation2 += 2*PI;
   if(elvation2 >= 2*PI) 
	    elvation2 -= 2*PI;
   if(azymut2 <= 0) 
        azymut2 += 2*PI;
   if(azymut2 >= 2*PI ) 
        azymut2 -= 2*PI;
   light_position1[0] = xR(azymut2,elvation2,R2);
   light_position1[1] = yR(elvation2,R2);
   light_position1[2] = zR(azymut2,elvation2,R2);


glLightfv(GL_LIGHT0, GL_POSITION, light_position);
glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

glLoadIdentity();
Camera.Render();

glRotatef(90,1,0,0);
SphereCreator(planetPoints);


glTranslatef(light_position[0],light_position[1],light_position[2]);
glutSolidSphere(2,20,20);
glTranslatef(-light_position[0],-light_position[1],-light_position[2]);
glTranslatef(light_position1[0],light_position1[1],light_position1[2]);
glutSolidSphere(1.5,20,20);

glFlush();

glutSwapBuffers();

 }
void MyInit(void)
{
	/*************************************************************************************/ 
 GLuint *pBytes;
 GLint ImWidth, ImHeight, ImComponents;
 GLenum ImFormat;
//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga

 pBytes = LoadTGAImage("a.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

 /*************************************************************************************/

// Zdefiniowanie tekstury 2-D 

/*************************************************************************************/
 PlanetTexture = pBytes;
 skyBoxTexture = new GLuint[7];

 glGenTextures(1, &skyBoxTexture[0]);
 glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);
 

// Zwolnienie pamiêci
       
 free(pBytes);


/*************************************************************************************/

// W³¹czenie mechanizmu teksturowania

 glEnable(GL_TEXTURE_2D);

/*************************************************************************************/

// Ustalenie trybu teksturowania

 glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

/*************************************************************************************/

// Okreœlenie sposobu nak³adania tekstur

 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
     


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GLfloat mat_ambient[]  = {1.0, 1.0, 1.0, 1.0};        
    GLfloat mat_diffuse[]  = {1.0, 1.0, 0.0, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess  = {50.0};
    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0}; 
    GLfloat light_diffuse1[] = {0.0, 1.0, 0.0, 1.0};        
    GLfloat light_specular[]= {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular1[]= {1.0, 1.0, 1.0, 1.0};
    GLfloat att_constant  = {1.0};
    GLfloat att_linear    = {0.0005f}; 
    GLfloat att_quadratic  = {0.0001f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);
/*************************************************************************************/
    glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
    glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny 
    glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora 
/*************************************************************************************/

}
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    glMatrixMode(GL_PROJECTION);
    // Prze??czenie macierzy bie??cej na macierz projekcji	
    glLoadIdentity();
    // Czyszcznie macierzy bie??cej 
    gluPerspective(70, 1.0, 1.0, 500.0);
    if(horizontal <= vertical)
        glViewport(0, (vertical-horizontal)/2, horizontal, horizontal);
    else
        glViewport((horizontal-vertical)/2, 0, vertical, vertical);
    // Ustawienie wielko?ci okna okna widoku (viewport) w zale?no?ci
    // relacji pomi?dzy wysoko?ci? i szeroko?ci? okna	
    glMatrixMode(GL_MODELVIEW);
    // Prze??czenie macierzy bie??cej na macierz widoku modelu  
    glLoadIdentity();
    // Czyszczenie macierzy bie??cej 

}

/*************************************************************************************/
void main(void)
{           
glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH);
glutInitWindowSize(300, 300);
srand(time(NULL));
glutCreateWindow("Rzutowanie perspektywiczne");
Camera.Move( F3dVector(0.0, 0.0, 50.0 ));
glutDisplayFunc (RenderScene);            
glutIdleFunc (spin);
glutReshapeFunc(ChangeSize);
glutKeyboardFunc (KeyDown);
all = diamond_squareAlg(numOfPoints,100,12,10);
planetPoints = startupPoints(all);
planetVector = processPlanetNormalVectors(planetPoints);


MyInit();

glEnable(GL_DEPTH_TEST);

glutMainLoop();

}

#pragma endregion Glut


 
 

