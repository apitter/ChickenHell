#ifndef PLAYER_H
#define PLAYER_H

#include <ngl/Camera.h>
#include <ngl/Vec3.h>
#include <ngl/Obj.h>
#include <ngl/Transformation.h>
#include <ngl/NCCAPointBake.h>
#include <maze.h>

#include <QTime>
#include <QElapsedTimer>
#include <QSet>

//----------------------------------------------------------------------------------------------------------------------
///@file player.h
///@auther Alex Pitter & Georgia Cook
///@version 2.0
///@date 1/6/16
///@class player
///@brief the class used to generate the player
//----------------------------------------------------------------------------------------------------------------------

class Player
{
public:

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Controls for the player
    /// @param _pos; position of player
    /// @param _fname; location of model to load
    /// @param _tname; location of texture to load
    //----------------------------------------------------------------------------------------------------------------------
    Player(ngl::Vec3 _pos, std::string _fname, std::string _tname);


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Draw player
    /// @param _shaderName; name of shader
    /// @param _globalTx; global transformation
    /// @param _cam; camera
    //----------------------------------------------------------------------------------------------------------------------
    void draw(const std::string &_shaderName, const ngl::Mat4 &_globalTx, ngl::Camera *_cam)const ;
    void loadMatricesToShader(ngl::Transformation &_tx, const ngl::Mat4 &_globalTx, ngl::Camera *_cam )const;

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Moves player
    /// @param _x, _y, _z; how much to move player by in x, y & z
    //----------------------------------------------------------------------------------------------------------------------
    void move(float _x, float _y, float _z);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Rotates player by _rot increment
    /// @param _rot; how much to rotate by(in degrees)
    //----------------------------------------------------------------------------------------------------------------------
    void rotate(float _rot);
    
    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Sets rotation of player
    /// @param _rot; how much to rotate by(in degrees)
    //----------------------------------------------------------------------------------------------------------------------
    void setRotate(float _rot);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Sets frame number for animation
    /// @param _frame; frame to set it to
    //----------------------------------------------------------------------------------------------------------------------
    void setFrame(uint _frame);

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Returns Position, rotation, radius or frame of player
    //----------------------------------------------------------------------------------------------------------------------
    inline ngl::Vec3 getPos() const {return m_pos;}
    inline float getRot() const {return m_rotation;}
    inline float getRad() const {return m_rad;}
    inline uint getFrame() const {return m_frame;}

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Sets position of player
    /// @param _newPos; position to set player to
    //----------------------------------------------------------------------------------------------------------------------
    inline void setPos(const ngl::Vec3 &_newpos) { m_pos = _newpos; }

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Functions to define which animation to use
    //----------------------------------------------------------------------------------------------------------------------
    void playerWalk();
    void playerJump();
    void playerDie();
    void playerStill();


private:

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Variables of player
    /// @param m_mesh; mesh the player uses
    /// @param m_transform; transform stack
    /// @param m_rotation; rotational value of player(degrees)
    /// @param m_pos; position of player
    /// @param m_rad; radius of player for collision
    //----------------------------------------------------------------------------------------------------------------------
    std::unique_ptr<ngl::Obj>m_mesh;
    ngl::Transformation m_transform;
    float m_rotation;
    ngl::Vec3 m_pos;
    float m_rad;


    //----------------------------------------------------------------------------------------------------------------------
    ///@brief pointers to pointbake used for animations
    //----------------------------------------------------------------------------------------------------------------------
    std::unique_ptr<ngl::NCCAPointBake> m_animWalkData;
    std::unique_ptr<ngl::NCCAPointBake> m_animJumpData;
    std::unique_ptr<ngl::NCCAPointBake> m_animDieData;

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief Loads animation data
    //----------------------------------------------------------------------------------------------------------------------
    void loadAnimation();

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief frame number for animation
    //----------------------------------------------------------------------------------------------------------------------
    uint m_frame;

};

#endif // PLAYER_H
