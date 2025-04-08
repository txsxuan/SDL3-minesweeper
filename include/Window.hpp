#pragma once
#include "SDL3/SDL_init.h"
#include "pch.hpp"
inline auto windowDestroy=[](SDL_Window* window){
    SDL_DestroyWindow(window);
};
class Window final{
public:
    friend class Renderer;
    Window(const std::string &title, int w, int h);
private:
    std::unique_ptr<SDL_Window,decltype(windowDestroy)>  window_=nullptr;
};