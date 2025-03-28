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
    virtual void modelTransform();
    virtual void resizeGL (int width, int height);

    void carregaShaders();
    void projectTransform();
    void viewTransform();

    void ini_camera();

    Model model_Tower_S;
    
    GLuint projLoc;
    GLuint viewLoc;
    GLuint VAO_Tower_S;

    glm::vec3 centre;
    float radi;

    float RA = 1.f;

    glm::vec3 VRP;
    glm::vec3 OBS;
    glm::vec3 UP;

    float FOV_ORIGINAL = -1;
    float FOV;
    float ZNEAR;
    float ZFAR;
    float D = 0;
};
