#include <gameui.h>
#include "VAOPrimitivesFix.h"
#include <ngl/Shader.h>
#include <ngl/ShaderLib.h>
#include <QGLWidget>
#include <QFontDatabase>
#include <QTextCharFormat>
#include <sstream>
#include <iostream>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define FADE_TEXT_SPEED (0.005f)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static const char *textPage1[] = {
      "New Game"
    , "High Score"
    , "Quit"

};
static const char *textPage2[] = {
      "Home"
    , "Results"
    , "Again?"

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static UIScene *s_uiSceneObserver;
static UIMouse *s_uiMouseObserver;
static UIKeyboard *s_uiKeyboardObserver;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// InitUI
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void InitUI()
{
    s_uiSceneObserver = new UIScene();
    s_uiMouseObserver = new UIMouse();
    s_uiKeyboardObserver = new UIKeyboard();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIText
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UIText::UIText(QFont &_font)
    : m_text(0)
    , m_bexist(false)
{
    m_tm = new QFontMetrics(_font);
    if(m_tm)
    {
        m_text = new ngl::Text(_font);
        if(m_text)
        {
            m_bexist = true;
        }
        else
        {
            delete m_tm;
            m_tm = 0;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIText::renderTextMultiline(float _x, float _y, const QString &_text)
{
    std::istringstream f(_text.toStdString());
    std::string line;
    float y = _y;
    while(std::getline(f, line))
    {
        m_text->renderText(_x, y, line.c_str());
        y += m_tm->height();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UILayer
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UILayer::UILayer(const char *_name, QSize* _size, QImage *_image)
    : m_textureId(0)
    , m_bexist(false)
{
    strcpy(m_name, _name);
    m_color = ngl::Vec4(1, 1, 1, 1);

    if(_image)
    {
        m_textureId = UIScene::LoadTexture2D(*_image, &m_size);
        if(_size)
        {
            m_size = *_size;
        }
    }
    else
    {
        m_size = *_size;
    }
    if(m_size.width() && m_size.height())
    {
        VAOPrimitivesPlane *prim = VAOPrimitivesPlane::instance();
        prim->createTrianglePlane(m_name, m_size.width(), m_size.height(), 2, 2, ngl::Vec3(0, 1, 0));
        m_bexist = true;
        // default layer position
        m_base.m_m[3][0] = m_size.width() / 2.0f;
        m_base.m_m[3][1] = m_size.height() / 2.0f;
        m_base.rotateX(-90);
        m_model = m_base;
        m_currentSize = m_size;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UILayer::~UILayer()
{
    if(m_textureId)
    {
        glDeleteTextures(1, &m_textureId);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::setPos(GLuint _x, GLuint _y, ngl::Real _z)
{
    m_base.m_m[3][0] = _x;
    m_base.m_m[3][1] = _y;
    m_model.m_m[3][2] = m_base.m_m[3][2] = _z;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool UILayer::isCursorOverLayer(QPoint _glPos)
{
    int left = m_model.m_m[3][0] - m_currentSize.width() / 2;
    int right = m_model.m_m[3][0] + m_currentSize.width() / 2;
    int top = m_model.m_m[3][1] + m_currentSize.height() / 2;
    int bottom = m_model.m_m[3][1] - m_currentSize.height() / 2;
    return (
        _glPos.x() && _glPos.y() &&
        left < _glPos.x() && right > _glPos.x() &&
        top > _glPos.y() && bottom < _glPos.y()
     );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::onResize(ngl::Real _x, ngl::Real _y, ngl::Real _aspectXY, ngl::Real _scale, bool _isInner)
{
    // apply the _scale
    ngl::Mat4 scale;
    scale.scale(_scale, 1, _scale);
    m_model = scale*m_base;
    // compute the current size
    m_currentSize.setWidth(_scale*(ngl::Real)m_size.width());
    m_currentSize.setHeight(_scale*(ngl::Real)m_size.height());
    // compute translation
    // correction
    ngl::Real sX = 0;
    ngl::Real sY = 0;
    if(!FCompare2(_scale, 1.0f))
    {
        if(_isInner)
        {
            if(_aspectXY < AspectRatio)
            {
                sY = ((_x / AspectRatio) - _y);
            }
            else
            {
                sX = ((_y*AspectRatio) - _x);
            }
        }
        else
        {
            if(_aspectXY >= AspectRatio)
            {
                sY = ((_x / AspectRatio) - _y);
            }
            else
            {
                sX = ((_y*AspectRatio) - _x);
            }
        }
    }
    m_model.m_m[3][0] = m_base.m_m[3][0] * _scale - sX / 2;
    m_model.m_m[3][1] = m_base.m_m[3][1] * _scale - sY / 2;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::setColor(GLint _r, GLint _g, GLint _b, GLint _a)
{
    setColor(ngl::Real(_r) / 255.0f, ngl::Real(_g) / 255.0f, ngl::Real(_b) / 255.0f, ngl::Real(_a) / 255.0f);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::setColor(ngl::Real _r, ngl::Real _g, ngl::Real _b, ngl::Real _a)
{
    m_color = ngl::Vec4(_r, _g, _b, _a);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::setAlpha(GLint _a)
{
    setAlpha(ngl::Real(_a) / 255.0f);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::setAlpha(ngl::Real _a)
{
    m_color.m_w = _a;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UILayer::draw(ngl::Mat4 &_p)
{
    // update shader
    if(m_bexist)
    {
        ngl::ShaderLib *shader = ngl::ShaderLib::instance();
        shader->use("UiShader");
        shader->setShaderParam1i("colorSet", (m_textureId) ? 1:0);
        shader->setShaderParamFromVec4("color", m_color);
        ngl::Mat4 MVP = m_model*_p;
        shader->setShaderParamFromMat4("MVP", MVP);
        VAOPrimitivesPlane *prim = VAOPrimitivesPlane::instance();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if(m_textureId)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureId);
        }
        prim->draw(m_name);
        if(m_textureId)
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glDisable(GL_BLEND);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIScene
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UIScene::UIScene()
    : SceneObserver()
    , m_title(0)
    , m_friedTitle(0)
    , m_firee_logo(0)
    , m_friedChicken(0)
    , m_papirus(0)
    , m_cickenAvatar(0)
    , m_text(0)
    , m_levelFontId(0)
{
    setName("UIScene");
    for(int i = 0; i < 3; i++)
    {
        m_blueButton[i][0] = 0;
        m_blueButton[i][1] = 0;
        m_redButton[i][0] = 0;
        m_redButton[i][1] = 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UIScene::~UIScene()
{
    // release resources
    if(m_title) { delete m_title; }
    if(m_friedTitle) { delete m_friedTitle; }
    if(m_firee_logo) { delete m_firee_logo; }
    if(m_friedChicken) { delete m_friedChicken; }
    if(m_whiteRect) { delete m_whiteRect; }
    if(m_cickenAvatar) { delete m_cickenAvatar; }
    if(m_papirus) { delete m_papirus; }
    for(int i = 0; i < 3; i++)
    {
        if(m_blueButton[i][0]) delete m_blueButton[i][0];
        if(m_blueButton[i][1]) delete m_blueButton[i][1];
        if(m_redButton[i][0]) delete m_redButton[i][0];
        if(m_redButton[i][1]) delete m_redButton[i][1];
    }
    if(m_text) { delete m_text; }
    if(m_levelFontId != 0)
    {
        if(m_levelFont) delete m_levelFont;
        QFontDatabase::removeApplicationFont(m_levelFontId);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool UIScene::onInitialize()
{
    // UI shader
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();
    // load a frag and vert shaders
    shader->createShaderProgram("UiShader");

    shader->attachShader("UiShaderVertex", ngl::ShaderType::VERTEX);
    shader->attachShader("UiShaderFragment", ngl::ShaderType::FRAGMENT);
    shader->loadShaderSource("UiShaderVertex", "shaders/ui_vert.glsl");
    shader->loadShaderSource("UiShaderFragment", "shaders/ui_frag.glsl");

    shader->compileShader("UiShaderVertex");
    shader->compileShader("UiShaderFragment");
    shader->attachShaderToProgram("UiShader", "UiShaderVertex");
    shader->attachShaderToProgram("UiShader", "UiShaderFragment");

    shader->linkProgramObject("UiShader");

    // load resources
    // textures
    QImage img;
    m_firee_logo = new UILayer("firee-logo", 0, &(img = QImage("textures/firee-logo.jpg")));
    if(!m_firee_logo->isExist()) { return false; }
    m_firee_logo->setPos(960, 540, -0.01f);

    m_title = new UILayer("hell-title", 0, &(img = QImage("textures/hell-title.png")));
    if(!m_title->isExist()) { return false; }
    m_title->setPos(960, 900, -0.008f);

    m_friedTitle = new UILayer("Fried-title", 0, &(img = QImage("textures/Fried-title.png")));
    if(!m_friedTitle->isExist()) { return false; }
    m_friedTitle->setColor(1.0f, 1.0f, 1.0f, 0.55f);
    m_friedTitle->setPos(960, 900, -0.008f);

    m_friedChicken = new UILayer("friedChicken", 0, &(img = QImage("textures/friedChicken.png")));
    if(!m_friedChicken->isExist()) { return false; }
    m_friedChicken->setPos(820, 635, -0.005f);

    m_papirus = new UILayer("papirus-title", 0, &(img = QImage("textures/diznain-papirus-title.png")));
    if(!m_friedChicken->isExist()) { return false; }
    m_papirus->setPos(960, 540, -0.005f);

    m_cickenAvatar = new UILayer("cicken-avatar", 0, &(img = QImage("textures/cicken-avatar.png")));
    if(!m_cickenAvatar->isExist()) { return false; }
    m_cickenAvatar->setPos(1290, 630, -0.006f);
    // rectangles
    QSize s(755, 710);
    m_whiteRect = new UILayer("whiteRect", &s, 0);
    if(!m_whiteRect->isExist()) { return false; }
    m_whiteRect->setColor(1.0f, 1.0f, 1.0f, 0.5f);
    m_whiteRect->setPos(960, 380, -0.008f);

    // font He пип чe пляc
    QString fontFileName = QString("data/OCRAStd.otf");
    if(!QFile::exists(fontFileName)) { return false; }
    int id = QFontDatabase::addApplicationFont(fontFileName);
    if(id == -1) { return false; }
    QStringList sList = QFontDatabase::applicationFontFamilies(id);
    QFont font(sList[0], 52);
    QFontMetrics tm(font);
    // space for button's name
    char name[64];
    // blue rectangles - Page 1
    // we have a 3 buttons with 2 stages at page 1
    GLuint y = 620;
    for(int i = 0; i < 3; y-=240, i++)
    {
        // cteate texture from image
        QImage image(608, 150, QImage::Format_ARGB32);
        //                R  G   B    A
        image.fill(QColor(1, 31, 101, 255));
        // paint the text
        QPainter painter(&image);
        painter.setPen(QPen(Qt::white));
        painter.setFont(font);
        painter.setBackgroundMode(Qt::TransparentMode);
        QSize s = tm.size(Qt::TextSingleLine, textPage1[i]);
        painter.drawText((608 - s.width()) / 2, (150 + s.height() / 2) / 2, QString(textPage1[i]));
        painter.end();
        // create layer
        sprintf(name, "blueButton%i0", i);
        m_blueButton[i][0] = new UILayer(name, 0, &image);
        if(!m_blueButton[i][0]->isExist()) { return false; }
        m_blueButton[i][0]->setPos(960, y, -0.005f);

        // refill the image
        image.fill(QColor(1, 31, 101, 255));
        painter.begin(&image);
        QPen pen(QColor(247, 181, 7, 255), 3);
        painter.setPen(pen);
        painter.setBrush(Qt::white);
        painter.setFont(font);
        painter.setBackgroundMode(Qt::TransparentMode);
        QPainterPath path; // 247,181,7
        path.addText((608 - s.width()) / 2, (150 + s.height() / 2) / 2, font, QString(textPage1[i]));
        painter.drawPath(path);
        painter.end();
        // create layer
        sprintf(name, "blueButton%i1", i);
        m_blueButton[i][1] = new UILayer(name, 0, &image);
        if(!m_blueButton[i][1]->isExist()) { return false; }
        m_blueButton[i][1]->setPos(960, y, -0.005f);
    }
    // red rectangles - Page 2
    // we have a 3 buttons with 2 stages at page 2
    QFont font2(sList[0], 42);
    QFontMetrics tm2(font);
    GLuint x = 490;
    for(int i = 0; i < 3; x+=470, i++)
    {
        // cteate texture from image
        QImage image(360, 122, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        // paint the text
        QPainter painter(&image);
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::red));
        painter.drawRoundRect(0, 0, 358, 120, 10, 25);
        painter.setPen(QPen(Qt::white));
        painter.setFont(font2);
        painter.setBackgroundMode(Qt::TransparentMode);
        QSize s = tm2.size(Qt::TextSingleLine, textPage2[i]);
        painter.drawText((400 - s.width()) / 2, (124 + s.height() / 2) / 2, QString(textPage2[i]));
        painter.end();
        // create layer
        sprintf(name, "redButton%i0", i);
        m_redButton[i][0] = new UILayer(name, 0, &image);
        if(!m_redButton[i][0]->isExist()) { return false; }
        m_redButton[i][0]->setPos(x, 85, -0.005f);

        // refill the image
        image.fill(Qt::transparent);
        painter.begin(&image);
        painter.setBrush(Qt::red);
        painter.setPen(QPen(Qt::red));
        painter.drawRoundRect(0, 0, 358, 120, 10, 25);
        painter.setPen(QPen(QColor(247, 181, 7, 255), 3));
        painter.setBrush(Qt::white);
        painter.setFont(font2);
        painter.setBackgroundMode(Qt::TransparentMode);
        QPainterPath path; // 247,181,7
        path.addText((370 - s.width()) / 2, (124 + s.height() / 2) / 2, font, QString(textPage2[i]));
        painter.drawPath(path);
        painter.end();
        // create layer
        sprintf(name, "redButton%i1", i);
        m_redButton[i][1] = new UILayer(name, 0, &image);
        if(!m_redButton[i][1]->isExist()) { return false; }
        m_redButton[i][1]->setPos(x, 85, -0.005f);
    }

    // red rectangle - Page 3(button "Proceed")
    {
        // cteate texture from image
        QImage image(360, 122, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        // paint the text
        QPainter painter(&image);
        painter.setBrush(Qt::red);
        painter.setPen(QPen(QColor(247, 181, 7, 255), 5));
        painter.drawRoundRect(0, 0, 358, 120, 5, 5);
        painter.setPen(QPen(Qt::white));
        painter.setFont(font2);
        painter.setBackgroundMode(Qt::TransparentMode);
        QSize s = tm2.size(Qt::TextSingleLine, "Proceed");
        painter.drawText((400 - s.width()) / 2, (124 + s.height() / 2) / 2, "Proceed");
        painter.end();
        // create layer
        m_backButton[0][0] = new UILayer("backButton00", 0, &image);
        if(!m_backButton[0][0]->isExist()) { return false; }
        m_backButton[0][0]->setPos(960, 85, -0.003f);

        // refill the image
        image.fill(Qt::transparent);
        painter.begin(&image);
        painter.setBrush(Qt::red);
        painter.setPen(QPen(QColor(247, 181, 7, 255), 5));
        painter.drawRoundRect(0, 0, 358, 120, 5, 5);
        painter.setPen(QPen(QColor(247, 181, 7, 255), 3));
        painter.setBrush(Qt::white);
        painter.setFont(font2);
        painter.setBackgroundMode(Qt::TransparentMode);
        QPainterPath path; // 247,181,7
        path.addText((360 - s.width()) / 2, (130 + s.height() / 2) / 2, font, QString("Proceed"));
        painter.drawPath(path);
        painter.end();
        // create layer
        m_backButton[0][1] = new UILayer("backButton01", 0, &image);
        if(!m_backButton[0][1]->isExist()) { return false; }
        m_backButton[0][1]->setPos(960, 85, -0.003f);
    }

    // rectangle for in-game
    QSize s1(4000, 80);
    m_inGameWhiteRect = new UILayer("inGameWhiteRect", &s1, 0);
    if(!m_inGameWhiteRect->isExist()) { return false; }
    m_inGameWhiteRect->setColor(1.0f, 1.0f, 1.0f, 0.5f);
    m_inGameWhiteRect->setPos(960, 1030, -0.008f);
    // icons
    s1 = QSize(23,30);
    m_eggs_icon = new UILayer("egg-icon", &s1, &(img = QImage("textures/egg_icon.png")));
    if(!m_eggs_icon->isExist()) { return false; }
    m_eggs_icon->setPos(960, 0, -0.005f);
    // fonts
    m_uitext = new UIText(font2);
    if(!m_uitext->isExist()) { return false; }
    m_uitext->setColour(1.0f, 1.0f, 1.0f);
    m_uitext->setScreenSize(MaxWindowWidth, MaxWindowHeigh);

    font = QFont(sList[0],16);
    m_text = new UIText(font);
    if(!m_text->isExist()) { return false; }
    m_text->setColour(1.0f, 1.0f, 1.0f);
    m_text->setScreenSize(MaxWindowWidth, MaxWindowHeigh);
    QFontDatabase::removeApplicationFont(id);
    // level up font
    fontFileName = QString("data/CALLI___.TTF");
    if(!QFile::exists(fontFileName)) { return false; }
    if(m_levelFontId == 0)
    {
        id = QFontDatabase::addApplicationFont(fontFileName);
    }
    if(id == -1) { return false; }
    if(m_levelFontId == 0)
    {
        m_levelFontId = id;
    }
    sList = QFontDatabase::applicationFontFamilies(m_levelFontId);
    m_levelFont = new QFont(sList[0], 40);
    if(!m_levelFont) { return false; }
    // create an empty level texture
    QSize s2(600, 300);
    m_levelLogo = new UILayer("levelLogo", &s2, 0);
    if(!m_levelLogo->isExist()) { return false; }
    glGenTextures(1, &m_levelLogo->m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_levelLogo->m_textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_levelLogo->m_textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_levelLogo->m_textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s2.width(), s2.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_levelLogo->setPos(960, 540, -0.01f);
    return true;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIScene::onRender()
{
    ngl::Mat4 orto2D;
    Scene *scene = Scene::getInstance();
    Game::State state = scene->getGameState();
    scene->getProjection2DMatrix(orto2D);
    if(state == Game::State::Intro)
    {
        QPoint mousepos = scene->getCursorPosGL();
        m_firee_logo->draw(orto2D);
        m_activeButton = 0;
        m_title->draw(orto2D);
        m_whiteRect->draw(orto2D);
        for(int i = 0; i < 3; i++)
        {
            int istage = 0;
            if(m_blueButton[i][0]->isCursorOverLayer(mousepos))
            {
                istage = 1;
                m_activeButton = i + 1;
            }
            m_blueButton[i][istage]->draw(orto2D);
        }
        m_levelLogo->m_color.m_w = 1;
    }
    else if(state == Game::State::HighestScore)
    {
        QPoint mousepos = scene->getCursorPosGL();
        m_firee_logo->draw(orto2D);
        m_cickenAvatar->draw(orto2D);
        m_papirus->draw(orto2D);
        int istage = (m_backButton[0][0]->isCursorOverLayer(mousepos))?1:0;
        m_backButton[0][istage]->draw(orto2D);
        m_activeButton = istage;

        char text[200];
        sprintf(text, "Highest Score: %i\nSaved Eggs: %i", scene->getTopScores(), scene->getTopEggs());
        ngl::Real aspect;
        ngl::Real scale = 1.0f / getScaleAndAspectInner(scene->width(), scene->height(), aspect);
        m_uitext->setScreenSize(scene->width() * scale, scene->height() * scale);
        m_uitext->renderTextMultiline(scale*(scene->width() / 3.2f), scale*(scene->height() / 1.8f), text);
    }
    else if(state == Game::State::GameOver)
    {
        QPoint mousepos = scene->getCursorPosGL();
        m_firee_logo->draw(orto2D);
        m_activeButton = 0;
        m_friedTitle->draw(orto2D);
        m_friedChicken->draw(orto2D);
        for(int i = 0; i < 3; i++)
        {
            int istage = 0;
            if(m_redButton[i][0]->isCursorOverLayer(mousepos))
            {
                istage = 1;
                m_activeButton = i + 1;
            }
            m_redButton[i][istage]->draw(orto2D);
        }
        char text[200];
        Game::Statistic *stat = scene->getGameStatistic();
        sprintf(text, "Saved Eggs: %i\nTotal Score: %i", stat->getEggs(), stat->getPoints());
        ngl::Real aspect;
        ngl::Real scale = 1.0f/getScaleAndAspectInner(scene->width(), scene->height(),aspect);
        m_uitext->setScreenSize(scene->width() * scale, scene->height() * scale);
        m_uitext->renderTextMultiline(scale*(scene->width() / 2.8f), scale*(scene->height() / 1.75f), text);

    }
    else if(state == Game::State::InGame)
    {
        Game::Statistic *stat = scene->getGameStatistic();
        m_inGameWhiteRect->m_model.m_m[3][1] = scene->height();
        m_inGameWhiteRect->draw(orto2D);

        m_eggs_icon->m_model.m_m[3][0] = 30; // x
        m_eggs_icon->m_model.m_m[3][1] = scene->height() - 20; // y
        m_eggs_icon->draw(orto2D);
        m_text->setScreenSize(scene->width(), scene->height());

        char text[200];
        if(stat)
        {
            sprintf(text, "Eggs: %i", stat->getEggs());
            m_text->renderText(50, 10, text);
            m_text->renderText(scene->width() / 2 - 50, 10, stat->getGameTime().c_str());
            sprintf(text, "Score: %i", stat->getPoints());
            m_text->renderText(scene->width() - 200, 10, text);
        }
        if(m_levelLogo->m_color.m_w > 0)
        {
            glEnable(GL_BLEND);
            m_levelLogo->draw(orto2D);
            m_levelLogo->m_color.m_w -= FADE_TEXT_SPEED; // fade effect
            if(m_levelLogo->m_color.m_w < 0)
            {
                m_levelLogo->m_color.m_w = 0;
            }
            glDisable(GL_BLEND);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ngl::Real UIScene::getScaleAndAspectInner(int _width, int _height, ngl::Real &o_aspect)
{
    ngl::Real scale = 1.0f;
    o_aspect = AspectRatio;
    if(_width != MaxWindowWidth && _height != MaxWindowHeigh)
    {
        o_aspect = (ngl::Real)_width / (ngl::Real)_height;
        if(o_aspect > AspectRatio)
        {
            scale = (ngl::Real)_height / (ngl::Real)MaxWindowHeigh;
        }
        else
        {
            scale = (ngl::Real)_width / (ngl::Real)MaxWindowWidth;
        }
    }
    return scale;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ngl::Real UIScene::getScaleAndAspectOuter(int _width, int _height, ngl::Real &o_aspect)
{
    ngl::Real scale = 1.0f;
    o_aspect = AspectRatio;
    if(_width != MaxWindowWidth && _height != MaxWindowHeigh)
    {
        o_aspect = (ngl::Real)_width / (ngl::Real)_height;
        if(o_aspect <= AspectRatio)
        {
            scale = (ngl::Real)_height / (ngl::Real)MaxWindowHeigh;
        }
        else
        {
            scale = (ngl::Real)_width / (ngl::Real)MaxWindowWidth;
        }
    }
    return scale;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIScene::onResize(int _x, int _y)
{
    // aspect ratio
    ngl::Real aspect;  
    ngl::Real scale = getScaleAndAspectOuter(_x, _y, aspect);
    // set a new scale and positions
    // backgound
    m_firee_logo->onResize(_x, _y, aspect, scale, false);

    scale = getScaleAndAspectInner(_x, _y, aspect);
    // Page 1
    // title
    m_title->onResize(_x, _y, aspect, scale);
    // white rectangle
    m_whiteRect->onResize(_x, _y, aspect, scale);
    // blue buttons
    for(int i = 0; i < 3; i++)
    {
        m_blueButton[i][0]->onResize(_x, _y, aspect, scale);
        m_blueButton[i][1]->onResize(_x, _y, aspect, scale);
    }
    // Page 2
    // title
    m_friedTitle->onResize(_x, _y, aspect, scale);
    // chicken
    m_friedChicken->onResize(_x, _y, aspect, scale);
    // Page 3
    m_papirus->onResize(_x, _y, aspect, scale);
    m_cickenAvatar->onResize(_x, _y, aspect, scale);
    m_backButton[0][0]->onResize(_x, _y, aspect, scale);
    m_backButton[0][1]->onResize(_x, _y, aspect, scale);
    // red buttons
    for(int i = 0; i < 3; i++)
    {
        m_redButton[i][0]->onResize(_x, _y, aspect, scale);
        m_redButton[i][1]->onResize(_x, _y, aspect, scale);
    }
    // level logo
    m_levelLogo->onResize(_x, _y, aspect, scale);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIScene::LoadTexture
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GLuint UIScene::LoadTexture2D(
    QImage &_image,
    QSize *o_imgsize,
    GLint _wrap_s,
    GLint _wrap_t
)
{
    GLuint textureId = 0;
    int width = 0;
    int height = 0;
    if(_image.isNull() == false)
    {
        width = _image.width();
        height = _image.height();
        // note this method is depracted as it uses the Older GLWidget but does work
        _image = QGLWidget::convertToGLFormat(_image);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(textureId, GL_TEXTURE_WRAP_S, _wrap_s);
        glTexParameteri(textureId, GL_TEXTURE_WRAP_T, _wrap_t);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, _image.bits());
        glGenerateMipmap(GL_TEXTURE_2D); //  Allocate the mipmaps
    }
    if(o_imgsize)
    {
        o_imgsize->setWidth(width);
        o_imgsize->setHeight(height);
    }
    return textureId;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIScene::UpdateLevelTexture
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIScene::UpdateLevelTexture()
{
    UILayer *levelLayer = s_uiSceneObserver->m_levelLogo;
    if(levelLayer)
    {
        levelLayer->m_color.m_w = 1;
        int gameLevel = (int)Game::Level::First;
        int timeBonus = 0;
        int distanceBonus = 0;
        Game::Statistic *stat = Scene::getInstance()->getGameStatistic();
        if(stat)
        {
            gameLevel = stat->getGameLevel();
            timeBonus = stat->getTimeBonus();
            distanceBonus = stat->getDistanceBonus();
        }
        int width = levelLayer->m_size.width();
        int height = levelLayer->m_size.height();
        QRect rect(0, 0, width, height);

        QImage image(width, height, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        // paint the text
        QPainter painter(&image);
        painter.setPen(QPen(Qt::white));
        painter.setFont(*s_uiSceneObserver->m_levelFont);
        painter.setBackgroundMode(Qt::TransparentMode);
        char text[200];
        sprintf(text, "Level %i\nTime Bonus %i\nDistance Bonus %i", gameLevel, timeBonus, distanceBonus);
        painter.drawText(rect, Qt::AlignHCenter, QString(text));
        painter.end();
        QImage imagem = image.mirrored();
        glBindTexture(GL_TEXTURE_2D, levelLayer->m_textureId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imagem.bits());
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIMouse
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIMouse::onPress(Qt::MouseButton _button)
{
    Scene *scene = Scene::getInstance();
    Game::State state = scene->getGameState();
    if(state == Game::State::Intro || state == Game::State::GameOver || state == Game::State::HighestScore)
    {
        if(_button == Qt::LeftButton)
        {
            Scene *scene = Scene::getInstance();
            if(state == Game::State::Intro) // in intro page
            {
                switch(s_uiSceneObserver->m_activeButton)
                {
                    case 1: // new game
                    {
                        scene->clearStatistic();
                        scene->setGameState(Game::State::InGame);
                        break;
                    }
                    case 2: // high score
                        scene->setGameState(Game::State::HighestScore);
                        m_prevGameState = Game::State::Intro;
                        break;
                    case 3: // quit
                        QCoreApplication::postEvent(scene, new QEvent(QEvent::Close));
                        break;
                }
            }
            else if(state == Game::State::HighestScore)
            {
                if(s_uiSceneObserver->m_activeButton == 1)
                {
                    scene->setGameState(m_prevGameState);
                }
            }
            else // game over page
            {
                switch(s_uiSceneObserver->m_activeButton)
                {
                    case 1: // home
                        scene->setGameState(Game::State::Intro);
                        break;
                    case 2: // results
                        scene->setGameState(Game::State::HighestScore);
                        m_prevGameState = Game::State::GameOver;
                        break;
                    case 3: // again
                        scene->clearStatistic();
                        scene->setGameState(Game::State::InGame);
                        m_prevGameState = Game::State::GameOver;
                        break;
                }
            }
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// UIKeyboard
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
UIKeyboard::UIKeyboard()
    : KeyboardObserver()
{
    // register the keys
    attachKey(Qt::Key_Escape);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UIKeyboard::onPress(Qt::Key _key)
{
    Q_UNUSED(_key);
    Scene *scene = Scene::getInstance();
    Game::State state = scene->getGameState();
    if(state == Game::State::Intro)
    {
        QCoreApplication::postEvent(scene, new QEvent(QEvent::Close));
    }
    else if(state == Game::State::InGame)
    {
        scene->setGameState(Game::State::GameOver);
    }
    else
    {
        scene->setGameState(Game::State::Intro);
    }
}
