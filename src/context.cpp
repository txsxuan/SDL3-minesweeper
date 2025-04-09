#include "context.hpp"
#include "SDL3/SDL_video.h"
#include <cassert>
#include <memory>
#include <stdexcept>
#include <sys/stat.h>
SDL_Texture *LoadTexture(SDL_Renderer *Renderer,const std::string& bmpFilename){
    // SDL_Surface *surface=SDL_LoadBMP(bmpFilename.c_str());
    SDL_Surface *surface=IMG_Load(bmpFilename.c_str());
    if(!surface){
        throw std::runtime_error(std::string("Failed to load bmp ")+bmpFilename+" : "+SDL_GetError());
    }
    SDL_Texture *texture=SDL_CreateTextureFromSurface(Renderer, surface);
    if(!texture){
        throw std::runtime_error(std::string("Failed to create texture from bmp ")+bmpFilename+" : "+SDL_GetError());
    }
    SDL_DestroySurface(surface);
    return texture;
}

void Mapinit(const Uint64 bombnums,Map &map,const int x,const int y){
    int i=0;
    if(bombnums>=map.MaxSize()){
        throw std::runtime_error(std::string("bombnums > = map.MaxSize()"));
    }
    else if(!map.IsIn(x, y)){
        throw std::runtime_error(std::string("x , y out of bound !"));
    }
    for(i=0;i<bombnums;i++){
        map.GetByIndex(i).value=BOMB;
    }
    for(i=map.MaxSize()-2;i>=0;i--){
        std::swap(map.GetByIndex(std::rand()%(i+1)).value,map.GetByIndex(i).value);
    }
    std::swap(map.Get(x, y).value,map.GetByIndex(map.MaxSize()-1).value);
    for(i=0;i<map.Width();i++){
        for(int j=0;j<map.Height();j++){
            if(map.Get(x,y).value==BOMB){
                continue;
            }
            int minecount=0;
            for(int nx=-1;nx<=1;nx++){
                for(int ny=-1;ny<=1;ny++){
                    int dx=i+nx;
                    int dy=j+ny;
                    if(map.IsIn(dx, dy)&&map.Get(dx, dy).value==BOMB){
                        minecount++;
                    }
                }
            }
            map.Get(i,j).value=static_cast<Tilevalue>(minecount);
        }
    }
}


Context::Context(Window &&window_,Renderer &&renderer_):Tilecases(LoadTexture(renderer_.renderer_.get(), "D:/Program/SDL3-minesweeper/resource/cases.bmp"),TextureDestroy),SmileImg(LoadTexture(renderer_.renderer_.get(),"D:/Program/SDL3-minesweeper/resource/smileys.bmp")),Digit( LoadTexture(renderer_.renderer_.get(),"D:/Program/SDL3-minesweeper/resource/digits.bmp")),background(LoadTexture(renderer_.renderer_.get(), "D:/Program/SDL3-minesweeper/resource/warma.jpg"),TextureDestroy),window(std::move(window_)),renderer(std::move(renderer_)),map(NGRIDX,NGRIDY){}


SDL_AppResult Context::Init(){
    if(!instance_){
        std::srand(std::time(0));
        if(!SDL_Init(SDL_INIT_EVENTS|SDL_INIT_EVENTS)){
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        try{
            Window window("YukiniSakuHana",WIDTH*scale,HEIGHT*scale);
            window.SetAlpha(240);
            Renderer renderer(window);
            SDL_Log("%d  %d  ",WIDTH,HEIGHT);
            renderer.setScale(scale,scale);
            instance_=std::unique_ptr<Context>(new Context(std::move(window),std::move(renderer)));
            
        }catch (const std::exception& e){
            SDL_Log("Couldn't initialize : %s",e.what());
            return SDL_APP_FAILURE;
        }

    }
    return SDL_APP_CONTINUE;
}
SDL_AppResult Context::EventHandle(SDL_Event *event){
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}
SDL_AppResult Context::Update(){
    renderer.SetColor({255,255,255,255});
    renderer.Clear(); 
    DrawUI();
    DrawMap();
    DrawSmile();
    DrawBack();

    renderer.Present();
    return SDL_APP_CONTINUE;
}

void Context::DrawUI(){
    SDL_FRect r;

    /* 20px pour le cadre de menus */
    r.x = 0;
    r.y = 0;
    r.w = WIDTH;
    r.h = 20;
    renderer.SetColor(n0xD7D3CE);
    renderer.FillRect(r);
    /* Cadre total (pour les bordures blanches) */
    r.x = 0;
    r.y = 20;
    r.w = WIDTH;
    r.h = HEIGHT;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre total (pour le gris) */
    r.x = 3;
    r.y = 23;
    r.w = WIDTH;
    r.h = HEIGHT;
    renderer.SetColor(n0xC6C3C6);
    renderer.FillRect(r);

    /* Cadre du haut (cadre gris foncé) */
    r.x = 9;
    r.y = 29;
    r.w = NGRIDX * TileLen + 6;
    r.h = 37;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);

    /* Cadre du haut (cadre blanc) */
    r.x = 11;
    r.y = 31;
    r.w = NGRIDX * TileLen + 4;
    r.h = 35;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du haut (cadre gris clair intérieur) */
    r.x = 11;
    r.y = 31;
    r.w = NGRIDX * TileLen + 2;
    r.h = 33;
    renderer.SetColor(n0xC6C3C6);
    renderer.FillRect(r);

    /* Cadre du nombre de mines */
    r.x = OFFSCORX - 1;
    r.y = OFFSCORY - 1;
    r.w = 40;
    r.h = 24;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);
    r.x = OFFSCORX;
    r.y = OFFSCORY;
    r.w = 40;
    r.h = 24;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du temps */
    r.x = WIDTH - 55;
    r.y = OFFSCORY - 1;
    r.w = 40;
    r.h = 24;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);

    r.x = WIDTH - 54;
    r.y = OFFSCORY;
    r.w = 40;
    r.h = 24;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du bas */
    r.x = OFFGRIDX - 3;
    r.y = OFFGRIDY - 3;
    r.w = NGRIDX * TileLen + 6;
    r.h = NGRIDY * TileLen + 6;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);


    r.x = OFFGRIDX;
    r.y = OFFGRIDY;
    r.w = NGRIDX * TileLen + 3;
    r.h = NGRIDY * TileLen + 3;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);
}

void Context::DrawMap(){
    for(int i=0;i<NGRIDX;i++){
        for(int j=0;j<NGRIDY;j++){
            if(map.Get(i,j).isCover==true){
                SDL_FRect srect{9*static_cast<float>(Tilecases->w)/16,0,static_cast<float>(Tilecases->w)/16,static_cast<float>(Tilecases->h)};
                float x=i*TileLen+OFFGRIDX;
                float y=j*TileLen+OFFGRIDY;
                renderer.DrawTexture(Tilecases.get(), srect, x, y);
            }
        }
    }
}
void Context::DrawBack(){
    if(!SDL_SetTextureAlphaMod(background.get(), 50)||!SDL_SetTextureBlendMode(background.get(), SDL_BLENDMODE_BLEND)){
        throw std::runtime_error(std::string("Set background failed !")+SDL_GetError());

    }
    renderer.DrawTexture(background.get());
}

void Context::DrawSmile() {
  SDL_FRect r;
      
  r.y = 0;
  r.w = SmileImg.get()->h;
  r.h =SmileImg.get()->h;
   
  if (state ==Win)
    r.x = r.w * 3;
  else if (state == Explode)
    r.x = r.w * 2;
  else {
    if (Mstate==clickSmile)
      r.x = r.w * 4;
    else if (Mstate ==None)
      r.x = 0;
    else if (Mstate==lHold||Mstate==rHold||Mstate==dHold) {
               r.x = r.w * 1;
             }
    else
      r.x = 0;
  }
renderer.DrawTexture(SmileImg.get(), r,static_cast<float>(WIDTH - SmileImg.get()->h )/ 2, OFFSCORY - 1);
}