#include "lvgl.h"
#include "SDL2/SDL.h"
#include <unistd.h>

static SDL_Window * window;
static SDL_Renderer * renderer;
static SDL_Texture * texture;
static uint32_t * pixels;

#define DISP_HOR_RES 800
#define DISP_VER_RES 480

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Button clicked!");
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    int32_t x, y;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    // Copy the rendered pixels to the texture
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            int idx = y * w + x;
            pixels[(area->y1 + y) * DISP_HOR_RES + area->x1 + x] = ((lv_color_t*)px_map)[idx].full;
        }
    }

    SDL_UpdateTexture(texture, NULL, pixels, DISP_HOR_RES * 4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    lv_display_flush_ready(disp);
}

static void hal_init(void)
{
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    window = SDL_CreateWindow("LVGL Test",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            DISP_HOR_RES, DISP_VER_RES,
                            SDL_WINDOW_SHOWN);
    if(!window) {
        printf("Window creation error: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        printf("Renderer creation error: %s\n", SDL_GetError());
        return;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STATIC, DISP_HOR_RES, DISP_VER_RES);
    if(!texture) {
        printf("Texture creation error: %s\n", SDL_GetError());
        return;
    }

    pixels = (uint32_t*)malloc(DISP_HOR_RES * DISP_VER_RES * 4);
    if(!pixels) {
        printf("Memory allocation error!\n");
        return;
    }

    // Initialize LVGL display
    static lv_color_t buf[DISP_HOR_RES * DISP_VER_RES];
    lv_display_t * disp = lv_display_create(DISP_HOR_RES, DISP_VER_RES);
    lv_display_set_buffers(disp, buf, NULL, DISP_HOR_RES * DISP_VER_RES * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, flush_cb);
}

static void handle_mouse_events(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEMOTION: {
                SDL_Point point = {event.motion.x, event.motion.y};
                lv_display_t * disp = lv_display_get_default();
                lv_indev_t * indev = lv_indev_create();
                lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
                lv_indev_set_group(indev, NULL);
                lv_indev_state_t state = event.type == SDL_MOUSEBUTTONDOWN ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
                lv_indev_set_data(indev, &point, state);
                break;
            }
            case SDL_QUIT:
                exit(0);
                break;
        }
    }
}

int main(void)
{
    // Initialize LVGL
    lv_init();
    
    // Initialize the HAL for LVGL
    hal_init();
    
    // Create a screen
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_scr_load(scr);
    
    // Create a button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    
    // Add label to the button
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Click me!");
    lv_obj_center(label);
    
    // Main loop
    while(1) {
        handle_mouse_events();
        lv_timer_handler();
        SDL_Delay(5);
    }
    
    return 0;
} 