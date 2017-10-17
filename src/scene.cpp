#include <scene.h>
#include <QtCore/QCoreApplication>
#include <QMessageBox>
#include <ngl/NGLInit.h>
#include <cfgfile.h>
#include <game_scores.h>
#include <ngl/ShaderLib.h>
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
Scene *Scene::getInstance()
{
    static Scene *s_instance = 0;
    if(s_instance == 0)
    {
        static auto atexit_handler = []() { delete s_instance; s_instance = 0; };
        s_instance = new Scene();
        std::atexit(atexit_handler);
    }
    return s_instance;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
Scene::Key &Scene::Key::operator=(Key _key)
{
    if (!(*this == _key))
    {
        m_key = _key.m_key;
        m_pressed = _key.m_pressed;
    }
    return *this;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
bool Scene::Key::operator ==(Key &_key)
{
    if (this == &_key) return true;
    return (_key.m_key == m_key && _key.m_pressed == m_pressed);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
Scene::Scene() 
    : OpenGLWindow()
    , m_isInitialized(false)
    , m_isMouseCursorInWindow(true)
    , m_leftMouseButtonPressed(false)
    , m_rightMouseButtonPressed(false)
    , m_frame(0)
    , m_gameState(Game::State::Intro)
    , m_stat(0)
{
    // load config file
    m_config = new CfgFile(this);
    // clear history
    memset(&m_fps, 0, sizeof(Fps));
    // set title
    setTitle(ApplicationTitle);
    // set minimal window size
    setMinimumSize(QSize(MinWindowWidth, MinWindowHeigh));
    // set window size and position depending from configuration
    m_isFullScreen = (m_config->m_visibility == QWindow::FullScreen) ? true : false;
    setPosition(m_config->m_windowPosition);
    resize(m_config->m_windowSize);
    // unlock the configuration
    m_config->m_isLocked = false;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
Scene::~Scene()
{
    m_isInitialized = 0;
    if(m_stat) { delete m_stat; }
    if (m_config) { delete m_config; }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::show()
{
    setVisibility(m_config->m_visibility);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::attach(Observer *_obs)
{
    switch(_obs->getType())
    {
        case ObserverUnknown:
            break;
        case ObserverScene:
            m_SceneObserver.push_back((SceneObserver *)_obs);
            break;
        case ObserverKeyboard:
            m_KeyboardObserver.push_back((KeyboardObserver *)_obs);
            break;
        case ObserverMouse:
            m_MouseObserver.push_back((MouseObserver *)_obs);
            break;
        default:
            break;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::detach(Observer *_obs)
{
    switch(_obs->getType())
    {
        case ObserverUnknown:
            break;
        case ObserverScene:
            if(m_SceneObserver.size())
            {
                m_SceneObserver.erase(std::find(m_SceneObserver.begin(), m_SceneObserver.end(), (SceneObserver *)_obs));
            }
            break;
        case ObserverKeyboard:
            if(m_KeyboardObserver.size())
            {
                m_KeyboardObserver.erase(std::find(m_KeyboardObserver.begin(), m_KeyboardObserver.end(), (KeyboardObserver *)_obs));
            }
            break;
        case ObserverMouse:
            if(m_MouseObserver.size())
            {
                m_MouseObserver.erase(std::find(m_MouseObserver.begin(), m_MouseObserver.end(), (MouseObserver *)_obs));
            }
            break;
        default:
            break;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::resizeGL()
{
    if (m_isInitialized)
    {
        glViewport(0, 0, width(), height());
        // setup camera
        ngl::Real w = (ngl::Real)width();
        ngl::Real h = (ngl::Real)height();
        m_projection2D = ngl::ortho(0, w, 0, h, -1, 1);
        for(std::vector<SceneObserver*>::iterator it = m_SceneObserver.begin(); it != m_SceneObserver.end(); ++it)
        {
            SceneObserver* o = *it;
            o->onResize(width(), height());
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::wrapMousePos()
{
    QPoint pt = mapFromGlobal(QCursor::pos());
    m_screenPos = QPoint(pt.x() + x(), pt.y() + y());
    if (m_isMouseCursorInWindow)
    { // wrap mouse pos
        if (pt.x() < 0)
        {
            m_winPos.setX(0);
            m_glWinPos.setX(0);
        }
        else if (pt.x() > width())
        {
            m_winPos.setX(width());
            m_glWinPos.setX(width());
        }
        else
        {
            m_winPos.setX(pt.x());
            m_glWinPos.setX(pt.x());
        }
        if (pt.y() < 0)
        {
            m_winPos.setY(0);
            m_glWinPos.setY(height());
        }
        else if (pt.y() > height())
        {
            m_winPos.setY(height());
            m_glWinPos.setY(0);
        }
        else
        {
            m_winPos.setY(pt.y());
            m_glWinPos.setY(height() - pt.y());
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
bool Scene::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::Move:
            m_config->OnMove();
            break;
        case QEvent::Resize:
            m_config->OnResize();
            resizeGL();
            break;
        case QEvent::Close:
            m_config->OnClose();
            break;
        case QEvent::WindowStateChange:
        {
            Qt::WindowStates state = windowState();
            m_config->OnWindowStateChange(state);
            break;
        }
        
        case QEvent::Enter:
            if(m_isInitialized && m_MouseObserver.size())
            {
                // notifyObserver
                for(std::vector<MouseObserver*>::iterator it = m_MouseObserver.begin(); it != m_MouseObserver.end(); ++it)
                {
                    MouseObserver* o = *it;
                    o->onMove();
                }
            }
            m_isMouseCursorInWindow = true;
            break;
        case QEvent::Leave:
            m_isMouseCursorInWindow = false;
            break;
        default:
            break;
    }
    return OpenGLWindow::event(event);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::wheelEvent(QWheelEvent *_event)
{
    if(m_isInitialized && m_MouseObserver.size())
    {
        // notifyObserver
        for(std::vector<MouseObserver*>::iterator it = m_MouseObserver.begin(); it != m_MouseObserver.end(); ++it)
        {
            MouseObserver* o = *it;
            o->onMouseWheel(_event->delta());
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::initialize()
{
    // we need to initialize the NGL lib which will load all of the OpenGL functions, this must
    // be done once we have a valid GL context but before we call any GL commands. If we don't do
    // this everything will crash
    ngl::NGLInit::instance();
    // init openGL
    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glDisable(GL_DEPTH_TEST);
    for(std::vector<SceneObserver*>::iterator it = m_SceneObserver.begin(); it != m_SceneObserver.end(); ++it)
    {
        SceneObserver* o = *it;
        if(!o->onInitialize())
        {
            QString message(o->getName().c_str());
            message.append(" observer:\nThe initialization failed :(");
            QMessageBox::critical(qobject_cast<QWidget *> (parent()), ApplicationTitle, message);
            QCoreApplication::postEvent(this, new QEvent(QEvent::Close));
        }
    }
    m_isInitialized = true;
    // start timer
    m_time.start();
    resizeGL();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!m_isInitialized)
    {
        return;
    }
    m_frame++;
    m_fps.Update(m_time.elapsed());

    if (!isExposed())
    {
        return;
    }
    for(std::vector<SceneObserver*>::iterator it = m_SceneObserver.begin(); it != m_SceneObserver.end(); ++it)
    {
        SceneObserver* o = *it;
        o->onRender();
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::mouseMoveEvent (QMouseEvent * _event )
{
    Q_UNUSED(_event);
    wrapMousePos();
    if(m_isInitialized && m_isMouseCursorInWindow && m_MouseObserver.size())
    {
        for(std::vector<MouseObserver*>::iterator it = m_MouseObserver.begin(); it != m_MouseObserver.end(); ++it)
        {
            MouseObserver* o = *it;
            o->onMove();
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::mousePressEvent ( QMouseEvent *_event)
{
    if (_event->button() == Qt::LeftButton)
    {
        m_leftMouseButtonPressed = true;
    }
    if(_event->button() == Qt::RightButton)
    {
        m_rightMouseButtonPressed = true;
    }
    if(m_MouseObserver.size())
    {
        for(std::vector<MouseObserver*>::iterator it = m_MouseObserver.begin(); it != m_MouseObserver.end(); ++it)
        {
            MouseObserver* o = *it;
            o->onPress(_event->button());
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::mouseReleaseEvent ( QMouseEvent *_event )
{
    if (_event->button() == Qt::LeftButton)
    {
        m_leftMouseButtonPressed = false;
    }
    if(_event->button() == Qt::RightButton)
    {
        m_rightMouseButtonPressed = false;
    }
    if(m_MouseObserver.size())
    {
        for(std::vector<MouseObserver*>::iterator it = m_MouseObserver.begin(); it != m_MouseObserver.end(); ++it)
        {
            MouseObserver* o = *it;
            o->onRelease(_event->button());
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::keyReleaseEvent(QKeyEvent *_event)
{
    if(m_isInitialized && m_KeyboardObserver.size())
    {
        // detect kbd release
        Qt::Key key = (Qt::Key)_event->key();
        for(std::vector<KeyboardObserver*>::iterator it = m_KeyboardObserver.begin(); it != m_KeyboardObserver.end(); ++it)
        {
            KeyboardObserver* o = *it;
            if(o->m_cnt)
            {
                for(std::vector<Scene::Key>::iterator it2 = o->m_keyMap.begin(); it2 != o->m_keyMap.end(); ++it2)
                {
                    if(it2->m_key == key)
                    {
                        // notifyObservers
                        it2->m_pressed = false;
                        o->m_cnt--;
                        o->onRelease(it2->m_key);
                        break;
                    }
                }
            }
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::keyPressEvent(QKeyEvent *_event)
{
    // this method is called every time the main window receives a key event.
    // we then switch on the key value and set the camera in the GLWindow
    Qt::Key key = (Qt::Key)_event->key();
    if (!m_isInitialized)
    {
        return;
    }
    if(m_KeyboardObserver.size())
    {
        for(std::vector<KeyboardObserver*>::iterator it = m_KeyboardObserver.begin(); it != m_KeyboardObserver.end(); ++it)
        {
            KeyboardObserver* o = *it;
            if(o->m_keyMap.size())
            {
                for(std::vector<Scene::Key>::iterator it2 = o->m_keyMap.begin(); it2 != o->m_keyMap.end(); ++it2)
                {
                    if(it2->m_key == key)
                    {
                        // notifyObservers
                        it2->m_pressed = true;
                        o->m_cnt++;
                        o->onPress(it2->m_key);
                        break;
                    }
                }
            }
        }
    }

    switch (key)
    {
        case Qt::Key_Tab:
            m_isFullScreen ^= 1;
            if (m_isFullScreen)
            {
                showFullScreen();
            }
            else
            {
                showNormal();
            }
            break;
        default :
            break;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::Fps::Update(int _elapsed)
{
    m_t[m_cnt] = _elapsed;
    m_cnt = (m_cnt + 1) & (_countof(m_t) - 1);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
float Scene::Fps::Get()
{
    return (m_t[m_cnt]) ? 1000.0f*(_countof(m_t) - 1) / ((float)(abs(m_t[(m_cnt - 1) & (_countof(m_t) - 1)] - m_t[m_cnt]))) : 0.0f;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::setGameState(Game::State _newState)
{
    switch(_newState)
    {
        case Game::State::InGame:
            if(m_stat == 0)
            {
                m_stat = new Game::Statistic();
            }
            break;
        case Game::State::LevelUp:
            if(m_stat != 0)
            {
                m_stat->levelUp();
            }
            else
            {
                m_stat = new Game::Statistic();
            }
            break;
        case Game::State::GameOver:
            if(m_stat != 0)
            {
                if(m_stat->m_scores > m_config->m_points)
                {
                    m_config->m_points = m_stat->m_scores;
                    m_config->m_eggs = m_stat->m_eggs;
                    m_config->m_level = m_stat->m_gameLevel;
                    m_config->m_distance = m_stat->m_tot_distance;
                    m_config->m_isChanges = true;
                }
            }
            break;
        default:
            break;
    }
    m_gameState = _newState;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
int Scene::getTopScores()
{
    return m_config->m_points;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
int Scene::getTopEggs()
{
    return m_config->m_eggs;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void Scene::clearStatistic()
{
    if (m_stat) { delete m_stat; m_stat = 0; }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void KeyboardObserver::attachKey(Qt::Key _key)
{
    Scene::Key key(_key);
    m_keyMap.push_back(key);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void KeyboardObserver::detachKey(Qt::Key _key)
{
    if(m_keyMap.size())
    {
        std::vector<Scene::Key>::iterator it = std::find(m_keyMap.begin(), m_keyMap.end(), _key);
        if(it != m_keyMap.end())
        {
            if(it->is_pressed())
            {
                m_cnt--;
            }
            m_keyMap.erase(it);
        }
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
bool KeyboardObserver::isPressed(Qt::Key _key)
{
    bool result = false;
    if(m_keyMap.size() && m_cnt != 0)
    {
        std::vector<Scene::Key>::iterator it = std::find(m_keyMap.begin(), m_keyMap.end(), _key);
        result = it->is_pressed();
    }
    return result;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void KeyboardObserver::mapPressedKeys(std::vector<Qt::Key> &o_keys)
{
    o_keys.empty();
    if(m_keyMap.size() && m_cnt != 0)
    {
        for(std::vector<Scene::Key>::iterator it = m_keyMap.begin(); it != m_keyMap.end(); ++it)
        {
            if(it->is_pressed())
            {
                o_keys.push_back(it->getKey());
            }
        }
    }
}
