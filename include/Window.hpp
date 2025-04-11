#pragma once
// #include "pch.hpp"
#include <utility>
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
    std::pair<int, int> GetPos();
private:
    std::unique_ptr<SDL_Window,decltype(windowDestroy)>  window_=nullptr;
};