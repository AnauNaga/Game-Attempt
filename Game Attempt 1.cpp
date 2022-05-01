
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

//local includes
#include "KeyTracker.h"
#include "BMPIO.h"
#include "BMPOUT.h"
#include "BMPIN.h"
#include "GuiUtilites.h"

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
    HWND window = CreateWindow(window_class.lpszClassName, L"Cosmic Database", WS_OVERLAPPED | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0, 1936, 1040, 0, 0, hInstance, 0);
    HDC hdc = GetDC(window);
    mainMenu(window, hdc, Global, buffer);
}//end winMain

int mainMenu(HWND window, HDC hdc,Globals* Global, struct bufferMem* buffer) {


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


    //preload mars and Planet images
    BMPIN* Mars = new BMPIN();
    Mars->Import("text/Mars.bmp", 0x00000000);
    BMPIN* Planet = new BMPIN();
    Planet->Import("text/Planet.bmp", 0x00000000);
    BMPIN* Sun = new BMPIN();
    Sun->Import("text/Sun.bmp", 0x00000000);
    int SunX = 1475;
    int SunY = 450;
    double PlanetTheta = 3.14;
    double moonTheta = 0;
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
    BMPIN* image = new BMPIN();
    image->Import("Planet.bmp", 0x00000000);
    drawImage(500, 500, 1, image, buffer, (unsigned int*)buffer->background);
    DrawRect(0, 0, buffer->width, 100, 0x0000FF00, buffer, (unsigned int*)buffer->background);
    Pos pos;
    Pos speed{0,0};
    Pos center{ 900,500 };
    Pos vector{0,0};
    Pos acceleration{ 0,0 };
    float massObject = 10;
    float massPlanet = 10000;
    float distance = 0;
    DrawRect(900, 500, 905, 505, 0x0000FF00, buffer, (unsigned int*)buffer->background);
    float theta = 0;
    speed.y = -5;
    int scroll = 0.0f;
    //run the window
    while (Global->runningWindow) {


        messageReturn = proccessMessages(Global->keyInput, window, Global, buffer);

        if (Global->keyInput->buttons[BUTTON_W].is_down) {
            speed.y += 100 * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_A].is_down) {
            speed.x -= 100 * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_D].is_down) {
            speed.x += 100 * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_S].is_down) {
            speed.y -= 100 * time.delta_time;
        }
        if (Global->keyInput->buttons[BUTTON_SPACE].is_down) {
            blur(buffer, (unsigned int*)buffer->background);
            Global->keyInput->buttons[BUTTON_SPACE].is_down = false;
        }
        vector.x = (center.x - pos.x);
        vector.y = (center.y - pos.y);
        distance = sqrt(vector.x * vector.x + vector.y * vector.y);

        if (vector.x != 0) {
            theta = atan((float)vector.y / (float)vector.x);
        }
        if (vector.x < 0) {
            theta += 3.1415f;
        }
        acceleration.x = ((massObject * massPlanet)/(distance * distance)) * cos(theta);
        acceleration.y = ((massObject * massPlanet) / (distance * distance)) * sin(theta);

        speed.x = speed.x + acceleration.x * time.delta_time;
        pos.x = pos.x + speed.x * time.delta_time;
        speed.y = speed.y + acceleration.y * time.delta_time;
        pos.y = pos.y + speed.y * time.delta_time;

        if (pos.y < 100) {
            pos.y = 100;
        }
        time.timeKeeperAlpha += time.delta_time;
        if (time.timeKeeperAlpha > 1) {
        
            setPixelColor(pos.x + 5, pos.y + 5, 0x0000FF00, buffer, (unsigned int*)buffer->background);
            time.timeKeeperAlpha = 0;
        }


        printNumber((int)pos.x, 103, 103, buffer, (unsigned int*)buffer->interactable);
        printNumber((int)pos.y, 203, 103, buffer, (unsigned int*)buffer->interactable);
        //combine all the bitmap layers
        unsigned int* background = (unsigned int*)buffer->background;
        unsigned int* memory = (unsigned int*)buffer->memory;
        unsigned int* interactable = (unsigned int*)buffer->interactable;
        for (int x = 0; x < buffer->width; x++) {
            for (int y = 0; y < buffer->height; y++) {
                if (*background >> 24 != 0xFF) {
                    *memory = *background;
                }
                if (*interactable >> 24 != 0xFF) {
                    *memory = *interactable;
                }
                memory++;
                background++;
                interactable++;
            }
        }

        DrawRect(pos.x, pos.y, pos.x + 10, pos.y + 10, 0x0000FF00, buffer, (unsigned int*)buffer->memory);


        //draw the created image to the screen
        StretchDIBits(hdc, 0, 0, buffer->width, buffer->height, 0, 0, buffer->width, buffer->height, buffer->memory, &buffer->buffer_bitmap_info, DIB_RGB_COLORS, SRCCOPY);
        setWindowColor(0xFF000000, buffer, (unsigned int*)buffer->interactable);

        //get the time a frame took to render.
        QueryPerformanceCounter(&time.frame_end_time);
        time.delta_time = (float)(time.frame_end_time.QuadPart - time.frame_begin_time.QuadPart) / time.performance_frequency;
        time.frame_begin_time.QuadPart = time.frame_end_time.QuadPart;

    }//end while

    return 0;
}

void blur(bufferMem* buffer, unsigned int* buffer_memory)
{
    void* tempBuffervoid = malloc(buffer->width * buffer->height * 4);
    unsigned int* tempbuffer;
    tempbuffer = (unsigned int*)tempBuffervoid;
    setWindowColor(0x00000000, buffer, tempbuffer);
    unsigned int newcolor;
    unsigned int* pixel1;
    unsigned int* pixel2;
    unsigned int* pixel3;
    unsigned int* pixel4;
    unsigned int* pixel5;
    unsigned int* pixel6;
    unsigned int* pixel7;
    unsigned int* pixel8;
    unsigned int* pixel9;
    tempbuffer += buffer->width;
    for (int y = 1; y < buffer->height - 1; y++) {
        tempbuffer++;
        for (int x = 1; x < buffer->width - 1; x++) {
            pixel1 = buffer_memory + x + y * buffer->width;
            pixel2 = buffer_memory + x + (y-1) * buffer->width;
            pixel3 = buffer_memory + x + (y + 1) * buffer->width;
            pixel4 = buffer_memory + x + 1 + y * buffer->width;
            pixel5 = buffer_memory + x + 1 + (y - 1) * buffer->width;
            pixel6 = buffer_memory + x + 1 + (y + 1) * buffer->width;
            pixel7 = buffer_memory + x - 1 + y * buffer->width;
            pixel8 = buffer_memory + x - 1 + (y - 1) * buffer->width;
            pixel9 = buffer_memory + x - 1 + (y + 1) * buffer->width;
            newcolor = 0;
            newcolor = newcolor + ((((*pixel1 & 0x000000FF) + (*pixel2 & 0x000000FF) + (*pixel3 & 0x000000FF) + (*pixel4 & 0x000000FF) + (*pixel5 & 0x000000FF) + (*pixel6 & 0x000000FF) + (*pixel7 & 0x000000FF) + (*pixel8 & 0x000000FF) + (*pixel9 & 0x000000FF)) / 9) & 0x000000FF);
            newcolor = newcolor + ((((*pixel1 & 0x0000FF00) + (*pixel2 & 0x0000FF00) + (*pixel3 & 0x0000FF00) + (*pixel4 & 0x0000FF00) + (*pixel5 & 0x0000FF00) + (*pixel6 & 0x0000FF00) + (*pixel7 & 0x0000FF00) + (*pixel8 & 0x0000FF00) + (*pixel9 & 0x0000FF00)) / 9) & 0x0000FF00);
            newcolor = newcolor + ((((*pixel1 & 0x00FF0000) + (*pixel2 & 0x00FF0000) + (*pixel3 & 0x00FF0000) + (*pixel4 & 0x00FF0000) + (*pixel5 & 0x00FF0000) + (*pixel6 & 0x00FF0000) + (*pixel7 & 0x00FF0000) + (*pixel8 & 0x00FF0000) + (*pixel9 & 0x00FF0000)) / 9) & 0x00FF0000);

            *tempbuffer = newcolor;
            tempbuffer++;
        }
        tempbuffer ++;
    }

    tempbuffer = (unsigned int*)tempBuffervoid;
    for (int i = 0; i < buffer->width * buffer->height; i++) {
        
        *(buffer_memory + i) = *(tempbuffer + i);
    }


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
                
            }break;
            case VK_DOWN: {
                

            }break;
            case VK_LEFT: {
                
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
            p.x;
            p.y;
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
