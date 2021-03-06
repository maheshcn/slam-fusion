#include <QtGui>
#include <QtOpenGL>

#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent, QGLWidget *shareWidget)
    : QGLWidget(parent, shareWidget)
{
    clearColor = Qt::black;
    xRot = 0;
    yRot = 0;
    zRot = 0;
#ifdef QT_OPENGL_ES_2
    program = 0;
#endif
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(200, 200);
}

void GLWidget::rotateBy(int xAngle, int yAngle, int zAngle)
{
    xRot += xAngle;
    yRot += yAngle;
    zRot += zAngle;
    updateGL();
}

void GLWidget::setClearColor(const QColor &color)
{
    clearColor = color;
    updateGL();
}

void GLWidget::updateTexture(QImage *image)
{



    deleteTexture(textures[0]);
    textures[0] = bindTexture(*image, GL_TEXTURE_2D);


     updateGL();


}

void GLWidget::initializeGL()
{

    makeCoords();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
#ifndef QT_OPENGL_ES_2
    glEnable(GL_TEXTURE_2D);
#endif

#ifdef QT_OPENGL_ES_2

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    program = new QGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    program->link();

    program->bind();
    program->setUniformValue("texture", 0);

#endif
}

void GLWidget::paintGL()
{
    qglClearColor(clearColor);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -10.0f);
        glRotatef(0 / 16.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(0 / 16.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(0 / 16.0f, 0.0f, 0.0f, 1.0f);
        glVertexPointer(3, GL_FLOAT, 0, vertices.constData());
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords.constData());
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glDrawArrays(GL_TRIANGLE_FAN,0, 4);

}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
       glViewport(0,0,width,height);
       qDebug()<<"Width"<<width;
        qDebug()<<"Height"<<height;

       glMatrixMode(GL_PROJECTION);
       glLoadIdentity();
        qDebug()<<"hello resize";

       // glOrtho( 0.0f, width, height, 0.0f, 0.0f, 1.0f);

        glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);

       glMatrixMode(GL_MODELVIEW);


}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        rotateBy(8 * dy, 8 * dx, 0);
    } else if (event->buttons() & Qt::RightButton) {
        rotateBy(8 * dy, 0, 8 * dx);
    }
    lastPos = event->pos();
}

void GLWidget::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

void GLWidget::makeCoords()
{

    static const int coords[6][4][3] = {
        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },

    };
    for (int j = 0; j < 4; ++j) {
           texCoords.append
               (QVector2D(j == 0 || j == 3, j == 0 || j == 1));
           vertices.append
               (QVector3D(1* coords[0][j][0], 1 * coords[0][j][1],
                          1 * coords[0][j][2]));
       }
}



