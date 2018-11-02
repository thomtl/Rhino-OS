#include <rhino/Deer/deer.h>
#include <libk/stdio.h>
#include <rhino/arch/x86/drivers/screen.h>

size_t framebuffer;

void deer_clear_screen_col(uint8_t r, uint8_t g, uint8_t b){
    for(uint32_t i = 0; i < SCREEN_WIDTH; i++){
        for(uint32_t j = 0; j < SCREEN_HEIGHT; j++){
            char buf[3] = {r, g, b};
            asm("rep movsb" : : "S"(&buf), "D"((void*)(&((uint8_t*)framebuffer)[(j * (SCREEN_WIDTH) + i) * 3])), "c"(3) : "memory");
        }
    }
    deer_reset_terminal();
}

void deer_start(){
    if(bga_is_active()){
        udi_video_mode_set_t mode;
        mode.width = SCREEN_WIDTH;
        mode.height = SCREEN_HEIGHT;
        mode.bpp = 24;
        mode.clearFramebuffer = true;
        bga_set_video_mode(&mode);
        framebuffer = bga_get_lfb();
    } else {
        PANIC_M("[DEER] No BGA device found");
    }

    deer_clear_screen();
    activate_deer();
}