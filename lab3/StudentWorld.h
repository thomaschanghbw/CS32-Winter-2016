#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

class Actor;
class FrackMan;
class Dirt;
int randInt(int min, int max);
double distance(int x1, int y1, int x2, int y2);
/**************************************
 Global variables
 **************************************/
const int NOACTORHERE = 0;
const int ACTORHERECANPASS = 1;
const int ACTORHERECANTPASS = 2;

/**************************************
 Declarations
 **************************************/
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
        for(int q = 0; q<64; q++)
            for (int w = 0; w<64; w++)
            {
                m_dirt[q][w] = nullptr;
            }
	}

    virtual int init();
    virtual int move();
    virtual ~StudentWorld();
    virtual void cleanUp();
    bool removeDirt(int x, int y);
    bool isDirt(int x, int y) const;
    int isActorHereAndCanPassThrough(Actor* me, int x, int y) const;
    void changeVector(int xi, int yi, int xf, int yf);
    // Annoy all other actors within radius of annoyer, returning the
    // number of actors annoyed.
    int annoyAllNearbyActors(Actor* annoyer, int points, int radius);
    // Return whether the Actor a is within radius of x, y.
    bool isNear(int x, int y, Actor* a, double radius) const;
    FrackMan* gimmeFrackMan();
    //Subtract from our oil counter
    void iPickedUpAPrince();
    //Determine what goodie, if any, should be placed
    void addGoodieIfPossible();
    //Get min or max
    int minOrMax(const int& a,const int& b, bool min);
    bool isValidPosition(int x, int y) const;
    void addActor(Actor* a);
    // Reveal all objects within radius of x,y.
    void revealAllNearbyObjects(int x, int y, int radius);
    vector<Actor*>& getActors();
    void protesterTick();

private:
    Dirt* m_dirt[64][64];
    FrackMan* m_FrackMan;
    void fill_objects(int b, int g, int l);
    void placeObjects(vector<int>& x_taken, vector<int>& y_taken, string objectToPut, int& howMany, int& temp_x, int& temp_y);
    vector<Actor*> m_actors;
    int m_barrelsCounter;
    int maxProtesters;
    int m_protesterTick = 0;
};


#endif // STUDENTWORLD_H_
