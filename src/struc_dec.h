#pragma once
#ifndef STRUC_DEC_H
#define STRUC_DEC_H

#include "raylib.h"   // daje Rectangle, Vector2, Color

// własny punkt (możesz też użyć gotowego Vector2)
typedef struct {
    float x;
    float y;
} Point;

// koło = punkt środka + promień
typedef struct {
    Point center;
    float radius;
    Color color;
} Circle;

// własny prostokąt, jeśli chcesz dodać coś od siebie
typedef struct {
    Rectangle rec;   // gotowy struct z raylibu
    Color color;
} Box;

#endif