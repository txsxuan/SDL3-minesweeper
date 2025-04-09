#include "context.hpp"
#include <SDL3/SDL_main.h>
extern "C" SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]){
    return Context::Init();
}

extern "C" SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event){
    auto &instance=Context::Inst();
    return instance.EventHandle(event);
}

extern "C" SDL_AppResult SDL_AppIterate(void *appstate){
    auto &instance=Context::Inst();
    return instance.Update();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    return Context::Quit();
}

