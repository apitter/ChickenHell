
#include "environment.h"
#include <ngl/VAOPrimitives.h>
#include <ngl/Texture.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>

Environment::Environment(std::string _texture, std::string _texture2, ngl::Camera *_cam)
{

  ngl::Texture t(_texture);
  m_texID=t.setTextureGL();
  ngl::Texture t2(_texture2);
  m_texID2=t2.setTextureGL();

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createSphere("Environment",300,20);
  m_camera=_cam;
}

void Environment::draw()
{
  static float rot=0.0;
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();

  glEnable (GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  (*shader)["Texture"]->use();
  float m_time=0;
  ++m_time;
  shader->setRegisteredUniform1f("time",m_time);
  ngl::Transformation t;
  t.setRotation(rot,0,0);
  rot+=0.01f;
  ngl::Mat4 MVP;

  MVP=t.getMatrix()*m_camera->getVPMatrix();
  shader->setShaderParamFromMat4("MVP",MVP);
  glBindTexture(GL_TEXTURE_2D,m_texID);
  prim->draw("Environment");

  t.setRotation(0,rot*8,0);
  t.setScale(0.91f,0.91f,0.91f);
  glBindTexture(GL_TEXTURE_2D,m_texID2);

  MVP=t.getMatrix()*m_camera->getVPMatrix();
  shader->setShaderParamFromMat4("MVP",MVP);
  prim->draw("Environment");
  glDisable(GL_BLEND);

}
