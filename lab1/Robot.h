//
//  Robot.h
//  CS32 P1
//
//  Created by Thomas Chang on 1/9/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#ifndef Robot_h
#define Robot_h

class Arena;
class Robot
{
public:
    // Constructor
    Robot(Arena* ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    
    // Mutators
    void move();
    bool getAttacked(int dir);
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    m_health;
};

#endif /* Robot_h */
