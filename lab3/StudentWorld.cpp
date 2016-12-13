#include "StudentWorld.h"
#include <string>
#include <random>
#include "Actor.h"
#include <cmath>

//Functions
int randInt(int min, int max);
int minOrMax(const int& a,const int& b, bool min);
bool notGreaterThanSix(int x1, int y1, int x2, int y2);
double distance(int x1, int y1, int x2, int y2);



GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


StudentWorld::~StudentWorld()
{
    for(int q = 0; q<64; q++)
        for (int w = 0; w<60; w++)
        {
            delete m_dirt[q][w];
            m_dirt[q][w] = nullptr;
        }
    delete m_FrackMan;
    m_FrackMan = nullptr;
}
void StudentWorld:: cleanUp()
{
    for(int q = 0; q<64; q++)
        for (int w = 0; w<60; w++)
        {
            delete m_dirt[q][w];
            m_dirt[q][w] = nullptr;
        }
    for(int i =0; i<m_actors.size(); i++)
    {
        delete m_actors[i];
        m_actors[i] = nullptr;
    }
    delete m_FrackMan;
    m_FrackMan = nullptr;
    m_protesterTick = 0;
}
//Returns true is we deleted some dirty dirt
bool StudentWorld::removeDirt(int x, int y)
{
    bool deletedSomeDirt = false;
    for (int delx = 0; delx < 4; delx ++)
        for (int dely = 0; dely <4; dely++)
        {
            if (x+delx < 64 && y+dely < 64 && x>=0 && y>=0)
            {
                if(m_dirt[x+delx][y+dely] == nullptr) {
                    delete m_dirt[x+delx][y+dely];
                    m_dirt[x+delx][y+dely] = nullptr;
                }
                else {
                    delete m_dirt[x+delx][y+dely];
                    m_dirt[x+delx][y+dely] = nullptr;
                    deletedSomeDirt = true;
                }
            }
            else break;
        }
    return deletedSomeDirt;;
    }
// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
int StudentWorld::init()
{
    {
        for (int col = 0; col< 64; col++)
            for (int row = 0; row < 60; row++)
            {
                m_dirt[col][row] = new Dirt(this, col, row);
            }
        for (int col = 30; col< 34; col++)
            for (int row = 4; row < 60; row++)
            {
                delete m_dirt[col][row];
                m_dirt[col][row] = nullptr;
            }
        m_FrackMan = new FrackMan(this);
        maxProtesters = minOrMax(15, 2+ getLevel()*1.5, true);
        
        //Fill the field with other junk, b is boulder, g is gold, l is oiL
        unsigned int current_level = getLevel();
        int b = minOrMax( (current_level / 2 +2), 6, true);
        int g = minOrMax( 5 - (current_level / 2), 2, false);
        int l = m_barrelsCounter = minOrMax(2+current_level , 20, true);
        fill_objects(b, g, l);
        return GWSTATUS_CONTINUE_GAME;
    }
}
//
void StudentWorld::protesterTick()
{
    if(m_protesterTick>0)
        m_protesterTick--;
    else if (m_protesterTick==0)
        m_protesterTick = minOrMax(25, 200-getLevel(), false);
}
int StudentWorld::move()
    {
        // This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
        // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
        setGameStatText("Scr: " + to_string(getScore()) + " Lvl: "+  to_string(getLevel()) + " Lives: " + to_string(getLives()) +  " Hlth: " + to_string(m_FrackMan->getHitPoints()*10) + "% Wtr: " + to_string(m_FrackMan->getWater()) + " Gld: " + to_string(m_FrackMan->getGold()) + " Sonar: " + to_string(m_FrackMan->getSonar()) + " Oil Left: " + to_string(m_barrelsCounter));
        if (m_protesterTick ==0 && maxProtesters >0)
        {
            protesterTick();
            maxProtesters--;
            if(randInt(1, minOrMax(90, getLevel()*10 +30, true)) == 1)
                m_actors.push_back(new HardcoreProtester(this, 60, 60));
            else {m_actors.push_back(new RegularProtester(this, 60, 60));
            }
        }
        //Make everything do something
        for(int i =0; i<m_actors.size(); i++)
            {
                if(m_actors[i] != nullptr)
                    m_actors[i]->doSomething();
            }
        if(m_barrelsCounter<=0)
            return GWSTATUS_FINISHED_LEVEL;
        if(m_FrackMan->getHitPoints() <=0 || !m_FrackMan->isAlive()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
        addGoodieIfPossible();
        m_FrackMan->doSomething();
        //Checks to see what died
        for(int i =0; i<m_actors.size(); i++)
            {
                if(m_actors[i] != nullptr)
                    if(!m_actors[i]->isAlive())
                    {
                        delete m_actors[i];
                        m_actors[i] = nullptr;
                    }
            }
        return GWSTATUS_CONTINUE_GAME;
    }
//
bool StudentWorld::isDirt(int x, int y) const
{
    if (m_dirt[x][y] != nullptr)
        return true;
    return false;
}
//
void StudentWorld::fill_objects(int b, int g, int l)
{
    vector<int> x_taken;
    vector<int> y_taken;
    //Place first object in the game, which can be in any valid position
    int temp_x = randInt(0, 60);
    int temp_y = randInt(0, 56);
    while (temp_x >=27 && temp_x <= 33 && temp_y >=1 && temp_y<=59)
    {
         temp_x = randInt(0, 60);
         temp_y = randInt(0, 56);
    }
    b--;
    removeDirt(temp_x, temp_y);
    m_actors.push_back(new Boulder(this, temp_x, temp_y)) ;
    x_taken.push_back(temp_x);
    y_taken.push_back(temp_y);
    placeObjects(x_taken, y_taken, "Boulder", b, temp_x, temp_y);
    placeObjects(x_taken, y_taken, "Gold Nugget", g, temp_x, temp_y);
    placeObjects(x_taken, y_taken, "Oil Barrel", l, temp_x, temp_y);
    }
//
void StudentWorld::placeObjects(vector<int>& x_taken, vector<int>& y_taken, string objectToPut, int& howMany, int& temp_x, int& temp_y)
{
    bool validSpot = true;
    for(; howMany!= 0; howMany--)
    {
        do {
            validSpot = true;
            temp_x = randInt(0, 60);
            if (objectToPut == "Boulder")
                temp_y = randInt(20, 56);
            else {
                temp_y = randInt(0, 56);
            }
            for(int i=0; i<x_taken.size(); i++)
                {
                    if (distance(x_taken[i], y_taken[i], temp_x, temp_y)<=6)
                    {
                        validSpot = false;
                        break;
                    }
                    //Cant be in the mine shaft
                    if (temp_x >=27 && temp_x <= 33)
                            if (temp_y >=1 && temp_y<=59)
                            {
                                validSpot = false;
                                break;
                            }
                    
                }
        } while (!validSpot);
        //needs change

        if (objectToPut == "Boulder")
        {
                removeDirt(temp_x, temp_y);
                m_actors.push_back(new Boulder(this, temp_x, temp_y)) ;
        }
        else if (objectToPut == "Gold Nugget")
        {
            m_actors.push_back(new GoldNugget(this, temp_x, temp_y, false));
        }
        else if (objectToPut == "Oil Barrel")
        {
            m_actors.push_back(new OilBarrel(this, temp_x, temp_y));
        }
        x_taken.push_back(temp_x);
        y_taken.push_back(temp_y);
    }
}
//
int StudentWorld::isActorHereAndCanPassThrough(Actor* me, int x, int y) const
{
    bool actorAndPassable = false;
    if (x<0 || x>64 || y<0 || y>64)
        return NOACTORHERE;
    for (int q = 0; q<m_actors.size(); q++) {
    if (m_actors[q] != nullptr) {
        if(m_actors[q]->getX() == x && m_actors[q]->getY() == y) {
        if (m_actors[q]->canActorsPassThroughMe())
            actorAndPassable = true;
        else
        {
            if (m_actors[q] != me)
            return ACTORHERECANTPASS;
            }
    }
    }
    }
    if(actorAndPassable)
        return ACTORHERECANPASS;
    return NOACTORHERE;
}
//
//void StudentWorld::changeVector(int xi, int yi, int xf, int yf)
//{
//    m_actors[xf][yf] = m_actors[xi][yi];
//    m_actors[xi][yi] = nullptr;
//}
//
int StudentWorld::annoyAllNearbyActors(Actor* annoyer, int points, int radius)
{
    int num_annoyed = 0;
    if(distance(m_FrackMan->getX(), m_FrackMan->getY(), annoyer->getX(), annoyer->getY()) <=3) {
        m_FrackMan->annoy(points);
        num_annoyed++;
    }
    for(int i =0; i<m_actors.size(); i++)
        {
            if(m_actors[i] != nullptr)
                if(distance(m_actors[i]->getX(), m_actors[i]->getY(), annoyer->getX(), annoyer->getY()) <=3) {
                      if(m_actors[i]->annoy(points))
                    num_annoyed++;
                }
        }
    return num_annoyed;
}
//
bool StudentWorld::isNear(int x, int y, Actor* a, double radius) const
{
    if (distance(x, y, a->getX(), a->getY()) <=radius)
        return true;
    return false;
}
//
FrackMan* StudentWorld::gimmeFrackMan()
{
    return m_FrackMan;
}
//
void StudentWorld::iPickedUpAPrince()
{
    m_barrelsCounter--;
}
void StudentWorld::addGoodieIfPossible()
{
    int G = getLevel()*25 +300;
    G = randInt(1, G);
    if(G==1)
    {
        G = randInt(1, 5);
        if(G==1)
        {
            m_actors.push_back(new SonarKit(this, 0, 60));
        }
        else {
            int temp_x;
            int temp_y;
            bool goodSpot = true;
            do {
                goodSpot = true;
                temp_x = randInt(0, 60);
                temp_y = randInt(0, 60);
                for(int j = 0; j<4; j++)
                for(int i = 0; i<4; i++)
                {
                    if(isDirt(temp_x+i, temp_y +j)){
                        goodSpot = false;
                    break;
                    }
                }
            }
            while (!goodSpot);
            m_actors.push_back(new WaterPool(this, temp_x, temp_y));
        }
    }
}
//Checks validity for random initial placement of objects
bool StudentWorld::isValidPosition(int x, int y) const
{
    for (int i=0; i<m_actors.size(); i++)
    {
        if(m_actors[i] != nullptr) {
        if(distance(x, y, m_actors[i]->getX(), m_actors[i]->getY()) <=6)
            return false;
        }
    }
    return true;
}
void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}
//
// Reveal all objects within radius of x,y from FrackMan
void StudentWorld::revealAllNearbyObjects(int x, int y, int radius)
{
    for (int i =0; i<m_actors.size(); i++)
    {
        if(m_actors[i] != nullptr) {
        if(isNear(x, y, m_actors[i], 12))
            m_actors[i]->setVisible(true);
        }
    }
}
vector<Actor*>& StudentWorld::getActors()
{
    return m_actors;
}
/**************************************
 Functions used in member functions
 **************************************/
//Random int generator from Project 1
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}
//Function to return the minimum or max of two numbers
int StudentWorld::minOrMax(const int& a,const int& b, bool min)
{
    bool a_greater = a >= b;
    if(min) {
        if (a_greater)
            return b;
        else return a;
    }
    if(!min) {
        if(a_greater)
            return a;
        else return b;
    }
    return a;
}
//This function  see  the distance
double distance(int x1, int y1, int x2, int y2)
{
    int effective_x = x2-x1;
    int effective_y = y2-y1;
    if (effective_x<0)
        effective_x *= -1;
    if (effective_y<0)
        effective_y *= -1;
    return (sqrt( pow( static_cast<double>(effective_x), 2) + pow(effective_y, 2) ));
}