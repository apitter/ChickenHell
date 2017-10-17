#ifndef _PLANE_H_
#define _PLANE_H_

#include <ngl/Vec3.h>
#include <ngl/Transformation.h>


#include <ngl/Camera.h>

class Plane
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Ctor
    /// @param _center; center of plane
    /// @param _w, _d; width and depth of plane
    //----------------------------------------------------------------------------------------------------------------------
    Plane(const ngl::Vec3 &_center, GLfloat _w, GLfloat _d);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Dtor
    //----------------------------------------------------------------------------------------------------------------------
    ~Plane();

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Moves plane
    /// @param _x, _y, _z; how much to move the plane
    //----------------------------------------------------------------------------------------------------------------------
    void move(float _x, float _y, float _z);


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Access the normal, center, depth and width values
    //----------------------------------------------------------------------------------------------------------------------
    inline ngl::Vec3 getNormal()const {return m_normal;}
    inline ngl::Vec3 getCenter()const {return m_center;}
    inline GLfloat getDepth()const {return m_depth;}
    inline GLfloat getWidth()const {return m_width;}

private:

    //----------------------------------------------------------------------------------------------------------------------
    ///@param m_verts; verts of plane
    /// @param m_overts; original vert positions
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_verts[4];
    ngl::Vec3 m_oVerts[4];

    //----------------------------------------------------------------------------------------------------------------------
    ///@param center position of plane
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_center;


    //----------------------------------------------------------------------------------------------------------------------
    ///@param m_width, m_depth; width and depth of plane
    //----------------------------------------------------------------------------------------------------------------------
    GLfloat m_width;
    GLfloat m_depth;

    //----------------------------------------------------------------------------------------------------------------------
    ///@param normal of plane
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_normal;
};

#endif // PLANE

