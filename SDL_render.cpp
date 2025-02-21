#include "SDL_render.hpp"



/* Drawing stuff below*/
void set_pixel(SDL_Surface* surf, int x, int y, Uint32 color){
    int offset = y * surf->pitch + x * SDL_BYTESPERPIXEL(surf->format);
    Uint32* const target = (Uint32*) ( (Uint8*)surf->pixels + offset);
    *target = color;
}

int to_tex_x(float real_x, float tex_size){
    return (int)(real_x + tex_size / 2);
}

int to_tex_y(float real_y, float tex_size){
    return (int)(-real_y + tex_size / 2);
}

void draw_horizontal_line(SDL_Surface* surf, float real_x1, float real_x2, float real_y, Uint32 color, float tex_size){
    int x1 = to_tex_x(real_x1, tex_size);
    int x2 = to_tex_x(real_x2, tex_size);
    int y = to_tex_y(real_y, tex_size);
    for(int x = x1; x <= x2; ++x){
        set_pixel(surf, x, y, color);
    }
}

Uint32 to_Uint32(const SDL_Color* color){
    return (Uint32)((color->r << 24) + (color->g << 16) + (color->b << 8) + color->a);
}

void fill_circle(SDL_Surface* surf, const SDL_Color& circle_color, unsigned int radius){
    float real_x = (float)radius;
    float real_y = 0;
    float mid_x = real_x;
    float mid_y = real_y;
    
    float tex_size = radius * 2;
    Uint32 color = to_Uint32(&circle_color);
    while(SDL_tanf(real_y / real_x) <= 2){
        draw_horizontal_line(surf, -real_x, real_x, real_y, color, tex_size);
        draw_horizontal_line(surf, -real_y, real_y, real_x, color, tex_size);
        draw_horizontal_line(surf, -real_x, real_x, -real_y, color, tex_size);
        draw_horizontal_line(surf, -real_y, real_y, -real_x, color, tex_size);

        if(mid_x * mid_x + mid_y * mid_y <= radius * radius){
            real_y += 1;
        }
        else{
            real_x -= 1;
            real_y += 1;
        }

        mid_x = real_x - 0.5;
        mid_y = real_y + 1;
    }
}


SDL_Texture* generate_texture(const SDL_Color& color, unsigned int radius){
    const unsigned int real_tex_size = 2 * radius + tex_margin;
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, real_tex_size, real_tex_size);
    if(!texture){
        return nullptr;
    }
    SDL_Surface* surf = 0;
    if(SDL_LockTextureToSurface(texture, 0, &surf)){
        fill_circle(surf, color, radius);
        SDL_UnlockTexture(texture);
        SDL_DestroySurface(surf);
    }
    else{
        return nullptr;
    }

    return texture;
}
/* end drawing stuff */


bool initializeSDL(unsigned int window_width, unsigned int window_height){
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (!SDL_CreateWindowAndRenderer("physnum1", window_width, window_height, 0, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return false;
    }

    win_width = window_width;
    win_height = window_height;

    earth_tx = generate_texture({10, 10, 230, 255}, earth_r);
    moon_tx = generate_texture({240, 220, 150, 255}, moon_r);
    sat_tx = generate_texture({10, 10, 10, 255}, sat_r);

    if(!earth_tx || !moon_tx || !sat_tx){
        return false;
    }
    return true;
}

void getDstRect(SDL_FRect& rct, unsigned int radius, double x, double y){
    unsigned int texSize = 2 * radius + tex_margin;
    rct.x = x;
    rct.y = y;
    rct.w = texSize;
    rct.h = texSize;
}

unsigned int toScreenCoords(double coord, double radius, bool isYCoord=false){
    double offset = radius;
    if(isYCoord){
        return -coord * scale + win_height / 2 - offset;
    }
    else{
        return coord * scale + win_width / 2 - offset;
    }
}

void drawObjects(double earthX, double moonX, double satX, double satY){
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(renderer, 0, win_height / 2, win_width, win_height / 2);

    SDL_FRect dst_rect = {0, 0, 0, 0};
    getDstRect(dst_rect, earth_r, toScreenCoords(earthX, earth_r) + xOffset, toScreenCoords(0, earth_r, true));
    SDL_RenderTexture(renderer, earth_tx, NULL, &dst_rect);

    getDstRect(dst_rect, moon_r, toScreenCoords(moonX, moon_r) + xOffset, toScreenCoords(0, moon_r, true));
    SDL_RenderTexture(renderer, moon_tx, NULL, &dst_rect);

    getDstRect(dst_rect, sat_r, toScreenCoords(satX, sat_r) + xOffset, toScreenCoords(satY, sat_r, true));
    SDL_RenderTexture(renderer, sat_tx, NULL, &dst_rect);

    SDL_RenderPresent(renderer);
}

bool processEventsAndQuit(){
    bool isRunning = true;
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_EventType::SDL_EVENT_QUIT:
                isRunning = false;
                break;
        }
    }

    if(!isRunning){
        SDL_DestroyTexture(earth_tx);
        SDL_DestroyTexture(moon_tx);
        SDL_DestroyTexture(sat_tx);
        SDL_Quit();
    }

    return isRunning;
}
