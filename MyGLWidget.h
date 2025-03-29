// MyGLWidget.h
#include "BL2GLWidget.h"
#include "model.h"

class MyGLWidget : public BL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL2GLWidget(parent) {}
    ~MyGLWidget();

  private:
    int printOglError(const char file[], int line, const char func[]);

    virtual void creaBuffers ();
    virtual void initializeGL();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void resizeGL (int width, int height);

    void modelTransform(glm::vec3 position, float rotationY, float rotationZ);

    void carregaShaders();
    void projectTransform();
    void viewTransform();

    void ini_camera();

    // MODEL DATA
    Model model_Tower_N;
    Model model_Tower_S;
    Model model_Plane;

    glm::vec3 model_Ground_Vertexs[6] = {
        glm::vec3(10, -1.0, 10),
        glm::vec3(-10, -1.0, -10),
        glm::vec3(-10, -1.0, 10),

        glm::vec3(10, -1.0, 10),
        glm::vec3(10, -1.0, -10),
        glm::vec3(-10, -1.0, -10)
    };

    glm::vec3 tower_N_Pos = glm::vec3(6.4, 0, 0);
    glm::vec3 tower_S_Pos = glm::vec3(-6.4, 0, 0);
    
    GLuint projLoc;
    GLuint viewLoc;

    GLuint VAO_Tower_N;
    GLuint VAO_Tower_S;
    GLuint VAO_Plane;
    GLuint VAO_Ground;
    GLuint vertexLocGround;
    GLuint colorLocGround;

    // PLANE DATA
    glm::vec3 plane_Pos = glm::vec3(-100, 30, 0);
    float plane_Rot_Z = 0;
    int plane_Rot_Y = 0;

    const float PLANE_MOVE_SPEED = 0.05;
    const int PLANE_ROTATE_SPEED = 1;

    // CAMERA DATA
    float radi = 25;

    float RA = 1.f;

    glm::vec3 VRP = glm::vec3(0,0,0);
    glm::vec3 OBS;
    glm::vec3 UP;

    float FOV_ORIGINAL = -1;
    float FOV;
    float ZNEAR;
    float ZFAR;
    float D = 0;
};
