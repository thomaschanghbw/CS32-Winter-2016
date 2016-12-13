//
//  History.hpp
//  CS32 P1
//
//  Created by Thomas Chang on 1/9/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#ifndef History_h
#define History_h
#include "globals.h"

class History
{
public:
    History(int nRows, int nCols);
    bool record(int r, int c);
    void display() const;
private:
    char m_arena[MAXROWS][MAXCOLS];
    int m_rows;
    int m_cols;
};


#endif /* History_hpp */
