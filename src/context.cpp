#include "context.hpp"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include <memory>
// Context::instance_=nullptr;
Context::Context(Window &&window_,Renderer &&renderer_):window(std::move(window_)),renderer(std::move(renderer_)){}
SDL_AppResult Context::Init(){
    if(!instance_){

        if(!SDL_Init(SDL_INIT_EVENTS|SDL_INIT_EVENTS)){
            SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        try{
            Window window("YukiniSakuHana",WIDTH,HEIGHT);
            Renderer renderer(window);
            instance_=std::unique_ptr<Context>(new Context(std::move(window),std::move(renderer)));
            
        }catch (const std::exception& e){
            SDL_Log("Couldn't initialize : %s",e.what());
            return SDL_APP_FAILURE;
        }


    }
    return SDL_APP_CONTINUE;
}
SDL_AppResult Context::EventHandle(SDL_Event *){

}
SDL_AppResult Context::Update(){

}