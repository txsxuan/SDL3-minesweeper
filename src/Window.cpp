#include "Window.hpp"
Window::Window(const std::string &title, int w, int h):window_(SDL_CreateWindow(title.c_str(),w,h,0),windowDestroy){
    if(!window_){
        throw std::runtime_error(std::string("Failed to create window : ")+SDL_GetError());
    }
}