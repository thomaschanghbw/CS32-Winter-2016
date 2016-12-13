//
//  History.cpp
//  CS32 P1
//
//  Created by Thomas Chang on 1/9/16.
//  Copyright © 2016 CS32. All rights reserved.
//

#include "History.h"
#include "globals.h"
#include <iostream>
using namespace std;


History::History(int nRows, int nCols) {
    m_rows = nRows;
    m_cols = nCols;
    for (int i = 0; i < m_rows; i++)
        for(int k = 0; k < m_cols; k++)
            m_arena[i][k] = '.';
}

bool History::record(int r, int c) {
    if (r > m_rows || c > m_cols || r <= 0 || c <= 0)
        return false;
    if (m_arena[r-1][c-1] == '.')
    {
        m_arena[r-1][c-1] = 'A';
        return true;
    }
    if (m_arena[r-1][c-1] == 'Z')
        return true;
    m_arena[r-1][c-1]++;
    return true;
}

void History::display() const {
    clearScreen();
    for (int i = 0; i < m_rows; i++)
    {
        for(int k = 0; k < m_cols; k++)
            cout << m_arena[i][k];
        cout << endl;
    }
}



