#pragma once
// #include "Matrix.hpp"
#include "Renderer.hpp"
#include "SDL3/SDL_stdinc.h"


constexpr int NGRIDX = 30;
constexpr int NGRIDY = 16;
constexpr int NMINES = 99;
constexpr int OFFGRIDX = 24;
constexpr int OFFGRIDY = 150;
constexpr int OFFSCORX = 34;
constexpr int OFFSCORY = 72;
constexpr int TileLen=32;
constexpr int WIDTH = NGRIDX * TileLen+ OFFGRIDX + 16;
constexpr int HEIGHT = NGRIDY * TileLen + OFFGRIDY + 16;

constexpr int   UNREV=9;
constexpr int   FLAG=10;
constexpr int   EXPLODE=14;
constexpr int   WFLAG=15;

constexpr float scale=1.5;


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
void Mapinit(const Uint64 bombnums,Map &map);
inline auto TextureDestroy = [](SDL_Texture* texture) {
    SDL_DestroyTexture(texture);
};

SDL_Texture *LoadTexture(SDL_Renderer *Renderer,const std::string& bmpFilename,bool isbmp=true);

struct  Context final{
    float nscale;
    enum GameState {
        NGaming,
        Gaming,
        Win,
        Explode,
    };
    struct Mouse{
        float Posx;
        float Posy;
        MouseState Pstate=None;
        MouseState Mstate=None; 
        void GetPos(){
            SDL_GetMouseState(&Posx,&Posy);
            Posx/=scale;
            Posy/=scale;
        }
    }mouse;

    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> Tilecases; 
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> SmileImg;
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> Digit;
    std::unique_ptr<SDL_Texture,decltype(TextureDestroy)> background;
    Map map;
    GameState state = NGaming;//最开始的时候，也是计时器开始前
    Window window;
    Renderer renderer;


    Uint64 currentms=0;
    Uint64 startms=0;
    int hasREV=0;
    int hasflagged=0;
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
    void DrawDigit(SDL_FRect&,int);
    void DrawTime();
    void DrawremainedMines();

    SDL_AppResult EventHandle(SDL_Event *);
    void MouseHandle(int X,int Y);
    void floodFill(int X,int Y);
    SDL_AppResult Update();

private:
    Context(Window &&window,Renderer &&renderer);
    inline static std::unique_ptr<Context> instance_=nullptr;

};