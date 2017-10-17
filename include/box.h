#ifndef BOX
#define BOX

#include <ngl/Vec3.h>
#include <ngl/Transformation.h>
#include <vector>
#include "Plane.h"
#include <ngl/Camera.h>

class Box
{
public:
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Ctor
    /// @param _center; vector for center position
    /// @param _w, _h, _d; width, height and depth of box
    //----------------------------------------------------------------------------------------------------------------------
    Box(const ngl::Vec3 &_center, GLfloat _w, GLfloat _h, GLfloat _d);
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Dtor
    //----------------------------------------------------------------------------------------------------------------------
    ~Box();
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Moves box
    /// @param _move; how much to move
    //----------------------------------------------------------------------------------------------------------------------
    void move(ngl::Vec3 _move);
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Sets box position
    /// @param _move; position to set box
    //----------------------------------------------------------------------------------------------------------------------
    void set(ngl::Vec3 _move);
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Returns information
    //----------------------------------------------------------------------------------------------------------------------
    inline ngl::Vec3 getCenter()const {return m_center;}
    inline ngl::Vec3 getDimensions()const{return ngl::Vec3(m_w, m_h, m_d);}
    inline ngl::Vec3 getMin()const{return m_min;}
    inline ngl::Vec3 getMax()const{return m_max;}

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @param m_center; center position of box
    /// @param m_min; minimum value for bounding box
    /// @param m_max; maximum value for bounding box
    /// @param m_w, m_h, m_d; the width, height and depth of the box
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_center;
    ngl::Vec3 m_min;
    ngl::Vec3 m_max;
    GLfloat m_w;
    GLfloat m_h;
    GLfloat m_d;
};

#endif // BOX

