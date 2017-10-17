#ifndef _BULLET_H
#define _BULLET_H
#include <ngl/Vec3.h>
#include <ngl/Transformation.h>

#include <ngl/Camera.h>
#include <ngl/ShaderLib.h>

class Bullet
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor
    /// @param pos; position of sphere
    /// @param rad; radius of sphere
    /// @param dir; direction of travel
    //----------------------------------------------------------------------------------------------------------------------
    Bullet(const ngl::Vec3 &_pos, GLfloat _rad, ngl::Vec3 _dir);


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief returns variables
    //----------------------------------------------------------------------------------------------------------------------
    inline ngl::Vec3 getPos() const {return m_pos;}
    inline ngl::Vec3 getOrigPos() const{return m_origPos;}
    inline GLfloat getRadius() const {return m_radius;}


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief moves bullet in direction of _dir
    /// @param _dir; where the bullet is moving towards
    //----------------------------------------------------------------------------------------------------------------------
    void move(ngl::Vec3 _dir);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief resets position of bullet
    /// @param _dir; where bullet is moving towards
    //----------------------------------------------------------------------------------------------------------------------
    void resetPos(ngl::Vec3 _dir);


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief draws bullet
    /// @param _shaderName; name of shader to use.
    /// @param _cam; camera being used to draw
    /// @param _tx; transformation
    //----------------------------------------------------------------------------------------------------------------------
    void draw(const std::string &_shaderName, ngl::Camera *_cam   );
    void loadMatricesToShader(ngl::Transformation &_tx, ngl::Camera *_cam )const;

private:

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Position, radius, starting position and target direction of bullet
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 m_pos;
    GLfloat m_radius;
    ngl::Vec3 m_origPos;
    ngl::Vec3 m_dir;

    //----------------------------------------------------------------------------------------------------------------------
    ///@param transformation matrix
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Transformation m_transform;


};

#endif // BULLET

