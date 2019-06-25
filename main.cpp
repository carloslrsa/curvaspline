#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include<conio.h>
#include <math.h>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

//Variables
const int OPCION_ACTIVADO = 1;
const int OPCION_DESACTIVADO = 2;

const int OPCION_AUMENTAR = 1;
const int OPCION_DISMINUIR = 2;

const int FORMA_TRIANGULO = 1;
const int FORMA_CUADRADO = 2;
const int FORMA_CIRCULO = 3;

const int COLOR_ROJO = 1;
const int COLOR_VERDE = 2;
const int COLOR_AZUL = 3;
const int COLOR_RANDOM = 4;

class Color{
    public:
    GLfloat r = 0;
    GLfloat g = 0;
    GLfloat b = 0;
    Color(GLfloat _r,GLfloat  _g,GLfloat  _b): r(_r) ,g(_g), b(_b) {}
};

class Coordenada{
    GLfloat x;
    GLfloat y;
    GLfloat z;
public:
    Coordenada(GLfloat _x, GLfloat _y, GLfloat _z = 0.0f) : x(_x), y(_y), z(_z) {}

    GLfloat GetX(){return x;}
    void SetX(GLfloat _x){x = _x;}
    GLfloat GetY(){return y;}
    void SetY(GLfloat _y){y = _y;}
    GLfloat GetZ(){return z;}
    void SetZ(GLfloat _z){z = _z;}

    Coordenada operator -(Coordenada c2){ return Coordenada(x-c2.x,y-c2.y,z-c2.z); }
    Coordenada operator +(Coordenada c2){ return Coordenada(x+c2.x,y+c2.y,z+c2.z); }
    Coordenada operator *(GLfloat n){ return Coordenada(x * n, y * n, z * n); }
    Coordenada operator /(GLfloat n){ return Coordenada(x / n, y / n, z / n); }

    ~Coordenada(){}
};

class CurvaSpline{
    class CurvaSplineConfiguracion{
    protected:
        const GLfloat GROSOR_MAXIMO = 10.0;
        const GLfloat GROSOR_MINIMO = 2.0;

        int visibilidad;
        Color *color;
        GLfloat grosor;

    public:
        CurvaSplineConfiguracion(int _visibilidad, Color* _color, GLfloat _grosor) {visibilidad = _visibilidad; color = _color; grosor = _grosor; }
        int GetVisibilidad(){return visibilidad;}
        void SetVisibilidad(int _visibilidad){visibilidad = _visibilidad;}
        Color* GetColor(){return color;}
        void SetColor(GLfloat _r, GLfloat _g, GLfloat _b){color->r = _r; color->g = _g; color->b = _b;}
        GLfloat GetGrosor(){return grosor;}
        void SetGrosor(GLfloat _grosor){grosor = _grosor;}

        ~CurvaSplineConfiguracion(){delete color;}
    };
    class PuntoControlConfiguracion : public CurvaSplineConfiguracion {
        int forma;
        int ingreso;
    public:
        PuntoControlConfiguracion(int _visibilidad, Color* _color, GLfloat _grosor, int _forma, int _ingreso) : CurvaSplineConfiguracion(_visibilidad,_color,_grosor){
            forma = _forma;
            ingreso = _ingreso;
        }
        int GetForma(){return forma;}
        void SetForma(int _forma){forma = _forma;}
        int GetIngreso(){return ingreso;}
        void SetIngreso(int _ingreso){ingreso = _ingreso;}
    };

    CurvaSplineConfiguracion *configuracionCurva;
    PuntoControlConfiguracion *configuracionPuntos;

    vector<Coordenada> *puntosControl;

    void dibujarPuntos(){
        if(configuracionPuntos->GetVisibilidad() == OPCION_ACTIVADO){
            glPointSize(configuracionPuntos->GetGrosor());
            glColor3f(configuracionPuntos->GetColor()->r,configuracionPuntos->GetColor()->g,configuracionPuntos->GetColor()->b);
            glBegin(GL_POINTS);
                for(int i = 0; i< puntosControl->size(); i++){
                    glVertex3f((*puntosControl)[i].GetX(),(*puntosControl)[i].GetY(),(*puntosControl)[i].GetZ());
                }
            glEnd();
        }
    }
    void dibujarCurvaSpline(){
        if(configuracionCurva->GetVisibilidad() == OPCION_ACTIVADO && puntosControl->size() > 0){
            int cantidadSegmentos = (puntosControl->size() - 1) / 2;

            glPointSize(configuracionCurva->GetGrosor());
            glColor3f(configuracionCurva->GetColor()->r,configuracionCurva->GetColor()->g,configuracionCurva->GetColor()->b);
            glBegin(GL_LINE_STRIP);
                for(int i=0; i < cantidadSegmentos ; i++){
                    int inicial = i * 2;
                    for(GLfloat t=0.0; t <= 1.0 ; t += 0.01){
                        Coordenada c = (((*puntosControl)[inicial]-(*puntosControl)[inicial+1]*2.0f+(*puntosControl)[inicial+2])*t*t + ((*puntosControl)[inicial]*-2.0f+(*puntosControl)[inicial+1]*2.0f)*t + (*puntosControl)[inicial] + (*puntosControl)[inicial+1])/2.0f;
                        glVertex3f(c.GetX(),c.GetY(),c.GetZ());
                    }
                }
            glEnd();
        }
    }
public:
    CurvaSpline(){
        puntosControl = new vector<Coordenada>();
        configuracionCurva = new CurvaSplineConfiguracion(OPCION_ACTIVADO,new Color(0.0,0.0,1.0),2);
        configuracionPuntos = new PuntoControlConfiguracion(OPCION_ACTIVADO,new Color(1.0,0.0,0.0),5,FORMA_CIRCULO, OPCION_ACTIVADO);
    }
    bool Dibujar(){
        glPushMatrix();
        dibujarPuntos();
        dibujarCurvaSpline();
        glPopMatrix();
        return true;
    }
    void AgregarPunto(void (*_display)(), GLfloat x, GLfloat y, GLfloat z = 0){
        AgregarPunto(_display,Coordenada(x,y,z));
    }
    void AgregarPunto(void (*_display)(), Coordenada coordenada){
        if(configuracionPuntos->GetIngreso() == OPCION_ACTIVADO){
            puntosControl->push_back(coordenada);
            _display();
        }
    }

    PuntoControlConfiguracion* GetConfiguracionPuntosControl(){return configuracionPuntos;}
    CurvaSplineConfiguracion* GetConfiguracionCurvaSpline(){return configuracionCurva;}

    ~CurvaSpline(){delete &puntosControl; delete &configuracionCurva; delete &configuracionPuntos; }

};
void init(void);
void display(void);
void reshape(int,int);
void keyboard(unsigned char, int, int);
void mouse(int button,int state,int x,int y);
void GraficarPuntos();
void GraficarCurvaSpline();

Coordenada mapScreenPointToWindow(int, int);

float dim=400;
int paso=0;

void menuIngresoPuntos(int);
void menuDibujarPcontrol(int);
void menuDibujarCurvaSpline(int);
void menuZoom(int);
void menuModificarPuntos(int);
void menuPincelMovilPuntos(int);
void menuGrosorPcontrol(int);
void menuGrosorCurvaSpline(int);
void menuColorPcontrol(int);
void menuColorCurvaSpline(int);
void menuPrincipal(int);

CurvaSpline *miCurva;

int main(int argc, char** argv)
{
    miCurva = new CurvaSpline();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(dim,dim);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Grafico de una curva spline");

    int menu_ingresopuntos = glutCreateMenu(menuIngresoPuntos);
    glutAddMenuEntry("Activado", OPCION_ACTIVADO);
    glutAddMenuEntry("Desactivado", OPCION_DESACTIVADO);

    int submenu_dibujar_pcontrol = glutCreateMenu(menuDibujarPcontrol);
    glutAddMenuEntry("Activado", OPCION_ACTIVADO);
    glutAddMenuEntry("Desactivado", OPCION_DESACTIVADO);

    int submenu_dibujar_curvaspline = glutCreateMenu(menuDibujarCurvaSpline);
    glutAddMenuEntry("Activado", OPCION_ACTIVADO);
    glutAddMenuEntry("Desactivado", OPCION_DESACTIVADO);

    int menu_dibujar = glutCreateMenu(NULL);
    glutAddSubMenu("Puntos de control", submenu_dibujar_pcontrol);
    glutAddSubMenu("Curva Spline", submenu_dibujar_curvaspline);

    int menu_zoom = glutCreateMenu(menuZoom);
    glutAddMenuEntry("Activado", OPCION_ACTIVADO);
    glutAddMenuEntry("Desactivado", OPCION_DESACTIVADO);

    int menu_modificarpuntos = glutCreateMenu(menuModificarPuntos);
    glutAddMenuEntry("Activado", OPCION_ACTIVADO);
    glutAddMenuEntry("Desactivado", OPCION_DESACTIVADO);

    int menu_pincelmovilpuntos = glutCreateMenu(menuPincelMovilPuntos);
    glutAddMenuEntry("Triangulo", FORMA_TRIANGULO);
    glutAddMenuEntry("Cuadrado", FORMA_CUADRADO);
    glutAddMenuEntry("Circulo", FORMA_CIRCULO);

    int submenu_grosor_pcontrol = glutCreateMenu(menuGrosorPcontrol);
    glutAddMenuEntry("Aumentar", OPCION_AUMENTAR);
    glutAddMenuEntry("Disminuir", OPCION_DISMINUIR);

    int submenu_grosor_curvaspline = glutCreateMenu(menuGrosorCurvaSpline);
    glutAddMenuEntry("Aumentar", OPCION_AUMENTAR);
    glutAddMenuEntry("Disminuir", OPCION_DISMINUIR);

    int menu_grosor = glutCreateMenu(NULL);
    glutAddSubMenu("Puntos de control",submenu_grosor_pcontrol);
    glutAddSubMenu("Curva Spline",submenu_grosor_curvaspline);

    int submenu_color_pcontrol = glutCreateMenu(menuColorPcontrol);
    glutAddMenuEntry("Rojo", COLOR_ROJO);
    glutAddMenuEntry("Verde", COLOR_VERDE);
    glutAddMenuEntry("Azul", COLOR_AZUL);
    glutAddMenuEntry("Random", COLOR_RANDOM);

    int submenu_color_curvaspline = glutCreateMenu(menuColorCurvaSpline);
    glutAddMenuEntry("Rojo", COLOR_ROJO);
    glutAddMenuEntry("Verde", COLOR_VERDE);
    glutAddMenuEntry("Azul", COLOR_AZUL);
    glutAddMenuEntry("Random", COLOR_RANDOM);

    int menu_color = glutCreateMenu(NULL);
    glutAddSubMenu("Puntos de control", submenu_color_pcontrol);
    glutAddSubMenu("Curva Spline", submenu_color_curvaspline);

    int menu = glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Ingreso de puntos", menu_ingresopuntos);
    glutAddSubMenu("Dibujar", menu_dibujar);
    glutAddSubMenu("Zoom", menu_zoom);
    glutAddSubMenu("Modificar puntos", menu_modificarpuntos);
    glutAddSubMenu("Pincel Movil", menu_pincelmovilpuntos);
    glutAddSubMenu("Grosor", menu_grosor);
    glutAddSubMenu("Color", menu_color);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}


void init(void)
{
    glClearColor(1.0,1.0,1.0,.0);
    glShadeModel(GL_FLAT);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    miCurva->Dibujar();
    glFlush();
}
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'b':
        paso=1;
// se grafica la curva de Bezier
        glutPostRedisplay();
        break;
    case 'n':
        paso=0;
// No se grafica la curva de Bezier
        glutPostRedisplay();
        break;
    case 27:
        exit(0);
        break;
    }
}
void mouse(int button,int state,int x,int y)
{
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(state==GLUT_DOWN){
            miCurva->AgregarPunto(display,mapScreenPointToWindow(x,y));
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;
	}
}
void reshape(int w, int h)
{
    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w/2,w/2,-h/2,h/2,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

Coordenada mapScreenPointToWindow(int x, int y){
    Coordenada retorno(0.0,0.0);
    x = x + glutGet((GLenum)GLUT_WINDOW_X);
    y = y + glutGet((GLenum)GLUT_WINDOW_Y);

    int px0 = glutGet((GLenum)GLUT_WINDOW_X) + glutGet(GLUT_WINDOW_WIDTH) / 2;
    int py0 = glutGet((GLenum)GLUT_WINDOW_Y) + glutGet(GLUT_WINDOW_HEIGHT) / 2;

    retorno.SetX(x - px0);
    retorno.SetY(py0 - y);

    return retorno;
}

void menuIngresoPuntos(int opc){
    miCurva->GetConfiguracionPuntosControl()->SetIngreso(opc);
}
void menuDibujarPcontrol(int opc){
    miCurva->GetConfiguracionPuntosControl()->SetVisibilidad(opc);
    display();
}
void menuDibujarCurvaSpline(int opc){
    miCurva->GetConfiguracionCurvaSpline()->SetVisibilidad(opc);
    display();
}
void menuZoom(int opc){
    switch(opc){
        case OPCION_ACTIVADO:{
            break;
        }
        case OPCION_DESACTIVADO:{
            break;
        }
    }
}
void menuModificarPuntos(int opc){
    switch(opc){
        case OPCION_ACTIVADO:{
            break;
        }
        case OPCION_DESACTIVADO:{
            break;
        }
    }
}
void menuPincelMovilPuntos(int opc){
    miCurva->GetConfiguracionPuntosControl()->SetForma(opc);
}
void menuGrosorPcontrol(int opc){
    switch(opc){
        case OPCION_AUMENTAR:{
            break;
        }
        case OPCION_DISMINUIR:{
            break;
        }
    }
}
void menuGrosorCurvaSpline(int opc){
    switch(opc){
        case OPCION_AUMENTAR:{
            break;
        }
        case OPCION_DISMINUIR:{
            break;
        }
    }
}
void menuColorPcontrol(int opc){
    switch(opc){
        case COLOR_ROJO:{
            break;
        }
        case COLOR_VERDE:{
            break;
        }
        case COLOR_AZUL:{
            break;
        }
        case COLOR_RANDOM:{
            break;
        }
    }
}
void menuColorCurvaSpline(int opc){
    switch(opc){
        case COLOR_ROJO:{
            break;
        }
        case COLOR_VERDE:{
            break;
        }
        case COLOR_AZUL:{
            break;
        }
        case COLOR_RANDOM:{
            break;
        }
    }
}
void menuPrincipal(int opc){
    //Definir salir si es necesario
}
