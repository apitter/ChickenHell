#include <cfgfile.h>
#include <QScreen>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
CfgFile::CfgFile(Scene *scene)
    : m_scene(scene)
    , m_isChanges(false)
    , m_isLocked(true)
{
    // open file
    QSettings settings(ConfigurationFile, QSettings::IniFormat);
    if (GetQSettings(settings) == false)
    {
        SetQSettings(settings);
        settings.sync();
    }
    m_oldSize = m_windowSize;
    m_oldPos = m_windowPosition;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
CfgFile::~CfgFile()
{
    // is there any changes - save to file
    if (m_isChanges)
    {
        if (m_visibility == QWindow::Maximized || m_visibility == QWindow::FullScreen)
        {
            m_windowSize = m_oldSize;
            m_windowPosition = m_oldPos;
        }
        QSettings settings(ConfigurationFile, QSettings::IniFormat);
        SetQSettings(settings);
        settings.sync();
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
bool CfgFile::GetQSettings(QSettings &settings)
{
    bool ok,result=true;
    settings.beginGroup("Window");
    m_windowSize = settings.value("Size").toSize();
    if (m_windowSize.width() == -1 || m_windowSize.height() == -1)
    {
        m_windowSize = QSize(DefaultWindowWidth,DefaultWindowHeigh);
        QScreen *screen = m_scene->screen();
        QSize size = screen->size();
        m_windowPosition.setX((size.width()-m_windowSize.width())/2);
        m_windowPosition.setY((size.height()-m_windowSize.height())/2);
        result=false;
    }
    else
    {
        m_windowPosition = settings.value("Position").toPoint();
    }
    m_visibility = (QWindow::Visibility)settings.value("Visibility").toInt(&ok);
    if (ok==false)
    {
        m_visibility = QWindow::Windowed;
        result=false;
    }
    else
    {
        // check again - AutomaticVisibility, Minimized and Hidden not accessible
        switch(m_visibility)
        {
            case QWindow::Windowed:
            case QWindow::Maximized:
            case QWindow::FullScreen:
                // all is ok
                break;
            default:
                m_visibility = QWindow::Windowed;
                result=false;
                break;
        }
    }
    settings.endGroup();

    settings.beginGroup("Game");
    m_eggs = settings.value("Eggs").toInt(&ok);
    if(!ok) { m_eggs = 0; result = false; }
    m_distance = settings.value("Distance").toFloat(&ok);
    if(!ok) { m_distance = 0; result = false; }
    m_points = settings.value("Points").toInt(&ok);
    if(!ok) { m_points = 0; result = false; }
    m_level = settings.value("Level").toInt(&ok);
    if(!ok) { m_level = 1; result = false; }
    settings.endGroup();
    return result;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void CfgFile::SetQSettings(QSettings &settings)
{
    settings.beginGroup("Window");
    settings.setValue("Size", m_windowSize);
    settings.setValue("Position", m_windowPosition);
    settings.setValue("Visibility", m_visibility);
    settings.endGroup();
    settings.beginGroup("Game");
    settings.setValue("Eggs", m_eggs);
    settings.setValue("Points", m_points);
    settings.setValue("Level", m_level);
    settings.setValue("Distance", m_distance);
    settings.endGroup();
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void CfgFile::OnResize()
{
    if (!m_isLocked)
    {
        m_oldSize = m_windowSize;
        m_windowSize = m_scene->size();
        m_isChanges = true;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void CfgFile::OnClose()
{
    m_isLocked = true;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void CfgFile::OnMove()
{
    if (!m_isLocked)
    {
        m_oldPos = m_windowPosition;
        m_windowPosition = QPoint(m_scene->x(),m_scene->y());
        m_isChanges = true;
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
void CfgFile::OnWindowStateChange(Qt::WindowStates state)
{
    if (!m_isLocked)
    {
        if (state&Qt::WindowMaximized)
        {
            m_visibility = QWindow::Maximized;
        }
        else
        {
            if (state&Qt::WindowFullScreen)
                m_visibility = QWindow::FullScreen;
            else
                m_visibility = QWindow::Windowed;
        }
        m_isChanges = true;
    }
}
