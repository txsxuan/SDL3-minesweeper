#pragma once
// #include "context.hpp"
#include "pch.hpp"
#include "Window.hpp"
inline auto RenderDestroy=[](SDL_Renderer* renderer){SDL_DestroyRenderer(renderer);};
class Renderer final{
    friend struct Context;
public:
    Renderer(const Window&);
    void SetColor(const SDL_Color&);
    void SetTarget(SDL_Texture *texture);
    void Clear();
    void DrawPoint(float x,float y);
    void DrawLine(const SDL_FPoint &p1,const SDL_FPoint& p2);
    void DrawRect(const SDL_FRect &);
    void FillRect(const SDL_FRect &);
    void DrawTexture(SDL_Texture *texture,const SDL_FRect & srect,float x,float y);
    void DrawTexture(SDL_Texture *texture,float x,float y);
    void DrawTexture(SDL_Texture *texture);
    void Present();
    void setScale(float ScaleX,float ScaleY);
private:
    std::unique_ptr<SDL_Renderer,decltype(RenderDestroy)> renderer_=nullptr;
};