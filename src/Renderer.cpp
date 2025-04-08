#include "Renderer.hpp"
#include "SDL3/SDL_error.h"
#include "Window.hpp"
#include <stdexcept>
Renderer::Renderer(const Window& window):renderer_(SDL_CreateRenderer(window.window_.get(),nullptr),RenderDestroy){
    if(!renderer_){
        throw std::runtime_error(std::string("Failed to initialize Renderer : ")+SDL_GetError());
    }
}
void Renderer::SetColor(const SDL_Color& color){
    SDL_SetRenderDrawColor(renderer_.get(),color.r,color.g,color.b,color.a);
}
void Renderer::SetTarget(SDL_Texture *texture){
    SDL_SetRenderTarget(renderer_.get(),texture);
}
void Renderer::Clear(){
    SDL_RenderClear(renderer_.get());
}
void Renderer::DrawPoint(float x,float y){
    SDL_RenderPoint(renderer_.get(), x, y);
}
void Renderer::DrawLine(const SDL_FPoint &p1,const SDL_FPoint& p2){
    SDL_RenderLine(renderer_.get(),p1.x,p1.y,p2.x,p2.y);
}
void Renderer::DrawRect(const SDL_FRect &r){
    SDL_RenderRect(renderer_.get(), &r);
}
void Renderer::FillRect(const SDL_FRect &r){
    SDL_RenderRect(renderer_.get(),&r);
}
void Renderer::DrawTexture(SDL_Texture *texture,const SDL_FRect & srect,float x,float y){
    SDL_FRect drect{.x=x,.y=y,.w=srect.w,.h=srect.h};
    SDL_RenderTexture(renderer_.get(),texture,&srect,&drect);
}
void Renderer::Present(){
    SDL_RenderPresent(renderer_.get());
}