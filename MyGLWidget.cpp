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
        plane_Rot_Y = (plane_Rot_Y - PLANE_ROTATE_INCREMENT) % 360;
      break;
    }
    case Qt::Key_A: {
        plane_Rot_Y = (plane_Rot_Y + PLANE_ROTATE_INCREMENT) % 360;
      break;
    }
    case Qt::Key_W: {
      break;
    }
    case Qt::Key_Space: {
        if (DEBUG_CAN_STOP) plane_move_speed = plane_move_speed == 0 ? PLANE_MOVE_SPEED_VALUE : 0;
        break;
    }
    default: event->ignore(); break;
  }
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

  glUniform3fv(bendLoc, 1, &tower_N_Bend[0]);
  modelTransform(tower_N_Pos, 0, 0);
  glBindVertexArray(VAO_Tower_N);
  glDrawArrays(GL_TRIANGLES, 0, model_Tower_N.faces().size() * 3);

  if (DEBUG_SHOW_HITBOXES) {
    glUniform3fv(bendLoc, 1, &ZERO_VECTOR[0]);
    modelTransform(tower_N_Pos + tower_N_Bend, 0, 0);
    glBindVertexArray(VAO_Debug_Hitbox_Tower_N);
    glDrawArrays(GL_TRIANGLES, 0, model_Debug_Tower.faces().size() * 3);
  }

  glUniform3fv(bendLoc, 1, &tower_S_Bend[0]);
  modelTransform (tower_S_Pos, 0, 0);
  glBindVertexArray (VAO_Tower_S);
  glDrawArrays(GL_TRIANGLES, 0, model_Tower_S.faces().size() * 3);

  if (DEBUG_SHOW_HITBOXES) {
    glUniform3fv(bendLoc, 1, &ZERO_VECTOR[0]);
    modelTransform(tower_S_Pos + tower_S_Bend, 0, 0);
    glBindVertexArray(VAO_Debug_Hitbox_Tower_S);
    glDrawArrays(GL_TRIANGLES, 0, model_Debug_Tower.faces().size() * 3);
  }

  float rotation_Radians = float(glm::radians(float(plane_Rot_Y)));
  plane_Pos = plane_Pos + glm::vec3(plane_move_speed * cos(rotation_Radians), 0, -plane_move_speed * sin(rotation_Radians));
  modelTransform(plane_Pos, rotation_Radians, plane_Rot_Z);
  glBindVertexArray(VAO_Plane);
  glDrawArrays(GL_TRIANGLES, 0, model_Plane.faces().size() * 3);

  glm::vec3 planeHitBoxPos = plane_Pos + glm::vec3(cos(rotation_Radians) * PLANE_HITBOX_POSITION_OFFSET, 0, sin(rotation_Radians) * PLANE_HITBOX_POSITION_OFFSET);
  float planeDistN = glm::distance(planeHitBoxPos, tower_N_Pos);
  float planeDistS = glm::distance(planeHitBoxPos, tower_S_Pos);
  std::cerr << planeDistN << std::endl;

  glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotation_Radians, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::vec3 forwardVector = glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 1));
  glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(0, 1, 0), forwardVector));
  glm::vec3 rightVector = -leftVector;

  // BEND TOWER N
  if (tower_N_Can_Bend && planeDistN < TOWER_BEND_DISTANCE) {
      glm::vec3 possibleBendPos_N_1 = tower_N_Pos + rightVector * TOWER_BEND_MAGNITUDE_MAX;
      glm::vec3 possibleBendPos_N_2 = tower_N_Pos + leftVector * TOWER_BEND_MAGNITUDE_MAX;

      float dist1_N = glm::distance(planeHitBoxPos, possibleBendPos_N_1);
      float dist2_N = glm::distance(planeHitBoxPos, possibleBendPos_N_2);
      if (dist1_N >= dist2_N) tower_N_Expected_Bend = glm::cross(forwardVector, glm::vec3(0, 1, 0)) * TOWER_BEND_MAGNITUDE_MAX;
      else tower_N_Expected_Bend = glm::cross(forwardVector, glm::vec3(0, -1, 0)) * TOWER_BEND_MAGNITUDE_MAX;

      tower_N_Bend =
          glm::vec3(tower_N_Bend.x > tower_N_Expected_Bend.x ? tower_N_Bend.x - TOWER_BEND_SPEED : tower_N_Bend.x + TOWER_BEND_SPEED,
              0,
              tower_N_Bend.z > tower_N_Expected_Bend.z ? tower_N_Bend.z - TOWER_BEND_SPEED : tower_N_Bend.z + TOWER_BEND_SPEED);
  }
  else {
      tower_N_Expected_Bend = ZERO_VECTOR;
  }


  // BEND TOWER S
  if (tower_S_Can_Bend && planeDistS < TOWER_BEND_DISTANCE) {
      glm::vec3 possibleBendPos_S_1 = tower_S_Pos + rightVector * TOWER_BEND_MAGNITUDE_MAX;
      glm::vec3 possibleBendPos_S_2 = tower_S_Pos + leftVector * TOWER_BEND_MAGNITUDE_MAX;

      float dist1_S = glm::distance(planeHitBoxPos, possibleBendPos_S_1);
      float dist2_S = glm::distance(planeHitBoxPos, possibleBendPos_S_2);
      if (dist1_S >= dist2_S) tower_S_Expected_Bend = glm::cross(forwardVector, glm::vec3(0, 1, 0)) * TOWER_BEND_MAGNITUDE_MAX;
      else tower_S_Expected_Bend = glm::cross(forwardVector, glm::vec3(0, -1, 0)) * TOWER_BEND_MAGNITUDE_MAX;
  }
  else {
      tower_S_Expected_Bend = ZERO_VECTOR;
  }


  tower_S_Bend =
      glm::vec3(tower_S_Bend.x > tower_S_Expected_Bend.x ? tower_S_Bend.x - TOWER_BEND_SPEED : tower_S_Bend.x + TOWER_BEND_SPEED,
          0,
          tower_S_Bend.z > tower_S_Expected_Bend.z ? tower_S_Bend.z - TOWER_BEND_SPEED : tower_S_Bend.z + TOWER_BEND_SPEED);

  bool col_Tower_N = isPlaneInTower(tower_N_Pos + tower_N_Bend);
  bool col_Tower_S = isPlaneInTower(tower_S_Pos + tower_S_Bend);
  if (DEBUG_PRINT_COLLISIONS) {
    if (col_Tower_N) std::cerr << "Boom torre N" << std::endl;
    if (col_Tower_S) std::cerr << "Boom torre S" << std::endl;
  }

  if (DEBUG_SHOW_HITBOXES) {
    glBindVertexArray(VAO_Debug_Hitbox_Plane);
    glDrawArrays(GL_TRIANGLES, 0, model_Debug_Plane.faces().size() * 3);
  }

  modelTransform (glm::vec3(0,0,0), 0, 0);
  glBindVertexArray (VAO_Ground);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindVertexArray (0);

  ini_camera();
  update();
}

void MyGLWidget::creaBuffers () 
{
    model_Tower_N.load("./Assets/Models/Tower_N.obj");
    model_Tower_S.load("./Assets/Models/Tower_S.obj");
    model_Plane.load("./Assets/Models/Plane.obj");
    model_Debug_Tower.load("./Assets/Models/Tower_Hitbox.obj");
    model_Debug_Plane.load("./Assets/Models/Plane_Hitbox.obj");

    // NORTH TOWER
    glGenVertexArrays(1, &VAO_Tower_N);
    glBindVertexArray(VAO_Tower_N);

    GLuint VBO_Tower_N[2];
    glGenBuffers(2, VBO_Tower_N);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_N[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_N.faces().size() * 3 * 3, model_Tower_N.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_N[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_N.faces().size() * 3 * 3, model_Tower_N.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // SOUTH TOWER
    glGenVertexArrays(1, &VAO_Tower_S);
    glBindVertexArray(VAO_Tower_S);

    GLuint VBO_Tower_S[2];
    glGenBuffers(2, VBO_Tower_S);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_S[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_S.faces().size() * 3 * 3, model_Tower_S.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_S[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Tower_S.faces().size() * 3 * 3, model_Tower_S.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // PLANE
    glGenVertexArrays(1, &VAO_Plane);
    glBindVertexArray(VAO_Plane);

    GLuint VBO_Plane[2];
    glGenBuffers(2, VBO_Plane);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Plane[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Plane.faces().size() * 3 * 3, model_Plane.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Plane[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Plane.faces().size() * 3 * 3, model_Plane.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // DEBUG TOWER NORTH
    glGenVertexArrays(1, &VAO_Debug_Hitbox_Tower_N);
    glBindVertexArray(VAO_Debug_Hitbox_Tower_N);

    GLuint VBO_Tower_Debug_N[2];
    glGenBuffers(2, VBO_Tower_Debug_N);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_Debug_N[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Tower.faces().size() * 3 * 3, model_Debug_Tower.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_Debug_N[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Tower.faces().size() * 3 * 3, model_Debug_Tower.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // DEBUG TOWER SOUTH
    glGenVertexArrays(1, &VAO_Debug_Hitbox_Tower_S);
    glBindVertexArray(VAO_Debug_Hitbox_Tower_S);

    GLuint VBO_Tower_Debug_S[2];
    glGenBuffers(2, VBO_Tower_Debug_S);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_Debug_S[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Tower.faces().size() * 3 * 3, model_Debug_Tower.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_Debug_S[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Tower.faces().size() * 3 * 3, model_Debug_Tower.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // DEBUG PLANE
    glGenVertexArrays(1, &VAO_Debug_Hitbox_Plane);
    glBindVertexArray(VAO_Debug_Hitbox_Plane);

    GLuint VBO_Plane_Debug[2];
    glGenBuffers(2, VBO_Plane_Debug);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Plane_Debug[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Plane.faces().size() * 3 * 3, model_Debug_Plane.VBO_vertices(), GL_STATIC_DRAW);

    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Tower_Debug_N[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model_Debug_Plane.faces().size() * 3 * 3, model_Debug_Plane.VBO_matdiff(), GL_STATIC_DRAW);

    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // GROUND
    glBindVertexArray(0);

    glm::vec3 ground_Color[6] = {
        glm::vec3(1,0,0),
        glm::vec3(0,1,0),
        glm::vec3(0,0,1),
        glm::vec3(1,0,0),
        glm::vec3(0,0,1),
        glm::vec3(0,1,0),
    };

    glGenVertexArrays(1, &VAO_Ground);
    glBindVertexArray(VAO_Ground);

    GLuint VBO_Ground[2];
    glGenBuffers(2, VBO_Ground);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Ground[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_Ground_Vertexs), model_Ground_Vertexs, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLocGround, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLocGround);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Ground[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ground_Color), ground_Color, GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLocGround, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLocGround);

    glBindVertexArray(0);
}

bool MyGLWidget::isPlaneInTower(glm::vec3 towerPosition) 
{
    float C = cos(plane_Rot_Y);
    float S = sin(plane_Rot_Y);

    glm::vec3 p1 = plane_Pos + glm::vec3(C * PLANE_HITBOX_SIZE, 0, S * PLANE_HITBOX_SIZE);
    glm::vec3 p2 = plane_Pos + glm::vec3(-C * PLANE_HITBOX_SIZE, 0, S * PLANE_HITBOX_SIZE);
    glm::vec3 p3 = plane_Pos + glm::vec3(C * PLANE_HITBOX_SIZE, 0, -S * PLANE_HITBOX_SIZE);
    glm::vec3 p4 = plane_Pos + glm::vec3(-C * PLANE_HITBOX_SIZE, 0, -S * PLANE_HITBOX_SIZE);

    float towerMaxX = towerPosition.x + TOWER_HITBOX_SIZE;
    float towerMinX = towerPosition.x - TOWER_HITBOX_SIZE;
    float towerMaxZ = towerPosition.z + TOWER_HITBOX_SIZE;
    float towerMinZ = towerPosition.z - TOWER_HITBOX_SIZE;
    
    if (p1.x > towerMinX && p1.x < towerMaxX && p1.z > towerMinZ && p1.z < towerMaxZ) return true;
    if (p2.x > towerMinX && p2.x < towerMaxX && p2.z > towerMinZ && p2.z < towerMaxZ) return true;
    if (p3.x > towerMinX && p3.x < towerMaxX && p3.z > towerMinZ && p3.z < towerMaxZ) return true;
    if (p4.x > towerMinX && p4.x < towerMaxX && p4.z > towerMinZ && p4.z < towerMaxZ) return true;

    return false;
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
    vertexLocGround = glGetAttribLocation(program->programId(), "vertex");
    colorLocGround = glGetAttribLocation(program->programId(), "color");
    bendLoc = glGetUniformLocation(program->programId(), "bend");
}

void MyGLWidget::ini_camera() 
{
    //VRP = glm::vec3(0, 30, 0);
    //OBS = glm::vec3(0, 20, 2*radi);

    float rotation_Radians = float(glm::radians(float(plane_Rot_Y)));
    //OBS = plane_Pos;
    OBS = plane_Pos + glm::vec3(0, 1, 0);
    VRP = plane_Pos + glm::vec3(10 * cos(rotation_Radians), 0, -10 * sin(rotation_Radians));
    UP = glm::vec3(0, 1, 0);

    D = 2 * radi;

    viewTransform();

    if (RA > 1) {
        FOV = 2 * asin(radi/D);
    } else {
        FOV = 2 * atan(tan(FOV_ORIGINAL/2)/RA);
    }
    
    if (FOV_ORIGINAL == -1) FOV_ORIGINAL = FOV;
    
    //ZNEAR = D - radi;
    //ZFAR = D + radi;
    ZNEAR = 1;
    ZFAR = 100;

    projectTransform();
}

void MyGLWidget::modelTransform (glm::vec3 position, float rotationY, float rotationZ)
{
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, rotationY, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, rotationZ, glm::vec3(0, 0, 1));
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
    ini_camera();
}

