#include "VAOPrimitivesFix.h"
/// Тhe source code in VAOPrimitivesFix.h and VAOPrimitivesFix.cpp was originally written by Jon Macey
#ifdef namespace_ngl_class_VAOPrimitives_method_createTrianglePlane_issue_not_fixed
/*
  Copyright (C) 2009 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "ngl/Util.h"
#include <iostream>


//----------------------------------------------------------------------------------------------------------------------
/// @file VAOPrimitivesFix.cpp
/// @brief implementation files for VAOPrimitives class
//----------------------------------------------------------------------------------------------------------------------
namespace ngl
{


//----------------------------------------------------------------------------------------------------------------------
VAOPrimitivesFix::VAOPrimitivesFix()
{
}


//----------------------------------------------------------------------------------------------------------------------

void VAOPrimitivesFix::draw( const std::string &_name )
{
  // get an iterator to the VertexArrayObjects
  auto VAO=m_createdVAOs.find(_name);
  // make sure we have a valid shader
  if(VAO!=m_createdVAOs.end())
  {
    // grab the pointer to the shader and call compile
    VAO->second->bind();
    VAO->second->draw();
    VAO->second->unbind();
  }
  else {std::cerr<<"Warning VAO not know in Primitive list "<<_name.c_str()<<"\n";}

}

void VAOPrimitivesFix::draw( const std::string &_name, GLenum _mode )
{
  // get an iterator to the VertexArrayObjects
  auto VAO=m_createdVAOs.find(_name);
  // make sure we have a valid shader
  if(VAO!=m_createdVAOs.end())
  {
    // grab the pointer to the shader and call compile
    VAO->second->bind();
    VAO->second->draw(_mode);
    VAO->second->unbind();
  }
  else {std::cerr<<"Warning VAO not know in Primitive list "<<_name.c_str()<<"\n";}

}

void VAOPrimitivesFix::createVAOFromHeader(const std::string &_name, const Real *_data,  unsigned int _size )
{
    VertexArrayObject *vao = VertexArrayObject::createVOA(GL_TRIANGLES);
    // next we bind it so it's active for setting data
    vao->bind();
    // now we have our data add it to the VAO, we need to tell the VAO the following
    // how much (in bytes) data we are copying
    // a pointer to the first element of data (in this case the address of the first element of the
    // std::vector
    vao->setData( _size*sizeof(Real),_data[0]);
    // in this case we have packed our data in interleaved format as follows
    // u,v,nx,ny,nz,x,y,z
    // If you look at the shader we have the following attributes being used
    // attribute vec3 inVert; attribute 0
    // attribute vec2 inUV; attribute 1
    // attribute vec3 inNormal; attribute 2
    // so we need to set the vertexAttributePointer so the correct size and type as follows
    // vertex is attribute 0 with x,y,z(3) parts of type GL_FLOAT, our complete packed data is
    // sizeof(vertData) and the offset into the data structure for the first x component is 5 (u,v,nx,ny,nz)..x
    vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(vertData),5);
    // uv same as above but starts at 0 and is attrib 1 and only u,v so 2
    vao->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(vertData),0);
    // normal same as vertex only starts at position 2 (u,v)-> nx
    vao->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(vertData),2);
    // now we have set the vertex attributes we tell the VAO class how many indices to draw when
    // glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
    // specify less (in steps of 3))
    // In this case each Generic Vertex attribute is 8 elements so we use _size / 8
    vao->setNumIndices(_size/8);
    // finally we have finished for now so time to unbind the VAO
    vao->unbind();
    m_createdVAOs[_name]=vao;
   // std::cout<<_name<<" Num Triangles "<<data.size()/3<<"\n";

}

void VAOPrimitivesFix::createVAO(const std::string &_name,const std::vector<vertData> &_data,	const GLenum _mode)
{

  VertexArrayObject *vao = VertexArrayObject::createVOA(_mode);
  // next we bind it so it's active for setting data
  vao->bind();

  // now we have our data add it to the VAO, we need to tell the VAO the following
  // how much (in bytes) data we are copying
  // a pointer to the first element of data (in this case the address of the first element of the
  // std::vector
  vao->setData(_data.size()*sizeof(vertData),_data[0].u);
  // in this case we have packed our data in interleaved format as follows
  // u,v,nx,ny,nz,x,y,z
  // If you look at the shader we have the following attributes being used
  // attribute vec3 inVert; attribute 0
  // attribute vec2 inUV; attribute 1
  // attribute vec3 inNormal; attribure 2
  // so we need to set the vertexAttributePointer so the correct size and type as follows
  // vertex is attribute 0 with x,y,z(3) parts of type GL_FLOAT, our complete packed data is
  // sizeof(vertData) and the offset into the data structure for the first x component is 5 (u,v,nx,ny,nz)..x
  vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(vertData),5);
  // uv same as above but starts at 0 and is attrib 1 and only u,v so 2
  vao->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(vertData),0);
  // normal same as vertex only starts at position 2 (u,v)-> nx
  vao->setVertexAttributePointer(2,3,GL_FLOAT,sizeof(vertData),2);
  // now we have set the vertex attributes we tell the VAO class how many indices to draw when
  // glDrawArrays is called, in this case we use buffSize (but if we wished less of the sphere to be drawn we could
  // specify less (in steps of 3))
  vao->setNumIndices(_data.size());
  // finally we have finished for now so time to unbind the VAO
  vao->unbind();
  m_createdVAOs[_name]=vao;
 // std::cout<<_name<<" Num Triangles "<<_data.size()/3<<"\n";

}

//----------------------------------------------------------------------------------------------------------------------
void VAOPrimitivesFix::createTrianglePlane(const std::string &_name, const Real _width, const Real _depth, const int _wP, const int _dP, const Vec3 &_vN)
{
    // calculate the VBO size basically we have 2 tris per quad based on the width and depth
    // _precision.

    // as our plane is centered on 0.0 we range from Width/2.0 and Depth/2.0
    Real w2 = _width / 2.0f;
    Real d2 = _depth / 2.0f;
    // calculate the steps for each quad / tri
    Real wStep = _width / _wP;
    Real dStep = _depth / _dP;
    // texture co-ords start at 0,0
    // texture steps
    Real du = 1.0f / _wP;
    Real dv = 1.0f / _dP;

    Real u = 0.0;
    Real v = 0.0;

    // a std::vector to store our verts, remember vector packs contiguously so we can use it
    std::vector <vertData> data;
    vertData vert;

    for(Real d = -d2; d<d2; d += dStep)
    {
        for(Real w = -w2; w<w2; w += wStep)
        {
            /* tri 1
            // counter clock wise
            3
            | \
            |  \
            |   \
            1____2
            */
            // the normals are always the same so set them for d first
            vert.nx=_vN.m_x;
            vert.ny=_vN.m_y;
            vert.nz=_vN.m_z;
            // y is always 0 as in a plane
            vert.y=0.0;
            // now for the per vert stuff
            vert.u=u; vert.v=v+dv;  vert.x=w;  vert.z=d+dStep;
            data.push_back(vert);
            // 2
            vert.u=u+du; vert.v=v+dv; vert.x=w+wStep;  vert.z=d+dStep;
            data.push_back(vert);
            // 3
            vert.u = u; vert.v = v; vert.x = w;  vert.z = d;
            data.push_back(vert);


            /* tri 2 w,0,d
            // counter clock wise
            3_____2
             \   |
              \  |
               \ |
                \|
                 1

            */
            vert.u = u + du; vert.v = v + dv;  vert.x = w + wStep;  vert.z = d + dStep;
            data.push_back(vert);
            // 2
            vert.u = u + du; vert.v = v;   vert.x = w + wStep;  vert.z = d;
            data.push_back(vert);
            // 3
            vert.u = u; vert.v = v;  vert.x = w;   vert.z = d;
            data.push_back(vert);
            u += du;
        } // end w loop
        u = 0.0;
        v += dv;
    } // end d loop
    // now create the VBO
    createVAO(_name, data, GL_TRIANGLES);
}


//----------------------------------------------------------------------------------------------------------------------
void VAOPrimitivesFix::clear()
{

  std::cerr<<"clearing VAOs\n";

  // loop through the map and delete the VBO's allocated
  // note glDeleteBuffers needs a const GLUint * so we need to de-reference the map object
  for(auto v : m_createdVAOs)
  {
    GLuint address=v.second->getID();
    glDeleteVertexArrays(1,&address);

  }

    m_createdVAOs.erase(m_createdVAOs.begin(),m_createdVAOs.end());
}



} // end ngl namespace
#endif // namespace_ngl_class_VAOPrimitives_method_createTrianglePlane_issue_not_fixed
