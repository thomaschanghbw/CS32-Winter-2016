//
//  Game.hpp
//  CS32 P1
//
//  Created by Thomas Chang on 1/9/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#ifndef Game_hpp
#define Game_hpp
#include "Arena.h"

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nRobots);
    ~Game();
    
    // Mutators
    void play();
    
private:
    Arena* m_arena;
};



#endif /* Game_hpp */
