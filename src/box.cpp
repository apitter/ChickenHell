#include "box.h"
#include <ngl/Vec3.h>

Box::Box(const ngl::Vec3 &_center, GLfloat _w, GLfloat _h, GLfloat _d)
{
    m_center=_center;
    m_w=_w;
    m_h=_h;
    m_d=_d;
    //setting the min (center - half of w,h,d)
    m_min=m_center-ngl::Vec3(0.5f*m_w, 0.5f*m_h, 0.5f*m_d);
    //seting the max (center + half of w,h,d)
    m_max=m_center+ngl::Vec3(0.5f*m_w, 0.5f*m_h, 0.5f*m_d);
}

//_______________________________________________________________________________________
Box::~Box()
{
    //dtor
}

//_______________________________________________________________________________________
void Box::move(ngl::Vec3 _move)
{
    m_center+=_move;
    m_min+=_move;
    m_max+=_move;
}

//_______________________________________________________________________________________
void Box::set(ngl::Vec3 _move)
{
    m_center=_move;
    m_min=m_center-ngl::Vec3(0.5f*m_w, 0.5f*m_h, 0.5f*m_d);
    m_max=m_center+ngl::Vec3(0.5f*m_w, 0.5f*m_h, 0.5f*m_d);
}


