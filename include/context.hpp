#pragma once
#include "Matrix.hpp"
#include "Renderer.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "pch.hpp"
#include "window.hpp"
#include <memory>


constexpr int NGRIDX = 30;
constexpr int NGRIDY = 16;
constexpr int NMINES = 99;
constexpr int OFFGRIDX = 12;
constexpr int OFFGRIDY = 75;
constexpr int OFFSCORX = 17;
constexpr int OFFSCORY = 36;
constexpr int TileLen=16;
constexpr int WIDTH = NGRIDX * TileLen+ OFFGRIDX + 8;
constexpr int HEIGHT = NGRIDY * TileLen + OFFGRIDY + 8;
constexpr float scale=2;

constexpr SDL_Color n0xD7D3CE{0xD7, 0xD3, 0xCE,255};
constexpr SDL_Color n0xFFFFFF{0xFF, 0xFF, 0xFF,255};
constexpr SDL_Color n0xC6C3C6{0xC6, 0xC3, 0xC6,255};
constexpr SDL_Color n0x848284{0x84, 0x82, 0x84,255};


enum MouseState{
    None,//没有任何一个键按下
    lHold,//按下左键
    rHold,//按下右键
    dHold,//lHold时按下右键或者rHold时按下左键
    clickSmile

};
enum{//用于绘制其他图片
    UNREV=9,
    FLAG=10,
    EXPLODE=14,
    WFLAG=15,
};
enum Tilevalue{
    REV=0,
    BOMB=13,
};
struct Tile{
    Tilevalue value=static_cast<Tilevalue>(0);
    bool isCover=true;
    bool isflag=false;
};
using Map=Matrix<Tile>;

void Mapinit(const Uint64 bombnums,Map &map,const int x,const int y);

inline auto TextureDestroy = [](SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
};

SDL_Texture *LoadTexture(SDL_Renderer *Renderer,const std::string& bmpFilename);

struct  Context final{
    enum GameState {
        NGaming,
        Gaming,
        Win,
        Explode,
    };
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> Tilecases; 
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> SmileImg;
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> Digit;
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> background;
    Map map;
    GameState state = NGaming;//最开始的时候，也是计时器开始前
    MouseState Mstate=None; 
    Window window;
    Renderer renderer;



    int hasREV=0;
    static void Quit(){instance_.reset();};
    static SDL_AppResult Init();
    static Context &Inst(){
        assert(instance_);
        return *instance_;
    }

    void DrawUI();
    void DrawMap();
    void DrawBack();
    void DrawSmile();
    SDL_AppResult EventHandle(SDL_Event *);
    SDL_AppResult Update();

private:
    Context(Window &&window,Renderer &&renderer);
    inline static std::unique_ptr<Context> instance_=nullptr;

};