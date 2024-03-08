//------------------------------------------------------------------------------
// Name:        Nuklear Visual Style Editor (main.c)
// Executable:  ANVSE[.EXE]
// URL:
// Purpose:     Visually create formatted Nuklear style tables
//              Load and save formatted nk table file.
//              [start_nk_style_v1] - [end_nk_style_v1]
//
// Platform:    Win64 W8.1+, Ubuntu64 20.04+
//
// Author:      Axle
// Created:     27/02/2024
// Updated:     04/03/2024
// Copyright:   (c) Axle 2024
// Licence:     MIT0 No Attribution
//------------------------------------------------------------------------------
// NOTES:
// Windows x86-64
// IDE: Dev-C++,
// STD ISO C99
// winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64msvcrt-11.0.1-r5
//
// Nuklear V4.12.0
// https://github.com/Immediate-Mode-UI/Nuklear/releases/tag/4.12.0
// "Nuklear-4.12.0.zip" "Nuklear-4.12.0.tar.gz"
// Allegro Version 5.2.9.1 Dynamic linking
// https://github.com/liballeg/allegro5/releases
// allegro-x86_64-w64-mingw32-gcc-13.2.0-posix-seh-dynamic-5.2.9.0.zip
// https://github.com/liballeg/allegro_wiki/wiki/Quickstart
//
// Dynamic linking: Under Linking options enter `-lallegro'
// You will need to link other library components as well when required.
// -lallegro-5.2
// -lallegro_image-5.2
// -lallegro_font-5.2
// -lallegro_ttf-5.2
// -lallegro_primitives-5.2
// -lallegro_dialog-5.2
// -lm
// Ubuntu x86-64
// Version: 2:5.2.9.1-1~focal
// liballegro5-dev
// https://github.com/liballeg/allegro_wiki/wiki/Quickstart
// sudo add-apt-repository ppa:allegro/5.2
// sudo apt-get install liballegro*5.2 liballegro*5-dev
// sudo apt-get install liballegro-ttf5-dev
// Redist:
// liballegro5.2
// wget -c http://ppa.launchpad.net/allegro/5.2/ubuntu/pool/main/a/allegro5/liballegro5.2_5.2.9.1-1~focal_amd64.deb
// Linker (Code::Blocks)
// allegro-5.2
// allegro_image-5.2
// allegro_font-5.2
// allegro_ttf-5.2
// allegro_primitives-5.2
// allegro_dialog-5.2
//
// allegro depends:
//libgcc_s_seh-1.dll
//libstdc++-6.dll
//libwinpthread-1.dll
//
// You will need to copy the above Allegro and MinGW rutimes as well As the
// resource files to your bin output directory.
//------------------------------------------------------------------------------
// NOTES/TODO:
// Logo_96x25.png and Icon.png are not provided.
//
// Ubuntu has left widget alignment/margin problem.
// NK_COLOR_SLIDER_CURSOR_ACTIVE not working linux?
//
// Window Close dialog does not return on Cancel!! [Done]
//
//-lmingw32
//
// Fix NK shutdown tasks!!! [Done]
    //nk_allegro5_del_image(logo);[Done]
    //nk_allegro5_del_image(backgrounds);[Done]
    //al_destroy_event_queue(event_queue);[Done]

    //nk_allegro5_shutdown();[Done]
//------------------------------------------------------------------------------

#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <stdarg.h>
//#include <string.h>
#include <math.h>
#include <assert.h>
//#include <limits.h>
//#include <time.h>

#if defined (_WIN32)// || defined (_WIN64)  // Restrict to 64bit
// GCC will correct file paths to the system, but don't take it for granted.
 #define PATH_STYLES ".\\"
// #define OS_Windows 1  // 1 = True (aka Bool)
// #define OS_Unix 0
// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>

//#   ifdef _WIN64
// Legacy printf(),scanf() "slow!". The default is 0
// https://sourceforge.net/p/mingw-w64/wiki2/gnu%20printf/
//#       define __USE_MINGW_ANSI_STDIO 1
//#   endif

#elif defined (__unix__)
// GCC will correct file paths to the system, but don't take it for granted.
 #define PATH_STYLES "./"
// #define OS_Unix 1
// #define OS_Windows 0  // 0 = False (aka Bool)
// #include <unistd.h>  // POSIX API
#endif

// Make sure to add the paths for Allegro 5 and Nuklear.
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NK_INCLUDE_FIXED_TYPES
//#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_ALLEGRO5_IMPLEMENTATION
#include <nuklear.h>
#include <demo/allegro5/nuklear_allegro5.h>
#include <demo/common/style.c>

#define UNUSED(a) (void)a

#include "style_ex.c"
#include "nk_theme.c"


int main(void)
    {
    UNUSED(nk_cos);
    UNUSED(nk_sin);
    UNUSED(nk_inv_sqrt);

    /* Platform */
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    NkAllegro5Font *font;
    NkAllegro5Font *font_l;
    struct nk_context *ctx;

    // Allegro Background images; black.png, bkimg1.png, bkimg2.png
    ALLEGRO_BITMAP* background[3];  // [0] is reserved (no image; black.png)
    int img_select = 0;  // Slect background image or 0 == none

    // Nk or Al al_map_rgba(255, 255, 255, 0)
    // Structure to hold Allegro background color.
    // Colors must be converted from NK to Al before adjusting.
    // ALLEGRO_COLOR nk_color_to_allegro_color(struct nk_color color)
    // nk_color is converted to ALLEGRO_COLOR befor rendering.
    // Structure to hold Nuklear background color.
    struct nk_color nk_bk_color[3];
    int i = 0;
    for (i = 0; i < 3; i++)
        {
        nk_bk_color[i].r = 0;
        nk_bk_color[i].g = 0;
        nk_bk_color[i].b = 0;
        nk_bk_color[i].a = 255;  // Alpha not used (rgb) Allegro defaults to Black canvas.
        }
    // Holds Allegro Background colors.
    struct ALLEGRO_COLOR al_bk_color;  // To capture nk colors
    al_bk_color = nk_color_to_allegro_color(nk_bk_color[0]);

    if (!al_init())
        {
        fprintf(stdout, "failed to initialize allegro5!\n");
        exit(1);
        }

    al_install_mouse();
    al_set_mouse_wheel_precision(150);
    al_install_keyboard();

    // For Allegro Native file dialogs.
    al_init_native_dialog_addon(); // Introduced in 5.0.9

    //al_init_image_addon();
    //must_init(al_init_image_addon(), "image");

    al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_RESIZABLE|ALLEGRO_OPENGL);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    display = al_create_display(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!display)
        {
        fprintf(stdout, "failed to create display!\n");
        exit(1);
        }

    event_queue = al_create_event_queue();
    if (!event_queue)
        {
        fprintf(stdout, "failed to create event_queue!\n");
        al_destroy_display(display);
        exit(1);
        }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    //10,14,16,18,24,30,36,48,60,72,84
    // If you wan't more fonts, load them as an array.
    // regular text
    font = nk_allegro5_font_create_from_file("Roboto-Regular.ttf", 12, 0);
    if (!font)
        {
        fprintf(stdout, "failed to Load ttf Font file!\n");
        exit(1);
        }
    // Title text.
    font_l = nk_allegro5_font_create_from_file("Roboto-Regular.ttf", 30, 0);
    ctx = nk_allegro5_init(font, display, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Note: This uses the internal default style.
    set_style(ctx, THEME_BLACK);  // THEME_WHITE,THEME_RED,THEME_BLUE,THEME_DARK

    //ALLEGRO_BITMAP* background[3];
    // Load background images (Uses int img_select in main loop)
    background[0] = al_load_bitmap("black.png");
    background[1] = al_load_bitmap("bkimg1.png");
    background[2] = al_load_bitmap("bkimg2.png");
    if ((!background[0]) || (!background[1]) || (!background[2]))
        {
        fprintf(stdout, "failed to Load Background image files!\n");
        exit(1);
        }

    // Array of structures to hold the Styles.
    // table[1] is never altered in the editor.
    struct nk_color table[3][NK_COLOR_COUNT];
    // Must be called after nk_allegro5_init() and set_style()
    get_current_style_table(ctx, table);  // table will be populated by reference
    //fprintf_table_dbg(table);  // DEBUG print the table

    // Logo file.
    const char *logo_file = "Logo_96x25.png";  // About Button
    struct nk_image *logo = NULL;
    logo = nk_allegro5_create_image(logo_file);
    if (!logo)
        {
        fprintf(stdout, "Failed to Load Logo image file!\n");
        exit(1);
        }

    // Load allegro icon (for Ubuntu)
    ALLEGRO_BITMAP *icon;
    icon = al_load_bitmap("Icon.png");  // Window icon Linux.
    if (!icon)
        {
        fprintf(stdout, "Icon.png not found!\n");
        }
    al_set_display_icon(display, icon);

    // Logic switch needs to be set outside of the nk_theme() function
    int alpha_flag = 0;  // Radio select once logic.
    // Enable alpha sliders lock
    int alpha_all = 255;  // Sets alpha sliders to opaque to start.

    int quit_app = 0;  // Logic to aks if exit application.


    static nk_bool isrunning = 1;
    while(isrunning)
        {
        bool get_event;
        ALLEGRO_EVENT ev;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.06);

        get_event = al_wait_for_event_until(event_queue, &ev, &timeout);

        // Do exit application tasks
        if (get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
            // Send quit_app = 1 to nk_popup dialog.
            quit_app = 1;
            }
        if (quit_app == 2)
        {
            // Confirmation to end main loop.
            isrunning = 0;
        }

        /* Very Important: Always do nk_input_begin / nk_input_end even if
           there are no events, otherwise internal nuklear state gets messed up */
        nk_input_begin(ctx);
        if (get_event)
            {
            while (get_event)
                {
                nk_allegro5_handle_event(&ev);
                get_event = al_get_next_event(event_queue, &ev);
                }
            }
        nk_input_end(ctx);

        // GUI
        nk_theme(ctx, display, table, logo, &img_select, nk_bk_color, font, font_l, &alpha_flag, &alpha_all, &quit_app);

        // Draw

        // Set different background colours for transparency tests.
        // Allegro color must be converted from NK color.
        al_bk_color = nk_color_to_allegro_color(nk_bk_color[0]);
        al_clear_to_color(al_bk_color);  // Use the converted background color.

        // Draw the background image. If img_select == 0 no image is rendered.
        // This is not well optimised. Consider
        // buffer = al_create_bitmap(BUFFER_W, BUFFER_H); and how this works with NK
        if (img_select > 0)
            {
            //printf("Image 1/2 Selected= %d\n", img_select);  // DEBUG
            al_draw_scaled_bitmap(background[img_select],
                                  0,
                                  0,
                                  al_get_bitmap_width(background[img_select]),
                                  al_get_bitmap_height(background[img_select]),
                                  0,
                                  0,
                                  al_get_display_width(display),
                                  al_get_display_height(display),
                                  0);
            }


        /* IMPORTANT: `nk_allegro5_render` changes the target backbuffer
        to the display set at initialization and does not restore it.
        Change it if you want to draw somewhere else. */
        nk_allegro5_render();
        al_flip_display();

        }

    // Do cleanup tasks.
    al_shutdown_native_dialog_addon();
    nk_allegro5_font_del(font);
    nk_allegro5_font_del(font_l);
    nk_allegro5_del_image(logo);
    nk_allegro5_shutdown();

    al_destroy_bitmap(icon);
    al_destroy_bitmap(background[0]);
    al_destroy_bitmap(background[1]);
    al_destroy_bitmap(background[2]);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_uninstall_keyboard();
    al_uninstall_mouse();

    return 0;
    }

