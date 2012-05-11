#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include "minifmod.h"
#include "resource.h"

#include "timer.h"
Timer *timer;
GLfloat t_g;

#define PI 3.14159265358// pi
#define PID PI/180.0    // pi ratio
#define CR 0.00390625		// color ratio
#define SNG true				// music flag
#ifdef SNG
	FMUSIC_MODULE *mod;		// music handle
	int ord=0;						// pattern order
	int row=0;						// row number
#endif

HDC				hDC=NULL;			// GDI device context
HGLRC			hRC=NULL;			// rendering context
HWND			hWnd=NULL;		// window handle
HINSTANCE	hInstance;		// instance application

int  keys[256];					// keyboard array
int	 active=true;				// window active flag
int  fullscreen=true;		// fullscreen flag
char *name="GLREZ";			// window name
GLfloat fov=80.0f;			// field of view angle
GLfloat nearplane=1.0f;	// nearplane
GLfloat farplane=200.0f;// farplane

/* window variable		*/
GLint w_w=800;		// width
GLint w_h=600;		// height
GLint w_c=32;			// color depth
GLint w_d=16;			// depth buffer

/* light variable			*/
GLfloat l_a[4];		// ambient light
GLfloat l_d[4];		// diffuse light
GLfloat l_p[4];		// position

/* object variable		*/
GLfloat	p_x;			// position x
GLfloat	p_y;			// position y
GLfloat	p_z;			// position z
GLfloat	a_x;			// angle x
GLfloat	a_y;			// angle y
GLfloat	a_z;			// angle z
GLfloat	r_y;			// rotation y

/* color variable			*/
GLint b_r=128;		// red value
GLint b_g=128;		// green value
GLint b_b=128;		// blue value
GLint nr,ng,nb;		// color values

/* fog variable				*/
GLfloat f_c[4];		// fog color
GLfloat f_v1;			// level 1
GLfloat f_v2;			// level 2
GLfloat f_n;			// level (new)
GLfloat f_t1;			// synchro time 1
GLfloat f_t2;			// synchro time 2

/* REZ variable				*/
int r_f=false;		// flag
GLint r_l=12;			// loop number
GLint r_n=8;			// ammount by loop
GLint r_r;				// color red
GLint r_g;				// color green
GLint r_b;				// color blue
GLuint r_d1;			// display list 1
GLuint r_d2;			// display list 2
GLuint r_d3;			// display list 3
GLuint r_d4;			// display list 4
GLuint r_d5;			// display list 5
GLuint r_d6;			// display list 6 (ring)
GLuint r_d7;			// display list 7 (particle)

/* cube variable			*/
GLfloat c_y=-3.85f;	// cube position y
GLfloat c_z=-15.0f;	// cube position z
GLfloat c_j;			// jump
GLfloat c_r;			// jump angle
GLfloat c_t;			// synchro time

/* zoom variable			*/
int z_f=false;		// flag
GLfloat z_z=10.0f;// zoom
GLfloat z_r=0.0f;	// zoom angle
GLfloat z_a=0.0f;	// main angle
GLfloat z_t;			// synchro time

/* jump variable			*/
int j_f=false;		// flag
GLfloat j_y=3.85f;// jump
GLfloat j_r=0.0f;	// jump angle
GLfloat j_t;			// synchro time

/* glenz variable			*/
int g_f=false;		// flag
GLint g_n=15;			// ammount

/* mapping						*/
int m_f=false;		// flag
GLfloat m_r;			// repeat ratio
GLfloat m_a;			// rotation angle
GLfloat m_t;			// synchro time
GLuint m_c;				// cube texture

/* particle variable	*/
int p_f=false;		// flag
GLfloat p_n=512;	// ammount
typedef struct
	{
	GLfloat d,y,a;
	GLint z,r,g,b;
	} PAR;
PAR par[512];

/* title						*/
int t_f=false;		// flag

/* liner variable			*/
int l_f=false;		// flag
int l_c;					// ascii code
int l_t;					// text length
int l_n;					// line number
int l_m;					// line max length
int l_l;					// line increment
int l_i;					// char increment
GLfloat l_w;			// width
GLfloat l_h;			// height
GLfloat l_s=0.025f;	// size
char *txt="REZ IS PROUD\r TO PRESENT\rA LITTLE 20K\rOPENGL INTRO\r";

int i,j;
GLfloat a;

LRESULT	CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);	// wndProc declaration

static PIXELFORMATDESCRIPTOR pfd=
	{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,									// version number
	PFD_DRAW_TO_WINDOW|	// format must support window
	PFD_SUPPORT_OPENGL|	// format must support openGL
	PFD_DOUBLEBUFFER,		// must support double buffering
	PFD_TYPE_RGBA,			// request an RGBA format
	w_c,								// select our color depth
	0,0,0,0,0,0,				// color bits ignored
	0,									// no alpha buffer
	0,									// shift bit ignored
	0,									// no accumulation buffer
	0,0,0,0,						// accumulation bits ignored
	w_d,								// z-buffer (depth buffer)
	0,									// no stencil buffer
	0,									// no auxiliary buffer
	PFD_MAIN_PLANE,			// main drawing layer
	0,									// reserved
	0,0,0								// layer masks ignored
	};

#ifdef SNG

	typedef struct 
		{
		int length,pos;
		void *data;
		} MEMFILE;

	MEMFILE *memfile;
	HRSRC	rec;	

	unsigned int memopen(char *name)
		{
		HGLOBAL	handle;
		// added by lightshow
		memfile=(MEMFILE *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(MEMFILE));
		rec=FindResource(NULL,name,RT_RCDATA);
		handle=LoadResource(NULL,rec);
		memfile->data=LockResource(handle);
		memfile->length=SizeofResource(NULL,rec);
		memfile->pos=0;
		return (unsigned int)memfile;
		}

	void memclose(unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		// added by lightshow
		GlobalFree(memfile);
		}

	int memread(void *buffer, int size, unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		if(memfile->pos+size>=memfile->length) size=memfile->length-memfile->pos;
		memcpy(buffer,(char *)memfile->data+memfile->pos,size);
		memfile->pos+=size;	
		return size;
		}

	void memseek(unsigned int handle,int pos,signed char mode)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		if(mode==SEEK_SET) 
			memfile->pos=pos;
		else if(mode==SEEK_CUR) 
			memfile->pos+=pos;
		else if(mode==SEEK_END)
			memfile->pos=memfile->length+pos;
		if(memfile->pos>memfile->length)
			memfile->pos=memfile->length;
		}

	int memtell(unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		return memfile->pos;
		}

	void load_song()
		{
		FSOUND_File_SetCallbacks(memopen,memclose,memread,memseek,memtell);
		mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_DATA1),NULL);
		FMUSIC_PlaySong(mod);
		}

#endif

int load_tex(WORD file,GLint filter,GLint clamp)
	{
	HBITMAP hBMP;	// bitmap handle
	BITMAP BMP;		// bitmap structure
	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(file),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	if(hBMP)
		{	
		GetObject(hBMP,sizeof(BMP),&BMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glBindTexture(GL_TEXTURE_2D,file);
		glTexImage2D(GL_TEXTURE_2D,0,3,BMP.bmWidth,BMP.bmHeight,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,BMP.bmBits);
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,BMP.bmWidth,BMP.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,BMP.bmBits);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
		DeleteObject(hBMP);
		}
	return 0;
	}

void calc_rgb(GLint r,GLint g,GLint b,GLfloat l)
	{
	GLfloat nr,ng,nb;
	nr=(GLfloat)(CR*r); if(nr>1) nr=1; if(nr<0) nr=0;
	ng=(GLfloat)(CR*g); if(ng>1) ng=1; if(ng<0) nr=0;
	nb=(GLfloat)(CR*b); if(nb>1) nb=1; if(nb<0) nr=0;	
	glColor4f(nr,ng,nb,l);
	}

void gen_rez_r(GLfloat d)
	{
	glBegin(GL_QUADS);
		glVertex3f( 0.0f, 0.0f,d);
		glVertex3f( 4.0f, 0.0f,d);
		glVertex3f( 3.0f,-1.0f,d);
		glVertex3f( 1.0f,-1.0f,d);
		glVertex3f( 4.0f, 0.0f,d);
		glVertex3f( 4.0f,-3.0f,d);
		glVertex3f( 3.0f,-2.0f,d);
		glVertex3f( 3.0f,-1.0f,d);
		glVertex3f( 3.0f,-2.0f,d);
		glVertex3f( 4.0f,-3.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 1.0f,-2.0f,d);
		glVertex3f( 0.0f, 0.0f,d);
		glVertex3f( 1.0f,-1.0f,d);
		glVertex3f( 1.0f,-2.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 1.0f,-3.0f,d);
		glVertex3f( 1.0f,-5.0f,d);
		glVertex3f( 0.0f,-5.0f,d);
		glVertex3f( 2.0f,-3.0f,d);
		glVertex3f( 3.0f,-3.0f,d);
		glVertex3f( 4.0f,-5.0f,d);
		glVertex3f( 3.0f,-5.0f,d);
	glEnd();
	}

void gen_rez_e(GLfloat d)
	{
	glBegin(GL_QUADS);
		glVertex3f( 0.0f, 0.0f,d);
		glVertex3f( 4.0f, 0.0f,d);
		glVertex3f( 3.0f,-1.0f,d);
		glVertex3f( 1.0f,-1.0f,d);
		glVertex3f( 4.0f, 0.0f,d);
		glVertex3f( 4.0f,-3.0f,d);
		glVertex3f( 3.0f,-2.0f,d);
		glVertex3f( 3.0f,-1.0f,d);
		glVertex3f( 3.0f,-2.0f,d);
		glVertex3f( 4.0f,-3.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 1.0f,-2.0f,d);
		glVertex3f( 0.0f, 0.0f,d);
		glVertex3f( 1.0f,-1.0f,d);
		glVertex3f( 1.0f,-2.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 0.0f,-3.0f,d);
		glVertex3f( 1.0f,-3.0f,d);
		glVertex3f( 1.0f,-5.0f,d);
		glVertex3f( 0.0f,-5.0f,d);
		glVertex3f( 1.0f,-4.0f,d);
		glVertex3f( 4.0f,-4.0f,d);
		glVertex3f( 4.0f,-5.0f,d);
		glVertex3f( 1.0f,-5.0f,d);
	glEnd();
	}

void gen_rez_z(GLfloat d)
	{
	glBegin(GL_QUADS);
		glVertex3f( 0.0f,-0.0f,d);
		glVertex3f( 4.0f,-0.0f,d);
		glVertex3f( 4.0f,-1.0f,d);
		glVertex3f( 0.0f,-1.0f,d);
		glVertex3f(2.75f,-1.0f,d);
		glVertex3f( 4.0f,-1.0f,d);
		glVertex3f(1.25f,-4.0f,d);
		glVertex3f( 0.0f,-4.0f,d);
		glVertex3f( 0.0f,-4.0f,d);
		glVertex3f( 4.0f,-4.0f,d);
		glVertex3f( 4.0f,-5.0f,d);
		glVertex3f( 0.0f,-5.0f,d);
	glEnd();	
	}

void gen_rez()
	{	
	GLfloat d=1.0f;	

	r_d1=glGenLists(5);
	glNewList(r_d1,GL_COMPILE);
		gen_rez_r(-d);
		gen_rez_r( d);
		glTranslatef(5.0f,0.0f,0.0f);
		gen_rez_e(-d);
		gen_rez_e( d);
		glTranslatef(5.0f,0.0f,0.0f);
		gen_rez_z(-d);
		gen_rez_z( d);
	glEndList();

	r_d2=r_d1+1;
	glNewList(r_d2,GL_COMPILE);
		glTranslatef(-10.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f, 0.0f, d);
			glVertex3f( 4.0f, 0.0f, d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f( 1.0f,-2.0f, d);
			glVertex3f( 3.0f,-2.0f, d);
			glVertex3f( 3.0f,-2.0f,-d);
			glVertex3f( 1.0f,-2.0f,-d);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f( 3.0f,-3.0f,	d);
			glVertex3f( 4.0f,-5.0f, d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f( 3.0f,-3.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f, 0.0f,	d);
			glVertex3f( 4.0f, 0.0f, d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f( 1.0f,-4.0f,	d);
			glVertex3f( 4.0f,-4.0f, d);
			glVertex3f( 4.0f,-4.0f,-d);
			glVertex3f( 1.0f,-4.0f,-d);
			glVertex3f( 1.0f,-2.0f,	d);
			glVertex3f( 3.0f,-2.0f, d);
			glVertex3f( 3.0f,-2.0f,-d);
			glVertex3f( 1.0f,-2.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f, 0.0f, d);
			glVertex3f( 4.0f, 0.0f, d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f(1.25f,-4.0f, d);
			glVertex3f( 4.0f,-4.0f, d);
			glVertex3f( 4.0f,-4.0f,-d);
			glVertex3f(1.25f,-4.0f,-d);
		glEnd();
	glEndList();

	r_d3=r_d2+1;
	glNewList(r_d3,GL_COMPILE);
		glTranslatef(-10.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 4.0f,-3.0f,	d);
			glVertex3f( 3.0f,-3.0f, d);
			glVertex3f( 3.0f,-3.0f,-d);
			glVertex3f( 4.0f,-3.0f,-d);
			glVertex3f( 4.0f,-5.0f,	d);
			glVertex3f( 3.0f,-5.0f, d);
			glVertex3f( 3.0f,-5.0f,-d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f( 2.0f,-3.0f,	d);
			glVertex3f( 1.0f,-3.0f, d);
			glVertex3f( 1.0f,-3.0f,-d);
			glVertex3f( 2.0f,-3.0f,-d);
			glVertex3f( 1.0f,-5.0f,	d);
			glVertex3f( 0.0f,-5.0f, d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 1.0f,-5.0f,-d);
			glVertex3f( 3.0f,-1.0f,	d);
			glVertex3f( 1.0f,-1.0f, d);
			glVertex3f( 1.0f,-1.0f,-d);
			glVertex3f( 3.0f,-1.0f,-d);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f( 2.0f,-3.0f,	d);
			glVertex3f( 2.0f,-3.0f, d);
			glVertex3f( 3.0f,-5.0f,-d);
			glVertex3f( 2.0f,-3.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 4.0f,-3.0f,	d);
			glVertex3f( 1.0f,-3.0f, d);
			glVertex3f( 1.0f,-3.0f,-d);
			glVertex3f( 4.0f,-3.0f,-d);
			glVertex3f( 4.0f,-5.0f,	d);
			glVertex3f( 0.0f,-5.0f, d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f( 3.0f,-1.0f,	d);
			glVertex3f( 1.0f,-1.0f, d);
			glVertex3f( 1.0f,-1.0f,-d);
			glVertex3f( 3.0f,-1.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 4.0f,-5.0f, d);
			glVertex3f( 0.0f,-5.0f, d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f(2.75f,-1.0f, d);
			glVertex3f( 0.0f,-1.0f, d);
			glVertex3f( 0.0f,-1.0f,-d);
			glVertex3f(2.75f,-1.0f,-d);
		glEnd();
		glBegin(GL_QUADS);		
			glVertex3f( 4.0f,-1.0f, d);
			glVertex3f(1.25f,-4.0f, d);
			glVertex3f(1.25f,-4.0f,-d);
			glVertex3f( 4.0f,-1.0f,-d);
		glEnd();
	glEndList();

	r_d4=r_d3+1;
	glNewList(r_d4,GL_COMPILE);
		glTranslatef(-10.0f,0.0f,0.0f);
		glBegin(GL_QUADS);	
			glVertex3f( 4.0f, 0.0f,	d);
			glVertex3f( 4.0f,-3.0f, d);
			glVertex3f( 4.0f,-3.0f,-d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 1.0f,-3.0f,	d);
			glVertex3f( 1.0f,-5.0f, d);
			glVertex3f( 1.0f,-5.0f,-d);
			glVertex3f( 1.0f,-3.0f,-d);
			glVertex3f( 1.0f,-2.0f,	d);
			glVertex3f( 1.0f,-1.0f, d);
			glVertex3f( 1.0f,-1.0f,-d);
			glVertex3f( 1.0f,-2.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 4.0f, 0.0f,	d);
			glVertex3f( 4.0f,-3.0f, d);
			glVertex3f( 4.0f,-3.0f,-d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 1.0f,-3.0f,	d);
			glVertex3f( 1.0f,-4.0f, d);
			glVertex3f( 1.0f,-4.0f,-d);
			glVertex3f( 1.0f,-3.0f,-d);
			glVertex3f( 4.0f,-4.0f,	d);
			glVertex3f( 4.0f,-5.0f, d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f( 4.0f,-4.0f,-d);
			glVertex3f( 1.0f,-2.0f,	d);
			glVertex3f( 1.0f,-1.0f, d);
			glVertex3f( 1.0f,-1.0f,-d);
			glVertex3f( 1.0f,-2.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 4.0f, 0.0f, d);
			glVertex3f( 4.0f,-1.0f, d);
			glVertex3f( 4.0f,-1.0f,-d);
			glVertex3f( 4.0f, 0.0f,-d);
			glVertex3f( 4.0f,-4.0f, d);
			glVertex3f( 4.0f,-5.0f, d);
			glVertex3f( 4.0f,-5.0f,-d);
			glVertex3f( 4.0f,-4.0f,-d);
		glEnd();
	glEndList();

	r_d5=r_d4+1;
	glNewList(r_d5,GL_COMPILE);
		glTranslatef(-10.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f,-5.0f,	d);
			glVertex3f( 0.0f, 0.0f, d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 3.0f,-1.0f,	d);
			glVertex3f( 3.0f,-2.0f, d);
			glVertex3f( 3.0f,-2.0f,-d);
			glVertex3f( 3.0f,-1.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f,-5.0f,	d);
			glVertex3f( 0.0f, 0.0f, d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 3.0f,-1.0f,	d);
			glVertex3f( 3.0f,-2.0f, d);
			glVertex3f( 3.0f,-2.0f,-d);
			glVertex3f( 3.0f,-1.0f,-d);
		glEnd();
		glTranslatef(5.0f,0.0f,0.0f);
		glBegin(GL_QUADS);
			glVertex3f( 0.0f,-5.0f, d);
			glVertex3f( 0.0f,-4.0f, d);
			glVertex3f( 0.0f,-4.0f,-d);
			glVertex3f( 0.0f,-5.0f,-d);
			glVertex3f( 0.0f,-1.0f, d);
			glVertex3f( 0.0f, 0.0f, d);
			glVertex3f( 0.0f, 0.0f,-d);
			glVertex3f( 0.0f,-1.0f,-d);
		glEnd();
		glBegin(GL_QUADS);
			glVertex3f( 0.0f,-4.0f, d);
			glVertex3f(2.75f,-1.0f, d);
			glVertex3f(2.75f,-1.0f,-d);
			glVertex3f( 0.0f,-4.0f,-d);
		glEnd();
	glEndList();
	}

void dsp_rez()
	{
	glLoadIdentity();
	glTranslatef(p_x,p_y,p_z);
	glRotatef(a_x,1.0f,0.0f,0.0f);
	glRotatef(a_y,0.0f,1.0f,0.0f);
	glRotatef(a_z,0.0f,0.0f,1.0f);
	glTranslatef(-7.0f,2.5f,0.0f);
	r_r=(GLint)(nr+p_x*1.5+16.0*cos(a/8.0));
	r_g=(GLint)(ng);
	r_b=(GLint)(nb+p_y*1.5-16.0*sin(a/8.0));
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	calc_rgb(r_r,r_g,r_b,0.65f);
	glCallList(r_d1);
	calc_rgb(r_r+64,r_g+64,r_b+64,0.65f);
	glCallList(r_d2);
	calc_rgb(r_r-64,r_g-64,r_b-64,0.65f);
	glCallList(r_d3);
	calc_rgb(r_r+32,r_g+32,r_b+32,0.65f);
	glCallList(r_d4);
	calc_rgb(r_r-32,r_g-32,r_b-32,0.65f);
	glCallList(r_d5);			
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);			
	calc_rgb(r_r-80,r_g-80,r_b-80,0.65f);
	glTranslatef(-10.0f,0.0f,0.0f);
	glCallList(r_d1);
	glCallList(r_d2);
	glCallList(r_d3);
	glCallList(r_d4);
	glCallList(r_d5);
	}

void cube_side(GLfloat d,GLfloat x,GLfloat y,GLfloat c,GLfloat s)
	{
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,-1.0f);
		glTexCoord2f((GLfloat)(x+c),(GLfloat)(y-s)); glVertex3f(-d, d, d);
		glTexCoord2f((GLfloat)(x-s),(GLfloat)(y-c)); glVertex3f( d, d, d);
		glTexCoord2f((GLfloat)(x-c),(GLfloat)(y+s)); glVertex3f( d,-d, d);
		glTexCoord2f((GLfloat)(x+s),(GLfloat)(y+c)); glVertex3f(-d,-d, d);
	glEnd();
	}

void gen_cube(GLfloat d,GLint r,GLint g,GLint b,GLfloat z,GLfloat l)
	{
	GLfloat w[4]={(GLfloat)(CR*r),(GLfloat)(CR*g),(GLfloat)(CR*b),l};
	GLfloat t=(GLfloat)(3.99-4.0*cos((m_a*z-4)*PID));
	GLfloat c=(GLfloat)(t*cos((m_a*z)*PID));
	GLfloat s=(GLfloat)(t*sin((m_a*z)*PID));
	GLfloat x=(GLfloat)(0.48);
	GLfloat y=(GLfloat)(0.48);

	glLoadIdentity();
	glTranslatef(p_x,p_y,p_z);

	glRotatef(a_x,1.0f,0.0f,0.0f);
	glRotatef(a_y,0.0f,1.0f,0.0f);
	glRotatef(a_z,0.0f,0.0f,1.0f);

	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,w);
	
	cube_side(d,x,y,c,s);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,1.0f,0.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(180,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(270,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(180,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);	
	}

void gen_envmap(GLfloat d,GLint r,GLint g,GLint b,GLfloat l)
	{
	GLfloat c1[4]={(GLfloat)(CR*r),(GLfloat)(CR*g),(GLfloat)(CR*b),l};
	GLfloat c2[4]={(GLfloat)(CR*(r+32)),(GLfloat)(CR*g),(GLfloat)(CR*(b-32)),l};
	GLfloat c3[4]={(GLfloat)(CR*(r-32)),(GLfloat)(CR*g),(GLfloat)(CR*(b+32)),l};
	GLfloat c=1.0f;
	GLfloat s=1.0f;
	GLfloat x=0.0f;
	GLfloat y=0.0f;

	glLoadIdentity();
	glTranslatef(p_x,p_y,p_z);

	glRotatef(a_x,1.0f,0.0f,0.0f);
	glRotatef(a_y,0.0f,1.0f,0.0f);
	glRotatef(a_z,0.0f,0.0f,1.0f);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glPushMatrix();
	
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,c1);
	cube_side(d,x,y,c,s);
	glRotatef(180,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,c2);	
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,1.0f,0.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(180,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,c3);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(270,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,0.0f,1.0f,0.0f);
	cube_side(d,x,y,c,s);
	
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	}

void glenz_side(GLint r,GLint g,GLint b,GLfloat l)
	{
	GLfloat d=0.5f;
	GLfloat w[4]={1.0f,1.0f,1.0f,l};
	GLfloat c[4]={(GLfloat)(CR*r),(GLfloat)(CR*g),(GLfloat)(CR*b),l};

	glBegin(GL_TRIANGLES);				
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,w);
		glNormal3f(0.25f,-0.25f,-0.75f);
		glVertex3f(-d, d, d);
		glVertex3f( d, d, d);
		glVertex3f(0.0,0.0,(GLfloat)(d*1.75));		
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,c);
		glNormal3f(-0.25f,-0.25f,-0.75f);
		glVertex3f( d, d, d);
		glVertex3f( d,-d, d);
		glVertex3f(0.0,0.0,(GLfloat)(d*1.75));		
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,w);
		glNormal3f(-0.25f,0.25f,-0.75f);
		glVertex3f( d,-d, d);
		glVertex3f(-d,-d, d);
		glVertex3f(0.0,0.0,(GLfloat)(d*1.75));		
		glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,c);
		glNormal3f(0.25f,0.25f,-0.75f);
		glVertex3f(-d,-d, d);
		glVertex3f(-d, d, d);
		glVertex3f(0.0,0.0,(GLfloat)(d*1.75));
	glEnd();
	}

void gen_glenz(GLint r,GLint g,GLint b,GLfloat l)
	{
	glLoadIdentity();
	glTranslatef(p_x,p_y,p_z);

	glRotatef(a_x,1.0f,0.0f,0.0f);
	glRotatef(a_y,0.0f,1.0f,0.0f);
	glRotatef(a_z,0.0f,0.0f,1.0f);

	glenz_side(r,g,b,l);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,1.0f,0.0f,0.0f);
	glenz_side(r,g,b,l);
	glRotatef(180,0.0f,1.0f,0.0f);
	glenz_side(r,g,b,l);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(270,0.0f,1.0f,0.0f);
	glenz_side(r,g,b,l);
	glRotatef(90,0.0f,0.0f,1.0f);
	glRotatef(90,0.0f,1.0f,0.0f);
	glenz_side(r,g,b,l);
	glRotatef(180,0.0f,1.0f,0.0f);
	glenz_side(r,g,b,l);
	}

void gen_quad(GLfloat d,GLint r,GLint g,GLint b,GLfloat l)
	{	
	glLoadIdentity();
	glTranslatef(p_x,p_y,p_z);

	glRotatef(a_x,1.0f,0.0f,0.0f);
	glRotatef(a_y,0.0f,1.0f,0.0f);
	glRotatef(a_z,0.0f,0.0f,1.0f);

	calc_rgb(r,g,b,l);
	
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f,1.0f); glVertex3f(-d, d,0.0f);
		glTexCoord2f(1.0f,1.0f); glVertex3f( d, d,0.0f);
		glTexCoord2f(1.0f,0.0f); glVertex3f( d,-d,0.0f);
		glTexCoord2f(0.0f,0.0f); glVertex3f(-d,-d,0.0f);
	glEnd();
	}

void gen_particle()
	{
	GLfloat d=1.5f;
	GLfloat a=0.025f;
	GLfloat b=0.975f;

	r_d7=glGenLists(1);
	glNewList(r_d7,GL_COMPILE);
		glBegin(GL_QUADS);
		glTexCoord2f(a,b); glVertex3f(  -d,  -d,0.0f);
		glTexCoord2f(b,b); glVertex3f(0.0f,  -d,0.0f);
		glTexCoord2f(b,a); glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(a,a); glVertex3f(  -d,0.0f,0.0f);
		glTexCoord2f(b,b); glVertex3f(0.0f,  -d,0.0f);
		glTexCoord2f(a,b); glVertex3f(   d,  -d,0.0f);
		glTexCoord2f(a,a); glVertex3f(   d,0.0f,0.0f);
		glTexCoord2f(b,a); glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(a,b); glVertex3f(  -d,   d,0.0f);
		glTexCoord2f(b,b); glVertex3f(0.0f,   d,0.0f);
		glTexCoord2f(b,a); glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(a,a); glVertex3f(  -d,0.0f,0.0f);
		glTexCoord2f(b,b); glVertex3f(0.0f,   d,0.0f);
		glTexCoord2f(a,b); glVertex3f(   d,   d,0.0f);
		glTexCoord2f(a,a); glVertex3f(   d,0.0f,0.0f);
		glTexCoord2f(b,a); glVertex3f(0.0f,0.0f,0.0f);
		glEnd();
	glEndList();
	}

void gen_ring()
	{
	GLfloat min=0.35f;
	GLfloat max=0.55f;
	GLfloat d;

	r_d6=glGenLists(1);
	glNewList(r_d6,GL_COMPILE);
		for(i=0;i<360;i+=5)
			{
			glBegin(GL_QUADS);
				d=max;
				glColor3f(0.0f,0.0f,0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				d=(GLfloat)((min+max)/2.0+(max-min)/4.0);
				glColor3f((GLfloat)(96*CR),(GLfloat)(64*CR),(GLfloat)(128*CR));
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				d=(GLfloat)((min+max)/2.0);
				glColor3f(1.0f,1.0f,1.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				d=(GLfloat)((min+max)/2.0-(max-min)/4.0);
				glColor3f((GLfloat)(96*CR),(GLfloat)(64*CR),(GLfloat)(128*CR));
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);				
				d=min;
				glColor3f(0.0f,0.0f,0.0f);
				glVertex3f((GLfloat)(d*cos((i+10.0)*PID)),(GLfloat)(d*sin((i+10.0)*PID)),0.0f);
				glVertex3f((GLfloat)(d*cos(i*PID)),(GLfloat)(d*sin(i*PID)),0.0f);
			glEnd();
			}
	glEndList();
	}

void calc_txt()
	{
	l_t=strlen(txt);
	l_n=0;
	l_m=0;
	l_i=0;
	for(i=0;i<l_t;i++)
		{
		if((byte)txt[i]!=13)
			{
			l_i+=1;
			}
		else
			{
			if(l_i>l_m) l_m=l_i;
			l_n+=1;
			l_i=0;
			}
		}
	}

GLvoid init3d(GLsizei width,GLsizei height)
	{
	glViewport(0,0,width,height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluPerspective(fov,(GLfloat)((GLfloat)width/(GLfloat)height),nearplane,farplane); // aspect ratio
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	}

GLvoid init2d(GLsizei width,GLsizei height)
	{
	glViewport(0,0,width,height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluOrtho2D(0,width,height,0);	// init orthographic mode
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	}	

GLvoid ReSizeGLScene(GLsizei width,GLsizei height)	// resize and initialize GL window
	{
	if(height==0) height=1;				// prevent division by 0
	init3d(width,height);					// init 3d mode
	}

int InitGL(GLvoid)	// openGL setup
	{
	glShadeModel(GL_SMOOTH);													// enable smooth shading
	glClearDepth(1.0f);																// set depth buffer
	glEnable(GL_DEPTH_TEST);													// enable depth testing
	glDepthFunc(GL_LEQUAL);														// depth testing mode
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);	// perspective calculations mode	
	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);// set mode S to sphere mapping
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);// set mode T to sphere mapping
	glLineWidth(1.0f);																// set line width
	// load texture
	load_tex(IDB_POUET,GL_NEAREST,GL_REPEAT);
	load_tex(IDB_COEUR,GL_NEAREST,GL_REPEAT);
	load_tex(IDB_ENVMAP,GL_LINEAR,GL_CLAMP);
	load_tex(IDB_PARTICLE,GL_LINEAR,GL_CLAMP);
	load_tex(IDB_SCREEN,GL_NEAREST,GL_REPEAT);
	load_tex(IDB_FONT,GL_LINEAR,GL_REPEAT);
	load_tex(IDB_TITLE,GL_LINEAR,GL_CLAMP);
	// init display list
	gen_rez();
	gen_ring();
	gen_particle();
	// init particles
	for(i=0;i<p_n;i++)
		{
		par[i].a=(GLfloat)(rand()%3600/10.0);
		par[i].d=(GLfloat)((1700+rand()%1000)/100.0);
		par[i].y=(GLfloat)((-1700+rand()%3400)/100.0);
		par[i].z=rand()%360;
		par[i].r=128+rand()%96;
		par[i].g=128+rand()%96;
		par[i].b=128+rand()%96;
		}
	calc_txt();
	timer=new Timer();
	return true;
	}

int DrawGLScene(GLvoid) // draw scene
	{
	// synchro
	timer->update();
	t_g=timer->elapsed;
	if(SNG)
		{
		ord=FMUSIC_GetOrder(mod);
		row=FMUSIC_GetRow(mod);
		if(row==0)
			{		
			switch(ord)
				{
				case 0:
					if(!m_f) m_t=t_g;
					m_f=true;
					break;
				case 1:
					l_f=true;
					break;
				case 2:
					g_f=true;
					if(!j_f) r_f=false; else r_f=true;
					p_f=false;
					l_f=false;
					t_f=true;
					if(!z_f) z_t=t_g;
					z_f=true;
					break;
				case 4:
					r_f=true;
					p_f=true;
					l_f=true;
					if(!j_f) j_t=t_g;
					j_f=true;
					txt="\r\r\r\r\r\r\r\r\r\r\r\r\r\r        CREDITS:\r        --------\r\r       CODE BY REZ\r      DESIGN BY REZ\r     CHIPTUNE BY REZ\r\r THANKS TO KEOPS/EQUINOX \rFOR HIS HELP AND SUPPORT!\r\rTHANKS TO RYG/FARBRAUSH\rFOR HIS GREAT KKRUNCHY!\r\r    I HOPE YOU LIKED\rTHIS LITTLE 20KB INTRO!\r";
					calc_txt();
					break;
				case 6:
					r_f=false;
					g_f=false;
					txt="\r\r\r\r\r\r\r\r\r\r\r\r\r\r    HEI HEI HEI\r\rNOTHING MORE TO SEE\rTHANKS FOR WATCHING\r\r    INTRO LOOP!";
					calc_txt();
					break;
				}
			}
		if(row%8==0)
			{
			f_n=0.25f;
			f_v2=f_n;
			f_t2=t_g;
			}
		if(row==0)
			{
			f_n=0.5f;
			f_v1=f_n;
			f_t1=t_g;
			f_v2=0;
			}
	if((((row%8-4)==0)||(row==60)||(row==62))&&(ord>1))
			{
			c_r=0.0f;
			c_j=0.5f;
			c_t=t_g;
			}
		}
	// compute color
	nr=(GLint)(b_r+16.0*cos(32.0*t_g*PID));
	ng=(GLint)(b_g+16.0*sin(16.0*t_g*PID));
	nb=(GLint)(b_b+16.0*cos(48.0*t_g*PID));
	f_c[0]=f_v1+f_v2;
	f_c[1]=f_v1+f_v2;
	f_c[2]=f_v1+f_v2;
	f_c[3]=1.0f;
	if(f_v1>0) f_v1=(GLfloat)(f_n-f_n*2.0*(t_g-f_t1));
	if(f_v2>0) f_v2=(GLfloat)(f_n-f_n*2.0*(t_g-f_t2));
	// 3d mode
	init3d(w_w,w_h);
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	// background color
	glClearColor(f_c[0],f_c[1],f_c[2],1.0f);
	// light
	l_a[0]=(GLfloat)(-0.5+CR*nr);
	l_a[1]=(GLfloat)(-0.5+CR*ng);
	l_a[2]=(GLfloat)(-0.5+CR*nb);
	l_a[3]=1.0f;
	l_d[0]=0.75f;
	l_d[1]=0.75f;
	l_d[2]=0.75f;
	l_d[3]=1.0f;
	l_p[0]=0.0f;
	l_p[1]=-50.0f;
	l_p[2]=(GLfloat)(c_z-75.0);
	l_p[3]=1.0f;
	glLoadIdentity();
	glLightfv(GL_LIGHT1,GL_AMBIENT,l_a);	// ambient light color
	glLightfv(GL_LIGHT1,GL_DIFFUSE,l_d);	// diffuse light color
	glLightfv(GL_LIGHT1,GL_POSITION,l_p);	// light position
	glEnable(GL_LIGHT1);
	// fog
	glFogi(GL_FOG_MODE,GL_LINEAR);				// fog mode
	glFogfv(GL_FOG_COLOR,f_c);						// fog color
	glFogf(GL_FOG_DENSITY,0.5f);					// fog density
	glHint(GL_FOG_HINT,GL_DONT_CARE);			// fog hint value
	glFogf(GL_FOG_START,20.0f);						// fog start depth
	glFogf(GL_FOG_END,65.0f);							// fog end depth
	glEnable(GL_FOG);											// enable fog
	// disable z-buffer, enable blend
	glDisable(GL_DEPTH_TEST);							// disable z-buffer
	glDepthMask(GL_FALSE);								// do not write z-buffer
	glEnable(GL_BLEND);										// enable blending
	// move scene
	if(z_f)
		{
		z_r=(GLfloat)(24.0*(t_g-z_t));
		if(z_z>5.0f) z_z=(GLfloat)(10.0-1.0*(t_g-z_t));
		z_a=24.0f;
		}
	c_z=(GLfloat)(-15.0+z_z*cos(z_r*2.0*PID));
	// rez	
	if(r_f)
		{		
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		if(l_f)
			{
			for(i=0;i<r_n*r_l;i++)
				{
				// compute values
				a=(GLfloat)(i*270.0/r_n+r_y);
				p_x=(GLfloat)(30.0*cos(a*PID));
				p_y=(GLfloat)(-(r_l/2)*6+(6.0/r_n)*i);
				p_z=(GLfloat)(c_z+30.0*sin(a*PID));
				a_x=(GLfloat)(0.0);
				a_y=(GLfloat)(-90.0-a);
				a_z=(GLfloat)(2.5);
				// display list
				dsp_rez();
				}
			}
		else
			{
			for(i=0;i<360;i+=12)
				{
				for(j=0;j<3;j++)
					{
					// compute values
					p_x=(GLfloat)(30.0*cos((i+r_y/2.0)*PID));
					p_y=(GLfloat)(30.0*sin((i+r_y/2.0)*PID));
					p_z=(GLfloat)(c_z-36.0+18.0*j);
					a_x=(GLfloat)(90.0);
					a_y=(GLfloat)(-90.0+i+r_y/2.0);
					a_z=(GLfloat)(90.0);
					// display list
					dsp_rez();
					}
				}
			}
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		}
	// 2d mode
	init2d(w_w,w_h);
	// draw 2d
	glEnable(GL_TEXTURE_2D);							// enable texture mode	
	if(!r_f)
		{
		glBindTexture(GL_TEXTURE_2D,IDB_ENVMAP);
		glBlendFunc(GL_ONE,GL_DST_COLOR);
		calc_rgb(nr,ng,nb,0.5f);
		}
	else
		{
		glBindTexture(GL_TEXTURE_2D,NULL);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_COLOR);
		calc_rgb(nr,ng,nb,0.375f);
		}
	// background
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,1.0f); glVertex2i(0  ,0  );
		glTexCoord2f(1.0f,1.0f); glVertex2i(w_w,0  );
		glTexCoord2f(1.0f,0.0f); glVertex2i(w_w,w_h);
		glTexCoord2f(0.0f,0.0f); glVertex2i(0  ,w_h);
	glEnd();
	// enable z-buffer, disable blend
	glDisable(GL_BLEND);									// disable blending	
	glDepthMask(GL_TRUE);									// write z-buffer
	glEnable(GL_DEPTH_TEST);							// enable z-buffer
	// 3d mode
	init3d(w_w,w_h);
	// mapping
	if(m_f) m_a=(GLfloat)(32.0*(t_g-m_t));
	// move cube and title
	if(c_j>0)
			{
			c_r=(GLfloat)(32.0*(t_g-c_t));
			c_j=(GLfloat)(0.2-0.8*(t_g-c_t));
			}
	if((j_f)&&(j_y>0))
			{
			j_r=(GLfloat)(5.0*(t_g-j_t));
			j_y=(GLfloat)(-c_y-7.0*(t_g-j_t));
			}
	// enable lighting
	glEnable(GL_LIGHTING);
	// cube
	p_x=0.0f;
	p_y=(GLfloat)(c_y+j_y*cos(j_r)+c_j*cos(c_r));
	p_z=(GLfloat)(c_z);
	a_x=(GLfloat)(r_y);
	a_y=(GLfloat)(r_y*2.0);
	a_z=(GLfloat)(r_y/2.0);
	glEnable(GL_CULL_FACE);								// enable cull face
	glCullFace(GL_FRONT);									// cull mode front
	glBindTexture(GL_TEXTURE_2D,IDB_POUET);
	glBlendFunc(GL_ZERO,GL_ONE);
	gen_cube(3.0f,160,160,160,1.0f,1.0f);
	glDisable(GL_DEPTH_TEST);							// disable z-buffer
	glDepthMask(GL_FALSE);								// do not write z-buffer
	glEnable(GL_BLEND);										// enable blending
	glBindTexture(GL_TEXTURE_2D,IDB_COEUR);
	glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
	gen_cube(3.0f,160,160,160,0.75f,1.0f);
	glBindTexture(GL_TEXTURE_2D,IDB_ENVMAP);
	glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
	gen_envmap(3.0f,(GLint)(96+z_a*6),(GLint)(96+z_a*6),(GLint)(96+z_a*6),0.75f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);	
	glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
	gen_envmap(3.0f,64,64,64,0.75f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glDisable(GL_CULL_FACE);							// disable cull face
	glDisable(GL_TEXTURE_2D);							// disable texture mode
	glDepthMask(GL_TRUE);									// write z-buffer
	glEnable(GL_DEPTH_TEST);							// enable z-buffer
	// glenz
	if(g_f)
		{
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		for(i=0;i<g_n;i++)
			{
			p_x=(GLfloat)((8.0+3.0*cos(r_y*4.0*PID))*cos((360/g_n*i+r_y*2.0)*PID));
			p_y=(GLfloat)(c_y+j_y*cos(j_r)+3.0*cos((360/g_n*i+r_y*4.0)*PID));	// -c_j*cos(c_r)
			p_z=(GLfloat)(c_z+(8.0+3.0*cos(r_y*4.0*PID))*sin((360/g_n*i+r_y*2.0)*PID));
			a_x=(GLfloat)(r_y*3.0+i*24.0);
			a_y=(GLfloat)(r_y*4.0+i*24.0);
			a_z=(GLfloat)(r_y*2.0+i*24.0);			
			gen_glenz((GLint)(nr/2.0+p_x*6),(GLint)(nr/2.0),(GLint)(nr/2.0-p_x*6),(GLfloat)((-p_z-2.0)/6.0));
			}
		}
	// disable lighting
	glDisable(GL_LIGHTING);
	// disable fog
	glDisable(GL_FOG);
	// enable texture mode
	glEnable(GL_TEXTURE_2D);							// enable texture mode
	// particles
	if(p_f)
		{		
		glDepthMask(GL_FALSE);							// do not write z-buffer
		glBindTexture(GL_TEXTURE_2D,IDB_PARTICLE);
		glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		a_x=0.0f;
		a_y=0.0f;		
		for(i=0;i<p_n;i++)
			{
			if(r_f)
				p_y=(GLfloat)(par[i].y+0.25*cos((par[i].a+r_y)*16.0*PID));
			else
				p_y=(GLfloat)(c_y+j_y*cos(j_r)+((27.0-par[i].d)/16.0)*cos((r_y*6.0+par[i].d*16.0)*PID));
			p_x=(GLfloat)(par[i].d*cos((par[i].a+r_y)*PID));
			p_z=(GLfloat)(c_z+par[i].d*sin((par[i].a+r_y)*PID));
			a_z=(GLfloat)(par[i].z);
			glLoadIdentity();
			glTranslatef(p_x,p_y,p_z);
			glRotatef(a_z,0.0f,0.0f,1.0f);
			calc_rgb(par[i].r,par[i].g,par[i].b,(GLfloat)((-p_z-2.0)/6.0));
			glCallList(r_d7);
			}
		glDepthMask(GL_TRUE);								// write z-buffer
		}
	// disable z-buffer
	glDisable(GL_DEPTH_TEST);							// disable z-buffer
	glDepthMask(GL_FALSE);								// do not write z-buffer	
	// title
	if(t_f)
		{		
		glBindTexture(GL_TEXTURE_2D,IDB_TITLE);
		glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
		a_x=(GLfloat)(30.0*cos((r_y*2)*PID));
		a_y=(GLfloat)(30.0*cos((r_y*2)*PID));
		a_z=0.0f;
		p_x=0.0f;
		p_y=(GLfloat)(-(c_y+j_y*cos(j_r))*0.2);
		p_z=-2.0f;		
		gen_quad(0.7f,224,224,224,0.5f);
		glBindTexture(GL_TEXTURE_2D,NULL);
		a_x+=30.0f;
		a_y+=30.0f;
		glLoadIdentity();
		glTranslatef(p_x,p_y,p_z);
		glRotatef(a_x,1.0f,0.0f,0.0f);
		glRotatef(a_y,0.0f,1.0f,0.0f);
		glRotatef(a_z,0.0f,0.0f,1.0f);
		glCallList(r_d6);
		}
	// liner
	if(l_f)
		{		
		glBindTexture(GL_TEXTURE_2D,IDB_FONT);
		glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
		p_x=(GLfloat)(-(l_m*l_s*1.75)/2.0);
		p_y=(GLfloat)((l_n*l_s*2.0)/2.0);
		p_z=-1.125f;
		l_l=0;
		l_i=0;
		for(i=0;i<(int)strlen(txt);i++)
			{
			l_c=(byte)txt[i];
			if(l_c!=13)
				{
				// compute position
				p_x+=(GLfloat)(l_s*1.625);
				if(l_c!=32)
					{
					glLoadIdentity();
					a_z=(GLfloat)(20.0*cos((24.0*t_g*6.0+l_i+90.0)*PID));
					glTranslatef((GLfloat)(p_x),(GLfloat)(p_y+0.1*cos((24.0*t_g*6.0+l_i)*PID)),p_z);
					glRotatef(a_z,0.0f,0.0f,1.0f);
					// render char
					l_w=(GLfloat)(l_c%16*0.0625);
					l_h=(GLfloat)((l_c-l_c%16)/16.0*0.0625);
					calc_rgb((GLint)(144+112*cos((24.0*t_g*8.0+l_i*1.5)*PID)),144,(GLint)(144+112*sin((24.0*t_g*8.0+l_i*1.5)*PID)),1.0f);
					glBegin(GL_QUADS);
						glTexCoord2f((GLfloat)(l_w+0.003),(GLfloat)(-l_h-0.003)); glVertex3f(-l_s, l_s,(GLfloat)( l_s/2.0));
						glTexCoord2f((GLfloat)(l_w+0.06 ),(GLfloat)(-l_h-0.003)); glVertex3f( l_s, l_s,(GLfloat)( l_s/2.0));
						glTexCoord2f((GLfloat)(l_w+0.06 ),(GLfloat)(-l_h-0.06 )); glVertex3f( l_s,-l_s,(GLfloat)( l_s/2.0));
						glTexCoord2f((GLfloat)(l_w+0.003),(GLfloat)(-l_h-0.06 )); glVertex3f(-l_s,-l_s,(GLfloat)( l_s/2.0));
					glEnd();
					}
				l_i+=10;
				}
			else
				{
				l_l+=1;
				l_i=0;
				p_x=(GLfloat)(-(l_m*l_s*1.75)/2.0);
				p_y=(GLfloat)((l_n*l_s*2.0)/2.0-l_l*l_s*2.0);
				}
			}		
		}
	// 2d mode
	init2d(w_w,w_h);
	// draw 2d	
	glBindTexture(GL_TEXTURE_2D,IDB_SCREEN);
	glBlendFunc(GL_DST_COLOR,GL_SRC_ALPHA);
	// alpha mask
	calc_rgb(255,255,255,0.75f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f            ,0.0f               ); glVertex2i(0  ,40);
		glTexCoord2f((GLfloat)(w_w/2),0.0f               ); glVertex2i(w_w,40);
		glTexCoord2f((GLfloat)(w_w/2),(GLfloat)(w_h/2-40)); glVertex2i(w_w,(GLint)(w_h-40));
		glTexCoord2f(0.0f            ,(GLfloat)(w_h/2-40)); glVertex2i(0  ,(GLint)(w_h-40));
	glEnd();
	// borders
	for(i=0;i<20;i++)
		{
		calc_rgb(255,255,255,(GLfloat)(0.75-0.75/20*i));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f            ,0.0f); glVertex2i(0  ,(GLint)(38-i*2));
			glTexCoord2f((GLfloat)(w_w/2),0.0f); glVertex2i(w_w,(GLint)(38-i*2));
			glTexCoord2f((GLfloat)(w_w/2),1.0f); glVertex2i(w_w,(GLint)(40-i*2));
			glTexCoord2f(0.0f            ,1.0f); glVertex2i(0  ,(GLint)(40-i*2));
		glEnd();	
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f            ,0.0f); glVertex2i(0  ,(GLint)(w_h-40+i*2));
			glTexCoord2f((GLfloat)(w_w/2),0.0f); glVertex2i(w_w,(GLint)(w_h-40+i*2));
			glTexCoord2f((GLfloat)(w_w/2),1.0f); glVertex2i(w_w,(GLint)(w_h-38+i*2));
			glTexCoord2f(0.0f            ,1.0f); glVertex2i(0  ,(GLint)(w_h-38+i*2));
		glEnd();
		}	
	// disable blending & texture mode
	glDisable(GL_TEXTURE_2D);							// disable texture mode
	glDisable(GL_BLEND);									// disable blending
	// enable z-buffer
	glDepthMask(GL_TRUE);									// write z-buffer
	glEnable(GL_DEPTH_TEST);							// enable z-buffer
	// compute rotation
	r_y=(GLfloat)(z_a*(t_g-z_t));	
	return true;
	}

GLvoid KillGLWindow(GLvoid) // kill window
	{
	if(fullscreen)
		{
		ChangeDisplaySettings(NULL,0);	// switch back to desktop
		ShowCursor(true);								// show mouse pointer
		}
	if(hRC)
		{
		if(!wglMakeCurrent(NULL,NULL)) MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION);
		if(!wglDeleteContext(hRC)) MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION);
		hRC=NULL;
		}
	if(hDC && !ReleaseDC(hWnd,hDC)) { MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hDC=NULL; }
	if(hWnd && !DestroyWindow(hWnd)) { MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hWnd=NULL; }
	if(!UnregisterClass("OpenGL",hInstance)) { MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hInstance=NULL; }
	delete timer;
	}

int CreateGLWindow(char* title,int width,int height,int bits,int fullscreenflag)
	{
	GLuint PixelFormat;							// pixel format result
	WNDCLASS wc;										// windows class structure
	DWORD dwExStyle;								// window extended style
	DWORD dwStyle;									// window style
	RECT WindowRect;								// upper_left/loweb_right values

	WindowRect.left=(long)0;				// set left value
	WindowRect.right=(long)width;		// set right value
	WindowRect.top=(long)0;					// set top value
	WindowRect.bottom=(long)height;	// set bottom value

	pfd.cColorBits=bits;            // set color depth

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance=GetModuleHandle(NULL);					// window instance
	wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;	// redraw on size, own DC for window
	wc.lpfnWndProc=(WNDPROC) WndProc;					// WndProc handles messages
	wc.cbClsExtra=0;													// no extra window data
	wc.cbWndExtra=0;													// no extra window data
	wc.hInstance=hInstance;										// set the instance
	wc.hIcon=LoadIcon(NULL,IDI_WINLOGO);			// load default icon
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);		// load arrow pointer
	wc.hbrBackground=NULL;										// no background
	wc.lpszMenuName=NULL;											// no menu
	wc.lpszClassName="OpenGL";								// set class name

	if(!RegisterClass(&wc))										// register window class
		{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	
	if(fullscreen)
		{
		DEVMODE dmScreenSettings;															// device mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// is memory cleared ?
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// devmode structure size
		dmScreenSettings.dmPelsWidth=width;										// screen width
		dmScreenSettings.dmPelsHeight=height;									// screen height
		dmScreenSettings.dmBitsPerPel=bits;										// bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// set selected mode
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{
			if(MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","rezGL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				{
				fullscreen=false;
				}
			else
				{
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return false;
				}
			}
		}

	if (fullscreen)
		{
		dwExStyle=WS_EX_APPWINDOW;									// window extended style
		dwStyle=WS_POPUP;														// windows style
		ShowCursor(false);													// hide cursor
		}
	else
		{
		dwExStyle=WS_EX_APPWINDOW|WS_EX_WINDOWEDGE;	// window extended style
		dwStyle=WS_OVERLAPPEDWINDOW;								// windows style
		}

	AdjustWindowRectEx(&WindowRect,dwStyle,false,dwExStyle);	// adjust window to requested size

	// create window
	if(!(hWnd=CreateWindowEx(dwExStyle,	// extended style for window
		"OpenGL",													// class name
		title,														// window title
		dwStyle|													// defined window style
		WS_CLIPSIBLINGS|									// required window style
		WS_CLIPCHILDREN,									// required window style
		0,0,															// window position
		WindowRect.right-WindowRect.left,	// calculate window width
		WindowRect.bottom-WindowRect.top,	// calculate window height
		NULL,															// no parent window
		NULL,															// no menu
		hInstance,												// instance
		NULL)))														// don't pass anything to WM_CREATE !
		{
		KillGLWindow();
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	if(!(hDC=GetDC(hWnd)))	// Did We Get A Device Context?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))	// Are We Able To Set The Pixel Format?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	if(!(hRC=wglCreateContext(hDC)))	// Are We Able To Get A Rendering Context?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	if(!wglMakeCurrent(hDC,hRC))	// Try To Activate The Rendering Context
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}

	ShowWindow(hWnd,SW_SHOW);			// show window
	SetForegroundWindow(hWnd);		// set higher priority
	SetFocus(hWnd);								// set keyboard focus to window
	ReSizeGLScene(width,height);	// set up perspective of GL screen

	if(!InitGL())									// initialize GL window
		{
		KillGLWindow();
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	return true;
	}

// window handle,window message,additional message,additional message
LRESULT CALLBACK WndProc(HWND	hWnd,UINT	uMsg,WPARAM	wParam,LPARAM	lParam)
	{
	switch(uMsg)								// check windows messages
		{
		case WM_ACTIVATE:					// watch for window activate message
			{
			if(!HIWORD(wParam))			// check minimization state
				{
				active=true;					// program is active
				}
			else
				{
				active=false;					// program is no longer active
				}
			return 0;								// return to the message loop
			}
		case WM_SYSCOMMAND:				// intercept system commands
			{
			switch(wParam)					// check system calls
				{
				case SC_SCREENSAVE:		// screensaver trying to start ?
				case SC_MONITORPOWER:	// monitor trying to enter powersave ?
				return 0;							// prevent from happening
				}
			break;									// exit
			}
		case WM_CLOSE:						// close message ?
			{
			PostQuitMessage(0);			// post quit message
			return 0;
			}
		case WM_KEYDOWN:					// key down ?
			{
			keys[wParam]=true;			// mark key as true
			return 0;
			}
		case WM_KEYUP:						// key released ?
			{
			keys[wParam]=false;			// mark key as false
			return 0;
			}
	  case WM_SIZE:							// resize openGL window
			{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));	// loword=width,hiword=height
			return 0;
			}
		}
	return DefWindowProc(hWnd,uMsg,wParam,lParam); // pass all unhandled messages to DefWindowProc
	}

// instance,previous instance,command line parameters,window show state
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
	{
	MSG msg;						// windows message structure
	int done=false;			// exit loop
	// ask for fullscreen mode
	if(MessageBox(NULL,"Run in fullscreen mode ?","GLREZ",MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
		fullscreen=false;
		}
	// create openGL window
	if(!CreateGLWindow(name,w_w,w_h,w_c,fullscreen)) return 0;// quit if window not created
	// load and play music
	if(SNG) load_song();
	// main loop
	while(!done)
		{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// a message is waiting ?
			{
			if(msg.message==WM_QUIT)								// a quit message ?
				{
				done=true;														// quit window
				}
			else																		// a window message ?
				{
				TranslateMessage(&msg);								// translate message
				DispatchMessage(&msg);								// dispatch message
				}
			}
		else																			// no messages
			{
			// draw the scene, watch for escape key and quit message from DrawGLScene()
			if((active&&!DrawGLScene())||keys[VK_ESCAPE]) done=true; else SwapBuffers(hDC);	// exit or swap buffers
		/*if(keys[VK_F1])													// F1 pressed ?
				{
				keys[VK_F1]=false;										// make key false again
				KillGLWindow();												// kill current window
				fullscreen=!fullscreen;								// toggle fullscreen/windowed mode
				// recreate openGL window
				if(!CreateGLWindow(name,w_w,w_h,w_c,fullscreen)) return 0; // quit if window not created
				}
			if(keys[VK_F2])													// F2 pressed ?
				{
				keys[VK_F2]=false;										// make key false again
				r_f=!r_f;															// toggle on/off REZ
				}
			if(keys[VK_F3])													// F3 pressed ?
				{
				keys[VK_F3]=false;										// make key false again
				p_f=!p_f;															// toggle on/off particles
				}
			if(keys[VK_F4])													// F4 pressed ?
				{
				keys[VK_F4]=false;										// make key false again
				g_f=!g_f;															// toggle on/off glenz
				}
			if(keys[VK_F5])													// F5 pressed ?
				{
				keys[VK_F5]=false;										// make key false again
				l_f=!l_f;															// toggle on/off liner
				}
			if(keys[VK_F6])													// F6 pressed ?
				{
				keys[VK_F6]=false;										// make key false again
				t_f=!t_f;															// toggle on/off title
				}
			if(keys[VK_F7])													// F7 pressed ?
				{
				keys[VK_F7]=false;										// make key false again
				m_f=!m_f;															// toggle on/off mapping
				m_t=t_g;
				}
			if(keys[VK_SPACE])											// space pressed ?
				{
				keys[VK_SPACE]=false;									// make key false again
				c_r=0.0f;
				c_j=0.5f;
				c_t=t_g;
				f_n=0.25f;
				f_v2=f_n;
				f_t2=t_g;
				}
			if(keys[VK_RETURN])											// return pressed ?
				{
				keys[VK_RETURN]=false;								// make key false again
				j_f=true;															// move cube and title
				f_n=0.5f;
				f_v1=f_n;
				f_t1=t_g;
				c_y=-3.85f;
				j_y=3.85f;
				j_t=t_g;
				}
			if(keys[VK_TAB])												// tab pressed ?
				{
				keys[VK_TAB]=false;										// make key false again
				z_f=true;															// move scene
				z_z=10.0f;
				z_a=0.0f;
				z_t=t_g;
				}*/
			}
		}

	// shutdown
	if(SNG) FMUSIC_FreeSong(mod);	// stop and kill music
	KillGLWindow();								// kill the window
	return (msg.wParam);					// exit the program
	}