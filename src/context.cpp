#include "context.hpp"
#include "Matrix.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_timer.h"
#include "Window.hpp"
#include <cassert>
#include <setjmp.h>
SDL_Texture *LoadTexture(SDL_Renderer *Renderer,const std::string& bmpFilename,bool isbmp){
    // SDL_Surface *surface=SDL_LoadBMP(bmpFilename.c_str());

    SDL_Surface *surface;
    if(!isbmp){
        surface=IMG_Load(bmpFilename.c_str());
    }
    else{
        surface=SDL_LoadBMP(bmpFilename.c_str());
    }
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
void Mapinit(const Uint64 bombnums,Map &map){
    int i=0;
    if(bombnums>=map.MaxSize()){
        throw std::runtime_error(std::string("bombnums > = map.MaxSize()"));
    }
    for(i=0;i<map.MaxSize();i++){
        map.GetByIndex(i).isCover=true;
        map.GetByIndex(i).isflag=false;
        if(i<bombnums){
            map.GetByIndex(i).value=BOMB;
        }
        else{
            map.GetByIndex(i).value=static_cast<Tilevalue>(0);
        }
    }
    for(i=map.MaxSize()-2;i>=0;i--){
        std::swap(map.GetByIndex(std::rand()%(i+1)).value,map.GetByIndex(i).value);
    }
        for(i=0;i<map.Width();i++){
        for(int j=0;j<map.Height();j++){
            if(map.Get(i,j).value==BOMB){
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


Context::Context(Window &&window_,Renderer &&renderer_):Tilecases(LoadTexture(renderer_.renderer_.get(), "./resource/cases.png",false),TextureDestroy),SmileImg(LoadTexture(renderer_.renderer_.get(),"./resource/smileys.bmp")),Digit( LoadTexture(renderer_.renderer_.get(),"./resource/digits.bmp")),background(LoadTexture(renderer_.renderer_.get(), "./resource/warma.jpg",false),TextureDestroy),window(std::move(window_)),renderer(std::move(renderer_)),map(NGRIDX,NGRIDY){
    Mapinit(NMINES,map);
    nscale=static_cast<float>(TileLen)/static_cast<float>(Tilecases->h);
}


SDL_AppResult Context::Init(){
    if(!instance_){
        std::srand(std::time(0));
        if(!SDL_Init(SDL_INIT_EVENTS|SDL_INIT_VIDEO)){
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        try{
            Window window("MineSweeper",WIDTH*scale,HEIGHT*scale);
            window.SetAlpha(255);
            Renderer renderer(window);
            SDL_Log("%d  %d  ",WIDTH,HEIGHT);
            renderer.setScale(scale,scale);
            instance_=std::unique_ptr<Context>(new Context(std::move(window),std::move(renderer)));
            // Mapinit(NMINES,map);
            
        }catch (const std::exception& e){
            SDL_Log("Couldn't initialize : %s",e.what());
            return SDL_APP_FAILURE;
        }

    }
    return SDL_APP_CONTINUE;
}
SDL_AppResult Context::EventHandle(SDL_Event *event){
    mouse.Posx=event->motion.x/scale;
    mouse.Posy=event->motion.y/scale;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    else {
        if(mouse.Posy<OFFGRIDY){
            if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN){
                mouse.Mstate=clickSmile;
                SDL_Log("clickSmile !");
                return SDL_APP_CONTINUE;
            }
        }
        int X=static_cast<int>((mouse.Posx-OFFGRIDX)/TileLen);
        int Y=static_cast<int>((mouse.Posy-OFFGRIDY)/TileLen);
        // SDL_Log("%f %f %d %d %d %d",mouse.Posx,mouse.Posy,OFFGRIDX,OFFGRIDY,X,Y);
        if(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN){
            if(state==Win||state==Explode){
                return SDL_APP_CONTINUE;
            }
            if(event->button.button==1){
                if(!map.IsIn(X,Y)){
                    return SDL_APP_CONTINUE;//很奇怪的逻辑，看起来只有左键受到棋盘的影响
                }
            }
            if(event->button.button==2||(event->button.button==1&&mouse.Mstate==rHold)||(event->button.button==3&&mouse.Mstate==lHold)){
                mouse.Mstate=dHold;
                SDL_Log("Double click hold !");
            }
            else if(event->button.button==1){
                mouse.Mstate=lHold;
                SDL_Log("Left click hold !");
            }
            else if(event->button.button==3){
                if(!map.IsIn(X,Y)){
                    return SDL_APP_CONTINUE;
                }
                if(map.Get(X,Y).isCover){
                    map.Get(X,Y).isflag=!(map.Get(X,Y).isflag);
                    hasflagged+=((map.Get(X,Y).isflag)?1:-1);
                }
                mouse.Mstate=rHold;
                SDL_Log("Right click hold !");
            }
            // else{
            //     SDL_Log("Other buttun clicked down ! : %d",event->button.button);
            // }
        }
        else if(event->type==SDL_EVENT_MOUSE_BUTTON_UP){
            if(event->button.button==1||event->button.button==2||event->button.button==3){
                mouse.Pstate=mouse.Mstate;
                mouse.Mstate=None;
            }
            MouseHandle(X, Y);
            SDL_Log("Release buttun %d",event->button.button);
            if(mouse.Pstate==clickSmile){
                SDL_Log("Release smlie");
            }

        }
    }
    return SDL_APP_CONTINUE;
}
SDL_AppResult Context::Update(){
    // mouse.GetPos();
    if(state==NGaming){
        renderer.SetColor({255,255,255,255});
        renderer.Clear();
    }
     
    DrawUI();
    DrawMap();
    DrawSmile();
    DrawremainedMines();
    DrawTime();
    // DrawBack();

    renderer.Present();
    return SDL_APP_CONTINUE;
}

void Context::DrawUI(){
    SDL_FRect r;

    /* 20px pour le cadre de menus */
    r.x = 0;
    r.y = 0;
    r.w = WIDTH;
    r.h = 20*2;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);
    /* Cadre total (pour les bordures blanches) */
    r.x = 0;
    r.y = 20;
    r.w = WIDTH;
    r.h = HEIGHT;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre total (pour le gris) */
    r.x = 3*2;
    r.y = 23*2;
    r.w = WIDTH;
    r.h = HEIGHT;
    renderer.SetColor(n0xC6C3C6);
    renderer.FillRect(r);

    /* Cadre du haut (cadre gris foncé) */
    r.x = 9*2;
    r.y = 29*2;
    r.w = NGRIDX * TileLen + 6*2;
    r.h = 37*2;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);

    /* Cadre du haut (cadre blanc) */
    r.x = 11*2;
    r.y = 31*2;
    r.w = NGRIDX * TileLen + 4*2;
    r.h = 35*2;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du haut (cadre gris clair intérieur) */
    r.x = 11*2;
    r.y = 31*2;
    r.w = NGRIDX * TileLen + 2*2;
    r.h = 33*2;
    renderer.SetColor(n0xC6C3C6);
    renderer.FillRect(r);

    /* Cadre du nombre de mines */
    r.x = OFFSCORX - 1*2;
    r.y = OFFSCORY - 1*2;
    r.w = 40*2;
    r.h = 24*2;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);
    r.x = OFFSCORX;
    r.y = OFFSCORY;
    r.w = 40*2;
    r.h = 24*2;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du temps */
    r.x = WIDTH - 55*2;
    r.y = OFFSCORY - 1*2;
    r.w = 40*2;
    r.h = 24*2;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);

    r.x = WIDTH - 54*2;
    r.y = OFFSCORY;
    r.w = 40*2;
    r.h = 24*2;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);

    /* Cadre du bas */
    r.x = OFFGRIDX - 3*2;
    r.y = OFFGRIDY - 3*2;
    r.w = NGRIDX * TileLen + 6*2;
    r.h = NGRIDY * TileLen + 6*2;
    renderer.SetColor(n0x848284);
    renderer.FillRect(r);


    r.x = OFFGRIDX;
    r.y = OFFGRIDY;
    r.w = NGRIDX * TileLen + 3*2;
    r.h = NGRIDY * TileLen + 3*2;
    renderer.SetColor(n0xFFFFFF);
    renderer.FillRect(r);
}
void Context::DrawMap(){
    // SDL_FRect srect{static_cast<float>(Tilecases->w)/16,0,static_cast<float>(Tilecases->w)/16,static_cast<float>(Tilecases->h)};
    SDL_FRect srect{static_cast<float>(Tilecases->h),0,static_cast<float>(Tilecases->h),static_cast<float>(Tilecases->h)};
    for(int i=0;i<NGRIDX;i++){
        for(int j=0;j<NGRIDY;j++){
            float x=i*TileLen+OFFGRIDX;
            float y=j*TileLen+OFFGRIDY;
            srect.x=static_cast<float>(Tilecases->h);
            auto const &grid=map.Get(i,j);
            if(grid.isflag){
                if(state==Explode&&grid.value!=BOMB){
                    srect.x*=WFLAG;
                }else{
                    srect.x*=FLAG;
                }
            }
            else if(grid.isCover==true){
                if(state==Explode&&grid.value==BOMB){
                    srect.x*=static_cast<float>(grid.value);
                }
                else{
                    srect.x*=UNREV;
                }
            }
            else{
                if(state==Explode&&grid.value==BOMB){
                    srect.x*=EXPLODE;
                }else{
                    srect.x*=static_cast<float>(grid.value);
                }
            }
            // srect.x+=0.2;
            renderer.DrawTexture(Tilecases.get(), srect,{x,y,static_cast<float>(TileLen),static_cast<float>(TileLen)});
        }
    }
    // if(mouse.Posx<OFFGRIDX||mouse.Posy<OFFGRIDY){
    //     return;
    // }
    if(mouse.Mstate==lHold){
        int X=static_cast<int>((mouse.Posx-OFFGRIDX)/TileLen);
        int Y=static_cast<int>((mouse.Posy-OFFGRIDY)/TileLen);
        // SDL_Log("%f %f %d %d %d %d",mouse.Posx,mouse.Posy,OFFGRIDX,OFFGRIDY,X,Y);
        if(!map.IsIn(X,Y)){
            return;
        }
        auto const &grid=map.Get(X,Y);
        if(!grid.isflag&&grid.isCover){
            srect.x=0;
            renderer.DrawTexture(Tilecases.get(), srect, {static_cast<float>(X*TileLen+OFFGRIDX), static_cast<float>(Y*TileLen+OFFGRIDY),static_cast<float>(TileLen),static_cast<float>(TileLen)});
        }
    }
    else if(mouse.Mstate==dHold){
        int X=static_cast<int>((mouse.Posx-OFFGRIDX)/TileLen);
        int Y=static_cast<int>((mouse.Posy-OFFGRIDY)/TileLen);
        // SDL_Log("%f %f %d %d %d %d",mouse.Posx,mouse.Posy,OFFGRIDX,OFFGRIDY,X,Y);
        if(!map.IsIn(X,Y)){
            return;
        }
        for(int i=X-1;i<=X+1;i++){
            for(int j=Y-1;j<=Y+1;j++){
                if(!map.IsIn(i,j)){
                    // SDL_Log("asdadsa");
                    continue;
                }
                auto const &grid=map.Get(i,j);
                if(!grid.isflag&&grid.isCover){
                    srect.x=0;
                    renderer.DrawTexture(Tilecases.get(), srect, {static_cast<float>(i*TileLen+OFFGRIDX), static_cast<float>(j*TileLen+OFFGRIDY),static_cast<float>(TileLen),static_cast<float>(TileLen)});
                }
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
    if (mouse.Mstate==clickSmile)
      r.x = r.w * 4;
    else if (mouse.Mstate==None)
      r.x = 0;
    else if (mouse.Mstate==lHold||mouse.Mstate==rHold||mouse.Mstate==dHold) {
               r.x = r.w * 1;
             }
    else
      r.x = 0;
  }
// renderer.DrawTexture(SmileImg.get(), r,static_cast<float>(WIDTH - SmileImg->h )/ 2, OFFSCORY - 1);
renderer.DrawTexture(SmileImg.get(), r,{static_cast<float>(WIDTH - SmileImg->h*nscale )/ 2, OFFSCORY - 1,static_cast<float>(TileLen*SmileImg->h)/static_cast<float>(Tilecases->h),static_cast<float>(TileLen*SmileImg->h)/static_cast<float>(Tilecases->h)});
}
void Context::DrawDigit(SDL_FRect & s,int n){
    SDL_FRect r;
    char str[4];
    Uint32 i;
    r.y = 0;
    r.w = Digit->w/11;
    r.h = Digit->h;
    // SDL_Log("r.w %f r.h %f",r.w,r.h);
    // int n=NMINES-hasflagged;
    if (n < -99){
        n = -99;
    }
    else if (n > 999){
        n = 999;
    }
    sprintf(str, "%03d", n);
    for (i = 0; i < 3; i++) {
        if (str[i] == '-'){ 
            r.x = 10 * r.w;
        }
        else{
            r.x = (str[i] - '0') * r.w;
        }
        renderer.DrawTexture(Digit.get(),r,s);
        s.x += r.w*nscale;
    }

}
void Context::DrawTime(){
    SDL_FRect r;
    int n = 0;
    if(state==Gaming){
        currentms=SDL_GetTicks();
    }
    n=(currentms-startms)/1000;
    if(state==NGaming){
        n=0;
    }
    r.x = WIDTH - 108;
    r.y = OFFSCORY;
    r.w = Digit->w/11*nscale;
    r.h = Digit->h*nscale;
    DrawDigit(r,n);
}
void Context::DrawremainedMines(){
    SDL_FRect r;
    int n = NMINES-hasflagged;
    r.x = OFFSCORX;
    r.y = OFFSCORY;
    r.w = Digit->w/11*nscale;
    r.h = Digit->h*nscale;
    
    DrawDigit(r,n);
} 

void Context::MouseHandle(int X,int Y){
    if(mouse.Pstate==clickSmile){
        state=NGaming;
        hasREV=0;
        Mapinit(NMINES,map);
        SDL_Log("hint");
        return;
    }
    if(!map.IsIn(X,Y)||state==Explode||state==Win){
        return;
    }
    auto &grid=map.Get(X,Y);
    if(mouse.Pstate==lHold){
        if(state==NGaming){
            state=Gaming;
            startms=SDL_GetTicks();
            currentms=startms;
            if(grid.value==BOMB){
                std::swap(map.GetByIndex(map.MaxSize()-1).value,grid.value);
                SDL_Log("is bomb!");
                if(grid.value==BOMB){
                    SDL_Log("fail !");
                }
                int count=0;
                for(int x=X-1;x<=X+1;x++){
                    for(int y=Y-1;y<=Y+1;y++){
                        if(!map.IsIn(x,y)){
                            continue;
                        }
                        if(x==X&&y==Y){
                            continue;
                        }
                        if(map.Get(x,y).value!=BOMB){
                            map.Get(x,y).value=static_cast<Tilevalue>(static_cast<int>(map.Get(x,y).value)-1);
                        }
                        else{
                            count++;
                        }
                        grid.value=static_cast<Tilevalue>(count);
                        SDL_Log("a :%d %d %d",x,y,static_cast<int>(map.Get(x,y).value));
                    }
                }

                for(int x=NGRIDX-2;x<NGRIDX;x++){
                    for(int y=NGRIDY-2;y<NGRIDY;y++){
                        if(map.Get(x,y).value!=BOMB){
                            map.Get(x,y).value=static_cast<Tilevalue>(static_cast<int>(map.Get(x,y).value)+1);
                        }
                            SDL_Log("b :%d",static_cast<int>(map.Get(x,y).value));
                    }
                }               
            }
            // Mapinit(NMINES, map, X,Y);
        }
        if(!grid.isCover||grid.isflag){
            return;
        }
        floodFill(X,Y);
        
    }
    else if(mouse.Pstate==dHold){
        if(grid.isCover||grid.isflag){
            return;
        }
        int count=0;
        for(int x=X-1;x<=X+1;x++){
            for(int y=Y-1;y<=Y+1;y++){
                if(map.IsIn(x,y)&&map.Get(x,y).isflag){
                    count++;
                }
            }
        }
        SDL_Log("count = %d",count);
        if(count==static_cast<int>(grid.value)){
            for(int x=X-1;x<=X+1;x++){
                for(int y=Y-1;y<=Y+1;y++){
                    floodFill(x,y);
                }
            }
        }
    }
    if((hasREV+NMINES==NGRIDX*NGRIDY)&&state==Gaming){
        state=Win;
    }

}
void Context::floodFill(int X,int Y){
    if(!map.IsIn(X,Y)){
        return;
    }
    auto &grid=map.Get(X,Y);
    if(!grid.isCover||grid.isflag){
            return;
        }
    if(grid.value==BOMB){
        grid.isCover=false;
        hasREV++;
        state=Explode;
        return ;
    }
    else if(grid.value==0){
        grid.isCover=false;
        hasREV++;
        floodFill(X-1,Y-1);
        floodFill(X-1,Y);
        floodFill(X-1,Y+1);
        floodFill(X+1,Y-1);
        floodFill(X+1,Y);
        floodFill(X+1,Y+1);
        floodFill(X,Y-1);
        floodFill(X,Y+1);
    }
    else{
        grid.isCover=false;
        hasREV++;
    }

}
