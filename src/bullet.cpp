#include "bullet.h"
#include <ngl/VAOPrimitives.h>
#include "scene.h"
const float SPEED = 0.1f;

//_______________________________________________________________________________________
Bullet::Bullet(const ngl::Vec3 &_pos,  GLfloat _rad, ngl::Vec3 _dir)
{
    m_origPos=_pos;
    m_pos=_pos;
    m_radius=_rad;
    m_dir=m_origPos-_dir;
}

//_______________________________________________________________________________________
void Bullet::move(ngl::Vec3 _dir)
{
    //sets the direction of bullet movement
    m_dir.normalize();
    float x = m_dir.m_x*-SPEED;
    float z = m_dir.m_z*-SPEED;
    m_pos.m_x += x;
    m_pos.m_z += z;


    //if outside map, reset
    if (m_pos.m_x<0 || m_pos.m_x>50 || m_pos.m_z<0 || m_pos.m_z>60)
    {
        resetPos(_dir);
    }
}

//_______________________________________________________________________________________
void Bullet::draw( const std::string &_shaderName,  ngl::Camera *_cam )
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use(_shaderName);
  // grab an instance of prims
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  ngl::Transformation t;
  t.setPosition(m_pos);
  t.setScale(m_radius*2,m_radius*2,m_radius*2);
  loadMatricesToShader(t,_cam);
  prim->draw("sphere");
}

//_______________________________________________________________________________________
void Bullet::loadMatricesToShader(ngl::Transformation &_tx, ngl::Camera *_cam ) const
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=_tx.getMatrix();
  MV=M*_cam->getViewMatrix() ;
  MVP=MV*_cam->getProjectionMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
}

//_______________________________________________________________________________________
void Bullet::resetPos(ngl ::Vec3 _dir)
{
    m_pos.set(m_origPos);
    m_dir=m_origPos-_dir;
}


