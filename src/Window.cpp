#include "Window.hpp"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_video.h"
#include <stdexcept>
#include <string>
Window::Window(const std::string &title, int w, int h):window_(SDL_CreateWindow(title.c_str(),w,h,SDL_WINDOW_HIGH_PIXEL_DENSITY),windowDestroy){
    SDL_Log("%f",SDL_GetWindowDisplayScale(window_.get()));
    if(!window_){
        throw std::runtime_error(std::string("Failed to create window : ")+SDL_GetError());
    }
}

std::pair<int, int> Window::GetPos(){
    int x,y;
    if(!SDL_GetWindowPosition(window_.get(), &x,&y)){
        throw std::runtime_error(std::string("failed to get windowPosition : ")+SDL_GetError());
    }
    return {x,y};
}