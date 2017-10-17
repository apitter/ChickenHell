#ifndef __OPENGLWINDOW_HEADER__
#define __OPENGLWINDOW_HEADER__
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// @class OpenGLWindow
/// @file openglwindow.h
/// @brief this is the base class for our OpenGL widget.
/// @author Gergina Petrova
/// @version 1.0
/// @date 10/11/15
/// Revision History :
/// 10.11.2015 - Initial version.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <QtGui/QWindow>
#include <QtGui/QtEvents>

QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
QT_END_NAMESPACE

//! [1]
class OpenGLWindow : public QWindow
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    virtual void render()=0;
    virtual void initialize()=0;

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event);

    void exposeEvent(QExposeEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_update_pending;

    QOpenGLContext *m_context;
    QOpenGLPaintDevice *m_device;
};

#endif // __OPENGLWINDOW_HEADER__
