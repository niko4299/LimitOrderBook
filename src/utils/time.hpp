#pragma once

#include <bits/stdc++.h>

constexpr int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

inline int day_of_week(int d, int m, int y) { 
    y -= m < 3;
    return ( y + y / 4 - y / 100 +  y / 400 + t[m - 1] + d) % 7; 
};

