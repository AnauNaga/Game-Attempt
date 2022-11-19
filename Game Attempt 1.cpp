
//windows includes
#include "windows.h"
#include "framework.h"
#include <tchar.h>//used to create a LPCWSTR variable
#include <math.h>
#include "resource.h"
#include "Game Attempt 1.h"

//tools
#include <string>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <fileSystem>
#include <fstream>
#include <sstream>
#include <vector>

//local includes
#include "KeyTracker.h"
#include "BMPIO.h"
#include "BMPOUT.h"
#include "BMPIN.h"
#include "GuiUtilites.h"
#include "Chunk.h"

enum ButtonList {
    NewFile,
    LoadFile,
    ListFile,
    DelFile,
    Exit,
    ButtonCount
};

struct Pos {
    float x = 400;
    float y = 400;
};




bufferMem* buffer = new bufferMem{};
Globals* Global = new Globals{};

//Read messages sent by windows and do something based on them.
LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    //Windows likes to store information in each individual bit of a byte. Get the bit reguarding if a key is pressed down from lparam.


    //do something based off the message read.
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY: {
        Global->runningWindow = false;
    } break;
    case WM_SIZE: {

        //if the window memory == 0 then it hasn't been initialized; Initialize window memory.
        //This if statement is necessary to prevent memory buffers from being deleted upon minimizing the window.
        if (buffer->memory == 0) {
            //get window dimensins
            RECT rect;
            GetClientRect(hwnd, &rect);
            buffer->width = rect.right - rect.left;
            buffer->height = rect.bottom - rect.top;

            buffer->widthHalf = buffer->width / 2;
            buffer->heightHalf = buffer->height / 2;
            int buffer_size = buffer->width * buffer->height * sizeof(unsigned int);

            if (buffer->memory) VirtualFree(buffer->memory, 0, MEM_RELEASE);
            buffer->memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            buffer->buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer->buffer_bitmap_info.bmiHeader);
            buffer->buffer_bitmap_info.bmiHeader.biWidth = buffer->width;
            buffer->buffer_bitmap_info.bmiHeader.biHeight = buffer->height;
            buffer->buffer_bitmap_info.bmiHeader.biPlanes = 1;
            buffer->buffer_bitmap_info.bmiHeader.biBitCount = 32;
            buffer->buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

            //allocate memory for buffer_background
            if (buffer->background) VirtualFree(buffer->background, 0, MEM_RELEASE);
            buffer->background = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            //allocate memory for buffer_interactable
            if (buffer->interactable) VirtualFree(buffer->interactable, 0, MEM_RELEASE);
            buffer->interactable = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            //set the unsigned int of each memory location;
            buffer->UIMemory = static_cast<unsigned int*>(buffer->memory);
            buffer->UIBackground = static_cast<unsigned int*>(buffer->background);
            buffer->UIInteractable = static_cast<unsigned int*>(buffer->interactable);
            setWindowColor(0x00000AA, buffer, (unsigned int*)buffer->background);
            setWindowColor(0xFF000000, buffer, (unsigned int*)buffer->interactable);
            printText((char*)"Cosmic Database", 200, 960, buffer, (unsigned int*)buffer->background);
            BMPIN* image = new BMPIN();
            image->Import("Text/System.bmp");
            drawImage(920, 0, 1, image, buffer, (unsigned int*)buffer->background);
            delete image;
        }
    }
    case WM_LBUTTONUP: {
        Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down = false;
        Global->keyInput->buttons[BUTTON_MOUSELEFT].changed = false;
        return result;
    }break;
    default: {
        result = DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    }

    return result;
}
//run the window
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {


    Global->keyInput = new Input{};
    //create a window and set its starting settings
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = L"Final Project";
    window_class.lpfnWndProc = window_callback;
    window_class.hInstance = hInstance;
    RegisterClass(&window_class);
    HWND window = CreateWindow(window_class.lpszClassName, L"Cosmic Database", WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0, 1920, 1040, 0, 0, hInstance, 0);
    HDC hdc = GetDC(window);
    mainMenu(window, hdc, Global, buffer);
}//end winMain

int mainMenu(HWND window, HDC hdc,Globals* Global, struct bufferMem* buffer) {


    shape cube;
    cube.tris = { 
            //front
            {0.0f,0.0f,0.0f ,0.0f,1.0f,0.0f, 1.0f,1.0f,0.0f},  {0.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,0.0f,0.0f },
            //right
            {1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,1.0f},  {1.0f,0.0f,0.0f, 1.0f,1.0f,1.0f, 1.0f,0.0f,1.0f },
            //left
            {0.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,0.0f},  {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f },
            //back
            {1.0f,0.0f,1.0f, 1.0f,1.0f,1.0f, 0.0f,1.0f,1.0f},  {1.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,0.0f,1.0f },
            //up
            {0.0f,1.0f,0.0f, 0.0f,1.0f,1.0f, 1.0f,1.0f,1.0f},  {0.0f,1.0f,0.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,0.0f },
            //bottom
            {0.0f,0.0f,1.0f, 0.0f,0.0f,0.0f,  1.0f,0.0f,0.0f},  {0.0f,0.0f,1.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,1.0f },
            
    };

    cube.UVMap = {
            //front
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
            //right
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
            //left
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
            //back
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
            //up
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
            //bottom
            {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},  {0.0f,0.0f ,0.0f,1.0f, 1.0f,0.0f},
    };

    BMPIN* stone = new BMPIN;
    stone->Import("Stone.bmp");

    mat4x4 matrixproj{0};
    mat3x3 matrixRotSide{0};
    mat3x3 matrixRotUp{ 0 };
    vertex Light{ sqrt(2),sqrt(2),0};

    // projection matrix
    float fNear = 0.1f;
    float fFar = 100000000.0f;
    float fFov = 90.045f;
    float fAspectRatio = (float)buffer->height / (float)buffer->width;
    float fFovRad = buffer->height / tanf((fFov * 3.141592) / 360.0f);

    //initialize projection matrix
    matrixproj.m[0][0] = fFovRad; //fAspectRatio*
    matrixproj.m[1][1] = fFovRad;
    matrixproj.m[2][2] = fFar / (fFar - fNear);
    matrixproj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrixproj.m[2][3] = 1.0f;
    matrixproj.m[3][3] = 0.0f;

    //initialize Rotation matrix
    matrixRotSide.m[0][0] = 1.0f;
    matrixRotSide.m[1][1] = 1.0f;
    matrixRotSide.m[2][2] = 1.0f;
    float RotateX = 0;
    float RotateY = 0;

    //tools used to keep track of time
    timeInfo time;
    QueryPerformanceCounter(&time.frame_begin_time);
    QueryPerformanceFrequency(&time.perf);
    //performance_frequency is the processor clock speed 
    time.performance_frequency = (float)time.perf.QuadPart;

    //current position of typing
    POINT typeCursor = { 0,0 };
    POINT typeCursorPrev = { 0,0 };
    char charsToPrint[2];


    int messageReturn;

    //linked list of words
    textLine* text = new textLine{ 0,0,0,0 };
    char* word = new char[2];
    word[0] = ' ';
    word[1] = '\0';

    textLine* fileList = new textLine{ 100,848,(char*)"File List",0 };
    word[0] = ' ';
    word[1] = '\0';
    bool printFileList = true;

    
    while (buffer->memory == 0) {
        messageReturn = proccessMessages(Global->keyInput, window, Global, buffer);
    }

    setWindowColor(0x00000000, buffer, (unsigned int*)buffer->background);
    setWindowColor(0xFF000000, buffer, (unsigned int*)buffer->interactable);

    Chunk *chunkList = new Chunk[16];
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            chunkList[x + y * 4] = Chunk(1, x * 512, y * 512, 20);
        }
    }
    float up = 0;
    float right = 0;
    float forward = 5;

    vertex viewVector{ 0,0,1 };

    int scroll = 0.0f;

    Pos pos1{ 0,0 };
    Pos pos2{ 0,0 };



    //run the window
    while (Global->runningWindow) {

        messageReturn = proccessMessages(Global->keyInput, window, Global, buffer);

        if (Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down) {
            if ((pos1.x == 0) && (pos1.y == 0)) {
                pos1.x = Global->cursorX;
                pos1.y = Global->cursorY;
                Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down = false;

            }
            else if ((pos2.x == 0) && (pos2.y == 0)) {
                pos2.x = Global->cursorX;
                pos2.y = Global->cursorY;
                drawLine(buffer, pos1.x, pos1.y, pos2.x, pos2.y, 1);
                Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down = false;
            }
            else {

                drawLine(buffer, pos1.x, pos1.y, Global->cursorX, Global->cursorY, 1);
                pos1.x = pos2.x;
                pos1.y = pos2.y;
                pos2.x = Global->cursorX;
                pos2.y = Global->cursorY;
                drawLine(buffer, pos1.x, pos1.y, pos2.x, pos2.y, 1);
                pos1.x = 0;
                pos1.y = 0;
                pos2.x = 0;
                pos2.y = 0;
                Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down = false;
            }

        
        }

        
        if (Global->keyInput->buttons[BUTTON_W].is_down) {
            

            forward -= 10.0f * sin((-RotateX / 100) + 1.57079) * time.delta_time;
            right -= 10.0f * cos((-RotateX / 100) + 1.57079) * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_A].is_down) {
            forward += 10.0f * sin(-RotateX / 100) * time.delta_time;
            right += 10.0f * cos(-RotateX / 100) * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_D].is_down) {
            forward -= 10.0f * sin(-RotateX / 100) * time.delta_time;
            right -= 10.0f * cos(-RotateX / 100) * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_S].is_down) {
            forward += 10.0f * sin((-RotateX / 100) + 1.57079) * time.delta_time;
            right += 10.0f * cos((-RotateX / 100) + 1.57079) * time.delta_time;
        }
        
        
        if (Global->keyInput->buttons[BUTTON_SPACE].is_down) {
            
            up -= 10 * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_CONTROL].is_down) {
            up += 10 * time.delta_time;
        }



        //change FOV
        if (Global->keyInput->buttons[BUTTON_RIGHT].is_down) {
            fFov -= 10*time.delta_time;
            fFovRad = buffer->height / tanf((fFov * 3.141592) / 360.0f);
            matrixproj.m[0][0] = fFovRad;
            matrixproj.m[1][1] = fFovRad;
            DrawRect(0, 750, 150, buffer->height,0x00FFFFFF, buffer, buffer->UIBackground);
            printFloat(fFov,3, 10, 800, buffer, buffer->UIBackground);
            Global->keyInput->buttons[BUTTON_RIGHT].is_down = false;
        }
        if (Global->keyInput->buttons[BUTTON_LEFT].is_down) {
            fFov += 10 * time.delta_time;
            fFovRad = buffer->height / tanf((fFov * 3.141592) / 360.0f);
            matrixproj.m[0][0] = fFovRad;
            matrixproj.m[1][1] = fFovRad;
            DrawRect(0, 750, 150, buffer->height,0x00FFFFFF, buffer, buffer->UIBackground);
            printFloat(fFov,3, 10, 800, buffer, buffer->UIBackground);
            Global->keyInput->buttons[BUTTON_LEFT].is_down = false;
        }




        DrawRect(0, 750, 150, buffer->height, 0x00FFFFFF, buffer, buffer->UIBackground);
        printFloat(fFov, 3, 10, 800, buffer, buffer->UIBackground);
        printFloat(fFovRad, 3, 10, 850, buffer, buffer->UIBackground);

        if ((abs(Global->cursorX+8 - buffer->widthHalf) > 0) || ((Global->cursorY+31 - buffer->heightHalf)> 0)) {

            if (GetForegroundWindow() == window) {
                RotateX += Global->cursorX + 8 - buffer->widthHalf;
                RotateY += Global->cursorY + 31 - buffer->heightHalf;
                if (RotateY > 122.16) {
                    RotateY = 122.16;
                }
                if (RotateY < -122.16) {
                    RotateY = -122.16;
                }
                SetCursorPos(buffer->widthHalf, buffer->heightHalf);
                matrixRotSide.m[0][0] = cos(RotateX / 100);
                matrixRotSide.m[0][2] = sin(RotateX / 100);
                matrixRotSide.m[2][0] = cos(RotateX / 100 + 1.57079);
                matrixRotSide.m[2][2] = sin(RotateX / 100 + 1.57079);
                matrixRotUp.m[1][1] = cos(RotateY / 100);
                matrixRotUp.m[1][2] = sin(RotateY / 100);
            }
        }
        
        //scale
        for (triangle tri : cube.tris) {

            
                triangle triTranslated;
                triangle triExpanded{};
                triangle triRotatedSide{};
                triangle triRotatedUp{};

                vertex vectorLeft{ tri.verts[1].x - tri.verts[0].x,tri.verts[1].y - tri.verts[0].y, tri.verts[1].z - tri.verts[0].z };
                vertex vectorRight{ tri.verts[2].x - tri.verts[0].x,tri.verts[2].y - tri.verts[0].y, tri.verts[2].z - tri.verts[0].z };
                vertex Face{ 0,0,0 };
                
                CrossProduct(Face, vectorLeft, vectorRight);
                float FaceScale = sqrt((Face.x * Face.x) + (Face.y * Face.y) + (Face.z * Face.z));
                Face = {Face.x/FaceScale,Face.y / FaceScale ,Face.z / FaceScale };
                float DP = DotProduct(Light, Face)+1;
                DP = DP * 0.75 + 0.5;
                //expand;
                triExpanded.verts[0].x = tri.verts[0].x * 1;
                triExpanded.verts[1].x = tri.verts[1].x * 1;
                triExpanded.verts[2].x = tri.verts[2].x * 1;

                triExpanded.verts[0].y = tri.verts[0].y * 1;
                triExpanded.verts[1].y = tri.verts[1].y * 1;
                triExpanded.verts[2].y = tri.verts[2].y * 1;

                triExpanded.verts[0].z = (tri.verts[0].z + 1) * 1;
                triExpanded.verts[1].z = (tri.verts[1].z + 1) * 1;
                triExpanded.verts[2].z = (tri.verts[2].z + 1) * 1;





                //shift foward;
                triTranslated.verts[0].x = triExpanded.verts[0].x - right;
                triTranslated.verts[1].x = triExpanded.verts[1].x - right;
                triTranslated.verts[2].x = triExpanded.verts[2].x - right;

                triTranslated.verts[0].y = triExpanded.verts[0].y - up;
                triTranslated.verts[1].y = triExpanded.verts[1].y - up;
                triTranslated.verts[2].y = triExpanded.verts[2].y - up;

                triTranslated.verts[0].z = triExpanded.verts[0].z - forward;
                triTranslated.verts[1].z = triExpanded.verts[1].z - forward;
                triTranslated.verts[2].z = triExpanded.verts[2].z - forward;

                //Rotate side to side
                multiplyByRotMat(triRotatedSide.verts[0], triTranslated.verts[0], matrixRotSide);
                multiplyByRotMat(triRotatedSide.verts[1], triTranslated.verts[1], matrixRotSide);
                multiplyByRotMat(triRotatedSide.verts[2], triTranslated.verts[2], matrixRotSide);

                ////Rotate Up/Down
                //multiplyByRotMat(triRotatedUp.verts[0], triRotatedSide.verts[0], matrixRotUp);
                //multiplyByRotMat(triRotatedUp.verts[1], triRotatedSide.verts[1], matrixRotUp);
                //multiplyByRotMat(triRotatedUp.verts[2], triRotatedSide.verts[2], matrixRotUp);



                //create scale;
                triangle triProjected;
                multiplyByProjMat(triProjected.verts[0], triRotatedSide.verts[0], matrixproj);
                multiplyByProjMat(triProjected.verts[1], triRotatedSide.verts[1], matrixproj);
                multiplyByProjMat(triProjected.verts[2], triRotatedSide.verts[2], matrixproj);

                vectorLeft = { triProjected.verts[1].x- triProjected.verts[0].x,triProjected.verts[1].y - triProjected.verts[0].y, triProjected.verts[1].z - triProjected.verts[0].z };
                vectorRight = { triProjected.verts[2].x - triProjected.verts[0].x,triProjected.verts[2].y - triProjected.verts[0].y, triProjected.verts[2].z - triProjected.verts[0].z };

                CrossProduct(Face, vectorLeft, vectorRight);

            if (DotProduct(viewVector,Face) > 0) {

                if ((triProjected.verts[0].z > 0) && (triProjected.verts[1].z > 0) && (triProjected.verts[2].z > 0)) {
                    //float xInc = cube.UVMap[0]. / stone->getHeight();
                    //set to orgin of screen
                    triProjected.verts[0].x += buffer->widthHalf;
                    triProjected.verts[1].x += buffer->widthHalf;
                    triProjected.verts[2].x += buffer->widthHalf;
                    triProjected.verts[0].y += buffer->heightHalf;
                    triProjected.verts[1].y += buffer->heightHalf;
                    triProjected.verts[2].y += buffer->heightHalf;

                    unsigned int color = 0x00009900;
                    unsigned int newColor = ((unsigned int)(((color >> 16)&0xFF)*DP)<< 16) + ((unsigned int)(((color >> 8) & 0xFF) *DP) << 8) + ((unsigned int)(((color) & 0xFF) * DP));
                    fillTriangle(buffer, triProjected.verts, newColor);
                    //drawLine(buffer, triProjected.verts[0].x, triProjected.verts[0].y, triProjected.verts[1].x, triProjected.verts[1].y, 1);
                    //drawLine(buffer, triProjected.verts[1].x, triProjected.verts[1].y, triProjected.verts[2].x, triProjected.verts[2].y, 1);
                    //drawLine(buffer, triProjected.verts[2].x, triProjected.verts[2].y, triProjected.verts[0].x, triProjected.verts[0].y, 1);
                }
            }
        }

        
        /*for (int y = 0; y < buffer->height/100; y++) {
            for (int x = 0; x < buffer->width/100; x++) {
                unsigned int* grid = buffer->UIBackground + x * 100 + y * 100 * buffer->width;
                *grid = 0x00FFFFFF;
            }
        }*/



        //combine all the bitmap layers
        unsigned int* background = (unsigned int*)buffer->background;
        unsigned int* memory = (unsigned int*)buffer->memory;
        unsigned int* interactable = (unsigned int*)buffer->interactable;
        int newx = 0;
        int newy = 0;
        for (int y = 0; y < buffer->height-1; y++) {
            for (int x = 0; x < buffer->width; x++) {
                if (*background >> 24 != 0xFF) {
                    *memory = *background;
                        /*newy = up - ((up * 1540) / (y + 1540));
                        if (x != buffer->width / 2) {
                            newx = (((newy - up) * (right - x)) / 1080) + right;
                        }
                        if ((newx + newy * buffer->width > 0) && (buffer->width * buffer->height > newx + newy * buffer->width)) {
                            *((unsigned int*)buffer->memory + newx + newy * buffer->width) = *background;
                        }*/
                }
                if (*interactable >> 24 != 0xFF) {
                    *memory = *interactable;
                }
                memory++;
                background++;
                interactable++;
            }
        }



        //draw the created image to the screen
        StretchDIBits(hdc, 0, 0, buffer->width, buffer->height, 0, 0, buffer->width, buffer->height, buffer->memory, &buffer->buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
        setWindowColor(0x00000000, buffer, (unsigned int*)buffer->memory);
        setWindowColor(0x00000000, buffer, (unsigned int*)buffer->background);
        //get the time a frame took to render.
        QueryPerformanceCounter(&time.frame_end_time);
        time.delta_time = (float)(time.frame_end_time.QuadPart - time.frame_begin_time.QuadPart) / time.performance_frequency;
        time.frame_begin_time.QuadPart = time.frame_end_time.QuadPart;

    }//end while


    return 0;
}


int proccessMessages(struct Input* keyInput, HWND window, Globals* Global, bufferMem* buffer) {
    //handle all current windows messages
    MSG message;
    int messageReturn = 0;
    int result = 0;
    //get cursor position, convert to coordinates of the window and set our global cursor position;
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(window, &p);
    Global->cursorX = p.x;
    Global->cursorY = p.y;
    while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {

        bool is_down = ((message.lParam & (1 << 31)) == 0);
        for (int i = 0; i < BUTTON_COUNT; i++) {
            keyInput->buttons[i].changed = false;
        }
        switch (message.message) {
        case WM_KEYDOWN:
        case WM_KEYUP: {
            unsigned int vk_code = (unsigned int)message.wParam;
            unsigned int key = vk_code;
            bool is_down = ((message.lParam & (1 << 31)) == 0);
            if (vk_code > 64 && vk_code < 91) {
                unsigned int key = vk_code;
                vk_code = 65;
            }
            else if (vk_code > 47 && vk_code < 74) {
                unsigned int key = vk_code - 17;
                vk_code = 48;
            }
            else if (vk_code == 8) {}
            switch (vk_code) {
            case VK_UP: {
                Global->keyInput->buttons[BUTTON_UP].is_down = is_down;
            }break;
            case VK_DOWN: {
                Global->keyInput->buttons[BUTTON_DOWN].is_down = is_down;

            }break;
            case VK_LEFT: {
                Global->keyInput->buttons[BUTTON_LEFT].is_down = is_down;
            }break;
            case VK_RIGHT: {
                Global->keyInput->buttons[BUTTON_RIGHT].is_down = is_down;
            }break;
            case 65: {
                switch(key) {
                    case 'W': {
                        Global->keyInput->buttons[BUTTON_W].is_down = is_down;
                    }break;
                    case 'A': {
                        Global->keyInput->buttons[BUTTON_A].is_down = is_down;
                    }break;
                    case 'S': {
                        Global->keyInput->buttons[BUTTON_S].is_down = is_down;
                    }break;
                    case 'D': {
                        Global->keyInput->buttons[BUTTON_D].is_down = is_down;
                    }
                }
                messageReturn = key + (5 << 16);
            }break;
            case 48: {
                
                messageReturn = key + (31 << 16);

            }break;
            case 190: {
               
                messageReturn = 46 + (BUTTON_PERIOD << 16);
            }
            case VK_SPACE: {
                Global->keyInput->buttons[BUTTON_SPACE].is_down = is_down;
            }break;
            case VK_RETURN: {
                
            }break;
            case VK_BACK: {
                return messageReturn;
            }break;
            case 187: {
                return messageReturn;
            }break;
            case VK_SHIFT: {

            }break;
            case VK_CONTROL: {
                Global->keyInput->buttons[BUTTON_CONTROL].is_down = is_down;
            }
            default: {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
            }
        }break;
        case WM_LBUTTONUP: {
        }return messageReturn;
        case WM_LBUTTONDOWN: {
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(window, &p);
            Global->cursorX = p.x;
            Global->cursorY = buffer->height - p.y;
            
            Global->keyInput->buttons[BUTTON_MOUSELEFT].is_down = is_down;
        }return messageReturn;
        case WM_CLOSE:
        case WM_DESTROY: {
            Global->runningWindow = false;
        } break;
        case WM_SIZE: {

            //if the window memory == 0 then it hasn't been initialized; Initialize window memory.
            //This if statement is necessary to prevent memory buffers from being deleted upon minimizing the window.
            if (buffer->memory == 0) {
                //get window dimensins
                RECT rect;
                GetClientRect(window, &rect);
                buffer->width = rect.right - rect.left;
                buffer->height = rect.bottom - rect.top;


                int buffer_size = buffer->width * buffer->height * sizeof(unsigned int);

                if (buffer->memory) VirtualFree(buffer->memory, 0, MEM_RELEASE);
                buffer->memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
                buffer->buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer->buffer_bitmap_info.bmiHeader);
                buffer->buffer_bitmap_info.bmiHeader.biWidth = buffer->width;
                buffer->buffer_bitmap_info.bmiHeader.biHeight = buffer->height;
                buffer->buffer_bitmap_info.bmiHeader.biPlanes = 1;
                buffer->buffer_bitmap_info.bmiHeader.biBitCount = 32;
                buffer->buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

                //allocate memory for buffer_background
                if (buffer->background) VirtualFree(buffer->background, 0, MEM_RELEASE);
                buffer->background = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

                //allocate memory for buffer_interactable
                if (buffer->interactable) VirtualFree(buffer->interactable, 0, MEM_RELEASE);
                buffer->interactable = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);


                setWindowColor(0x00000AA, buffer, (unsigned int*)buffer->background);
                setWindowColor(0xFF000000, buffer, (unsigned int*)buffer->interactable);
                printText((char*)"Cosmic Database", 200, 960, buffer, (unsigned int*)buffer->background);
                BMPIN* image = new BMPIN();
                image->Import("Text/System.bmp");
                drawImage(920, 0, 1, image, buffer, (unsigned int*)buffer->background);
                delete image;
            }
        }
        case WM_MOUSEWHEEL: {
            keyInput->buttons[BUTTON_WHEEL].changed = true;
            keyInput->buttons[BUTTON_WHEEL].is_down = true;

            messageReturn = message.wParam >> 16;
            return messageReturn;
        }
        default:
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        if (message.message == 15) {
            return messageReturn;
        }
    }
    return messageReturn;
}
