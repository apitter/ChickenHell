#include "game.h"
#include <cmath>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// chicken speed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MAX_SPEED (0.12f)
#define STOPPING_SPEED (0.005f)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static variables
static GameScene *s_GameScene;
static GameKeyboard *s_GameKeyboard;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// startup function
void InitGame()
{
    s_GameScene = new GameScene();
    s_GameKeyboard = new GameKeyboard();
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 4. class implementation - GameScene
GameScene::GameScene()
    : SceneObserver()
{
    setName("Quing Scene");
    m_prevState = Game::State::Intro;
    m_mazeP = 0;
    m_firstTime = 1;
}

//________________________________________________________________________________
void GameScene::loadMatricesToShader()
{
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

    ngl::Mat4 MV;
    ngl::Mat4 MVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 M;
    M = m_transform.getMatrix();
    MV = M*m_cam.getViewMatrix();
    MVP = MV*m_cam.getProjectionMatrix();
    normalMatrix = MV;
    normalMatrix.inverse();
    shader->setShaderParamFromMat4("MV", MV);
    shader->setShaderParamFromMat4("MVP", MVP);
    shader->setShaderParamFromMat3("normalMatrix", normalMatrix);
}


//________________________________________________________________________________
void GameScene::loadMatricesToShaderForCube()
{
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

    ngl::Mat4 mV;
    ngl::Mat4 mVP;
    ngl::Mat3 normalMatrix;
    ngl::Mat4 m;

    m = m_transform.getMatrix();
    mV = m*m_cam.getViewMatrix();
    normalMatrix = mV;
    normalMatrix.inverse();
    mVP = m_transform.getMatrix()*m_cam.getVPMatrix();

    shader->setShaderParamFromMat4("mV", mV);
    shader->setShaderParamFromMat4("mVP", mVP);
    shader->setShaderParamFromMat3("normalMatrix", normalMatrix);
    shader->setShaderParamFromMat4("m", m);
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void GameScene::initPlayerModel()
{
    // load a frag and vert shaders
    m_player.reset(new Player(ngl::Vec3(24, 1.5, 0), "models/smallChicken.obj", "textures/CHICKENTEXTURE02.jpg"));
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

    shader->createShaderProgram("ChickenShader");

    shader->attachShader("ChickenVertex", ngl::ShaderType::VERTEX);
    shader->attachShader("ChickenFragment", ngl::ShaderType::FRAGMENT);
    shader->loadShaderSource("ChickenVertex", "shaders/ChickenVertex.glsl");
    shader->loadShaderSource("ChickenFragment", "shaders/ChickenFragment.glsl");

    shader->compileShader("ChickenVertex");
    shader->compileShader("ChickenFragment");
    shader->attachShaderToProgram("ChickenShader", "ChickenVertex");
    shader->attachShaderToProgram("ChickenShader", "ChickenFragment");

    // link the shader no attributes are bound
    shader->linkProgramObject("ChickenShader");

    shader->use("ChickenShader");

    shader->setShaderParam4f("Colour", 1, 1, 1, 1);
    shader->setShaderParam3f("lightPos", 0, 100, 100);
    shader->setShaderParam4f("lightDiffuse", 1, 1, 1, 1);
}
//________________________________________________________________________________
void GameScene::newGame()
{
    ngl::Vec3 from(24, 2.5, -3);
    ngl::Vec3 to(-24, 2.5, 0);
    ngl::Vec3 up(0, 1, 0);
    m_cam.set(from, to, up);
    m_cam.yaw(180);
    m_cam.update();

    // create our maze
    if(m_mazeP)
    {
        delete m_mazeP;
    }
    Scene *scene = Scene::getInstance();
    int cur_level = (int)Game::Level::First;
    Game::Statistic *stat = scene->getGameStatistic();
    if(stat)
    {
        cur_level = stat->getMazeLevel();
    }
    UIScene::UpdateLevelTexture();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    m_mazeP = new Maze(cur_level);
    m_mazeP->createWorldMap();
    m_mazeP->createMaze();
    if(stat)
    {
        stat->setLevelDistance(m_mazeP->getTotalDistance());
    }

    // clear the arrays
    m_boxArray.clear();
    m_planeArray.clear();
    m_eggArray.clear();
    m_bulletArray.clear();
    m_moveObArrayH.clear();
    m_moveObArrayV.clear();
    // set the player initial position
    m_oldPos = ngl::Vec3(24, 1.5f, 0);
    m_player->setPos(m_oldPos);
    m_player->setRotate(-90.0f);
    m_speed = 0;
    m_jumpHt = 0.18f;
    m_grav = -0.1f;
    m_acceleration = 0.015f;
    m_firstTime = 0;
    m_playerWalk = false;
    m_playerJump = false;
    m_playerJumpAnim = false;
    m_playerDie = false;
    m_cloudCounter = 0;
    // this method take a great CPU usage. sometime block the game
    m_player->playerStill();

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool GameScene::onInitialize()
{
    // grab an instance of shader manager
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

    shader->createShaderProgram("Texture");
    // now we are going to create empty shaders for Frag and Vert
    shader->attachShader("TextureVertex",ngl::ShaderType::VERTEX);
    shader->attachShader("TextureFragment",ngl::ShaderType::FRAGMENT);
    // attach the source
    shader->loadShaderSource("TextureVertex","shaders/TextureVertex.glsl");
    shader->loadShaderSource("TextureFragment","shaders/TextureFragment.glsl");
    // compile the shaders
    shader->compileShader("TextureVertex");
    shader->compileShader("TextureFragment");
    // add them to the program
    shader->attachShaderToProgram("Texture","TextureVertex");
    shader->attachShaderToProgram("Texture","TextureFragment");

    // now we have associated this data we can link the shader
    shader->linkProgramObject("Texture");
    // and make it active ready to load values
    (*shader)["Texture"]->use();

    m_Environment = new Environment("textures/surface.png","textures/sun.jpg",&m_cam);

    // we are creating a shader called Phong
    shader->createShaderProgram("Toonshader");
    shader->createShaderProgram("Phong");
    // now we are going to create empty shaders for Frag and Vert
    shader->attachShader("ToonVertex", ngl::ShaderType::VERTEX);
    shader->attachShader("ToonFragment", ngl::ShaderType::FRAGMENT);

    shader->attachShader("PhongVertex", ngl::ShaderType::VERTEX);
    shader->attachShader("PhongFragment", ngl::ShaderType::FRAGMENT);
    // attach the source
    shader->loadShaderSource("ToonVertex", "shaders/toonVertex.glsl");
    shader->loadShaderSource("ToonFragment", "shaders/toonFragment.glsl");

    shader->loadShaderSource("PhongVertex", "shaders/PhongVertex.glsl");
    shader->loadShaderSource("PhongFragment", "shaders/PhongFragment.glsl");
    // compile the shaders
    shader->compileShader("ToonVertex");
    shader->compileShader("ToonFragment");

    shader->compileShader("PhongVertex");
    shader->compileShader("PhongFragment");
    // add them to the program
    shader->attachShaderToProgram("Toonshader", "ToonVertex");
    shader->attachShaderToProgram("Toonshader", "ToonFragment");

    shader->attachShaderToProgram("Phong", "PhongVertex");
    shader->attachShaderToProgram("Phong", "PhongFragment");
    // now bind the shader attributes for most NGL primitives we use the following
    // layout attribute 0 is the vertex data (x,y,z)
    shader->bindAttribute("Toonshader", 0, "inVert");
    // attribute 1 is the UV data u,v (if present)
    //  shader->bindAttribute("Toonshader",1,"inUV");
    // attribute 2 are the normals x,y,z
    shader->bindAttribute("Toonshader", 2, "inNormal");

    shader->bindAttribute("Phong", 0, "inVert");
    // attribute 1 is the UV data u,v (if present)
    shader->bindAttribute("Phong", 1, "inUV");
    // attribute 2 are the normals x,y,z
    shader->bindAttribute("Phong", 2, "inNormal");

    // now we have associated that data we can link the shader
    shader->linkProgramObject("Toonshader");
    // and make it active ready to load values
    (*shader)["Toonshader"]->use();

    shader->setShaderParam1f("scale", 0.8f);
    //shader ->setShaderParam4f("material.diffuse", 0.1, 0.1, 0.1, 0.1);
    //shader ->setShaderParam4f("material.diffuse", 0.8, 0.4, 0.5, 0.3);


    shader->linkProgramObject("Phong");
    // and make it active ready to load values
    (*shader)["Phong"]->use();


    QImage img("textures/lava_04.jpg");
    QSize imgSize;
    m_textureName = UIScene::LoadTexture2D(img, &imgSize, GL_REPEAT, GL_REPEAT);
    if(m_textureName == 0)
    {
        return false;
    }
    // the shader will use the currently active material and light0 so set them
    ngl::Material m(ngl::STDMAT::GOLD);
    // load our material values to the shader into the structure material (see Vertex shader)
    m.loadToShader("material");
    // Now we will create a basic Camera from the graphics library

    //m_cam.move(23,30,-32);
    // set the shape using FOV 45 Aspect Ratio based on Width and Height
    // The final two are near and far clipping pm_columns of 0.5 and 10

    //shader->setShaderParam4f("scale",2.0f,2.0f,0.5f,0.3f);
    shader->setUniform("viewerPos", m_cam.getEye().toVec3());
    // now create our light that is done after the camera so we can pass the
    // transpose of the projection matrix to the light to do correct eye space
    // transformations
    ngl::Mat4 iv = m_cam.getViewMatrix();
    iv.transpose();

    ngl::Light light(ngl::Vec3(-2, 5, 2), ngl::Colour(1, 1, 1, 1), ngl::Colour(1, 1, 1, 1), ngl::LightModes::POINTLIGHT);
    light.setTransform(iv);
    // load these values to the shader as well
    light.loadToShader("light");
    // as re-size is not explicitly called we need to do that.
    // set the viewport for openGL we need to take into account retina display

    //test for textures
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
    prim->createSphere("sphere", 0.5f, 50);

    //init shader
    (*shader)["nglDiffuseShader"]->use();
    shader->setShaderParam4f("Colour",0.2f, 0.2f,0.2f,1.0f);
    shader->setShaderParam3f("lightPos",1.0f,1.0f,1.0f);
    shader->setShaderParam4f("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

    // init player
    initPlayerModel();
    //random seed
    srand(time(NULL));
    //onscreen text - temp
    m_text.reset(new ngl::Text(QFont("Arial", 18)));

    //init meshes
    m_goalMesh.reset(new ngl::Obj("models/goal.obj"));
    m_goalMesh->createVAO();
    m_cannonMesh.reset(new ngl::Obj("models/cannon.obj"));
    m_cannonMesh->createVAO();
    m_cloudMesh.reset(new ngl::Obj("models/cloud.obj"));
    m_cloudMesh->createVAO();


    return true;

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GameScene::onResize(int _x, int _y)
{
    m_cam.setShape(45.0f, (float)_x / _y, 0.05f, 350.0f);
    m_text->setScreenSize(_x, _y);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void GameScene::onRender()
{
    // check game state
    Scene *scene = Scene::getInstance();
    Game::State state = scene->getGameState();
    if(state != Game::State::InGame)
    {
        if(m_prevState == Game::State::LevelUp)
        {
            scene->setGameState(Game::State::InGame);
            state = Game::State::InGame;
        }
        else
        {
            m_prevState = state;
            return;
        }
    }
    if(m_prevState == Game::State::Intro || m_prevState == Game::State::GameOver || m_prevState == Game::State::LevelUp)
    {
        newGame();
        m_prevState = state;
    }
    // ref: Bullet physics documentattion
    // By default, Bullet physics simulator runs at an internal fixed frame rate of 60 Hz(0.01666).
    // method GameScene::onRender() has the same fixed frame rate
    controlPlayer();

    //if player isn't jumping, apply gravity
    if(m_playerJump==false)
    {   gravity();}

    //check collisions
    checkCollisions();
    movePlayer();

    // grab an instance of the shader manager
    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

    // get the VBO instance and draw the built in teapot
    ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();


    //creates initial map
    int x = 0, y = 0;

    for(x = 0; x<50; x++)
    {
        for(y = 0; y<60; y++)
        {
            if(m_mazeP->getObstacleBlock(x, y) == 1)
            {
                m_transform.reset();
                m_transform.setPosition(x, 0, y);
                if (m_firstTime==0)
                {
                    //floor
                    m_planeArray.push_back(Plane(ngl::Vec3(x,1.02f,y),1.2f,1.2f));
                    m_boxArray.push_back(Box(ngl::Vec3(x,0,y),1, 1, 1));
                }
                (*shader)["Phong"]->use();
                loadMatricesToShaderForCube();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                prim->draw("cube");
            }
        }
    }

    //only want the goal zone to be drawn once
    int check=0;
    //draw goal zone
    for(x = 0; x<50; x++)
    {
        for(y = 0; y<60; y++)
        {
            if(m_mazeP->getObstacleBlock(x, y) == 3)
            {
                m_transform.reset();
                m_transform.setPosition(x, 0, y);
                (*shader)["Phong"]->use();
                loadMatricesToShaderForCube();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                prim->draw("cube");

                //position and orient goalzone
                if(check==0)
                {
                    m_transform.reset();
                    (*shader)["nglDiffuseShader"]->use();
                    m_transform.setRotation(0,180.0f,0);
                    m_transform.setScale(0.8f,0.5f,1);
                    m_transform.setPosition(x+4,3,y);
                    ngl::ShaderLib *shader = ngl::ShaderLib::instance();

                    ngl::Mat4 MV;
                    ngl::Mat4 MVP;
                    ngl::Mat3 normalMatrix;
                    ngl::Mat4 M;
                    M = m_transform.getMatrix();
                    MV = M*m_cam.getViewMatrix();
                    MVP = MV*m_cam.getProjectionMatrix();
                    normalMatrix = MV;
                    normalMatrix.inverse();

                    shader->setShaderParamFromMat4("MVP", MVP);
                    shader->setShaderParamFromMat3("normalMatrix", normalMatrix);
                    m_goalMesh->draw();
                    check=1;
                }


            }
        }
    }

    //sets up 3 lanes

    for(x = 0; x<50; x++)
    {
        for(y = 0; y<60; y++)
        {

            if((m_mazeP->getObstacleBlock(x, y) == 2))
            {
                m_transform.reset();
                m_transform.setPosition(x, 1, y);
                ngl::Vec3 pos;
                pos.set(x, 1, y);
                if (m_firstTime==0)
                {
                    //add box to end of list
                    m_boxArray.push_back(Box(pos,1,1,1));
                    m_planeArray.push_back(Plane(ngl::Vec3(x,2.0f,y),1.2f,1.2f));
                }
                (*shader)["Phong"]->use();
                loadMatricesToShaderForCube();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                prim->draw("cube");
            }

            //cloud blocks
            m_cloudCounter+=0.0001f;
            if (m_cloudCounter>=360)
            {
                m_cloudCounter=0;
            }
            if(m_mazeP->getCloudBlock(x, y) == 1)
            {
                m_transform.reset();
                m_transform.setRotation(0, m_cloudCounter, 0);
                m_transform.setPosition(x, 4, y);
                m_transform.addPosition(((rand()%1-1)+m_cloudCounter)*0.03,0,((rand()%1-1)+m_cloudCounter)*0.03);
                (*shader)["Phong"]->use();
                loadMatricesToShaderForCube();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                m_cloudMesh->draw();
            }

            //shooting blocks
            if (m_mazeP->getObstacleBlock(x,y) == 4)
            {
                //calculating cannon rotation
                float dY = m_player->getPos().m_z - y;
                float dX = m_player->getPos().m_x - x;
                float angle = (atan2(dY,dX) * 180 / M_PI)*-1 + 90;
                m_transform.reset();
                (*shader)["nglDiffuseShader"]->use();
                m_transform.setRotation(0,angle,0);
                m_transform.setScale(0.5f,0.6f,0.5f);
                m_transform.setPosition(x,0.6f,y);
                loadMatricesToShader();
                m_cannonMesh->draw();

                //draw ground cubes
                m_transform.reset();
                m_transform.setPosition(x, 0, y);
                (*shader)["Phong"]->use();
                loadMatricesToShaderForCube();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                prim->draw("cube");

                if (m_firstTime==0)
                {
                    m_planeArray.push_back(Plane(ngl::Vec3(x,2.0f,y),1.2f,1.2f));
                    m_cannonArray.push_back(Box(ngl::Vec3(x,1,y),1, 1 ,1));
                    m_bulletArray.push_back(Bullet(ngl::Vec3(x,1,y),0.15f, m_player->getPos()));
                }
            }
            if (m_mazeP->getObstacleBlock(x,y) == 5)
            {
                if(m_mazeP->getObstaclePos(x,y))
                {
                    m_transform.reset();
                    m_transform.setPosition(x,1,y);
                    if (m_firstTime==0)
                    {
                        m_moveObArrayH.push_back(MoveObstacle(ngl::Vec3(x,1.5,y),1,3,1));
                    }
                }
            }
            if (m_mazeP->getObstacleBlock(x,y) == 6)
            {
                if(m_mazeP->getObstaclePos(x,y))
                {
                    m_transform.reset();
                    m_transform.setPosition(x,1,y);
                    if (m_firstTime==0)
                    {
                        m_moveObArrayV.push_back(MoveObstacle(ngl::Vec3(x,1.5,y),1,3,1));

                    }
                }
            }

        }
    }

    //Sets egg map
    for(x = 0; x<50; x++)
    {
        for(y = 0; y<60; y++)
        {
            if(m_mazeP->getEggBlock(x, y))
            {
                m_transform.reset();
                //get position of egg
                ngl::Vec3 pos;
                pos.set(x, 1, y);
                if (m_firstTime==0)
                {
                    //add egg to end of list
                    m_eggArray.push_back(Egg(pos,0.45f));
                }
                m_transform.reset();
                m_transform.setPosition(x, 1, y);
                m_transform.setScale(0.7f,1,0.7f);
                (*shader)["Toonshader"]->use();
                glBindTexture(GL_TEXTURE_2D, m_textureName);
                loadMatricesToShader();
                prim->draw("sphere");
            }
        }
    }


   // draw the environment

    /// draw the environment
     m_Environment->draw();




    //render all bullets
    for (Bullet &s : m_bulletArray)
    {
        s.move(m_player->getPos());
        s.draw("nglDiffuseShader",&m_cam);
    }
    for (MoveObstacle &d : m_moveObArrayH)
    {
        d.moveH(0.02f);
        d.draw("Toonshader", &m_cam);
    }
    for (MoveObstacle &d : m_moveObArrayV)
    {
        d.moveV(0.02f);
        d.draw("Toonshader", &m_cam);
    }

    m_firstTime=1;


    //Player setup
    ngl::Vec3 newPos = m_player->getPos();

    //Figuring gamestate
    int block = m_mazeP->getObstacleBlock(round(newPos.m_x), round(newPos.m_z));
    //END OF GAME or CURRENT LEVEL
    if (block == 3)
    {
        scene->setGameState(Game::State::LevelUp);
        return;
    }
    //GAME OVER
    else if(m_player->getPos().m_y<-1)
    {
        m_playerDie=true;
    }

    if(scene->getGameState() == Game::State::InGame)
    {
        // compute the distance
        Game::Statistic *stat = scene->getGameStatistic();
        if(stat)
        {
            ngl::Real mx = m_oldPos.m_x - newPos.m_x;
            ngl::Real my = m_oldPos.m_z - newPos.m_z;
            if(mx < 0) mx = -mx;
            if(my < 0) my = -my;
            ngl::Real tot_move = mx + my;
            stat->updateDistance(tot_move);
            m_oldPos = newPos;
        }
    }

    //player shader setup
    shader->setRegisteredUniform("Colour", 0.0f, 1.0f, 0.0f, 1.0f);

    //animate
    if(m_playerJumpAnim==true)
    {
        m_player->setFrame(m_player->getFrame()+1);
        m_player->playerJump();
        if(m_player->getFrame()>=58)
        {
            m_playerJumpAnim=false;
        }
    }
    else if(m_playerWalk==true)
    {
        m_player->setFrame(m_player->getFrame()+1);
        m_player->playerWalk();
        m_playerWalk=false;
    }
    else if(m_playerDie==true)
    {
        m_player->setFrame(m_player->getFrame()+1);
        if(m_player->getFrame() == 119)
        {
            m_playerDie=false;
            scene->setGameState(Game::State::GameOver);
        }
        m_player->playerDie();
    }
    else
    {
        m_player->playerStill();
    }

    //update camera eye to behind the player
    cameraFollow(m_player->getPos(), m_player->getRot());
    m_cam.update();

    //load shaders and draw(player)
    loadMatricesToShader();
    ngl::Mat4 tmp;
    m_player->draw("ChickenShader", tmp, &m_cam);
}

//________________________________________________________________________________
void GameScene::eggCollision()
{
    //counter to get which egg reference
    //for each egg
    for(std::vector<Egg>::iterator s = m_eggArray.begin(); s != m_eggArray.end();)
    {
        //see if they collide
        bool collide = m_collision->ssCollision(s->getPos(), s->getRadius(),
                                 m_player->getPos(),m_player->getRad());
        if(collide== true)
        {
            //if collide, find egg and append
            int x = s->getPos().m_x;
            int z = s->getPos().m_z;
            m_mazeP->changeEggMap(x,z);
            s = m_eggArray.erase(s);
            Game::Statistic *stat = Scene::getInstance()->getGameStatistic();
            if(stat)
            {
                stat->addEgg();
            }
        }
        else
        {
            s++;
        }
    }
}

//_______________________________________________________________________________________________________
void GameScene::floorCollision()
{
    for(Plane &s : m_planeArray)
    {
        ngl::Vec3 center = s.getCenter();
        GLfloat width = s.getWidth();
        GLfloat depth = s.getDepth();
        ngl::Vec3 norm = s.getNormal();
        bool collide = m_collision->spCollision(center, width, depth, norm,
                                                m_player->getPos(), m_player->getRad());
        if (collide==true)
        {
            m_playerJump=false;
            m_grav=0;
        }
    }
}


//_______________________________________________________________________________________________________
void GameScene::wallCollision()
{
    //for each box
    for(Box &s : m_boxArray)
    {
        bool collide = m_collision->sbCollision(s.getMin(), s.getMax(),
                                                m_player->getRad(), m_player->getPos());
        if (collide==true)
        {
            if(m_speed>=0)
                m_player->move(-0.2f, -m_grav*2, -0.2f);
            else
                m_player->move(0.2f, -m_grav*2, 0.2f);
            m_speed=m_speed*-0.75f;

        }
    }

    //for each cannon
    for(Box &s : m_cannonArray)
    {
        bool collide = m_collision->sbCollision(s.getMin(), s.getMax(),
                                                m_player->getRad(), m_player->getPos());
        if (collide==true)
        {
            if(m_speed>=0)
                m_player->move(-0.2f, -m_grav*2, -0.2f);
            else
                m_player->move(0.2f, -m_grav*2, 0.2f);
            m_speed=m_speed*-0.75f;

        }
    }

    // for each moving obstacle
    for(MoveObstacle &s : m_moveObArrayH)
    {
        bool collide = m_collision->sbCollision(s.getMin(), s.getMax(),
                                                m_player->getRad(), m_player->getPos());
        if (collide==true)
        {
            if(m_speed>=0)
                m_player->move(-0.2f, -m_grav*2, -0.2f);
            else
                m_player->move(0.2f, -m_grav*2, 0.2f);
            m_speed=m_speed*-0.75f;

        }
    }

    //for vertical moving obstacles
    for(MoveObstacle &s : m_moveObArrayV)
    {
        bool collide = m_collision->sbCollision(s.getMin(), s.getMax(),
                                                m_player->getRad(), m_player->getPos());
        if (collide==true)
        {
            if(m_speed>=0)
                m_player->move(-0.2f, -m_grav*2, -0.2f);
            else
                m_player->move(0.2f, -m_grav*2, 0.2f);
            m_speed=m_speed*-0.75f;

        }
    }
}

//_______________________________________________________________________________________________________
void GameScene::bulletCollision()
{
    //for each bullet
    for(Bullet &s : m_bulletArray)
    {
        //see if they collide
        bool collide = m_collision->ssCollision(s.getPos(), s.getRadius(),
                                 m_player->getPos(),m_player->getRad());
        if(collide== true)
        {
            m_playerDie=true;
        }


        //if collides with wall, reset bullet shooting
        for (Box &t : m_boxArray)
        {
            bool collideWall = m_collision->sbCollision(t.getMin(),t.getMax(),
                                                        s.getRadius(), s.getPos());
            if(collideWall==true)
            {
                s.resetPos(m_player->getPos());
            }
        }
    }
}

//________________________________________________________________________________
void GameScene::checkCollisions()
{
    eggCollision();
    floorCollision();
    wallCollision();
    bulletCollision();
}


//________________________________________________________________________________
void GameScene::controlPlayer()
{
    //resets movement vlaues
    float xDirection = 0.0;
    float rotation=0.0;


    //loop for each pressed key in set, if pressed, set mmovedment calue to be incremented
    std::vector<Qt::Key> pressed_keys;
    s_GameKeyboard->mapPressedKeys(pressed_keys);
    if(pressed_keys.size())
    {
        for(std::vector<Qt::Key>::iterator it = pressed_keys.begin(); it != pressed_keys.end(); ++it)
        {
            switch(*it)
            {
                case Qt::Key_A:
            case Qt::Key_Left:
            {
                rotation = 3;
                break;
            }

                case Qt::Key_D:
            case Qt::Key_Right:
            {
                rotation = -3;
                break;
            }

                case Qt::Key_W:
            case Qt::Key_Up:
            {
                xDirection = m_acceleration;
                m_playerWalk = true;
                break;
            }

                case Qt::Key_S:
            case Qt::Key_Down:
            {
                xDirection = -m_acceleration;
                m_playerWalk = true;
                break;
            }
            case Qt::Key_Space:
            {
                if (m_grav==0)
                {   m_playerJump=true;
                    m_playerJumpAnim=true;
                }
                break;
            }

                default: break;
            }
        }
    }

    //if key pressed
    if(pressed_keys.size() != 0)
    {
        //updates speed
        m_speed+=xDirection;
        //turns correctly based on orientation
        if (m_speed>=0)
            m_player->rotate(rotation);
        else if (m_speed<0)
            m_player->rotate(-rotation);
    }


    //if on the floor and jump button pressed
    if (m_grav==0 && m_playerJump==true)
    {
        jump();
    }
}

//________________________________________________________________________________
void GameScene::jump()
{

    float yDirection=0.0;
    yDirection = m_jumpHt - 0.01f;
    m_jumpHt -= 0.01f;

    //if at peak of jump
    if (yDirection<=0.0)
    {
        yDirection=0;
        m_jumpHt=0.18f;
        m_playerJump=false;
    }
    m_player->move(0,yDirection,0);

}

//________________________________________________________________________________
void GameScene::gravity()
{
    if (m_grav > -0.18f)
    {   m_grav-=0.01f;}
}

//________________________________________________________________________________
void GameScene::movePlayer()
{
    //top speed
    if(m_speed >= MAX_SPEED)
    {
        m_speed = MAX_SPEED;
    }
    else if(m_speed <= - MAX_SPEED)
    {
        m_speed = - MAX_SPEED;
    }

    //deceleration
    if (m_speed>0)
    {
        m_speed -= STOPPING_SPEED;
    }
    else if (m_speed<0)
    {
        m_speed += STOPPING_SPEED;
    }

    //stopping
    if(-STOPPING_SPEED < m_speed && m_speed < STOPPING_SPEED)
    {   m_speed=0;}
    m_player->move(m_speed, m_grav, m_speed);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 4. class implementation - GameKeyboard
GameKeyboard::GameKeyboard()
    : KeyboardObserver()
{
    // register the keys
    attachKey(Qt::Key_A /* Move Left */);
    attachKey(Qt::Key_D /* Move Right */);
    attachKey(Qt::Key_Left /* Move Left */);
    attachKey(Qt::Key_Right /* Move Right */);
    attachKey(Qt::Key_W /* Move Forward */);
    attachKey(Qt::Key_S /* Move Backwards */);
    attachKey(Qt::Key_Up /* Move Forwards */);
    attachKey(Qt::Key_Down /* Move Backwords */);
    attachKey(Qt::Key_Space);//jump

}


//________________________________________________________________________________
void GameScene::cameraFollow(ngl::Vec3 _playerPos, float _playerRot)
{
    ngl::Vec3 newCamLook = _playerPos;
    ngl::Vec3 newCamPos = _playerPos;
    ngl::Vec3 cameraUp = ngl::Vec3(0,1,0);
    newCamPos[0] += 3 * cos((_playerRot+180) * M_PI / 180);
    newCamPos[1] += 1;
    newCamPos[2] += (-3) * sin((_playerRot+180) * M_PI / 180);


    m_cam.set(newCamPos, newCamLook, cameraUp);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

void GameScene::timerEvent(QTimerEvent *_event)

{
    //If need timer
    Q_UNUSED(_event);
}
