//
//  Player.hpp
//  CS32 P1
//
//  Created by Thomas Chang on 1/9/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#ifndef Player_h
#define Player_h


class Arena;
class Player
{
public:
    // Constructor
    Player(Arena *ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    int  age() const;
    bool isDead() const;
    
    // Mutators
    void   stand();
    void   moveOrAttack(int dir);
    void   setDead();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_age;
    bool   m_dead;
};



#endif /* Player_hpp */
