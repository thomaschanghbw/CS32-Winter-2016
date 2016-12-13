#include "Actor.h"
#include <random>


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
//Need to determine visiblity still



/******************************
Actor Implementations
 ******************************/
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned  int depth, bool visible) : GraphObject( imageID,  startX,  startY,  dir ,  size ,  depth )
{
    setVisible(visible);
    m_world = world;
    m_alive = true;
}
Actor::~Actor() {}
void Actor::setDead() {m_alive = false;}
bool Actor::isAlive() const {return m_alive;}
bool Actor::annoy(unsigned int amount) {return false;}
void Actor::addGold() {return;}
void Actor::doSomething()
{
    return;
}

bool Actor::canPickThingsUp() const
{return false;}
StudentWorld* Actor::getWorld() const
{
    return m_world;
}
bool Actor::moveToIfPossible(bool standardVector, Actor* me, int x, int y)
{
    for (int xx = -3; xx < 4; xx++)
        for(int yy = -3; yy <4; yy++)
        {
            int checkForActor = getWorld()->isActorHereAndCanPassThrough(me, x+xx, y+yy);
            if (checkForActor == ACTORHERECANTPASS)
                return false;
        }
    if (x <= 60 && x >= 0 && y<= 60 && y>=0) {
//        if(standardVector)
//        {
//            getWorld()->changeVector(getX(), getY(), x, y);
//        }
        if(!canDigThroughDirt())
        {
            if( anyDirtInFourSpaces(x, y, getDirection()))
                return false;
        }
        moveTo(x, y);

        return true;
    }
    return false;
}
bool Actor::canActorsPassThroughMe() const
{
    return true;
}

// Can this actor dig through dirt?
bool Actor::canDigThroughDirt() const
{
    return false;
}
bool Actor::anyDirtInFourSpaces(int x, int y, Direction dir) const
{
    switch(getDirection())
    {
        case down:
        case up:
            for(int ii=0; ii<4; ii++)
            {
                if(getWorld()->isDirt(x+ii, y))
                    return true;
            }
            break;
        case left:
            for(int ii=0; ii<4; ii++)
            {
                if(getWorld()->isDirt(x, y+ii))
                    return true;
            }
            break;
        case right:
            for(int ii=0; ii<4; ii++)
            {
                if(getWorld()->isDirt(x+3, y+ii))
                    return true;
            }
            break;
        default:
            break;
    }
    return false;
}
/******************************
 Dirt Implementations
 ******************************/
Dirt::Dirt(StudentWorld* world, int startX, int startY) : Actor(world, IID_DIRT,  startX,  startY,  right ,  .25 ,  3, true) {}
Dirt::~Dirt() {}
/******************************
 Agent Implementations
 ******************************/
Agent::Agent(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth, unsigned int hp) : Actor(world, imageID,  startX,  startY,  dir ,  size,  depth, true)
{
    m_hp = hp;
}
Agent::~Agent() {}
bool Agent::annoy(unsigned int amount)
{
    m_hp -= amount;
    return true;
}
 int Agent::getHitPoints() const
{
    return m_hp;
}
void Agent::addGold()
{return;}
/******************************
 Protester Implementations
 ******************************/
Protester::Protester(StudentWorld* world, int startX, int startY, int imageID,
                     unsigned int hitPoints, bool hardcore) : Agent(world, imageID, startX, startY, left, 1, 0, hitPoints), m_leaveOilField(false), m_restState(0), m_ticksUntilShout(0)  {
    m_ticksPerMove = getWorld()->minOrMax(0, 3- (getWorld()->getLevel() /4), false);
    m_gold = 0;
    numSquaresInCurrentDirection = randInt(8, 60);
    m_ticksSincePerpendicular = 0;
    m_hasMaze=false;
    m_hardcore = hardcore;
}
int Protester::getRestState() const
{
    return m_restState;
}
Protester::m_mazeSolve::m_mazeSolve()
{
    c = ' ';
    prev = nullptr;
}
Protester::m_mazeSolve::m_mazeSolve(char cc, m_mazeSolve* pprev)
{
    c = cc;
    prev = pprev;
}
//Computes a maze to either FrackMan (F) or the Exit (E)
void Protester::computeMazeToFMorExit(char key, int startX, int startY)
{
    for(int w=0; w<64; w++)
        for(int t=0; t<64; t++)
        {
            m_maze[w][t].xCoord = w;
            m_maze[w][t].yCoord = t;
            if(getWorld()->isDirt(w, t))
            {
                m_maze[w][t].c = 'x';
            }
        }
    for(int w=0; w<getWorld()->getActors().size(); w++)
    {
        if(getWorld()->getActors()[w]!=nullptr) {
        if(!getWorld()->getActors()[w]->canActorsPassThroughMe()) {
            int my_x = getWorld()->getActors()[w]->getX();
            int my_y = getWorld()->getActors()[w]->getY();
            for(int k=0; k<4; k++)
                for(int y=0; y<4; y++)
            {
                m_maze[my_x +k][my_y+y].c = 'x';
            }
        }
        }
    }
    switch(key)
    {
        case 'E':
            q_maze.push(&m_maze[60][60]);
            break;
        case 'F':
            q_maze.push(&m_maze[getWorld()->gimmeFrackMan()->getX()][getWorld()->gimmeFrackMan()->getY()]);
            break;
    }
    while (!q_maze.empty())
    {
    //left

    int tempX = q_maze.front()->xCoord;
    int tempY =     q_maze.front()->yCoord;
        if(tempX == startX && tempY == startY) {
            while(!q_maze.empty())
                q_maze.pop();
            break;
        }
                if(tempX == startX && tempY == startY)
                    break;
    bool emptySpot = true;
    for(int w=0; w<4; w++)
    {
        if(tempX-1 <0 || tempY+w >64)
            emptySpot=false;
     if(m_maze[tempX-1][tempY+w].c == 'x' )
        emptySpot = false;
    }
        if (m_maze[tempX-1][tempY].c == 's')
            emptySpot = false;
    if(emptySpot)
    {
        m_maze[tempX-1][ tempY].c = 's';
        m_maze[tempX-1][tempY].prev =  q_maze.front();
        q_maze.push(&m_maze[tempX-1][ tempY]);
    }
    //down
        emptySpot = true;
        for(int w=0; w<4; w++)
        {
            if(tempX+w >64 || tempY-1 <0)
                emptySpot=false;
            if(m_maze[tempX+w][ tempY-1].c == 'x' )
                emptySpot = false;
        }
        if(m_maze[tempX][tempY-1].c == 's')
            emptySpot = false;
        if(emptySpot)
        {
            m_maze[tempX][ tempY-1].c = 's';
            m_maze[tempX][tempY-1].prev =     q_maze.front();
            q_maze.push(&m_maze[tempX][ tempY-1]);
        }
    //right
        emptySpot = true;
        for(int w=0; w<4; w++)
        {
            if(tempX+4 >64 || tempY+w >64)
                emptySpot=false;
            if(m_maze[tempX+4][ tempY+w].c == 'x' )
                emptySpot = false;
        }
        if(m_maze[tempX+4][tempY].c == 's')
            emptySpot = false;
        if(emptySpot)
        {
            m_maze[tempX+1][ tempY].c = 's';
            m_maze[tempX+1][tempY].prev =     q_maze.front();
            q_maze.push(&m_maze[tempX+1][ tempY]);
        }
    //up
        emptySpot = true;
        for(int w=0; w<4; w++)
        {
            if(tempX+w >64 || tempY+4 >64)
                emptySpot=false;
            if(m_maze[tempX+w][ tempY+4].c == 'x'  )
                emptySpot = false;
        }
        if(m_maze[tempX][tempY+4].c == 's')
            emptySpot = false;
        if(emptySpot)
        {
            m_maze[tempX][ tempY+1].c = 's';
            m_maze[tempX][tempY+1].prev =     q_maze.front();
            q_maze.push(&m_maze[tempX][ tempY+1]);
        }
         q_maze.pop();
    }
    
   // m_hasMaze = true;
}
bool Protester::hardcoreMove()
{
    if (!m_hardcore)
        return false;
    int M = 16 + getWorld()->getLevel()*2;
    computeMazeToFMorExit('F', getX(), getY());
    int counter = 0;
    int tempX =getX();
    int tempY = getY();
    while (m_maze[tempX][ tempY].prev !=nullptr) {
        tempX = m_maze[tempX][ tempY].prev->xCoord;
        tempY = m_maze[tempX][ tempY].prev->yCoord;
        counter ++;
    }
    
    if(counter <= 16) {
        followMaze('F');
        return true;
    }
    return false;
}
bool Protester::hasMaze()
{
    return m_hasMaze;
}
bool Protester::canPickThingsUp() const {return true;}
void Protester::doSomething() {
    int yFM = getWorld()->gimmeFrackMan()->getY();
    int xFM = getWorld()->gimmeFrackMan()->getX();
    //1
    if (!isAlive())
        return;
    if(!setTicksToNextMove())
        return;
    //2
    //3
    if(leavingOilField())
    {
        if(getX()==60 && getY() == 60)
            setDead();
        followMaze('E');
        return;
    }
    if(getWorld()->isNear(getX(), getY(), getWorld()->gimmeFrackMan(), 4.0))
    {
        if(inFrackMansDirection(getDirection()))
        {
            if(ticksUntilShout() == 0)
            {
                getWorld()->playSound(SOUND_PROTESTER_YELL);
                setTicksToNextShout();
                getWorld()->gimmeFrackMan()->annoy(2);
                return;
            }
        }
    }
    if (hardcoreMove())
        return;
    //4
    if(distance(getX(), getY(), getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY())==4)
        if(inFrackMansDirection(getDirection()))
            return;
    if(getX()== xFM && !getWorld()->isNear(getX(), getY(), getWorld()->gimmeFrackMan(), 4))
    {
        bool clearLine = true;
        int lower, upper;
        if(getY() -yFM>0)
        {
            lower = yFM;
            upper = getY();
        }
        else {
            lower = getY();
            upper = yFM;
        }
        for (int i = lower; i<=upper; i++)
            for(int j = getX(); j<getX() + 4; j++)
            {
                if(getWorld()->isDirt(j, i)){
                    clearLine = false;
                    break;
                }
                for(int k =0; k<getWorld()->getActors().size(); k++)
                    if(getWorld()->getActors()[k] != nullptr)
                        if(getWorld()->getActors()[k]->getX() == j && getWorld()->getActors()[k]->getY() == i)
                            if(!getWorld()->getActors()[k]->canActorsPassThroughMe()) {
                                clearLine=false;
                                break;
                            }
            }
        if(clearLine)
        {
            if(lower == yFM)
            {
                setDirection(down);
                moveToIfPossible(true, this, getX(), getY()-1);
                setNumSquaresSameDirection(0);
                return;
            }
            if(lower==getY() && getY() != getWorld()->gimmeFrackMan()->getY())
            {
                setDirection(up);
                moveToIfPossible(true, this, getX(), getY()+1);
                setNumSquaresSameDirection(0);
                return;
            }
        }
    }
    
    //5
    if (getY()== yFM && !getWorld()->isNear(getX(), getY(), getWorld()->gimmeFrackMan(), 4))
    {
        bool clearLine = true;
        int lower, upper;
        if(getX() -xFM>0)
        {
            lower = xFM;
            upper = getX();
        }
        else {
            lower = getX();
            upper = xFM;
        }
        for (int i = lower; i<=upper; i++)
            for(int j = getY(); j<getY() + 4; j++)
            {
                if(getWorld()->isDirt(i, j)){
                    clearLine = false;
                    break;
                }
                for(int k =0; k<getWorld()->getActors().size(); k++)
                    if(getWorld()->getActors()[k] != nullptr)
                        if(getWorld()->getActors()[k]->getX() == i && getWorld()->getActors()[k]->getY() == j)
                            if(!getWorld()->getActors()[k]->canActorsPassThroughMe()) {
                                clearLine=false;
                                break;
                            }
            }
        if(clearLine)
        {
            if(lower == xFM)
            {
                setDirection(left);
                moveToIfPossible(true, this, getX()-1, getY());
                setNumSquaresSameDirection(0);
                return;
            }
            if(lower==getX() && getX() != getWorld()->gimmeFrackMan()->getX())
            {
                setDirection(right);
                moveToIfPossible(true, this, getX()+1, getY());
                setNumSquaresSameDirection(0);
                return;
            }
        }
    }
    //6
    setNumSquaresSameDirection(getNumSquaresSameDirection()-1);
    if (getNumSquaresSameDirection()<=0)
    {
        bool canMove = true;
        Direction d;
        do {
            canMove = true;
            int newDirection=randInt(1, 4);
            //1 up, 2 down, 3 left, 4 right
            switch(newDirection)
            {
                case 1:
                    d = up;
                    if(getDirection()==d)
                        canMove=false;
                    for(int ii=0; ii<4; ii++) {
                        if(getWorld()->isDirt(getX()+ii, getY()+4))
                            canMove = false;
                        if(getX()+ii>64 || getY()+4>64)
                            canMove = false;
                    }
                    break;
                case 2:
                    d=down;
                    if(getDirection()==d)
                        canMove=false;
                    for(int ii=0; ii<4; ii++) {
                        if(getWorld()->isDirt(getX()+ii, getY()-1))
                            canMove = false;
                        if(getX()+ii>64 || getY()-1<0)
                            canMove = false;
                    }
                    break;
                case 3:
                    d=left;
                    if(getDirection()==d)
                        canMove=false;
                    for(int ii=0; ii<4; ii++) {
                        if(getWorld()->isDirt(getX()-1, getY()+ii))
                            canMove = false;
                        if(getX()-1<0 || getY()+ii>64)
                            canMove = false;
                    }
                    break;
                case 4:
                    d=right;
                    if(getDirection()==d)
                        canMove=false;
                    for(int ii=0; ii<4; ii++) {
                        if(getWorld()->isDirt(getX()+4, getY()+ii))
                            canMove = false;
                        if(getX()+4>64 || getY()+ii>64)
                            canMove = false;
                    }
                    break;
            }
        } while(canMove==false);
        setDirection(d);
        setNumSquaresSameDirection(randInt(8, 60));
    }
    //7
    if (getNumSquaresSameDirection()>0 && getTicksSincePerpendicular()==0)
    {
        bool goodDir = true;
        bool u, d, l, r;
        switch(getDirection())
        {
            case right:
                u = d = false;
                for(int q=0; q<4; q++)
                    if(getWorld()->isDirt(getX()+q, getY()-1) && getWorld()->isDirt(getX()+q, getY()+4)) {
                        goodDir = false;
                        r = true;
                    }
                break;
            case left:
                u = d = false;
                for(int q=0; q<4; q++)
                    if(getWorld()->isDirt(getX()+q, getY()-1) && getWorld()->isDirt(getX()+q, getY()+4)) {
                        goodDir = false;
                        l = true;
                    }
                break;
            case up:
                l = r = false;
                for(int q=0; q<4; q++)
                    if(getWorld()->isDirt(getX()-1, getY()+q) && getWorld()->isDirt(getX()+4, getY()+q)) {
                        goodDir = false;
                        u = true;
                    }
                break;
            case down:
                l = r = false;
                for(int q=0; q<4; q++)
                    if(getWorld()->isDirt(getX()-1, getY()+q) && getWorld()->isDirt(getX()+4, getY()+q)) {
                        goodDir = false;
                        d = true;
                    }
                break;
        }
        if (goodDir)
        {
            if (u || d)
            {
                if(randInt(1,2) == 1 && u)
                {
                    setDirection(up);
                    setNumSquaresSameDirection(randInt(8, 60));
                }
                else if(d) {
                    setDirection(down);
                    setNumSquaresSameDirection(randInt(8, 60));
                }
            }
            else if(l || r){
                if(randInt(1,2)==1 && l)
                {
                    setDirection(left);
                    setNumSquaresSameDirection(randInt(8, 60));
                }
                else if (r){
                    setDirection(right);
                    setNumSquaresSameDirection(randInt(8, 60));
                }
            }
        }
    }
    switch(getDirection())
    {
        case right:
            if(!moveToIfPossible(true, this, getX()+1, getY()))
                setNumSquaresSameDirection(0);
            break;
        case left:
            if(!moveToIfPossible(true, this, getX()-1, getY()))
                setNumSquaresSameDirection(0);
            break;
        case up:
            if(!moveToIfPossible(true, this, getX(), getY()+1))
                setNumSquaresSameDirection(0);
            break;
        case down:
            if(!moveToIfPossible(true, this, getX()+1, getY()-1))
                setNumSquaresSameDirection(0);
            break;
    }
}
bool Protester::setTicksToNextMove()
{
    if(m_restState>0) {
        m_restState--;
        return false;
    }
    if(m_restState==0){
        if(m_ticksUntilShout >0)
            m_ticksUntilShout--;
        m_ticksPerMove =getWorld()->minOrMax(0, 3- (getWorld()->getLevel() /4), false);
        m_restState = m_ticksPerMove;
        if(m_ticksSincePerpendicular >0)
            m_ticksSincePerpendicular--;
    }
    return true;
}
bool Protester::inFrackMansDirection(Direction dir)
{
    switch(dir) {
        case left:
            if(getX() == getWorld()->gimmeFrackMan()->getX()+4)
                return true;
            if( (getWorld()->gimmeFrackMan()->getX() - getX())<=3 && (getWorld()->gimmeFrackMan()->getX() - getX())>=1 && getY() == getWorld()->gimmeFrackMan()->getY())
                return true;
            else return false;
            break;
        case right:
            if(getX() ==getWorld()->gimmeFrackMan()->getX()-4)
                return true;
            if( (getX() - getWorld()->gimmeFrackMan()->getX() )<=3 && (getX() - getWorld()->gimmeFrackMan()->getX() )>=1&& getY() == getWorld()->gimmeFrackMan()->getY())
                return true;
            else return false;
            break;
        case up:
            if(abs(getX() - getWorld()->gimmeFrackMan()->getX()) <=3)
            {
                if(getWorld()->gimmeFrackMan()->getY() > getY())
                    return true;
            }
            return false;
            break;
        case down:
            if(abs(getX() - getWorld()->gimmeFrackMan()->getX()) <=3)
            {
                if(getWorld()->gimmeFrackMan()->getY() < getY())
                    return true;
            }
            return false;
            break;
    }
    return false;
}
int Protester::ticksUntilShout()
{
    return m_ticksUntilShout;
}
void Protester::addGold()
{
    m_gold++;
}
void Protester::setTicksToNextShout()
{
    m_ticksUntilShout = 15;
}
void Protester::setNumSquaresSameDirection(int x)
{
    if(x>=0)
    numSquaresInCurrentDirection = x;
}
int Protester::getNumSquaresSameDirection()
{
    return numSquaresInCurrentDirection;
}
int Protester::getTicksSincePerpendicular() const
{
    return m_ticksSincePerpendicular;
}
void Protester::resetTicksSincePerpendicular()
{
    m_ticksSincePerpendicular=200;
}
bool Protester::annoy(unsigned int amount)
{
    if(!m_leaveOilField)
    {
        Agent::annoy(amount);
        if(getHitPoints()<=0)
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            setMustLeaveOilField();
            m_ticksPerMove=0;
            m_restState =0;
            if(getHitPoints()>=100)
                getWorld()->increaseScore(500);
            if(getHitPoints()<100)
                getWorld()->increaseScore(100);
        }

        else{
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
            int n = m_restState + getWorld()->minOrMax(50, 100-getWorld()->getLevel()*10, false);
            m_ticksPerMove = n;
        }
        return true;
    }
    return false;
}
void Protester::setMustLeaveOilField()
{
    m_leaveOilField = true;
}
void Protester::followMaze(char key)
{
    if(key == 'F')
    {
        computeMazeToFMorExit('F', getX(), getY());
    }
    if(key== 'E')
    {
        //Seems to not go all the way to (60, 60)
        if(getX() == 59 && getY()==60)
        {
            moveTo(60, 60);
            setDead();
        }
        if(getX() == 60 && getY() ==59)
        {
            moveTo(60, 60);
            setDead();
        }
        if(!hasMaze())
        {
            computeMazeToFMorExit('E', getX(), getY());
            if(m_maze[getX()][getY()].prev!=nullptr) {
                if(m_maze[getX()][getY()].prev->xCoord > getX())
                {
                    setDirection(right);
                }
                if(m_maze[getX()][getY()].prev->xCoord < getX())
                {
                    setDirection(left);
                }
                if(m_maze[getX()][getY()].prev->yCoord > getY())
                {
                    setDirection(up);
                }
                if(m_maze[getX()][getY()].prev->yCoord < getY())
                {
                    setDirection(down);
                }
            moveToIfPossible(true, this, m_maze[getX()][getY()].prev->xCoord, m_maze[getX()][getY()].prev->yCoord);
            }
        }
        else if(hasMaze())
        {
            if(m_maze[getX()][getY()].prev!=nullptr)
                if(m_maze[getX()][getY()].prev->xCoord > getX())
                {
                    setDirection(right);
                }
            if(m_maze[getX()][getY()].prev->xCoord < getX())
            {
                setDirection(left);
            }
            if(m_maze[getX()][getY()].prev->yCoord > getY())
            {
                setDirection(up);
            }
            if(m_maze[getX()][getY()].prev->yCoord < getY())
            {
                setDirection(down);
            }
            if(!moveToIfPossible(true, this, m_maze[getX()][getY()].prev->xCoord, m_maze[getX()][getY()].prev->yCoord))
                computeMazeToFMorExit('E', getX(), getY());
        }

    }
}
bool Protester::leavingOilField() const
{
    return m_leaveOilField;
}
void Protester::setTicksPerMove(int x)
{
    m_ticksPerMove = x;
}
void Protester::setRestState(int x)
{
    m_restState = x;
}
/******************************
 Regular Protester Implementations
 ******************************/
RegularProtester::RegularProtester(StudentWorld* world, int startX, int startY) : Protester(world, startX, startY, IID_PROTESTER, 5, false) {}
void RegularProtester::addGold()
{
    Protester::addGold();
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(25);
    setMustLeaveOilField();
}


/******************************
 Hardcore Protester Implementations
 ******************************/
HardcoreProtester::HardcoreProtester(StudentWorld* world, int startX, int startY) : Protester(world, startX, startY, IID_HARD_CORE_PROTESTER, 20, true) {}
void HardcoreProtester::addGold()
{
    Protester::addGold();
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    getWorld()->increaseScore(50);
    int n = getRestState() + getWorld()->minOrMax(50, 100-getWorld()->getLevel()*10, false);
    setTicksPerMove(n);
}
bool HardcoreProtester::annoy(unsigned int amount)
{
    if(!leavingOilField())
    {
        Agent::annoy(amount);
        if(getHitPoints()<=0)
        {
            getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
            setMustLeaveOilField();
            setTicksPerMove(0);
            setRestState(0);
            if(getHitPoints()>=100)
                getWorld()->increaseScore(500);
            if(getHitPoints()<100)
                getWorld()->increaseScore(250);
        }
        
        else{
            getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
            int n = getRestState() + getWorld()->minOrMax(50, 100-getWorld()->getLevel()*10, false);
            setTicksPerMove(n);
        }
        return true;
    }
    return false;
}

/******************************
 FrackMan Implementations
 ******************************/
FrackMan::FrackMan(StudentWorld* world) : Agent(world, IID_PLAYER,  30,  60,  right ,  1 ,  0,  10) {m_gold = 0; m_sonar = 0; m_water=5;}
FrackMan::~FrackMan() {}//60
void FrackMan::doSomething()
{
    if(!isAlive())
        return;
    {
        if(getWorld()->removeDirt(getX(), getY()))
            getWorld()->playSound(SOUND_DIG);
    }
    int ch;
    int x = getX();
    int y = getY();
    if (getWorld()->getKey(ch) == true)
    {
        // user hit a key this tick!
        switch (ch)
        {
            case KEY_PRESS_ESCAPE:
                setDead();
                break;
            case KEY_PRESS_LEFT:
                if (getDirection() != left) {
                        setDirection(left);
                    break;
                }
                if(!moveToIfPossible(false, this, x-1, y))
                 {
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() != right) {
                    setDirection(right);
                    break;
                }
                if(!moveToIfPossible(false, this, x+1, y))
                {
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_UP:
                if (getDirection() != up) {
                    setDirection(up);
                    break;
                }
                if(!moveToIfPossible(false, this, x, y+1))
//                {
//                    if(getWorld()->removeDirt(x, y+1))
//                    getWorld()->playSound(SOUND_DIG);
//                }
                 {
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_DOWN:
                if (getDirection() != down) {
                    setDirection(down);
                    break;
                }
                if(!moveToIfPossible(false, this, x, y-1))
//                {
//                    if(getWorld()->removeDirt(x, y-1))
//                    getWorld()->playSound(SOUND_DIG);
//                }
                 {
                    moveTo(x, y);
                }
                break;
            case KEY_PRESS_SPACE:
                squirtWater();
                break;
            case 'z':
            case 'Z':
                if(m_sonar>0)
                {
                    getWorld()->revealAllNearbyObjects(getX(), getY(), 12);
                    m_sonar--;
                }
                break;
            case KEY_PRESS_TAB:
                if(getGold() >0)
                {
                    getWorld()->getActors().push_back(new GoldNugget(getWorld(), getX(), getY(), true));
                    m_gold--;
                }
        }
    }
}
void FrackMan::addGold()
{
    m_gold++;
}
unsigned int FrackMan::getGold() const
{
    return m_gold;
}
void FrackMan::addSonar()
{
    m_sonar++;
}
unsigned int FrackMan::getSonar() const
{
    return m_sonar;
}
void FrackMan::removeSonar()
{
    m_sonar--;
    if(m_sonar <0)
        m_sonar = 0;
}
// Pick up water.
void FrackMan::addWater()
{
    m_water += 5;
}
// Get amount of water
unsigned int FrackMan::getWater() const
{
    return m_water;
}
bool FrackMan::canDigThroughDirt() const
{
    return true;
}
void FrackMan::squirtWater()
{
    if(m_water>0) {
    switch(getDirection())
    {
        case right:
            if (getX()+4 > 60 || getX()+4 < 0 || getY()> 60 || getY()<0 || anyDirtInFourSpaces(getX()+4, getY(), getDirection()) || getWorld()->isActorHereAndCanPassThrough(this, getX()+4, getY()))
            {
                m_water--;
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                break;
            }
            getWorld()->addActor(new Squirt(getWorld(), getX()+4, getY(), right));
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            m_water--;
            break;
        case left:
            if (getX()-4 > 60 || getX()-4 < 0 || getY()> 60 || getY()<0 || anyDirtInFourSpaces(getX()+4, getY(), getDirection()) || getWorld()->isActorHereAndCanPassThrough(this, getX()-4, getY()))
            {
                m_water--;
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                break;
            }
            getWorld()->addActor(new Squirt(getWorld(), getX()-4, getY(), getDirection()));
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            m_water--;
            break;
        case up:
            if (getX()> 60 || getX()< 0 || getY()+4> 60 || getY()+4<0 || anyDirtInFourSpaces(getX(), getY()+4, getDirection()) || getWorld()->isActorHereAndCanPassThrough(this, getX(), getY()+4))
            {
                m_water--;
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                break;
            }
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY()+4, getDirection()));
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            m_water--;
            break;
        case down:
            if (getX()> 60 || getX()< 0 || getY()-4> 60 || getY()-4<0 || anyDirtInFourSpaces(getX(), getY()-4, getDirection()) || getWorld()->isActorHereAndCanPassThrough(this, getX(), getY()-4))
            {
                m_water--;
                getWorld()->playSound(SOUND_PLAYER_SQUIRT);
                break;
            }
            getWorld()->addActor(new Squirt(getWorld(), getX(), getY()-4, getDirection()));
            getWorld()->playSound(SOUND_PLAYER_SQUIRT);
            m_water--;
            break;
    }
    }
}
/*******************
 Boulder
 *******************/
Boulder::Boulder(StudentWorld* world, int startX, int startY) : Actor(world, IID_BOULDER, startX, startY, down, 1.0, 1, true) {
    m_state =1;
    m_ticks = 30;
}
Boulder::~Boulder() {};
void Boulder::doSomething()
{
    switch (m_state) {
        case 1: {
            bool babyBoulder = false;
            for (int i = 0; i<4; i++)
            {
                    if (getWorld()->isDirt(getX() + i, getY()-1))
                        {
                            babyBoulder = true;
                            break;
                        }
            }
            if (!babyBoulder)
                m_state =2; }
            break;
        case 2:
            if (m_ticks ==0)
                m_state =3;
            m_ticks--;
            break;
        case 3:
            getWorld()->annoyAllNearbyActors(this, 100, 3);
            if(!moveToIfPossible(true, this, getX(), getY()-1))
                setDead();
            if(getY()==0)
                setDead();
            break;
            
    }
}
bool Boulder::canActorsPassThroughMe() const
{
    return false;
}
/*******************
 Squirt
 *******************/
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction startDir) : Actor(world, IID_WATER_SPURT, startX, startY, startDir, 1.0, 1, true) , m_ticksToLive(4) {}
void Squirt::doSomething()
{
    //1
    if(getWorld()->annoyAllNearbyActors(this, 2, 3)>=1)
        setDead();
    //2
        m_ticksToLive--;
    if(m_ticksToLive == 0)
    {
        setDead();
        return;
    }
    //3
    int move = 0;
    switch(getDirection())
    {
        case right: case up: move = 1;
            break;
        case left: case down: move = -1;
            break;
        default: break;
    }
    if(getDirection() == up || getDirection() == down)
    {
        if(!moveToIfPossible(true, this, getX(), getY()+move))
            setDead();
    }
    if(getDirection() == right || getDirection() == left)
    {
        if(!moveToIfPossible(true, this, getX()+move, getY()))
            setDead();
    }
    
}
/*******************
 Activating Object
 *******************/
ActivatingObject::ActivatingObject(StudentWorld* world, int startX, int startY, int imageID, Direction dir, double size,  int depth, bool visible) : Actor(world, startX, startY, imageID, dir, size, depth, visible) {
    tempState=false;
}
void ActivatingObject::doSomething() {return;}
// Set number of ticks until this object dies
void ActivatingObject::setTicksToLive(int ticks)
{
    m_lifeTicks = ticks;
    tempState = true;
}
//Decrement life ticks
int ActivatingObject::getTicksToLive() const
{
    return m_lifeTicks;
}
bool ActivatingObject::isTempState() const
{
    return tempState;
}
void ActivatingObject::makeTempState()
{
    tempState = true;
}

/*******************
 Gold Nugget
 *******************/
GoldNugget::GoldNugget(StudentWorld* world, int startX, int startY, bool temp) : ActivatingObject(world, IID_GOLD, startX, startY, right, 1.0, 2, temp) {
if(temp)
{
    makeTempState();
    setTicksToLive(100);
}
}
void GoldNugget::doSomething()
{
    if (!isAlive())
        return;
    if(!isTempState())
    {
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(), this, 4) && !isVisible())
       {
           setVisible(true);
           return;
       }
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(), this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->gimmeFrackMan()->addGold();
        getWorld()->increaseScore(10);
    }
    }
    if(isTempState())
    {
        setTicksToLive(getTicksToLive()-1);
        for(int w=0; w<getWorld()->getActors().size(); w++)
        {
            if(getWorld()->getActors()[w] != nullptr)
            if(getWorld()->isNear(getX(), getY(), getWorld()->getActors()[w], 3) && getWorld()->getActors()[w]->canPickThingsUp())
            {
                setDead();
                getWorld()->getActors()[w]->addGold();
                break;
            }
        }
        if(getTicksToLive()<=0)
            setDead();
    }
        
}
/*******************
 Oil Barrel
 *******************/
OilBarrel::OilBarrel(StudentWorld* world, int startX, int startY) : ActivatingObject(world,IID_BARREL, startX, startY, right, 1.0, 2, false) {}
void OilBarrel::doSomething()
{
    if (!isAlive())
        return;
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(), this, 4) && !isVisible())
    {
        setVisible(true);
        return;
    }
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(), this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_FOUND_OIL);
        getWorld()->increaseScore(1000);
        getWorld()->iPickedUpAPrince();
    }
}
/*******************
 SonarKit
 *******************/
SonarKit::SonarKit(StudentWorld* world, int startX, int startY) : ActivatingObject(world, IID_SONAR, startX, startY, right, 1.0, 2, true) {
    setTicksToLive( getWorld()->minOrMax(100, 300- 10*getWorld()->getLevel(), false));
}
void SonarKit::doSomething()
{
    if(!isAlive())
        return;
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(), this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->gimmeFrackMan()->addSonar();
        getWorld()->increaseScore(75);
    }
    setTicksToLive(getTicksToLive()-1);
    if(getTicksToLive()<=0)
        setDead();
}
/*******************
 Water
 *******************/
WaterPool::WaterPool(StudentWorld* world, int startX, int startY) : ActivatingObject(world, IID_WATER_POOL, startX, startY, right, 1.0, 2, true)
{
    int ticks =getWorld()->minOrMax(100, 300 - 10 * getWorld()->getLevel(), false);
    setTicksToLive(ticks);
}
void WaterPool::doSomething()
{
    if(!isAlive())
        return;
    if(getWorld()->isNear(getWorld()->gimmeFrackMan()->getX(), getWorld()->gimmeFrackMan()->getY(),this, 3))
    {
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->gimmeFrackMan()->addWater();
        getWorld()->increaseScore(100);
    }
}
