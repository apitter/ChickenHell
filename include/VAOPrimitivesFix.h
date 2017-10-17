#ifndef VAOPrimitivesFix_H__
#define VAOPrimitivesFix_H__
/// Тhe source code in VAOPrimitivesFix.h and VAOPrimitivesFix.cpp was originally written by Jon Macey
/*
    I did the changes in method VAOPrimitivesFix::createTrianglePlane() because when using
    the original VAOPrimitives the textures in the UI were being cropped. I discovered this
    issue when debugging the drawing of the textures and found that this happens because of
    inccorect uv calculations. By overwriting a part of the class VAOPrimitives, this was able
    to fix the problem. No intention is made at plagarism and as soon as this mistake is fixed
    in NGL itself, the code below can be removed by commenting it. Attempts were made to point
    out the problem by contacing Jon on e-mail but no action was taken to fix them.
 */
// if the ngl::VAOPrimitives::createTrianglePlane issue does fixed comment out the next line
#define namespace_ngl_class_VAOPrimitives_method_createTrianglePlane_issue_not_fixed
#ifdef namespace_ngl_class_VAOPrimitives_method_createTrianglePlane_issue_not_fixed
/*
  Copyright (C) 2011 Jon Macey

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
// must include types.h first for Real
#include "ngl/Singleton.h"
#include "ngl/Types.h"
#include "ngl/Vec3.h"
#include "ngl/VertexArrayObject.h"
#include "ngl/VAOPrimitives.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace ngl
{
//----------------------------------------------------------------------------------------------------------------------
/// @class VAOPrimitives "include/VAOPrimitives.h"
/// @brief VAO based object primitives used for fast openGL drawing this is a singelton class
///  so we use the instance method to get access and construct the only instance
/// @author Jonathan Macey
/// @version 1.0
/// @date Last Revision 20/06/11 created and removed VBO class
//----------------------------------------------------------------------------------------------------------------------
// a simple structure to hold our vertex data this is a
// legacy format form GLDebugger as we could set to 2 3 3 format
class VAOPrimitivesFix: public  Singleton<VAOPrimitivesFix>
{

    friend class Singleton<VAOPrimitivesFix>;
  friend class Vec3;

public :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Draw one of the VBO's created via a name lookup
  /// @param[in] _name the name of the VBO to lookup in the VBO map
  //----------------------------------------------------------------------------------------------------------------------
  void draw( const std::string &_name );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Draw one of the VBO's created via a name lookup
  /// @param[in] _name the name of the VBO to lookup in the VBO map
  /// @param[in] _mode the draw mode for the VBO override the default
  //----------------------------------------------------------------------------------------------------------------------
  void draw( const std::string &_name,GLenum _mode );
    //----------------------------------------------------------------------------------------------------------------------
  /// @brief create a triangulated plane as a vbo with auto generated texture cords
  /// @param[in] _name the name of the object created used when drawing
  /// @param[in] _width the width of the plane based on the center of the plane being at 0,0,0
  /// @param[in] _depth the height of the plane based on the center of the plane being at 0,0,0
  /// @param[in] _wP the precision of the width, this is basically the steps (per quad) which will be
  ///    triangulated for each (wP == 1 will give 1 quad mad of 2 tris)
  /// @param[in] _dP the precision of the Depth, this is basically the steps (per quad) which will be
  ///    triangulated for each (wP == 1 will give 1 quad mad of 2 tris)
  /// @param[in] _vN The Vertex normal (used for each vertex)
  //----------------------------------------------------------------------------------------------------------------------
  void createTrianglePlane( const std::string &_name,const Real _width,const Real _depth,const int _wP, const int _dP,const Vec3 &_vN );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief load a VBO from a binary file created from Obj2VBO program
  /// @param[in] _name the name of the VBO to be stored as ref to this object
  /// @param[in] _fName the name of the file to load.
  /// @param[in] _type the draw mode type
  //----------------------------------------------------------------------------------------------------------------------
  void loadBinary( const std::string &_name, const std::string &_fName,const GLenum _type );
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief clear the VAO elements created (is also called by dtor) This is usefull if you
  /// don't want the default primitives
  //----------------------------------------------------------------------------------------------------------------------
  void clear();


private :
	//----------------------------------------------------------------------------------------------------------------------
	///  a map to store the VAO by name
	//----------------------------------------------------------------------------------------------------------------------
	std::unordered_map <std::string,VertexArrayObject *> m_createdVAOs;

	//----------------------------------------------------------------------------------------------------------------------
	/// @brief default constructor
	//----------------------------------------------------------------------------------------------------------------------
	VAOPrimitivesFix();
	//----------------------------------------------------------------------------------------------------------------------
	/// @brief dtor
	//----------------------------------------------------------------------------------------------------------------------
	virtual ~VAOPrimitivesFix(){clear();}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create a VAO from a static header file of data in the TNV format
  /// this will usually be created from the Obj2VBO program in the Models directory
  /// @param[in] _name the name reference for the VBO lookup
  /// @param[in] _data a pointer to the data to load
  /// @param[in] _Size the size of the array of data to load
  //----------------------------------------------------------------------------------------------------------------------
  void createVAOFromHeader( const std::string &_name, Real const *_data,  unsigned int _Size );

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the method to actually create the VAO from the various other methods
  /// Note this is used in conjunction with the vertData struct
  /// @param[in] _name the name to store in the map of the VBO
  /// @param[in] _data the raw data packed into the vertData structure
  /// @param[in] _mode the mode to draw
  //----------------------------------------------------------------------------------------------------------------------
  void createVAO( const std::string &_name, const std::vector <vertData> &_data, const GLenum _mode );

};

}// end ngl namespace
typedef ngl::VAOPrimitivesFix VAOPrimitivesPlane;
#else
#include "HeaderBase.h"
#include <ngl/VAOPrimitives.h>
typedef ngl::VAOPrimitives VAOPrimitivesPlane;
#endif
#endif
