#ifndef COLLISIONS
#define COLLISIONS

#include "Plane.h"
#include "box.h"
#include "EggC.h"
#include "Player.h"
#include <scene.h>

class Collisions
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief sphere to sphere collision test
    /// @param _pos1, _rad1; position and radius of first sphere
    /// @param _pos2, _rad2; position and radius of second sphere
    //----------------------------------------------------------------------------------------------------------------------
    bool ssCollision(ngl::Vec3 _pos1, GLfloat _rad1, ngl::Vec3 _pos2, GLfloat _rad2);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief sphere to plane collision test
    /// @param _pPos, _w, _d, _norm; position, width, depth and normal of plane
    /// @param _sPos, _rad; position and radius of sphere
    //----------------------------------------------------------------------------------------------------------------------
    bool spCollision(ngl::Vec3 _pPos, GLfloat _w, GLfloat _d, ngl::Vec3 _norm, ngl::Vec3 _sPos, GLfloat _rad);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief sphere to AABB collision test
    /// @param _min, _max; position of minimum and maximum corners of AABB
    /// @param _rad, _sPos; position and radius of sphere
    //----------------------------------------------------------------------------------------------------------------------
    bool sbCollision(ngl::Vec3 _min, ngl::Vec3 _max, GLfloat _rad, ngl::Vec3 _sPos);

private:

};

#endif // COLLISION

