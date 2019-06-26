#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include<conio.h>
#include <math.h>
#include <iostream>
#include <list>
#include <vector>
#include <time.h>

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

GLfloat const EJE_TAMANO = 200;

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

    void Set(GLfloat _x, GLfloat _y, GLfloat _z){x = _x; y = _y; z = _z;}

    Coordenada operator -(Coordenada c2){ return Coordenada(x-c2.x,y-c2.y,z-c2.z); }
    Coordenada operator +(Coordenada c2){ return Coordenada(x+c2.x,y+c2.y,z+c2.z); }
    Coordenada operator *(GLfloat n){ return Coordenada(x * n, y * n, z * n); }
    Coordenada operator /(GLfloat n){ return Coordenada(x / n, y / n, z / n); }

    GLfloat Distancia(GLfloat _x, GLfloat _y, GLfloat _z = 0){
        GLfloat dx = _x - x;
        GLfloat dy = _y - y;
        GLfloat dz = _z - z;
        return sqrt(dx*dx + dy*dy + dz*dz);
    }

    void Interpolar(GLfloat _velocidad, GLfloat _x, GLfloat _y, GLfloat _z = 0){
        GLfloat dx = _x - x;
        GLfloat dy = _y - y;
        GLfloat dz = _z - z;
        GLfloat d = sqrt(dx*dx + dy*dy + dz*dz);
        if(d >= 0.01 * _velocidad){
            GLfloat factor = 0.01 * _velocidad / d;
            dx = dx * factor;
            dy = dy * factor;
            dz = dz * factor;
            x = x + dx;
            y = y + dy;
            z = z + dz;
        }
    }

    bool PuntoCercano(GLfloat _distancia, GLfloat _x, GLfloat _y, GLfloat _z = 0){
        return Distancia(_x,_y,_z) <= _distancia;
    }


    ~Coordenada(){}
};

class GeneradorFormas{
public:
    void Circulo(int radio, Coordenada centro){
        glPointSize(0.1f);
        glPushMatrix();
            glTranslatef(centro.GetX(), centro.GetY(), centro.GetZ());
            int cantidadPuntos = 100;
            float angulo = 2.0f * 3.1416f / cantidadPuntos;

            glBegin(GL_POLYGON);
                double angulo1=0.0;
                glVertex2d(radio * cos(0.0) , radio * sin(0.0));
                int i;
                for (i=0; i<cantidadPuntos; i++)
                {
                    glVertex2d(radio * cos(angulo1), radio *sin(angulo1));
                    angulo1 += angulo;
                }
            glEnd();
        glPopMatrix();
    }
    void Cuadrado(int lado, Coordenada centro){
        glPointSize(lado);
        glBegin(GL_POINTS);
            glVertex3f(centro.GetX(),centro.GetY(),centro.GetZ());
        glEnd();
    }
    void Triangulo(int lado, Coordenada centro){
        glPointSize(0.01f);
        glPushMatrix();
            glTranslatef(centro.GetX(), centro.GetY(), centro.GetZ());

            glBegin(GL_TRIANGLES);
                glVertex3f(0.0, 0.57735f * lado, 0.0);
                glVertex3f(0.5 * lado, - 0.28868 * lado, 0.0);
                glVertex3f(- 0.5 * lado, - 0.28868 * lado, 0.0);
            glEnd();
        glPopMatrix();
    }
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
        void SetGrosor(GLfloat _grosor){if(_grosor <= GROSOR_MAXIMO && _grosor >= GROSOR_MINIMO) grosor = _grosor;}

        ~CurvaSplineConfiguracion(){delete color;}
    };
    class PuntoControlConfiguracion : public CurvaSplineConfiguracion {
        int forma;
        int ingreso;
        int modificabilidad;
    public:
        PuntoControlConfiguracion(int _visibilidad, Color* _color, GLfloat _grosor, int _forma, int _ingreso, int _modificabilidad) : CurvaSplineConfiguracion(_visibilidad,_color,_grosor){
            forma = _forma;
            ingreso = _ingreso;
            modificabilidad = _modificabilidad;
        }
        int GetForma(){return forma;}
        void SetForma(int _forma){forma = _forma;}
        int GetIngreso(){return ingreso;}
        void SetIngreso(int _ingreso){ingreso = _ingreso;}
        int GetModificabilidad(){return modificabilidad;}
        void SetModificabilidad(int _modificabilidad){modificabilidad = _modificabilidad;}
    };

    CurvaSplineConfiguracion *configuracionCurva;
    PuntoControlConfiguracion *configuracionPuntos;
    GeneradorFormas *generadorFormas;
    vector<Coordenada> *puntosControl;

    Coordenada *puntoDestino;
    int indicePuntoSeleccionado = -1;

    void dibujarPuntos(){
        if(configuracionPuntos->GetVisibilidad() == OPCION_ACTIVADO){
            glColor3f(configuracionPuntos->GetColor()->r,configuracionPuntos->GetColor()->g,configuracionPuntos->GetColor()->b);
            for(int i = 0; i< puntosControl->size(); i++){
                if(i != indicePuntoSeleccionado){
                    switch(configuracionPuntos->GetForma()){
                        case FORMA_CIRCULO:{
                            generadorFormas->Circulo(configuracionPuntos->GetGrosor() / 2.0,(*puntosControl)[i]);
                            break;
                        }
                        case FORMA_CUADRADO:{
                            generadorFormas->Cuadrado(configuracionPuntos->GetGrosor(),(*puntosControl)[i]);
                            break;
                        }
                        case FORMA_TRIANGULO:{
                            generadorFormas->Triangulo(configuracionPuntos->GetGrosor(),(*puntosControl)[i]);
                            break;
                        }
                    }
                }
            }

            if(indicePuntoSeleccionado >= 0){
                glColor3f(1 - configuracionPuntos->GetColor()->r,1 - configuracionPuntos->GetColor()->g,1 - configuracionPuntos->GetColor()->b);
                switch(configuracionPuntos->GetForma()){
                    case FORMA_CIRCULO:{
                        generadorFormas->Circulo(configuracionPuntos->GetGrosor() / 2.0,(*puntosControl)[indicePuntoSeleccionado]);
                        break;
                    }
                    case FORMA_CUADRADO:{
                        generadorFormas->Cuadrado(configuracionPuntos->GetGrosor(),(*puntosControl)[indicePuntoSeleccionado]);
                        break;
                    }
                    case FORMA_TRIANGULO:{
                        generadorFormas->Triangulo(configuracionPuntos->GetGrosor(),(*puntosControl)[indicePuntoSeleccionado]);
                        break;
                    }
                }

            }
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
        generadorFormas = new GeneradorFormas();
        puntosControl = new vector<Coordenada>();
        configuracionCurva = new CurvaSplineConfiguracion(OPCION_ACTIVADO,new Color(0.0,0.0,1.0),2);
        configuracionPuntos = new PuntoControlConfiguracion(OPCION_ACTIVADO,new Color(1.0,0.0,0.0),5,FORMA_CIRCULO, OPCION_ACTIVADO, OPCION_ACTIVADO);
        puntoDestino = new Coordenada(0.0,0.0,0.0);
    }
    bool Dibujar(){
        glPushMatrix();
        dibujarPuntos();
        dibujarCurvaSpline();
        glPopMatrix();
        return true;
    }
    void AgregarPunto(void (*_display)(), GLfloat _x, GLfloat _y, GLfloat _z = 0){
        AgregarPunto(_display,Coordenada(_x,_y,_z));
    }
    void AgregarPunto(void (*_display)(), Coordenada coordenada){
        if(configuracionPuntos->GetIngreso() == OPCION_ACTIVADO){
            puntosControl->push_back(coordenada);
            _display();
        }
    }
    bool EliminarPuntoSeleccionado(void (*_display)()){
        if(indicePuntoSeleccionado >= 0 && configuracionPuntos->GetModificabilidad() == OPCION_ACTIVADO){
            puntosControl->erase(puntosControl->begin() + indicePuntoSeleccionado);
            indicePuntoSeleccionado = -1;
            _display();
            return true;
        }
        return false;
    }

    int SeleccionarPuntoAMover(GLfloat _radioDeteccion, Coordenada cord){
        SeleccionarPuntoAMover(_radioDeteccion,cord.GetX(),cord.GetY(),cord.GetZ());
    }
    int SeleccionarPuntoAMover(GLfloat _radioDeteccion, GLfloat _x, GLfloat _y, GLfloat _z = 0){
        if(indicePuntoSeleccionado < 0 && configuracionPuntos->GetModificabilidad() == OPCION_ACTIVADO){
            int ultimo = puntosControl->size() - 1;
            for(int i=ultimo; i >= 0; i--){
                if((*puntosControl)[i].PuntoCercano(_radioDeteccion,_x,_y,_z)){
                    indicePuntoSeleccionado = i;
                    puntoDestino->Set(_x,_y,_z);
                    break;
                }
            }
        }
        return indicePuntoSeleccionado;
    }
    void SoltarPuntoAMover(void (*_display)()){
        indicePuntoSeleccionado = -1;
        _display();
    }
    void MoverPuntoSeleccionado(GLfloat _x, GLfloat _y, GLfloat _z = 0){
        puntoDestino->Set(_x,_y,_z);
    }

    void Update(void (*_display)()){
        if(indicePuntoSeleccionado >= 0 && configuracionPuntos->GetModificabilidad() == OPCION_ACTIVADO){
            (*puntosControl)[indicePuntoSeleccionado].Interpolar(100.0f,puntoDestino->GetX(),puntoDestino->GetY(),puntoDestino->GetZ());
            _display();
        }
    }

    PuntoControlConfiguracion* GetConfiguracionPuntosControl(){return configuracionPuntos;}
    CurvaSplineConfiguracion* GetConfiguracionCurvaSpline(){return configuracionCurva;}

    ~CurvaSpline(){delete &puntosControl; delete &configuracionCurva; delete &configuracionPuntos; }

};
void init(void);
void display(void);
void update(void);
void reshape(int,int);
void keyboard(unsigned char, int, int);
void mouse(int button,int state,int x,int y);
void mousePassive(int x,int y);
void GraficarPuntos();
void GraficarCurvaSpline();

void ejes();
Coordenada mapeoPuntoPantallaAVentana(int, int);

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

float dim=400;
int w, h;
GLfloat zoom = 1;
int estadoZoom = OPCION_ACTIVADO;
bool puntoSeleccionado = false;

CurvaSpline *miCurva;

int main(int argc, char** argv)
{
    srand (time(0));

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
    glutAddSubMenu("Modificacion puntos", menu_modificarpuntos);
    glutAddSubMenu("Pincel Movil", menu_pincelmovilpuntos);
    glutAddSubMenu("Grosor", menu_grosor);
    glutAddSubMenu("Color", menu_color);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(mousePassive);
    glutIdleFunc(update);
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
    reshape(w,h);

    ejes();
    miCurva->Dibujar();
    glFlush();
}
void update(void){
    miCurva->Update(display);

    Sleep(1);
}
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '+':
        if(estadoZoom == OPCION_ACTIVADO){
            zoom -= 0.05;
            glutPostRedisplay();
        }
        break;
    case '-':
        if(estadoZoom == OPCION_ACTIVADO){
            zoom += 0.05;
            glutPostRedisplay();
        }
        break;
    case 27:
        exit(0);
        break;
    }
}
time_t tiempoAnterior;
time_t tiempoActual;

void mouse(int button,int state,int x,int y)
{
    Coordenada puntoMapeado = mapeoPuntoPantallaAVentana(x,y);
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(state==GLUT_DOWN){
            if(!puntoSeleccionado){
                puntoSeleccionado = miCurva->SeleccionarPuntoAMover(8.0f,puntoMapeado) >= 0;

                if(!puntoSeleccionado){
                    miCurva->AgregarPunto(display,puntoMapeado);
                }else{
                    time(&tiempoAnterior);
                }
            }else{
                time(&tiempoActual);
                if(tiempoActual - tiempoAnterior <= 0.5){
                    miCurva->EliminarPuntoSeleccionado(display);
                }else{
                    miCurva->SoltarPuntoAMover(display);
                }
                puntoSeleccionado = false;
            }
		}
		break;

	default:
		break;
	}
}

void mousePassive(int x,int y){
    if(puntoSeleccionado){
        Coordenada puntoMapeado = mapeoPuntoPantallaAVentana(x,y);
        miCurva->MoverPuntoSeleccionado(puntoMapeado.GetX(),puntoMapeado.GetY(),puntoMapeado.GetZ());
    }
}

void reshape(int _w, int _h)
{
    w = _w; h = _h;

    glViewport(0,0,(GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(w*zoom)/2,(w*zoom)/2,-(h*zoom)/2,(h*zoom)/2,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

Coordenada mapeoPuntoPantallaAVentana(int x, int y){

    Coordenada retorno(0.0,0.0);
    x = x + glutGet((GLenum)GLUT_WINDOW_X);
    y = y + glutGet((GLenum)GLUT_WINDOW_Y);

    int px0 = glutGet((GLenum)GLUT_WINDOW_X) + glutGet(GLUT_WINDOW_WIDTH) / 2;
    int py0 = glutGet((GLenum)GLUT_WINDOW_Y) + glutGet(GLUT_WINDOW_HEIGHT) / 2;

    retorno.SetX((x - px0)*zoom);
    retorno.SetY((py0 - y)*zoom);

    return retorno;
}

void ejes(){
    //x
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(EJE_TAMANO,0.0,0.0);
    glEnd();

    //y
    glBegin(GL_LINES);
        glColor3f(0.0,1.0,0.0);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,EJE_TAMANO,0.0);
    glEnd();
    //x
    glBegin(GL_LINES);
        glColor3f(0.0,0.0,1.0);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,0.0,EJE_TAMANO);
    glEnd();
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
    estadoZoom = opc;
}
void menuModificarPuntos(int opc){
    miCurva->GetConfiguracionPuntosControl()->SetModificabilidad(opc);
    if(opc == OPCION_DESACTIVADO){
        miCurva->SoltarPuntoAMover(display);
        puntoSeleccionado = false;
    }
}
void menuPincelMovilPuntos(int opc){
    miCurva->GetConfiguracionPuntosControl()->SetForma(opc);
    display();
}
void menuGrosorPcontrol(int opc){
    GLfloat aumento = 1.0f;
    switch(opc){
        case OPCION_AUMENTAR:{
            miCurva->GetConfiguracionPuntosControl()->SetGrosor(miCurva->GetConfiguracionPuntosControl()->GetGrosor() + aumento);
            break;
        }
        case OPCION_DISMINUIR:{
            miCurva->GetConfiguracionPuntosControl()->SetGrosor(miCurva->GetConfiguracionPuntosControl()->GetGrosor() - aumento);
            break;
        }
    }
    display();
}
void menuGrosorCurvaSpline(int opc){
    GLfloat aumento = 1.0f;
    switch(opc){
        case OPCION_AUMENTAR:{
            miCurva->GetConfiguracionCurvaSpline()->SetGrosor(miCurva->GetConfiguracionCurvaSpline()->GetGrosor() + aumento);
            break;
        }
        case OPCION_DISMINUIR:{
            miCurva->GetConfiguracionCurvaSpline()->SetGrosor(miCurva->GetConfiguracionCurvaSpline()->GetGrosor() - aumento);
            break;
        }
    }
    display();
}
void menuColorPcontrol(int opc){
    switch(opc){
        case COLOR_ROJO:{
            miCurva->GetConfiguracionPuntosControl()->SetColor(1.0,0.0,0.0);
            break;
        }
        case COLOR_VERDE:{
            miCurva->GetConfiguracionPuntosControl()->SetColor(0.0,1.0,0.0);
            break;
        }
        case COLOR_AZUL:{
            miCurva->GetConfiguracionPuntosControl()->SetColor(0.0,0.0,1.0);
            break;
        }
        case COLOR_RANDOM:{
            GLfloat r = ((double) rand() / (RAND_MAX));
            GLfloat g = ((double) rand() / (RAND_MAX));
            GLfloat b = ((double) rand() / (RAND_MAX));
            miCurva->GetConfiguracionPuntosControl()->SetColor(r,g,b);
            break;
        }
    }
    display();
}
void menuColorCurvaSpline(int opc){
    switch(opc){
        case COLOR_ROJO:{
            miCurva->GetConfiguracionCurvaSpline()->SetColor(1.0,0.0,0.0);
            break;
        }
        case COLOR_VERDE:{
            miCurva->GetConfiguracionCurvaSpline()->SetColor(0.0,1.0,0.0);
            break;
        }
        case COLOR_AZUL:{
            miCurva->GetConfiguracionCurvaSpline()->SetColor(0.0,0.0,1.0);
            break;
        }
        case COLOR_RANDOM:{
            GLfloat r = ((double) rand() / (RAND_MAX));
            GLfloat g = ((double) rand() / (RAND_MAX));
            GLfloat b = ((double) rand() / (RAND_MAX));
            miCurva->GetConfiguracionCurvaSpline()->SetColor(r,g,b);
            break;
        }
    }
    display();
}
void menuPrincipal(int opc){
    //Definir salir si es necesario
}
