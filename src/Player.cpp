#include "Player.h"
#include <ngl/ShaderLib.h>
#include <math.h>
#include <iostream>

//____________________________________________________________________________________________________________________
Player::Player(ngl::Vec3 _pos, std::string _fname, std::string _tname)
{
    m_pos=_pos;
    m_mesh.reset(new ngl::Obj(_fname, _tname));
    m_mesh->createVAO();
    m_rotation=-90;
    m_rad=0.3f;
    m_frame=0;
    loadAnimation();
}

//____________________________________________________________________________________________________________________
void Player::draw( const std::string &_shaderName, const ngl::Mat4 &_globalTx,  ngl::Camera *_cam )const
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use(_shaderName);
  // grab an instance of the primitives for drawing
  ngl::Transformation t;
  //adjust y because draws off-center
  t.setPosition(m_pos.m_x, m_pos.m_y-0.4, m_pos.m_z);
  t.setRotation(0,m_rotation,0);
  loadMatricesToShader(t,_globalTx,_cam);
  m_mesh->draw();
}

//____________________________________________________________________________________________________________________
void Player::loadMatricesToShader(ngl::Transformation &_tx, const ngl::Mat4 &_globalTx, ngl::Camera *_cam ) const
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=_tx.getMatrix()*_globalTx;
  MV=M*_cam->getViewMatrix() ;
  MVP=MV*_cam->getProjectionMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
}
//____________________________________________________________________________________________________________________

void Player::move(float _x, float _y, float _z)
{
    m_pos.m_x += _x * sin((m_rotation+90) * M_PI / 180);
    m_pos.m_y += _y;
    m_pos.m_z += _z * cos((m_rotation+90) * M_PI / 180);
}

//____________________________________________________________________________________________________________________
void Player::rotate(float _rot)
{
    m_rotation += _rot;
}
//____________________________________________________________________________________________________________________
void Player::setRotate(float _rot)
{
    m_rotation = _rot;
}
//____________________________________________________________________________________________________________________
void Player::setFrame(uint _frame)
{
    m_frame = _frame;
}
//____________________________________________________________________________________________________________________
void Player::loadAnimation()
{
    m_animWalkData.reset(new ngl::NCCAPointBake("models/GAMEChickenWalkAnimation.xml"));
    m_animWalkData->setFrame(0);
    m_animWalkData->attachMesh(m_mesh.get());

    m_animJumpData.reset(new ngl::NCCAPointBake("models/GAMEChickenJumpAnimation.xml"));
    m_animJumpData->setFrame(0);
    m_animJumpData->attachMesh(m_mesh.get());

    m_animDieData.reset(new ngl::NCCAPointBake("models/GAMEChickenDieAnimation.xml"));
    m_animDieData->setFrame(0);
    m_animDieData->attachMesh(m_mesh.get());

    m_frame=0;
    glEnable(GL_MULTISAMPLE);
    //m_animTimer=startTimer(8);
}
//____________________________________________________________________________________________________________________
void Player::playerWalk()
{
    if(m_frame>m_animWalkData->getNumFrames())
      {
          m_frame=0;
      }
      m_animWalkData->setMeshToFrame(m_frame);
}

//____________________________________________________________________________________________________________________
void Player::playerJump()
{
    if(++m_frame>m_animJumpData->getNumFrames())
         {
             m_frame=0;
         }
         m_animJumpData->setMeshToFrame(m_frame);
}
//____________________________________________________________________________________________________________________
void Player::playerDie()
{
    if(++m_frame>m_animDieData->getNumFrames())
         {
             m_frame=0;
         }
         m_animDieData->setMeshToFrame(m_frame);
}
//____________________________________________________________________________________________________________________
void Player::playerStill()
{
    m_frame=0;
    m_animWalkData->setMeshToFrame(m_frame);
}

