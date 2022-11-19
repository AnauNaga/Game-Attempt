#ifndef GUIUTILITES_H_INCLUDED
#define GUIUTILITES_H_INCLUDED
#include "framework.h"
#include "BMPIN.h"
#include "BMPOUT.h"
#include <stdio.h>
#include <iostream>

struct bufferMem {
    void* memory = 0;
    void* background = 0;
    void* interactable = 0;
    unsigned int* UIMemory = 0;
    unsigned int* UIBackground = 0;
    unsigned int* UIInteractable;
    int width = 0;
    int height = 0;
    int widthHalf = 0;
    int heightHalf = 0;
    BITMAPINFO buffer_bitmap_info;
};

struct timeInfo {
    int frame_rate;
    float timeKeeperAlpha = 0.0f;
    float timeKeeperBeta = 0.0f;
    float timeKeeperCharlie = 0.0f;
    float delta_time = 0.016666f;
    LARGE_INTEGER frame_begin_time;
    LARGE_INTEGER frame_end_time;

    //performance_frequency is the processor clock speed 
    float performance_frequency;
    LARGE_INTEGER perf;

};

struct textLine {
    int x;
    int y;
    char* text;
    textLine* nextNode;
};



struct Vector{
    float x,y;
};
struct UVMap {

    Vector UVmap[3];
};

struct vertex {
    float x, y, z;
};

struct triangle {
    vertex verts[3];

};
struct shape {
    std::vector<triangle> tris;
    std::vector<UVMap> UVMap;
};
struct mat4x4 {
    float m[4][4] = { 0.0f };
};
struct mat3x3 {
    float m[3][3] = { 0.0f };

};


int setPixelColor(int X, int Y, unsigned int color, bufferMem* buffer, unsigned int* buffer_memory);
int DrawRect(int X1, int Y1, int X2, int Y2, unsigned int color, bufferMem* buffer, unsigned int* buffer_memory);
int drawLine(struct bufferMem* buffer, int firstX, int firstY, int secondX, int secondY, int thickness);
int setWindowColor(unsigned int color, struct bufferMem* buffer, unsigned int* buffer_memory);
void printText(char textToPrint[], int xStart, int yStart, struct bufferMem* buffer, unsigned int* buffer_memory);
void printNumber(int numberToPrint, int xStart, int yStart, struct bufferMem* buffer, unsigned int* buffer_memory);
void printFloat(float numberToPrint, int trunc, int xStart, int yStart, struct bufferMem* buffer, unsigned int* buffer_memory);

unsigned int getPixelColor(int X, int Y, struct bufferMem* buffer, unsigned int* buffer_memory);
unsigned int compareColors(unsigned int colorTop, unsigned int colorBottom);
void drawImage(int x, int y, float scale, BMPIN* image, struct bufferMem* buffer, unsigned int* buffer_memory);
void getFiles(std::string path, std::string fileExtention, textLine** fileLinkedList, int startinX, int startingY);

char capitalize(char text);
void blur(bufferMem* buffer, unsigned int* buffer_memory);
void multiplyByProjMat(vertex& newVector, vertex& Vector, mat4x4& Matrix);
void multiplyByRotMat(vertex& newVector, vertex& Vector, mat3x3& Matrix);
void CrossProduct(vertex& newVector, vertex& Vector1, vertex& Vector2);
float DotProduct(vertex& Vector1, vertex& Vector2);
void fillTriangle(bufferMem* buffer, vertex* verts, unsigned int color);
void sortVertsY(vertex* sortedArray, vertex* verts, int arraySize);
void sortVertsX(vertex* sortedArray, vertex* verts, int arraySize);
#endif