#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H



///@file environment.h
///@auther Di Wu adapted from Jon Macey's SDLJoypad
///@version 1.0
///@date 7/5/16
///@class Environment
///@brief the class used to load texture information for the environment



#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/ShaderLib.h>
#include <ngl/Camera.h>

class Environment
{
public :
  //----------------------------------------------------------------------------------------------------------------------
  ///@brief ctor
  ///@param _texture; texture file to be loaded
  ///@param _texture2; texture2 file to be loaded
  ///@param _cam; camera being used to draw
  //----------------------------------------------------------------------------------------------------------------------
  Environment(std::string _texture,std::string _texture2, ngl::Camera *_cam);
  //~Environment();
  //----------------------------------------------------------------------------------------------------------------------
  ///@brief draw function to draw the environment
  //----------------------------------------------------------------------------------------------------------------------
  void draw();

private :
  //----------------------------------------------------------------------------------------------------------------------
  ///@param textureID
  //----------------------------------------------------------------------------------------------------------------------
  GLuint m_texID;
  GLuint m_texID2;
  //----------------------------------------------------------------------------------------------------------------------
  ///@param a pointer to the ngl::camera class
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Camera *m_camera;




};









































#endif

