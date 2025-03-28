// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: {
      break;
    }
    case Qt::Key_D: {
      break;
    }
    case Qt::Key_A: {
      break;
    }
    case Qt::Key_W: {
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::paintGL () 
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  modelTransform();
  glBindVertexArray (VAO_Tower_S);
  glDrawArrays(GL_TRIANGLES, 0, model_Tower_S.faces().size() * 3);

  //modelTransform ();
  //glBindVertexArray (VAO_Terra);
  //glDrawArrays(GL_TRIANGLES, 0, 6);
  //glBindVertexArray (0);
}

void MyGLWidget::creaBuffers () 
{
    model_Tower_S.load("./Assets/Models/Tower_S.obj");

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Tower_S);
    glBindVertexArray(VAO_Tower_S);

    GLuint VBO_Tower_S[2];
    glGenBuffers(2, VBO_Tower_S);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_S[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_S.faces().size() * 3 * 3, model_Tower_S.VBO_vertices(), GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_S[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_S.faces().size() * 3 * 3, model_Tower_S.VBO_matdiff(), GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);
 
    glBindVertexArray (0);
}

void MyGLWidget::initializeGL() {
    initializeOpenGLFunctions(); 

    glClearColor(0.5, 0.7, 1.0, 1.0);

    glEnable(GL_DEPTH_TEST);

    carregaShaders();
    creaBuffers();
    ini_camera();
}

void MyGLWidget::carregaShaders() {
    BL2GLWidget::carregaShaders();

    projLoc = glGetUniformLocation(program->programId(), "PM");
    viewLoc = glGetUniformLocation(program->programId(), "VM");
}

void MyGLWidget::ini_camera() 
{
    VRP = centre;
    OBS = centre + glm::vec3(0,0,2*radi);
    UP = glm::vec3(0,1,0);

    D = glm::distance(VRP, OBS);

    viewTransform();

    if (RA > 1) {
        FOV = 2 * asin(radi/D);
    } else {
        FOV = 2 * atan(tan(FOV_ORIGINAL/2)/RA);
    }
    
    if (FOV_ORIGINAL == -1) FOV_ORIGINAL = FOV;
    
    ZNEAR = D - radi;
    ZFAR = D + radi;

    projectTransform();
}

void MyGLWidget::modelTransform () 
{
  glm::mat4 transform (1.0f);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::projectTransform() {
    glm::mat4 Proj = glm::perspective(FOV, RA, ZNEAR, ZFAR);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform() {
    glm::mat4 view = glm::lookAt(OBS, VRP, UP);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
}

void MyGLWidget::resizeGL (int width, int height) 
{
    RA = (float)width / (float)height;
}

