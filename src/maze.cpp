#include"maze.h"

//----------------------------------------------------------------------------------------------------------------------
Maze::Maze(int _level)
{
    //take in the level within intialization
    level = _level;
}
//----------------------------------------------------------------------------------------------------------------------
void Maze::changeEggMap(int x, int y)
{
    //set egg taken to 0
    eggMap[x][y]=0;
}
//----------------------------------------------------------------------------------------------------------------------
void Maze::createWorldMap()
{
    int x=0, y=0;
    //initialize all the arrays before we start editing them
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            worldMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            outerMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            obstacleMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            obstaclePosMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            cloudMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            eggMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            turningMap[x][y] = 0;
        }
    }
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            fullMap[x][y] = 0;
        }
    }
    //initialize the distance as well.
    distance = 0;
}

//----------------------------------------------------------------------------------------------------------------------
int Maze::detectTurn(int x, int y)
{
    //around blocks all 0
    //even if one of the blocks around the current position
    //no turn
    if ( !( worldMap[x-1][y-1]
          ||worldMap[x-1][y-2]
          ||worldMap[x-1][y-3]
          ||worldMap[x+1][y-1]
          ||worldMap[x+1][y-2]
          ||worldMap[x+1][y-3]))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::shuffleObstacle(int x, int y)
{
    //i for shuffle the type
    //offset for make displacement of the obstacle
    int i=0, offset=0, obstacleOrTrap=0, a=0;
    i = rand()%6;
    //random number -1, 0, 1
    offset = rand()%2 - rand()%2;
    //random is the obstacle a trap or a block
    a = rand()%4;
    if(a==0)
    {
        obstacleOrTrap=2;
    }
    if(a==1)
    {
        obstacleOrTrap=0;
    }
    //horizontal
    if(a==2)
    {
        obstacleOrTrap=5;
    }
    //verticle
    if(a==3)
    {
        obstacleOrTrap=6;
    }
    //horizontal 3
    if (i==0)
    {
        if (obstacleOrTrap==6)
        {
            obstacleMap[x][y+offset]=5;
            obstacleMap[x-1][y+offset]=5;
            obstacleMap[x+1][y+offset]=5;
        }
        else
        {
            obstacleMap[x][y+offset]=obstacleOrTrap;
            obstacleMap[x-1][y+offset]=obstacleOrTrap;
            obstacleMap[x+1][y+offset]=obstacleOrTrap;
        }
    }
    //vertical 3
    if (i==1)
    {
        if (obstacleOrTrap==5)
        {
            obstacleMap[x+offset][y]=6;
            obstacleMap[x+offset][y-1]=6;
            obstacleMap[x+offset][y+1]=6;
        }
        else
        {
            obstacleMap[x+offset][y]=obstacleOrTrap;
            obstacleMap[x+offset][y-1]=obstacleOrTrap;
            obstacleMap[x+offset][y+1]=obstacleOrTrap;
        }
    }
    //double 3
    if (i==2)
    {
        obstacleMap[x+1][y]=obstacleOrTrap;
        obstacleMap[x+1][y-1]=obstacleOrTrap;
        obstacleMap[x+1][y+1]=obstacleOrTrap;

        obstacleMap[x-1][y]=obstacleOrTrap;
        obstacleMap[x-1][y-1]=obstacleOrTrap;
        obstacleMap[x-1][y+1]=obstacleOrTrap;
    }
    //U shape
    if (i==3)
    {
        obstacleMap[x][y-1]=obstacleOrTrap;
        obstacleMap[x-1][y-1]=obstacleOrTrap;
        obstacleMap[x+1][y-1]=obstacleOrTrap;
        obstacleMap[x-1][y]=obstacleOrTrap;
        obstacleMap[x+1][y]=obstacleOrTrap;
        obstacleMap[x-1][y+1]=obstacleOrTrap;
        obstacleMap[x+1][y+1]=obstacleOrTrap;
    }
    //cross shape
    if (i==4)
    {
        obstacleMap[x][y]=obstacleOrTrap;
        obstacleMap[x][y-1]=obstacleOrTrap;
        obstacleMap[x][y+1]=obstacleOrTrap;
        obstacleMap[x-1][y]=obstacleOrTrap;
        obstacleMap[x+1][y]=obstacleOrTrap;
    }
    //Z shape
    if (i==5)
    {
        if (obstacleOrTrap==5)
        {
            obstacleMap[x][y]=6;
            obstacleMap[x][y+1]=6;
            obstacleMap[x][y-1]=6;
            obstacleMap[x-1][y+1]=6;
            obstacleMap[x+1][y-1]=6;
        }
        else
        {
            obstacleMap[x][y]=obstacleOrTrap;
            obstacleMap[x][y+1]=obstacleOrTrap;
            obstacleMap[x][y-1]=obstacleOrTrap;
            obstacleMap[x-1][y+1]=obstacleOrTrap;
            obstacleMap[x+1][y-1]=obstacleOrTrap;
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::setObstacle()
{
    int x=0, y=0, count=0, decO=0;

    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            count += 1;
            //is on the road/ is the difficulty limit allows to set trap/ is not in a corner
            if (worldMap[x][y]&&!(count%6))
            {
                decO=rand()%(6-level);
                if (!decO)
                {
                    shuffleObstacle(x,y);
                    //on the obstacle pos map when you find a 1 it means
                    //an obstacle using that coords as a center pivot is existing
                    obstaclePosMap[x][y]=1;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::setCloud()
{
    int x=0, y=0, count=0, decC=0;

    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            count += 1;
            if (worldMap[x][y]&&!(count%6))
            {
                decC=rand()%5;
                if (!decC)
                {
                    cloudMap[x+1][y]=1;
                    cloudMap[x][y]=1;
                    cloudMap[x+1][y+1]=1;
                    cloudMap[x][y+1]=1;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::changeObstacleType()
{
    int x=0, y=0;
    int decide = 0;

    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            //only create the cannons after level 3
            decide=rand()%(10-level);
            if ((level>2)&&(obstacleMap[x][y]==2)&&(!decide))
            {
                obstacleMap[x][y]=4;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::setEgg()
{
    //decE is a random value to control the distance between each eggs
    //offset is moving the eggs away from the central line
    int x=0, y=0, count = 20, decE, offset;

    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            if (worldMap[x][y]&&(count>=0))
            {
                decE=rand()%8;
                if (!decE)
                {
                    offset = rand()%2 - rand()%2;
                    eggMap[x+offset][y+offset] = 1;
                    //each time you set an egg, reduce the total egg amount by 1
                    //so there won't be so many eggs on the map
                    count -= 1;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
//find 3s for detecting the goal zone
void Maze::setGoalZone()
{
    int x=0, y=0;
    for (y=58;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            if (fullMap[x][y])
            {
                obstacleMap[x][y] = 3;
            }
        }
    }
}

/*
 *  A B C
 *  D x E
 *  F G H
 * x[x][y]
 * A[x-1][y+1]
 * B[x][y+1]
 * C[x+1][y+1]
 * D[x-1][y]
 * E[x+1][y]
 * F[x-1][y-1]
 * G[x][y-1]
 * H[x+1][y-1]
 *
 * case1 EB fill FG
 * case2 BD fill GH
 * case3 DG fill BC
 * case4 GE fill AB
*/

//----------------------------------------------------------------------------------------------------------------------
void Maze::fillCorner()
{
    int x=0, y=0;
    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            if (turningMap[x][y])
            {
                obstacleMap[x][y]=1;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::genWorldMap()
{
    int y=0;
    //dtm = distance to next level's middle
    int tmpX=0, tmpY=0;
    //decide Turn, decide distance to turn
    //decide left = 1, right = 0
    int decT=0, decLR=0, decD=0, i=0, t=0;

    srand((unsigned int)time(NULL));

    worldMap[24][0]= 1;
    tmpX = 24;


    for (y=0; y<60; y++)
    {
        tmpY=y;

        worldMap[tmpX][tmpY]=1;
        turningMap[tmpX][tmpY]=1;
        outerMap[tmpX][tmpY]=0;
        outerMap[tmpX-1][tmpY]=2;
        outerMap[tmpX+1][tmpY]=2;

        //limit the range of the road width
        //avoid sharp turn, only decide to turn from each point of mult of 3
        if (tmpY<5||tmpY>=55)
        {
            t=1;
        }
        else
        {
            t=detectTurn(tmpX, tmpY);
        }

        decT=rand()%(6-level);

        if (!(decT||t))
        {
            //std::cout<<"turn"<<std::endl;
            //after turn decided, check if the road is already on the boarder line
            //if tmpx touches the left boundary, set the next turn a right turn
            if (tmpX<8)
            {
                decLR = 0;
            }
            //if tmpx touches the right boundary, set the next turn a left turn
            else if (tmpX>41)
            {
                decLR = 1;
            }
            //if tmpx doesn't touch any boundary, random the next turn direction
            else
            {
                decLR=rand()%2;
            }

            if (decLR)
            {
                //a left turn
                //a left turn is 1
                //a right turn is 2
                //turningMap[tmpX][tmpY]=2;

                //left before BC
                outerMap[tmpX][tmpY+1]=2;
                outerMap[tmpX+1][tmpY+1]=2;

                do
                {
                    decD = rand()%9 + 2;
                //when condition is false get result
                }while((tmpX-decD)<2);
                //left
                for (i=0; i<decD; i++)
                {
                    tmpX -= 1;
                    worldMap[tmpX][tmpY]=1;
                    turningMap[tmpX][tmpY]=2;
                    outerMap[tmpX][tmpY]=0;
                    outerMap[tmpX][tmpY+1]=2;
                    outerMap[tmpX][tmpY-1]=2;
                }
                //left after GFH
                outerMap[tmpX-1][tmpY-1]=2;
                outerMap[tmpX][tmpY-1]=2;
                outerMap[tmpX-1][tmpY]=2;
                //turningMap[tmpX][tmpY]=1;
            }

            if (!decLR)
            {
                //a right turn
                //turningMap[tmpX][tmpY]=1;

                //right before AB
                outerMap[tmpX-1][tmpY+1]=2;
                outerMap[tmpX][tmpY+1]=2;
                do
                {
                    decD = rand()%9 + 2;
                //while condition is true return to code block
                }while((tmpX+decD)>47);
                //right
                //fill the hole

                for (i=0; i<decD; i++)
                {
                    tmpX += 1;
                    worldMap[tmpX][tmpY]=1;
                    turningMap[tmpX][tmpY]=2;
                    outerMap[tmpX][tmpY]=0;
                    outerMap[tmpX][tmpY+1]=2;
                    outerMap[tmpX][tmpY-1]=2;
                }
                //right after FED
                outerMap[tmpX][tmpY-1]=2;
                outerMap[tmpX+1][tmpY-1]=2;
                outerMap[tmpX+1][tmpY]=2;
                turningMap[tmpX][tmpY]=1;
            }

        }

    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::genFullMap()
{
    int x=0, y=0;
    for (x=0;x<50;x++)
    {
        for (y=0;y<60;y++)
        {
            if (worldMap[x][y]||outerMap[x][y])
            {
                obstacleMap[x][y]=1;
                fullMap[x][y]=1;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::createMaze()
{
    genWorldMap();
    calculateDistance();
    genFullMap();
    setObstacle();
    setGoalZone();
    //fillCorner();
    changeObstacleType();
    setEgg();
    setCloud();
}

//----------------------------------------------------------------------------------------------------------------------
void Maze::calculateDistance()
{
    int x=0, y=0;
    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            if (worldMap[x][y])
            {
                distance+=1;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

#if 0
void Maze::print()
{
    int x=0, y=0;

    for (y=0;y<60;y++)
    {
        for (x=0;x<50;x++)
        {
            std::cout<<worldMap[x][y];
        }
        std::cout<<std::endl;
    }

}
#endif
