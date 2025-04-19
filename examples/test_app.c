#include "lvgl.h"
#include "lv_drivers/sdl/sdl.h"
#include <unistd.h>

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Button clicked!");
    }
}

static void hal_init(void)
{
    // Initialize SDL
    sdl_init();

    // SDL driver will handle display and input device initialization
    lv_display_t * disp = lv_sdl_get_display();
    lv_indev_t * mouse = lv_sdl_get_mouse();
    lv_indev_t * mousewheel = lv_sdl_get_mousewheel();
    lv_indev_t * keyboard = lv_sdl_get_keyboard();
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
        lv_timer_handler();
        lv_sdl_window_handle_events();
        usleep(5000);
    }
    
    return 0;
} 