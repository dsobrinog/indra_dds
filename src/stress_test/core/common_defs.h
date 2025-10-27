#pragma once

enum TestMode
{
    // Manual
    DefaultMode = 0,

    // Auto
    // 1:1
    One_to_One_Pub = 1,
    One_to_One_Sub = 2,

    // 1:many
    One_to_Many_Pub = 3,
    One_to_Many_Sub = 4
}; 