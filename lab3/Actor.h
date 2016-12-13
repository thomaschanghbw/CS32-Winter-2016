#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include <queue>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
    Actor(StudentWorld* world, int imageID, int startX, int startY, Direction startDir,
          double size, unsigned int depth, bool visible);
    ~Actor();
    virtual void doSomething();

    // Get this actor's world
    StudentWorld* getWorld() const;
    
     //Is this actor alive?
    bool isAlive() const;

    // Mark this actor as dead.
    void setDead();
    // Can other actors pass through this actor?
    virtual bool canActorsPassThroughMe() const;
    // Annoy this actor.
    virtual bool annoy(unsigned int amt);
    // Can this actor dig through dirt?
    virtual bool canDigThroughDirt() const;
    //Checks if there is any dirt either above, below, left or right
    bool anyDirtInFourSpaces(int x, int y, Direction dir) const;
    // Can this actor pick items up?
    virtual bool canPickThingsUp() const;
    virtual void addGold();
            /********

    

    
    // Does this actor hunt the FrackMan?
    virtual bool huntsFrackMan() const;
    
    // Can this actor need to be picked up to finish the level?
    virtual bool needsToBePickedUpToFinishLevel() const;
         ********/
    // Move this actor to x,y if possible, and return true; otherwise,
    // return false without moving.
    bool moveToIfPossible(bool standardVector, Actor* me, int x, int y);

private:
    StudentWorld* m_world;
    bool m_alive;
};

//Dirt
class Dirt: public Actor
{
public:
    Dirt(StudentWorld* world, int startX, int startY);
    virtual ~Dirt();
    //virtual void move();
private:
  
};
//Agent : base class for FrackMan and protestors
class Agent : public Actor
{
public:
    virtual ~Agent();
    Agent(StudentWorld* world,int imageID, int startX, int startY, Direction Dir, double size, unsigned int depth, unsigned int hp);
    virtual bool annoy(unsigned int amount);
    // How many hit points does this actor have left?
     int getHitPoints() const;

    // Pick up a gold nugget.
    virtual void addGold();
        /*
    virtual bool canPickThingsUp() const;
     */
private:
    int m_hp;
};
class Protester : public Agent
{
public:
    Protester(StudentWorld* world, int startX, int startY, int imageID,
              unsigned int hitPoints, bool hardcore);
    virtual void doSomething();
    bool inFrackMansDirection(Direction dir);
    virtual void addGold();
    virtual bool annoy(unsigned int amount);
    virtual bool canPickThingsUp() const;
    //    // Set state to having gien up protest
        void setMustLeaveOilField();
    bool hardcoreMove();
    
//    virtual bool huntsFrackMan() const;
//    

//    // Set number of ticks until next move, return true if we can move
    bool setTicksToNextMove();
//Returns how many ticks until can shout again
    int ticksUntilShout();
    void setTicksToNextShout();
    void setNumSquaresSameDirection(int x);
    int getNumSquaresSameDirection();
    int getTicksSincePerpendicular() const;
    void resetTicksSincePerpendicular();
    void computeMazeToFMorExit(char key, int startX, int startY);
    void followMaze(char key);
    bool hasMaze();
    bool leavingOilField() const;
    void setTicksPerMove(int x);
    void setRestState(int x);
    int getRestState() const;
private:
    bool m_leaveOilField;
    int m_restState;
     int m_ticksPerMove;
    int m_ticksUntilShout;
    int m_gold;
    int numSquaresInCurrentDirection;
    int m_ticksSincePerpendicular ;
    struct m_mazeSolve {
        char c;
        m_mazeSolve* prev;
        m_mazeSolve();
        m_mazeSolve(char cc, m_mazeSolve* pprev);
        int xCoord;
        int yCoord;

    };
    m_mazeSolve m_maze[64][64];
    queue<m_mazeSolve*> q_maze;
    bool m_hasMaze;
            bool m_hardcore;
};

class RegularProtester : public Protester
{
public:
    RegularProtester(StudentWorld* world, int startX, int startY);

    virtual void addGold();

};

class HardcoreProtester : public Protester
{
public:
    HardcoreProtester(StudentWorld* world, int startX, int startY);

    virtual void addGold();
    virtual bool annoy(unsigned int amount);
};
//FrackMan
class FrackMan: public Agent
{
public:
    FrackMan(StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    virtual void addGold();
    // Get amount of gold
    unsigned int getGold() const;
    // Pick up a sonar kit.
    void addSonar();
    // Get amount of sonar charges
    unsigned int getSonar() const;
    void removeSonar();
    // Pick up water.
    void addWater();
    // Get amount of water
    unsigned int getWater() const;
    virtual bool canDigThroughDirt() const;
    void squirtWater();
   // virtual bool annoy(unsigned int amount);
    /*

     
    




     */
private:
    int m_gold;
    int m_sonar;
    int m_water;
};
/*******************
 Boulder
 *******************/
class Boulder : public Actor
{ 
public:
    Boulder(StudentWorld* world, int startX, int startY);
    virtual ~Boulder();
    virtual void doSomething();
    //virtual void move();
    virtual bool canActorsPassThroughMe() const;
private:
    //1 stable, 2 waiting, 3 falling
    int m_state;
    int m_ticks;
};
/*******************
 Squirt
 *******************/
class Squirt : public Actor
{
public:
    Squirt(StudentWorld* world, int startX, int startY, Direction startDir);
    virtual void doSomething();
private:
    int m_ticksToLive;
};
/*******************
 Activating Object
 *******************/
class ActivatingObject : public Actor
{
public:
    ActivatingObject(StudentWorld* world, int startX, int startY, int imageID, Direction dir, double size,  int depth, bool visible);
    virtual void doSomething();
    
    // Set number of ticks until this object dies
    void setTicksToLive(int ticks);
    //Decrement life ticks
    int getTicksToLive() const;
    bool isTempState() const;
    void makeTempState();
private:
        int m_lifeTicks;
    bool tempState;
};
/*******************
 Gold Nugget
 *******************/
class GoldNugget : public ActivatingObject
{
public:
    GoldNugget(StudentWorld* world, int startX, int startY, bool temp);
    virtual void doSomething();
};
/*******************
 Oil Barrel
 *******************/
class OilBarrel : public ActivatingObject
{
public:
    OilBarrel(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
    //virtual bool needsToBePickedUpToFinishLevel() const;
};
/*******************
 SonarKit
 *******************/
class SonarKit : public ActivatingObject
{
public:
    SonarKit(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
private:
};
/*******************
 Water
 *******************/
class WaterPool : public ActivatingObject
{
public:
    WaterPool(StudentWorld* world, int startX, int startY);
    virtual void doSomething();
};


#endif // ACTOR_H_
