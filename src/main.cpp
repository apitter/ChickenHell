#include <QString>
#include <openglwindow.h>
#include <QApplication>
#include <scene.h>
#include <gameui.h>
#include "game.h"

QApplication *g_pApp;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    g_pApp = &app;
    // create an OpenGL format specified
    QSurfaceFormat glFormat(QSurfaceFormat::DeprecatedFunctions);
    // Desktop OpenGL rendering
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    // set Open GL format
    glFormat.setVersion(4,0);
    // set CompatibilityProfile - Functionality from earlier OpenGL versions(before 3.0) is available.
    glFormat.setProfile(QSurfaceFormat::CompatibilityProfile);
    // set the number of samples for multisampling
    // will need to enable glEnable(GL_MULTISAMPLE); once we have a context
    glFormat.setSamples(4);
    // set the depth buffer to 24 bits
    glFormat.setDepthBufferSize(24);
    // set double buffering
    glFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    // set swap interval - cannot be changed
    glFormat.setSwapInterval(1);

    // now we are going to create our scene window
    Scene *window = Scene::getInstance();
    window->setFormat(glFormat);
    // init observers
    // game
    InitGame();
    // UI
    InitUI();
    // and show the window
    window->show();

    int r = app.exec();
    return r;
}
