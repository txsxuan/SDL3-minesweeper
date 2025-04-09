#pragma once
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include "pch.hpp"
#include <stdexcept>
#include <string>
inline auto windowDestroy=[](SDL_Window* window){
    SDL_DestroyWindow(window);
};
class Window final{
public:
    friend class Renderer;
    Window(const std::string &title, int w, int h);
    void SetAlpha(Uint8 alpha){
        if(!SDL_SetWindowOpacity(window_.get(), static_cast<float>(alpha)/255)){
            throw std::runtime_error(std::string("Failed to set the Alpha of window")+SDL_GetError());
        }
    }
private:
    std::unique_ptr<SDL_Window,decltype(windowDestroy)>  window_=nullptr;
};