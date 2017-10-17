#include "EggC.h"
#include <ngl/VAOPrimitives.h>


//_______________________________________________________________________________________
Egg::Egg(const ngl::Vec3 &_pos, GLfloat _rad)
{
    //set values from params
    m_pos=_pos;
    m_radius=_rad;
}


//_______________________________________________________________________________________
void Egg::set(const ngl::Vec3 &_pos, GLfloat _rad)
{
    m_pos=_pos;
    m_radius=_rad;
}
