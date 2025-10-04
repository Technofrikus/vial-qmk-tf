#include "qp.h"

//Fonts
#include "fonts/SFProDisplayLight16.qff.h"
//#include "fonts/SFProDisplayLight20.qff.h"
//#include "fonts/SFProDisplaySemibold16.qff.h"
//#include "fonts/SFProDisplaySemibold20.qff.hk"

#include "qp_comms.h"
#include "qp_ili9xxx_opcodes.h"

//Graphics
#include "graphics/ClackyRainbow_480x320.qgf.h"
//#include "Comp_3_Pride.qgf.h"


//Variables/Objects
static painter_device_t display;   //Display Object

//Font objects
static painter_font_handle_t SFProL16;
//static painter_font_handle_t SFProL20;
//static painter_font_handle_t SFProSB16;
//static painter_font_handle_t SFProSB20;

//Graphic objects
static painter_image_handle_t my_image;
//static deferred_token my_anim; //for animations

//Load Graphic



//Initialization during keyboard boot
void keyboard_post_init_kb(void) {

debug_enable=true;


//Display setup
display = qp_ili9488_make_spi_device(480, 320, LCD_CS_PIN, LCD_DC_PIN, LCD_RST_PIN, 16, 0); // Create the display
qp_init(display, QP_ROTATION_90);   // Initialise the display, 90° rotated

qp_comms_start(display);
qp_comms_command(display, ILI9XXX_CMD_INVERT_ON);
qp_comms_stop(display);


backlight_enable(); // enable the backlight
backlight_set(31);
qp_rect(display, 0, 0, 480, 320, HSV_BLACK, true); //Fill screen with black
my_image = qp_load_image_mem(gfx_ClackyRainbow_480x320);
qp_drawimage(display, 0, 0, my_image);

//Load Fonts into memory
SFProL16 = qp_load_font_mem(font_SFProDisplayLight16);
// SFProL20 = qp_load_font_mem(font_SFProDisplayLight20);
// SFProSB16 = qp_load_font_mem(font_SFProDisplaySemibold16);
// SFProSB20 = qp_load_font_mem(font_SFProDisplaySemibold20);

//Create grid
// 4x5 grid, with 4px margins: 116x60px each
// bool qp_line(device, x0, y0, x1, y1,  hue, sat, val); for reference only

//const int linewidth = 1; //linewidth for Grit
//Horizontal Lines
/* qp_line(display, 0, 0, 480, 0, HSV_WHITE);
qp_line(display, 0, 64, 480, 64, HSV_WHITE);
qp_line(display, 0, 128, 480, 128, HSV_WHITE);
qp_line(display, 0, 192, 480, 192, HSV_WHITE);
qp_line(display, 0, 256, 480, 256, HSV_WHITE);
qp_line(display, 0, 320, 480, 320, HSV_WHITE);

//Vertical lines
qp_line(display, 0, 0, 0, 320, HSV_WHITE);
qp_line(display, 120, 0, 120, 320, HSV_WHITE);
qp_line(display, 240, 0, 240, 320, HSV_WHITE);
qp_line(display, 360, 0, 360, 320, HSV_WHITE);
qp_line(display, 480, 0, 480, 320, HSV_WHITE); */


/* //Draw Text
//qp_drawtext(device, x, y, font, char *str); for reference only
    static char *text;
    text = "Cmd+Alt+Shift";
    int16_t width = qp_textwidth(SFProL16, text);
    qp_drawtext(display, (60 - (width/2)), (32 - (SFProL16->line_height/2)), SFProL16, text); */

}; //Setup over




//Backlight handling on power down
static uint8_t last_backlight = 31;
void suspend_power_down_user(void) {
    last_backlight = get_backlight_level();
    backlight_set(0);
    rgb_matrix_set_suspend_state(true);
    backlight_disable();
};

void suspend_wakeup_init_user(void) {
    backlight_enable();
    rgb_matrix_set_suspend_state(false);
    backlight_set(last_backlight);
};

// set the current layer variable, triggered when layer is changed by user
static uint8_t current_layer = 0;
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);
    return state;
};

// Loop for updating the display during runtime
//static uint16_t intKCArray[14]; //to store the current keycodes of all keys, incl encoder. Encoder is 13+14
static uint32_t last_draw = 0;  // When was the display last drawn
void housekeeping_task_user(void) {
    if (timer_elapsed32(last_draw) > 3300) { // Throttle to 30fps
        last_draw = timer_read32();

/*         //clear display before redraw
        qp_rect(display, 0, 0, 480, 320, HSV_BLACK, true); //Fill screen with black
        //Horizontal Lines
        qp_line(display, 0, 0, 480, 0, HSV_WHITE);
        qp_line(display, 0, 64, 480, 64, HSV_WHITE);
        qp_line(display, 0, 128, 480, 128, HSV_WHITE);
        qp_line(display, 0, 192, 480, 192, HSV_WHITE);
        qp_line(display, 0, 256, 480, 256, HSV_WHITE);
        qp_line(display, 0, 320, 480, 320, HSV_WHITE);
        //Vertical lines
        qp_line(display, 0, 0, 0, 320, HSV_WHITE);
        qp_line(display, 120, 0, 120, 320, HSV_WHITE);
        qp_line(display, 240, 0, 240, 320, HSV_WHITE);
        qp_line(display, 360, 0, 360, 320, HSV_WHITE);
        qp_line(display, 480, 0, 480, 320, HSV_WHITE); */


       // Get all keycodes (except encoder) of the current layer into the array and draw them
      // probably wrong static char cKC[2] = "00";
   /*    static char *cKC;
      int textX = 60; //x middle of the first cell
      int textY = 32; //y middle of the first cell
      int16_t width = 0;
      //int i = 0; // Array index
      for (int r = 0; r < 3; ++r) {
          textX = 60;
        for (int c = 0; c < 4; ++c) {
           cKC = "Test";
           // dynamic_keymap_get_keycode(current_layer,r ,c );
           //cKC = intKCArray[i]; //currentKeyCode
           width = qp_textwidth(SFProL16, cKC);
           qp_rect(display, textX-58 , textY-30, textX+58 , textY+30, HSV_BLACK, true);
           qp_drawtext(display, (textX - (width/2)), (textY - (SFProL16->line_height/2)), SFProL16, cKC);
           //++i;
           textX = textX + 120;


           }
           textY = textY + 64;
        } */
    }
};





//Unused, for maybe later
// my_image = qp_load_image_mem(gfx_ClackyRainbow_480x320);
// my_image = qp_load_image_mem(gfx_Comp_3_Pride);
// my_anim = qp_animate(display, 0, 0, my_image);
// qp_drawimage(display, 0, 0, my_image);
