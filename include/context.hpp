#pragma once
#include "Renderer.hpp"
#include "SDL3/SDL_events.h"
#include "pch.hpp"
#include "window.hpp"
enum {
    NGRIDX = 30,
    NGRIDY = 16,
    NMINES = 99,
    OFFGRIDX = 12,
    OFFGRIDY = 75,
    OFFSCORX = 17,
    OFFSCORY = 36,
    WIDTH = NGRIDX * 16 + OFFGRIDX + 8,
    HEIGHT = NGRIDY * 16 + OFFGRIDY + 8,
};
inline auto TextureDestroy = [](SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
};
struct  Context final{
    enum GameState {
        Gaming,
        Win,
        Explode,
    };
    GameState state = Gaming;
    Window window;
    Renderer renderer;


    static void Quit(){instance_.reset();};
    static SDL_AppResult Init();
    static Context &Inst(){
        assert(instance_);
        return *instance_;
    }


    SDL_AppResult EventHandle(SDL_Event *);
    SDL_AppResult Update();

private:
    Context(Window &&window,Renderer &&renderer);
    inline static std::unique_ptr<Context> instance_=nullptr;

};