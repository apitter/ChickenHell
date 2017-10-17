#include "Plane.h"
#include <ngl/Util.h>
#include <ngl/Vec3.h>
#include <ngl/VertexArrayObject.h>
#include <ngl/VAOPrimitives.h>


#include <ngl/ShaderLib.h>

Plane::Plane(const ngl::Vec3 &_center, GLfloat _w, GLfloat _d)
{
    //store values
    m_center=_center;
    m_width=_w;
    m_depth=_d;
    //create verts based on values passed in
    m_oVerts[0].m_x=m_center.m_x-(m_width/2.0f);
    m_oVerts[0].m_y=m_center.m_y;
    m_oVerts[0].m_z=m_center.m_z+(m_depth/2.0f);

    m_oVerts[1].m_x=m_center.m_x+(m_width/2.0f);
    m_oVerts[1].m_y=m_center.m_y;
    m_oVerts[1].m_z=m_center.m_z+(m_depth/2.0f);

    m_oVerts[2].m_x=m_center.m_x+(m_width/2.0f);
    m_oVerts[2].m_y=m_center.m_y;
    m_oVerts[2].m_z=m_center.m_z-(m_depth/2.0f);

    m_oVerts[3].m_x=m_center.m_x-(m_width/2.0f);
    m_oVerts[3].m_y=m_center.m_y;
    m_oVerts[3].m_z=m_center.m_z-(m_depth/2.0f);
    // finally calculate the surface m_normal
    m_normal=calcNormal(m_oVerts[3],m_oVerts[2],m_oVerts[1]);
}

Plane::~Plane()
{
    //do nothing
}


void Plane::move(float _x, float _y, float _z)
{
    //set new values

    m_center.m_x+=_x;
    m_center.m_y+=_y;
    m_center.m_z+=_z;
    //move verts based on values passed in
    m_verts[0].m_x+=_x;
    m_verts[0].m_y+=_y;
    m_verts[0].m_z+=_z;

    m_verts[1].m_x+=_x;
    m_verts[1].m_y+=_y;
    m_verts[1].m_z+=_z;

    m_verts[2].m_x+=_x;
    m_verts[2].m_y+=_y;
    m_verts[2].m_z+=_z;

    m_verts[3].m_x+=_x;
    m_verts[3].m_y+=_y;
    m_verts[3].m_z+=_z;

}
