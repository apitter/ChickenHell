#include "moveobstacle.h"
#include <ngl/VAOPrimitives.h>

//_______________________________________________________________________________________
MoveObstacle::MoveObstacle(const ngl::Vec3 &_pos,  GLfloat _w, GLfloat _h, GLfloat _d)
{
    m_origPos=_pos;
    m_pos=_pos;
    //load in moving obstacle mesh
    m_moveObMesh.reset(new ngl::Obj("models/ForkObstacle.obj"));
    m_moveObMesh->createVAO();
    m_dirH = true;
    m_dirV = true;
    m_w=_w;
    m_h=_h;
    m_d=_d;
    m_min=m_pos-ngl::Vec3(0.5*m_w, 0.5*m_h, 0.5*m_d);
    m_max=m_pos+ngl::Vec3(0.5*m_w, 0.5*m_h, 0.5*m_d);

}

//_______________________________________________________________________________________
void MoveObstacle::moveH(float _x)
{
    if (m_dirH==true)
    {
        m_pos.m_x += _x;
        m_min.m_x += _x;
        m_max.m_x += _x;
        if(m_pos.m_x>=m_origPos.m_x+1)
        {
                m_dirH=false;
        }
    }
    if (m_dirH==false)
    {
        m_pos.m_x -= _x;
        m_min.m_x -= _x;
        m_max.m_x -= _x;
        if(m_pos.m_x<=m_origPos.m_x-1)
        {
            m_dirH=true;
        }
    }
}

//_______________________________________________________________________________________
void MoveObstacle::moveV(float _z)
{
    if (m_dirV==true)
    {
        m_pos.m_z += _z;
        m_min.m_z += _z;
        m_max.m_z += _z;
        if(m_pos.m_z>=m_origPos.m_z+1)
        {
                m_dirV=false;
        }
    }
    if (m_dirV==false)
    {
        m_pos.m_z -= _z;
        m_min.m_z -= _z;
        m_max.m_z -= _z;
        if(m_pos.m_z<=m_origPos.m_z-1)
        {
            m_dirV=true;
        }
    }
}

//_______________________________________________________________________________________
void MoveObstacle::draw(const std::string &_shaderName, ngl::Camera *_cam )
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use(_shaderName);
  // grab an instance of prims
  //ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  ngl::Transformation t;
  t.setPosition(m_pos.m_x, m_pos.m_y-1.5, m_pos.m_z);
  loadMatricesToShader(t,_cam);
  m_moveObMesh->draw();
}

//_______________________________________________________________________________________
void MoveObstacle::loadMatricesToShader(ngl::Transformation &_tx, ngl::Camera *_cam ) const
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

