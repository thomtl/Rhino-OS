#include <rhino/Deer/deer.h>
#include <libk/stdio.h>

size_t framebuffer;

void deer_clear_screen_col(uint8_t r, uint8_t g, uint8_t b){
    for(uint32_t i = 0; i < SCREEN_WIDTH; i++){
        for(uint32_t j = 0; j < SCREEN_HEIGHT; j++) draw_pixel(i, j, r, g, b);
    }
}

void deer_start(){
    if(bga_is_active()){
        bga_set_video_mode(SCREEN_WIDTH, SCREEN_HEIGHT, 24, true, true);
        framebuffer = bga_get_lfb();
    } else {
        PANIC_M("[DEER] No BGA device found");
    }

    deer_clear_screen_col(0, 0, 0);

    deer_printf("abcdefghijklmnopqrstuvwxyz1234567890abcdefghjkmnopabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw");

    draw_circle(100, 100, 250, 255, 0, 0);
}