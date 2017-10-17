#include "Collisions.h"




//________________________________________________________________________________________________
bool Collisions::ssCollision(ngl::Vec3 _pos1, GLfloat _rad1, ngl::Vec3 _pos2, GLfloat _rad2)
{
    //position relative to each other
    ngl::Vec3 relPos;
    //distance
    GLfloat dist;
    //min dist they can be before collision
    GLfloat minDist;
    //stores length of distance
    GLfloat len;

    relPos = _pos1-_pos2;
    len=relPos.length();
    dist=len*len;
    minDist=_rad1+_rad2;

    if(dist <= (minDist*minDist))
    {    return true;}
    else
    {   return false;}
}

//________________________________________________________________________________________________
bool Collisions::spCollision(ngl::Vec3 _pPos, GLfloat _w, GLfloat _d, ngl::Vec3 _norm, ngl::Vec3 _sPos, GLfloat _rad)
{
    //boolean to return
    bool coll=false;
    //distance
    GLfloat dist;
    //calculate distance
    dist = (_norm).dot(_sPos - _pPos);
    dist-=_rad;
    if (dist <= 0.0f)
    {
        if(    (_sPos.m_x > _pPos.m_x - (_w/2.0f)) &&
               (_sPos.m_x < _pPos.m_x + (_w/2.0f)) &&
               (_sPos.m_z > _pPos.m_z - (_d/2.0f)) &&
               (_sPos.m_z < _pPos.m_z + (_d/2.0f))  )
        {   coll=true;}
        else
        {   coll=false;}
    }
    return coll;

}

//________________________________________________________________________________________________
bool Collisions::sbCollision(ngl::Vec3 _min, ngl::Vec3 _max, GLfloat _rad, ngl::Vec3 _sPos)
{
    //sphere collision radius
    GLfloat dist = _rad*_rad;

    //check x
    if (_sPos.m_x < _min.m_x)
    {   dist -= (_sPos.m_x - _min.m_x)*(_sPos.m_x - _min.m_x);}
    else if (_sPos.m_x > _max.m_x)
    {   dist -= (_sPos.m_x - _max.m_x)*(_sPos.m_x - _max.m_x);}

    //check y
    if (_sPos.m_y < _min.m_y)
    {   dist -= (_sPos.m_y - _min.m_y)*(_sPos.m_y - _min.m_y);}
    else if (_sPos.m_y > _max.m_y)
    {   dist -= (_sPos.m_y - _max.m_y)*(_sPos.m_y - _max.m_y);}

    //check z
    if (_sPos.m_z < _min.m_z)
    {   dist -= (_sPos.m_z - _min.m_z)*(_sPos.m_z - _min.m_z);}
    else if (_sPos.m_z > _max.m_z)
    {   dist -= (_sPos.m_z - _max.m_z)*(_sPos.m_z - _max.m_z);}

    //calculate if collision
    if (dist>0)
    {   return true;}
    else
    {   return false;}
}


