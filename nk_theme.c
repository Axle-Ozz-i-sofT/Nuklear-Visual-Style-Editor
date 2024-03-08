//------------------------------------------------------------------------------
// Name:        Nuklear Visual Style Editor (nk_theme.c)
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
// TODO:
// Get system theme colors (Pro)
// Load save multiple styles style.c (Pro)
//------------------------------------------------------------------------------

static int
nk_theme(struct nk_context *ctx,
         ALLEGRO_DISPLAY *display,
         struct nk_color table[3][NK_COLOR_COUNT],
         struct nk_image *logo,
         int *img_select,
         struct nk_color nk_bk_color[3],
         NkAllegro5Font *font,
         NkAllegro5Font *font_l,
         int *alpha_flag,
         int *alpha_all,
         int *quit_app)
    {

    // Test examples
    /* window flags */
    static nk_bool show_menu = nk_true;  // overview.c
    static nk_bool about_dialog = nk_false;  // overview.c
    /* popups */
    static enum nk_style_header_align header_align = NK_HEADER_RIGHT;
    // ???
    ctx->style.window.header.align = header_align;


    // Application flags
    static nk_bool show_menu1 = nk_true;
    static nk_bool show_hr = nk_true;
    static nk_bool show_title = nk_true;

    static nk_bool show_app_about = nk_false;  // overview.c
    static nk_bool help_dialog = nk_false;
    static nk_bool popup_button = nk_false; // nk_bool!!!!

    static nk_bool theme_apply = nk_false;  // Apply theme to main application
    static nk_bool resetall = nk_false;  // Revert to original color style.
    static int reset_nk_color = -1;  // Reset single original color style.
    static nk_bool clear_change = nk_false;  // Clear current color adjustment.

    // enable background image
    enum {IMG_0, IMG_1, IMG_2};
    static int bg_img_op = IMG_0;

    // Enable alpha sliders lock
    // There is somethink wrong with this initialisation being inside of the function!!!
    enum alpha_op {NONE, ALL};
    static int alpha_op = NONE;  // Always reset to NONE every frame?
    //static int alpha_all = 255;  // Always reset to 255 every frame?

    static nk_bool bg_theme_apply = nk_false;  // Apply background color to main application
    static nk_bool bg_resetall = nk_false;  // Revert to original background color style.
    static nk_bool bg_reset_nk_color = nk_false;  // Reset single original background color style.
    static nk_bool bg_clear_change = nk_false;  // Clear current background color adjustment.

    static nk_bool disable_widgets = nk_false;  // Examples overview.c

    // User save file check befor exiting.
    static nk_bool exit_check = nk_false;  // Check popup.
    static nk_bool quit = nk_false;  // confirmed quit.

    /* File select dialogs */
    ALLEGRO_FILECHOOSER *file_path = NULL;  // Selected file path (Load/Save).
    static nk_bool file_ret = nk_false;  // Test if file selected.

    // NOTE: Allegro native file dialogs stalls the NK widgets. I am allowing
    // 4 passes before opening the native file dialogs to allow menus to close.
    // I am not sure what is causing this behaviour at the moment.
    static int file_load = 0;  // Flag to do file load tasks.
    static int file_save = 0;  // Flag to do file load tasks.

    // File load/read errors.
    static int file_read_err = 0;
    static const char fr_err_list[7][64] =
        {
            {"NULL"},  // 0
            {"File not selected!"},  // 1
            {"Error opening file!"},  // 2
            {"Error! Header not found or incorrect length!"},  // 3
            {"Error! Footer not found or incorrect length!"},  // 4
            {"Error! File too long!"},  // 5
            {"Error! File too short!"},  // 6
        };

    // Application window
    //static nk_flags window_flags = NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_SCALABLE|NK_WINDOW_MOVABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_CLOSABLE;
    //nk_flags actual_window_flags;
    static nk_flags window_flags = NK_WINDOW_BORDER;

    // Get the current Allegro window dimensions for auto resizing.
    // This is checked every main loop and resize applied.
    // WINDOW_WIDTH, WINDOW_HEIGHT = nk_begin (Window)
    int AL_WINDOW_W = al_get_display_width(display);
    int AL_WINDOW_H = al_get_display_height(display);

    // ###-> Start main Nuklear window (full size to Allegro window) ###
    if (nk_begin(ctx, "NK_Theme", nk_rect(0, 0, AL_WINDOW_W, AL_WINDOW_H), window_flags))
        {
        // ###-> START MAIN MENU ###
        if (show_menu1)
            {
            /* menubar */
            nk_menubar_begin(ctx);
            nk_layout_row_begin(ctx, NK_STATIC, 30, 2);  // Row H px, and number rows.

            /* menu #1 */
            nk_layout_row_push(ctx, 50);
            if (nk_menu_begin_label(ctx, "FILE", NK_TEXT_LEFT, nk_vec2(200, 600)))
                {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_menu_item_label(ctx, "Open", NK_TEXT_LEFT))
                    {
                    file_load = 5;  // Needs minimum 3 loops or UI stops responding?
                    // Native file dialogs moved to if (file_load){}
                    }
                if (nk_menu_item_label(ctx, "Save", NK_TEXT_LEFT))
                    {
                    file_save = 5;
                    // Native file dialogs moved to if (file_save){}
                    }
                if (nk_menu_item_label(ctx, "Exit", NK_TEXT_LEFT))
                    {
                    // Do exit tasks
                    exit_check = nk_true;
                    }
                nk_menu_end(ctx);
                }

            /* Menu #2 */
            // Future implementation. Alow copy paste of RGB colors.
            /*
            nk_layout_row_push(ctx, 50);
            if (nk_menu_begin_label(ctx, "EDIT", NK_TEXT_LEFT, nk_vec2(200, 600)))
                {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_menu_item_label(ctx, "Copy", NK_TEXT_LEFT))
                    {
                    // Do_new_task = nk_true;
                    fprintf(stdout, "EDIT -> Copy not used.\n");  // DEBUG
                    }
                if (nk_menu_item_label(ctx, "Paste", NK_TEXT_LEFT))
                    {
                    // Do_new_task = nk_true;
                    fprintf(stdout, "EDIT -> Paste not used.\n");  // DEBUG
                    }
                nk_menu_end(ctx);
                }
            */

            /* menu #3 */
            nk_layout_row_push(ctx, 50);
            if (nk_menu_begin_label(ctx, "VIEW", NK_TEXT_LEFT, nk_vec2(200, 600)))
                {
                nk_layout_row_dynamic(ctx, 25, 1);
                if (nk_menu_item_label(ctx, "Help", NK_TEXT_LEFT))
                    {
                    help_dialog = nk_true;
                    }
                // Future implementation
                /*if (nk_menu_item_label(ctx, "Options", NK_TEXT_LEFT))
                    {
                    // Do_new_task = nk_true;
                    fprintf(stdout, "VIEW -> Options not used.\n");  // DEBUG
                    }*/
                if (nk_menu_item_label(ctx, "About", NK_TEXT_LEFT))
                    {
                    about_dialog = nk_true;
                    }
                nk_menu_end(ctx);
                }

            nk_menubar_end(ctx);
            }  // ### END MAIN MENU <-###

        // Horizontal rule/line
        if (show_hr)
            {
            // Will create a horizontal line/divider. I need to get the default border color.
            nk_layout_row_dynamic(ctx, 2, 1);
            // The last argument is for rounding of the box true|false
            //nk_rule_horizontal(ctx, table[2][NK_COLOR_BORDER], nk_false);
            nk_rule_horizontal(ctx, ctx->style.window.border_color, nk_false);
            }

        bool cancel_ops = false;  // ### This needs to be moved out of the function!
        // See Allegro File I/O Native file dialogs
        if (file_load > 0)
            {
            //al_rest(0.5);  // wait must be given or 5 loops
            if (file_load < 2)
                {
                // Remember that Win and Unix have different paths.
                file_path = al_create_native_file_dialog(PATH_STYLES,"Choose a file [.txt|.h|.c]...", "*.txt;*.c;*.h", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
                file_ret = al_show_native_file_dialog(display, file_path);
                if (file_ret)  // Rename this
                    {
                    // Do file open taks...
                    FILE *FileIn;// File open handle

                    // ### Rename these buffers
                    // String buffer length is enforced by fgets()
                    char tok_temp_buffer[128] = {'\0'};// Temp buffer for string manipulations
                    char string_temp_buffer[128] = {'\0'};//

                    FileIn = fopen(al_get_native_file_dialog_path(file_path, 0), "r");// Open file for read ops text.
                    if(FileIn == NULL)// Test if file open success.
                        {
                        file_read_err = 2;
                        cancel_ops = true;
                        }

                    if (!cancel_ops)
                        {

                        int cnt_rows = 0;// track array row position
                        int count_colors = 0;
                        while(fgets(string_temp_buffer, 128, FileIn) != NULL)
                            {
                            // Walk each line from the file (returns ',' in the string
                            // with '\n' at the end).
                            // Strip the newline character from the line.
                            // Replace newline char '\n' with '\0'
                            string_temp_buffer[strcspn(string_temp_buffer, "\r\n")] = '\0';

                            // Test if style header is correct.
                            // If not cancel operation.
                            if (cnt_rows == 0)
                                {
                                if ((!strstr(string_temp_buffer, "[start_nk_style_v1]")) || (strlen(string_temp_buffer) > 19))
                                    {
                                    file_read_err = 3;
                                    cancel_ops = true;
                                    break;
                                    }
                                }
                            // Test if style footer is correct.
                            // If not cancel operation.
                            if (cnt_rows == 29)
                                {
                                if ((!strstr(string_temp_buffer, "[end_nk_style_v1]")) || (strlen(string_temp_buffer) > 17))
                                    {
                                    file_read_err = 4;
                                    cancel_ops = true;
                                    break;
                                    }
                                }

                            // Capture string (r, g, b, a) from string row.
                            int count_token = 0;
                            char tok_temp_buffer2[32] = {'\0'};  // should be no more than 18 chars
                            if ((cnt_rows > 0) && (cnt_rows < 29))
                                {
                                // Token split string at '(' and ')'
                                char* token1 = strtok(string_temp_buffer, "()");
                                while( token1 != NULL )
                                    {
                                    // We are keeping the second token "r, g, b, a"
                                    if (count_token == 1)
                                        {
                                        strncpy(tok_temp_buffer, token1, 32);
                                        }
                                    token1 = strtok(NULL, "()");
                                    count_token++;  // Increment the array position for next column (next token)
                                    }

                                // Remove any space ' ' characters.
                                // "r, g, b, a" to "r,g,b,a"
                                char c;
                                int i = 0;
                                int k = 0;
                                do
                                    {
                                    c = tok_temp_buffer[i];
                                    if ((c != ' ') && (c != '\0'))
                                        {
                                        tok_temp_buffer2[k] = c;
                                        k++;
                                        tok_temp_buffer2[k] = '\0';
                                        }
                                    i++;
                                    }
                                while (c != '\0');  // End of string

                                // set up a temporary rgba array and structure.
                                char str_color[4][8] = {'\0'};

                                // Break the rgba string numerics out from the ',' delimiter.
                                char* token2 = strtok(tok_temp_buffer2, ",");
                                int str_col_cnt = 0;
                                while( token2 != NULL )
                                    {
                                    strncat(str_color[str_col_cnt], token2, 8);
                                    token2 = strtok(NULL, ",");
                                    str_col_cnt++;
                                    }

                                // Convert string numeric to int and populate nk_color structure for NK_COLOR_COUNT
                                char *ptr;
                                // struct nk_color table[3][NK_COLOR_COUNT]
                                for (int r = 0; r < 3; r++)
                                    {
                                    table[r][count_colors].r = strtol(str_color[0], &ptr, 10);
                                    table[r][count_colors].g = strtol(str_color[1], &ptr, 10);
                                    table[r][count_colors].b = strtol(str_color[2], &ptr, 10);
                                    table[r][count_colors].a = strtol(str_color[3], &ptr, 10);
                                    }

                                count_colors++;  // Increment table2[0 - 27] aka NK_COLOR_COUNT
                                }  // End RGBA rows

                            cnt_rows++; // move to next line/row and repeat.

                            // If too many rows in file cancel file read operations.
                            if (cnt_rows > 30)
                                {
                                file_read_err = 5;
                                cancel_ops = true;
                                fclose( FileIn );
                                break;
                                }
                            }  // END while read lines as string

                        // If too few rows in file cancel operation
                        if (!cancel_ops)
                            {
                            if (cnt_rows < 30)
                                {
                                file_read_err = 6;
                                cancel_ops = true;
                                }
                            }

                        //theme_apply = nk_true;
                        nk_style_from_table(ctx, table[0]);

                        fclose( FileIn );// finished file reads, close the file.
                        //fprintf(stdout, "Close File - True!\n");  // DEBUG
                        }  // END if (!cancel_ops)
                    }  // END al_show_native_file_dialog == true
                else
                    {
                    file_read_err = 1;
                    cancel_ops = true;
                    }

                al_destroy_native_file_dialog(file_path);  // Check if this is in the correct location??
                file_load = 0;  // Check this is getting passed on error?
                }
            else
                {
                file_load--;  // Work around for stalled NK wingets.
                }

            cancel_ops = false;
            }  // END (file_load)


        // See Allegro File I/O Native file dialogs.
        if (file_save > 0)
            {
            //al_rest(0.5);  // wait must be given or 5 loops
            if (file_save < 2)
                {
                // Remember that Win and Unix have different paths.
                file_path = al_create_native_file_dialog(PATH_STYLES,"Choose a save file [.txt|.h|.c]...", "*.txt", ALLEGRO_FILECHOOSER_SAVE);
                file_ret = al_show_native_file_dialog(display, file_path);
                if (file_ret)
                    {
                    // table[0] == current edit
                    // table[2] == applied/undo (Use this)
                    // Do file save tasks...
                    FILE *FileOut;// File open handle

                    // NOTE! Overwrites the file contents.
                    FileOut = fopen(al_get_native_file_dialog_path(file_path, 0), "w");  // Open file for write ops text.
                    if(FileOut == NULL)// Test if file open success.
                        {
                        file_read_err = 2;
                        cancel_ops = true;
                        }

                    if (!cancel_ops)
                        {
                        // Do File Write ops.
                        // See enum nk_style_colors 0 - 28
                        // ### Fix this? Loop through 28 lines with NK_COLOR_xxx from an array.
                        fprintf(FileOut, "[start_nk_style_v1]\n");
                        fprintf(FileOut, "table[NK_COLOR_TEXT] = nk_rgba(%d, %d, %d, %d)\n", table[2][0].r, table[2][0].g, table[2][0].b, table[2][0].a);
                        fprintf(FileOut, "table[NK_COLOR_WINDOW] = nk_rgba(%d, %d, %d, %d)\n", table[2][1].r, table[2][1].g, table[2][1].b, table[2][1].a);
                        fprintf(FileOut, "table[NK_COLOR_HEADER] = nk_rgba(%d, %d, %d, %d)\n", table[2][2].r, table[2][2].g, table[2][2].b, table[2][2].a);
                        fprintf(FileOut, "table[NK_COLOR_BORDER] = nk_rgba(%d, %d, %d, %d)\n", table[2][3].r, table[2][3].g, table[2][3].b, table[2][3].a);
                        fprintf(FileOut, "table[NK_COLOR_BUTTON] = nk_rgba(%d, %d, %d, %d)\n", table[2][4].r, table[2][4].g, table[2][4].b, table[2][4].a);
                        fprintf(FileOut, "table[NK_COLOR_BUTTON_HOVER] = nk_rgba(%d, %d, %d, %d)\n", table[2][5].r, table[2][5].g, table[2][5].b, table[2][5].a);
                        fprintf(FileOut, "table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(%d, %d, %d, %d)\n", table[2][6].r, table[2][6].g, table[2][6].b, table[2][6].a);
                        fprintf(FileOut, "table[NK_COLOR_TOGGLE] = nk_rgba(%d, %d, %d, %d)\n", table[2][7].r, table[2][7].g, table[2][7].b, table[2][7].a);
                        fprintf(FileOut, "table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(%d, %d, %d, %d)\n", table[2][8].r, table[2][8].g, table[2][8].b, table[2][8].a);
                        fprintf(FileOut, "table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(%d, %d, %d, %d)\n", table[2][9].r, table[2][9].g, table[2][9].b, table[2][9].a);
                        fprintf(FileOut, "table[NK_COLOR_SELECT] = nk_rgba(%d, %d, %d, %d)\n", table[2][10].r, table[2][10].g, table[2][10].b, table[2][10].a);
                        fprintf(FileOut, "table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(%d, %d, %d, %d)\n", table[2][11].r, table[2][11].g, table[2][11].b, table[2][11].a);
                        fprintf(FileOut, "table[NK_COLOR_SLIDER] = nk_rgba(%d, %d, %d, %d)\n", table[2][12].r, table[2][12].g, table[2][12].b, table[2][12].a);
                        fprintf(FileOut, "table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(%d, %d, %d, %d)\n", table[2][13].r, table[2][13].g, table[2][13].b, table[2][13].a);
                        fprintf(FileOut, "table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(%d, %d, %d, %d)\n", table[2][14].r, table[2][14].g, table[2][14].b, table[2][14].a);
                        fprintf(FileOut, "table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(%d, %d, %d, %d)\n", table[2][15].r, table[2][15].g, table[2][15].b, table[2][15].a);
                        fprintf(FileOut, "table[NK_COLOR_PROPERTY] = nk_rgba(%d, %d, %d, %d)\n", table[2][16].r, table[2][16].g, table[2][16].b, table[2][16].a);
                        fprintf(FileOut, "table[NK_COLOR_EDIT] = nk_rgba(%d, %d, %d, %d)\n", table[2][17].r, table[2][17].g, table[2][17].b, table[2][17].a);
                        fprintf(FileOut, "table[NK_COLOR_EDIT_CURSOR] = nk_rgba(%d, %d, %d, %d)\n", table[2][18].r, table[2][18].g, table[2][18].b, table[2][18].a);
                        fprintf(FileOut, "table[NK_COLOR_COMBO] = nk_rgba(%d, %d, %d, %d)\n", table[2][19].r, table[2][19].g, table[2][19].b, table[2][19].a);
                        fprintf(FileOut, "table[NK_COLOR_CHART] = nk_rgba(%d, %d, %d, %d)\n", table[2][20].r, table[2][20].g, table[2][20].b, table[2][20].a);
                        fprintf(FileOut, "table[NK_COLOR_CHART_COLOR] = nk_rgba(%d, %d, %d, %d)\n", table[2][21].r, table[2][21].g, table[2][21].b, table[2][21].a);
                        fprintf(FileOut, "table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(%d, %d, %d, %d)\n", table[2][22].r, table[2][22].g, table[2][22].b, table[2][22].a);
                        fprintf(FileOut, "table[NK_COLOR_SCROLLBAR] = nk_rgba(%d, %d, %d, %d)\n", table[2][23].r, table[2][23].g, table[2][23].b, table[2][23].a);
                        fprintf(FileOut, "table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(%d, %d, %d, %d)\n", table[2][24].r, table[2][24].g, table[2][24].b, table[2][24].a);
                        fprintf(FileOut, "table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(%d, %d, %d, %d)\n", table[2][25].r, table[2][25].g, table[2][25].b, table[2][25].a);
                        fprintf(FileOut, "table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(%d, %d, %d, %d)\n", table[2][26].r, table[2][26].g, table[2][26].b, table[2][26].a);
                        fprintf(FileOut, "table[NK_COLOR_TAB_HEADER] = nk_rgba(%d, %d, %d, %d)\n", table[2][27].r, table[2][27].g, table[2][27].b, table[2][27].a);
                        fprintf(FileOut, "[end_nk_style_v1]\n");

                        fclose( FileOut );// finished file reads, close the file.
                        }  // END if (!cancel_ops)
                    }  // END al_show_native_file_dialog
                else
                    {
                    file_read_err = 1;
                    cancel_ops = true;
                    }

                al_destroy_native_file_dialog(file_path);  // Check if this is in the correct location??
                file_save = 0;
                }  // END file_ret
            else
                {
                file_save--;  // Work around for stalled NK widgets.
                }

            cancel_ops = false;
            }


        // Do file read/write errors.
        if (file_read_err > 0)
            {
            // Do NK file dialog
            int about_w = 300 ;
            int about_h = 110;
            int about_x = (AL_WINDOW_W / 2) - (about_w/2);
            int about_y = (AL_WINDOW_H / 2) - (about_h/2);
            struct nk_rect error_rect = {about_x, about_y, about_w, about_h};
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "File Error", NK_WINDOW_TITLE|NK_WINDOW_CLOSABLE|NK_WINDOW_BORDER, error_rect))
                {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                // Consider nk_label_color
                nk_label(ctx, fr_err_list[file_read_err], NK_TEXT_CENTERED);
                nk_popup_end(ctx);
                }
            else
                {
                file_read_err = 0;
                cancel_ops = false;  // Should be redundant
                }
            }


        // Help message.
        if (help_dialog)
            {
            // Center dialog box on screen.
            // TODO: Clean this up.
            int help_w = 500;
            int help_h = 400;
            int help_x = (AL_WINDOW_W / 2) - (help_w/2);
            int help_y = (AL_WINDOW_H / 2) - (help_h/2);
            struct nk_rect help_rect = {help_x, help_y, help_w, help_h};
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "ANVSE Help", NK_WINDOW_TITLE|NK_WINDOW_CLOSABLE|NK_WINDOW_BORDER, help_rect))
                {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Allegro Nuklear Visual Style Editor", NK_TEXT_CENTERED);
                nk_label(ctx, "The editor starts with the default Nuklear Dark style.", NK_TEXT_LEFT);
                nk_label(ctx, "Use FILE -> Open to load a saved style.", NK_TEXT_LEFT);
                nk_label(ctx, "Use FILE -> Save to save a style. Make sure you give it a unique name.", NK_TEXT_LEFT);
                nk_label(ctx, "When saving the style add the .txt extension my_style[.txt|.c|.h].", NK_TEXT_LEFT);
                nk_label(ctx, "NOTE! Saved style files must follow the example format.", NK_TEXT_LEFT);
                nk_label(ctx, "Additional lines or C style comments (// /* */) are not allowed.", NK_TEXT_LEFT);
                nk_label(ctx, "", NK_TEXT_LEFT);  // Seperator
                nk_label(ctx, "Edit Theme.", NK_TEXT_CENTERED);
                nk_label(ctx, "The Reset All Colors will reset all items to the original style", NK_TEXT_LEFT);
                nk_label(ctx, "from the currently loaded file (or default style if no file loaded).", NK_TEXT_LEFT);
                nk_label(ctx, "Most of the controls are straight forward. select each NK_COLOR_xxx item", NK_TEXT_LEFT);
                nk_label(ctx, "and adjust the RGBA for the item. Press Apply to keep and test the color.", NK_TEXT_LEFT);
                nk_label(ctx, "If the Cancel Change is pressed before Apply, the color setting is undone.", NK_TEXT_LEFT);
                nk_label(ctx, "The top Reset this Color button will reset the adjustment for this item only.", NK_TEXT_LEFT);
                nk_label(ctx, "", NK_TEXT_LEFT);  // Seperator
                nk_label(ctx, "BACKGROUND_COLOR.", NK_TEXT_CENTERED);
                nk_label(ctx, "The background tree edit panel is seperat from all other items. You can", NK_TEXT_LEFT);
                nk_label(ctx, "adjust a background color to test alpha transparency. In the right panel", NK_TEXT_LEFT);
                nk_label(ctx, "you can select one of 2 background images or none for a flat color.",  NK_TEXT_LEFT);
                nk_label(ctx, "The Lock None and Lock All selection allows you to lock the alpha color",  NK_TEXT_LEFT);
                nk_label(ctx, "sliders togeter for all item adjustments.",  NK_TEXT_LEFT);
                nk_label(ctx, "---------",  NK_TEXT_CENTERED);
                nk_popup_end(ctx);
                }
            else help_dialog = nk_false;
            }

        /* About NK Theme */
        if (about_dialog)
            {
            // Center dialog box on screen.
            // TODO: Clean this up.
            int about_w = 300;
            int about_h = 210;
            int about_x = (AL_WINDOW_W / 2) - (about_w/2);
            int about_y = (AL_WINDOW_H / 2) - (about_h/2);
            struct nk_rect about_rect = {about_x, about_y, about_w, about_h};
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About", NK_WINDOW_TITLE|NK_WINDOW_CLOSABLE|NK_WINDOW_BORDER, about_rect))
                {
                nk_layout_row_dynamic(ctx, 20, 1);
                nk_label(ctx, "Allegro Nuklear Visual Style Editor Std (ANVSE)", NK_TEXT_CENTERED);
                nk_label(ctx, "Created by Axle",  NK_TEXT_CENTERED);  //
                nk_label(ctx, "Licence MIT0",  NK_TEXT_CENTERED);
                nk_label(ctx, "Many thanks to Nuklear and Allegro creators",  NK_TEXT_CENTERED);
                nk_label(ctx, "as well as all contributors :)",  NK_TEXT_CENTERED);

                float ratios0[] = {0.30f, 0.40f, 0.30f};
                nk_layout_row(ctx, NK_DYNAMIC, 40, 3, ratios0);
                nk_label(ctx, "", NK_TEXT_LEFT);
                if (nk_button_image(ctx, *logo))
                    {
                    ;  // Unused, Future.
                    }
                nk_label(ctx, "", NK_TEXT_RIGHT);
                nk_popup_end(ctx);
                }
            else about_dialog = nk_false;
            }

        // Do Exit check popup
        // quit_app gets the logic from ALLEGRO_EVENT_DISPLAY_CLOSE in main()
        if ((exit_check) || (*quit_app == 1))
            {
            int help_w = 260;
            int help_h = 140;
            int help_x = (AL_WINDOW_W / 2) - (help_w/2);
            int help_y = (AL_WINDOW_H / 2) - (help_h/2);
            struct nk_rect exit_rect = {help_x, help_y, help_w, help_h};
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "End application", NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_CLOSABLE, exit_rect))
                {
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Do you want to end the application?", NK_TEXT_CENTERED);
                nk_label(ctx, "Unsaved styles will be lost!", NK_TEXT_CENTERED);
                nk_layout_row_dynamic(ctx, 25, 2);
                if (nk_button_label(ctx, "OK"))
                    {
                    quit = nk_true;
                    nk_popup_close(ctx);
                    }
                if (nk_button_label(ctx, "Cancel"))
                    {
                    exit_check = nk_false;
                    *quit_app = 0;
                    nk_popup_close(ctx);
                    }
                nk_popup_end(ctx);
                }
            else
                {
                exit_check = nk_false;
                }

            if (quit)
                {
                // Send the quit logic to main loop.
                *quit_app = 2;
                exit_check = nk_false;
                }
            }


        // Test header color popup.
        if (popup_button)
            {
            static struct nk_rect s = {200, 100, 220, 110};
            if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Header Color", NK_WINDOW_TITLE|NK_WINDOW_BORDER|NK_WINDOW_CLOSABLE, s))
                {
                nk_layout_row_dynamic(ctx, 25, 1);
                nk_label(ctx, "Example header color", NK_TEXT_LEFT);
                nk_layout_row_dynamic(ctx, 25, 2);
                if (nk_button_label(ctx, "OK"))
                    {
                    popup_button = 0;
                    nk_popup_close(ctx);
                    }
                if (nk_button_label(ctx, "Cancel"))
                    {
                    popup_button = 0;
                    nk_popup_close(ctx);
                    }
                nk_popup_end(ctx);
                }
            else
                {
                popup_button = nk_false;
                }
            }

        if (show_title)
            {
            nk_layout_row_dynamic(ctx, 50, 1);
            // nk_allegro5_font_set_font(NkAllegro5Font *allegro5font)
            nk_allegro5_font_set_font(font_l);
            nk_label(ctx, "Allegro Nuklear Visual Style Editor Std (ANVSE)", NK_TEXT_LEFT);
            nk_allegro5_font_set_font(font);
            }

        // Horizontal rule/line
        if (show_hr)
            {
            // Will create a horizontal line/divider.
            nk_layout_row_dynamic(ctx, 2, 1);
            // The last argument is for rounding of the box true|false
            nk_rule_horizontal(ctx, ctx->style.window.border_color, nk_false);
            }


        // ###-> START TREE EDIT THEME ###
        if (nk_tree_push(ctx, NK_TREE_TAB, "Edit Theme", NK_MINIMIZED))
            {
            //enum menu_states {MENU_DEFAULT, MENU_WINDOWS};
            // 28 COLOR ITEMS
            // EDIT THEME Adjusters
            float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
            // EDIT THEME storage examples
            static int inactive = 1;  // Active button examples
            static int inactive_s = 1;  // Active slider examples
            static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
            static char text1[1][64];  // Text enter examples
            static int text_len[1];  // Text enter examples

            // Check box examples
            enum options_e {A,B,C};
            static int option_left_e;
            static int option_right_e;
            // Selectable examples
            static int selected_e[4] = {nk_false, nk_false, nk_true, nk_false};
            // Color Slider examples
            static struct nk_color combo_color_e = {130, 50, 50, 255};
            float ratios_e[] = {0.15f, 0.85f};
            // Progress bar examples
            static nk_size prog_value_e = 40;

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "Adjust NK_THEME_COLOR_xxx", NK_TEXT_LEFT);
            //nk_label(ctx, "Pressing reset all colors will revert to the original loaded theme.", NK_TEXT_LEFT);
            nk_label_colored(ctx, "Pressing reset all colors will revert to the original loaded theme.", NK_TEXT_LEFT, nk_rgba(200,120,50,255));
            if (nk_button_label(ctx, "Reset All Colors"))
                {
                // Apply the Altered color to the current theme|style
                resetall = nk_true;
                bg_resetall = nk_true;
                }


            if (nk_tree_push(ctx, NK_TREE_NODE, "BACKGROUND_COLOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Background color reset"))
                    {
                    // Apply the Altered color to the current theme|style
                    bg_reset_nk_color = nk_true;
                    }


                // START BACKGROUND_COLOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_BACKGROUND_COLOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    // Do NK tree Backgrounds, transparency etc
                    // nk_bk_color
                    // Use to get NK colors
                    // ALLEGRO_COLOR al_map_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)

                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT1", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, nk_bk_color[0]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        nk_bk_color[0].r = (nk_byte)nk_slide_int(ctx, 0, nk_bk_color[0].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        nk_bk_color[0].g = (nk_byte)nk_slide_int(ctx, 0, nk_bk_color[0].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        nk_bk_color[0].b = (nk_byte)nk_slide_int(ctx, 0, nk_bk_color[0].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        nk_bk_color[0].a = (nk_byte)nk_slide_int(ctx, 0, nk_bk_color[0].a, 255, 5);

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        nk_bk_color[0].r = nk_propertyf(ctx, "#R:", 0, nk_bk_color[0].r, 255, 1,1);
                        nk_bk_color[0].g = nk_propertyf(ctx, "#G:", 0, nk_bk_color[0].g, 255, 1,1);
                        nk_bk_color[0].b = nk_propertyf(ctx, "#B:", 0, nk_bk_color[0].b, 255, 1,1);
                        nk_bk_color[0].a = nk_propertyf(ctx, "#A:", 0, nk_bk_color[0].a, 255, 1,1);
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Background"))
                            {
                            // Apply the Altered color to the current theme|style
                            // Note that nk_bk_color[0] automatically updates in real time,
                            // Which is different behaviour to the rest of the adjusters.
                            // An additional nk_bk_color[n] needs to be added to hold the adjustment,
                            // and then send to nk_bk_color[0] on "Apply".
                            bg_theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            bg_clear_change = nk_true;
                            }
                        nk_group_end(ctx);  // END GROUP EDIT1
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View1", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);

                        //nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Select background image [None|Image1|Image2].", NK_TEXT_LEFT);
                        if (nk_option_label(ctx, "Image None", bg_img_op == IMG_0))
                            {
                            bg_img_op = IMG_0;
                            *img_select = 0;
                            }
                        if (nk_option_label(ctx, "Image One", bg_img_op == IMG_1))
                            {
                            bg_img_op = IMG_1;
                            *img_select = 1;
                            }
                        if (nk_option_label(ctx, "Image Two", bg_img_op == IMG_2))
                            {
                            bg_img_op = IMG_2;
                            *img_select = 2;
                            }

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Lock all widget alpha sliders together.", NK_TEXT_LEFT);
                        nk_label(ctx, "Selecting Lock All will reset all transparency to 255.", NK_TEXT_LEFT);
                        nk_label(ctx, "Selecting Lock NONE will leave all transparency at current position.", NK_TEXT_LEFT);
                        nk_label(ctx, "This does not affect the background color.", NK_TEXT_LEFT);
                        // The alpha_flag logic switch is required.
                        if (nk_option_label(ctx, "Lock None", alpha_op == NONE))
                            {
                            alpha_op = NONE;
                            if (*alpha_flag == 0)
                               {
                               //alpha_all = 255;  // Leave alpha sliders where they are.
                               *alpha_flag = 1;
                               }
                            }
                        if (nk_option_label(ctx, "Lock All", alpha_op == ALL))
                            {
                            alpha_op = ALL;
                            if (*alpha_flag == 1)
                               {
                               *alpha_all = 255;  // this will reset all alpha on first pass.
                               *alpha_flag = 0;
                               theme_apply = nk_true;
                               }
                            
                            }

                        nk_group_end(ctx);  // END GROUP VIEW1
                        }// END GROUP VIEW1
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END Group_BACKGROUND_COLOR
                    }  // END GROUP_NK_COLOR_TEXT
                nk_tree_pop(ctx);  // END Tree "Background Color"
                }  // END Tree "Background Color"


            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_TEXT", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_TEXT;
                    }

                // START GROUP_NK_COLOR_TEXT
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_TEXT", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT1", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_TEXT]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TEXT].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TEXT].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TEXT].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TEXT].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TEXT].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TEXT].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TEXT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TEXT].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TEXT].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TEXT].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_TEXT].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_TEXT].r, 255, 1,1);
                        table[0][NK_COLOR_TEXT].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_TEXT].g, 255, 1,1);
                        table[0][NK_COLOR_TEXT].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_TEXT].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TEXT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TEXT].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_TEXT].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TEXT].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT1
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View1", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW1
                        }  // END GROUP VIEW1
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }  // END GROUP_NK_COLOR_TEXT

                nk_tree_pop(ctx);  // END Tree "Edit Theme"
                }  // END Tree "Edit Theme"



            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_WINDOW", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_WINDOW;
                    }
                // Group_NK_COLOR_WINDOW
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_WINDOW", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT2", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_WINDOW]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_WINDOW].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_WINDOW].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_WINDOW].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_WINDOW].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_WINDOW].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_WINDOW].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_WINDOW].a = *alpha_all;
                            }
                        table[0][NK_COLOR_WINDOW].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_WINDOW].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_WINDOW].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_WINDOW].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_WINDOW].r, 255, 1,1);
                        table[0][NK_COLOR_WINDOW].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_WINDOW].g, 255, 1,1);
                        table[0][NK_COLOR_WINDOW].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_WINDOW].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_WINDOW].a = *alpha_all;
                            }
                        table[0][NK_COLOR_WINDOW].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_WINDOW].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_WINDOW].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View2", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_HEADER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_HEADER;
                    }
                // Group_NK_COLOR_HEADER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_HEADER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT3", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_HEADER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_HEADER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_HEADER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_HEADER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_HEADER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_HEADER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_HEADER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_HEADER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_HEADER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_HEADER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_HEADER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_HEADER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_HEADER].r, 255, 1,1);
                        table[0][NK_COLOR_HEADER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_HEADER].g, 255, 1,1);
                        table[0][NK_COLOR_HEADER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_HEADER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_HEADER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_HEADER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_HEADER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_HEADER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View3", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        if (nk_button_label(ctx, "Test header color"))
                            {
                            popup_button = nk_true;
                            }
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_BORDER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_BORDER;
                    }
                //Group_NK_COLOR_BORDER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_BORDER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT4", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_BORDER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BORDER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BORDER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BORDER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BORDER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BORDER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BORDER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BORDER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BORDER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BORDER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BORDER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_BORDER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_BORDER].r, 255, 1,1);
                        table[0][NK_COLOR_BORDER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_BORDER].g, 255, 1,1);
                        table[0][NK_COLOR_BORDER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_BORDER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BORDER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BORDER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_BORDER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BORDER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View4", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }

                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_BUTTON", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_BUTTON;
                    }
                //Group_NK_COLOR_BUTTON
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_BUTTON", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT5", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height
                        nk_button_color(ctx, table[0][NK_COLOR_BUTTON]);  //nk_button_color(ctx, nk_rgb(0,0,255));

                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_BUTTON].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_BUTTON].r, 255, 1,1);
                        table[0][NK_COLOR_BUTTON].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_BUTTON].g, 255, 1,1);
                        table[0][NK_COLOR_BUTTON].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_BUTTON].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_BUTTON].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;
                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View5", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_BUTTON_HOVER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_BUTTON_HOVER;
                    }
                //Group_NK_COLOR_BUTTON_HOVER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_BUTTON_HOVER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT6", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_BUTTON_HOVER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_HOVER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_HOVER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_HOVER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_HOVER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_HOVER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_HOVER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON_HOVER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_HOVER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON_HOVER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_BUTTON_HOVER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_BUTTON_HOVER].r, 255, 1,1);
                        table[0][NK_COLOR_BUTTON_HOVER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_BUTTON_HOVER].g, 255, 1,1);
                        table[0][NK_COLOR_BUTTON_HOVER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_BUTTON_HOVER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON_HOVER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_BUTTON_HOVER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON_HOVER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;
                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View6", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_BUTTON_ACTIVE", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_BUTTON_ACTIVE;
                    }
                //Group_NK_COLOR_BUTTON_ACTIVE
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_BUTTON_ACTIVE", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT7", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_BUTTON_ACTIVE]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_ACTIVE].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_ACTIVE].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_ACTIVE].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_ACTIVE].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_BUTTON_ACTIVE].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_ACTIVE].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON_ACTIVE].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_BUTTON_ACTIVE].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON_ACTIVE].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_BUTTON_ACTIVE].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_BUTTON_ACTIVE].r, 255, 1,1);
                        table[0][NK_COLOR_BUTTON_ACTIVE].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_BUTTON_ACTIVE].g, 255, 1,1);
                        table[0][NK_COLOR_BUTTON_ACTIVE].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_BUTTON_ACTIVE].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_BUTTON_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_BUTTON_ACTIVE].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_BUTTON_ACTIVE].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_BUTTON_ACTIVE].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View7", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_TOGGLE", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_TOGGLE;
                    }
                //Group_NK_COLOR_TOGGLE
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_TOGGLE", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT8", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_TOGGLE]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_TOGGLE].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_TOGGLE].r, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_TOGGLE].g, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_TOGGLE].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_TOGGLE].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View8", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //enum options {A,B,C};
                        //static int option_left;
                        //static int option_right;
                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_left_e = nk_option_label(ctx, "optionA", option_left_e == A) ? A : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionB", option_left_e == B) ? B : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionC", option_left_e == C) ? C : option_left_e;

                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_right_e = nk_option_label_align(ctx, "optionA", option_right_e == A, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? A : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionB", option_right_e == B, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? B : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionC", option_right_e == C, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? C : option_right_e;

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_TOGGLE_HOVER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_TOGGLE_HOVER;
                    }
                //Group_NK_COLOR_TOGGLE_HOVER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_TOGGLE_HOVER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {
                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT9", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_TOGGLE_HOVER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_HOVER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_HOVER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_HOVER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_HOVER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_HOVER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_HOVER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE_HOVER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_HOVER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE_HOVER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_TOGGLE_HOVER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_TOGGLE_HOVER].r, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE_HOVER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_TOGGLE_HOVER].g, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE_HOVER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_TOGGLE_HOVER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE_HOVER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_TOGGLE_HOVER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE_HOVER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View9", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        //enum options {A,B,C};
                        //static int option_left;
                        //static int option_right;
                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_left_e = nk_option_label(ctx, "optionA", option_left_e == A) ? A : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionB", option_left_e == B) ? B : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionC", option_left_e == C) ? C : option_left_e;

                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_right_e = nk_option_label_align(ctx, "optionA", option_right_e == A, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? A : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionB", option_right_e == B, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? B : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionC", option_right_e == C, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? C : option_right_e;

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }


            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_TOGGLE_CURSOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_TOGGLE_CURSOR;
                    }
                //Group_NK_COLOR_TOGGLE_CURSOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_TOGGLE_CURSOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT10", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_TOGGLE_CURSOR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_CURSOR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_CURSOR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_CURSOR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_CURSOR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TOGGLE_CURSOR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_CURSOR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE_CURSOR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TOGGLE_CURSOR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE_CURSOR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_TOGGLE_CURSOR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_TOGGLE_CURSOR].r, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE_CURSOR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_TOGGLE_CURSOR].g, 255, 1,1);
                        table[0][NK_COLOR_TOGGLE_CURSOR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_TOGGLE_CURSOR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TOGGLE_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TOGGLE_CURSOR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_TOGGLE_CURSOR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TOGGLE_CURSOR].a;
                            }
                        if (nk_button_label(ctx, "Apply Color"))
                            nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View10", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //enum options_e {A,B,C};
                        //static int option_left_e;
                        //static int option_right_e;
                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_left_e = nk_option_label(ctx, "optionA", option_left_e == A) ? A : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionB", option_left_e == B) ? B : option_left_e;
                        option_left_e = nk_option_label(ctx, "optionC", option_left_e == C) ? C : option_left_e;

                        nk_layout_row_static(ctx, 30, 80, 3);
                        option_right_e = nk_option_label_align(ctx, "optionA", option_right_e == A, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? A : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionB", option_right_e == B, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? B : option_right_e;
                        option_right_e = nk_option_label_align(ctx, "optionC", option_right_e == C, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? C : option_right_e;

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SELECT", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SELECT;
                    }
                //Group_NK_COLOR_SELECT
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SELECT", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT11", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SELECT]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SELECT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SELECT].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SELECT].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SELECT].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SELECT].r, 255, 1,1);
                        table[0][NK_COLOR_SELECT].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SELECT].g, 255, 1,1);
                        table[0][NK_COLOR_SELECT].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SELECT].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SELECT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SELECT].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SELECT].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SELECT].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View11", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //static int selected[4] = {nk_false, nk_false, nk_true, nk_false};
                        nk_layout_row_static(ctx, 18, 100, 1);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[0]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[1]);
                        nk_label(ctx, "Not Selectable", NK_TEXT_LEFT);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[2]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[3]);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SELECT_ACTIVE", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SELECT_ACTIVE;
                    }
                //Group_NK_COLOR_SELECT_ACTIVE
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SELECT_ACTIVE", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT12", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SELECT_ACTIVE]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT_ACTIVE].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT_ACTIVE].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT_ACTIVE].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT_ACTIVE].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SELECT_ACTIVE].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT_ACTIVE].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SELECT_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SELECT_ACTIVE].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SELECT_ACTIVE].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SELECT_ACTIVE].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SELECT_ACTIVE].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SELECT_ACTIVE].r, 255, 1,1);
                        table[0][NK_COLOR_SELECT_ACTIVE].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SELECT_ACTIVE].g, 255, 1,1);
                        table[0][NK_COLOR_SELECT_ACTIVE].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SELECT_ACTIVE].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SELECT_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SELECT_ACTIVE].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SELECT_ACTIVE].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SELECT_ACTIVE].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View12", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //static int selected_e[4] = {nk_false, nk_false, nk_true, nk_false};
                        nk_layout_row_static(ctx, 18, 100, 1);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[0]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[1]);
                        nk_label(ctx, "Not Selectable", NK_TEXT_LEFT);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[2]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected_e[3]);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SLIDER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SLIDER;
                    }
                //Group_NK_COLOR_SLIDER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SLIDER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT13", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SLIDER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SLIDER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SLIDER].r, 255, 1,1);
                        table[0][NK_COLOR_SLIDER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SLIDER].g, 255, 1,1);
                        table[0][NK_COLOR_SLIDER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SLIDER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SLIDER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View13", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Slider active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive_s);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive_s)
                            {
                            nk_widget_disable_begin(ctx);
                            }

                        //static struct nk_color combo_color = {130, 50, 50, 255};
                        //float ratios[] = {0.15f, 0.85f};
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratios_e);
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        combo_color_e.r = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        combo_color_e.g = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        combo_color_e.b = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        combo_color_e.a = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.a, 255, 5);
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //static nk_size prog_value = 40;
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %u", (int)prog_value_e);
                        nk_progress(ctx, &prog_value_e, 100, NK_MODIFIABLE);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SLIDER_CURSOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SLIDER_CURSOR;
                    }
                //Group_NK_COLOR_SLIDER_CURSOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SLIDER_CURSOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT14", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SLIDER_CURSOR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SLIDER_CURSOR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SLIDER_CURSOR].r, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SLIDER_CURSOR].g, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SLIDER_CURSOR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SLIDER_CURSOR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View14", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Slider active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive_s);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive_s)
                            {
                            nk_widget_disable_begin(ctx);
                            }

                        //static struct nk_color combo_color = {130, 50, 50, 255};
                        //float ratios[] = {0.15f, 0.85f};
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratios_e);
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        combo_color_e.r = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        combo_color_e.g = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        combo_color_e.b = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        combo_color_e.a = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.a, 255, 5);
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //static nk_size prog_value = 40;
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %u", (int)prog_value_e);
                        nk_progress(ctx, &prog_value_e, 100, NK_MODIFIABLE);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SLIDER_CURSOR_HOVER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SLIDER_CURSOR_HOVER;
                    }
                //Group_NK_COLOR_SLIDER_CURSOR_HOVER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SLIDER_CURSOR_HOVER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT15", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SLIDER_CURSOR_HOVER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].r, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].g, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR_HOVER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View15", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Slider active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive_s);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive_s)
                            {
                            nk_widget_disable_begin(ctx);
                            }

                        //static struct nk_color combo_color = {130, 50, 50, 255};
                        //float ratios[] = {0.15f, 0.85f};
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratios_e);
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        combo_color_e.r = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        combo_color_e.g = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        combo_color_e.b = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        combo_color_e.a = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.a, 255, 5);
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        //static nk_size prog_value = 40;
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %u", (int)prog_value_e);
                        nk_progress(ctx, &prog_value_e, 100, NK_MODIFIABLE);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            // This is used for progress bar
            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SLIDER_CURSOR_ACTIVE", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SLIDER_CURSOR_ACTIVE;
                    }
                //Group_NK_COLOR_SLIDER_CURSOR_ACTIVE
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SLIDER_CURSOR_ACTIVE", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT16", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].r, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].g, 255, 1,1);
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SLIDER_CURSOR_ACTIVE].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View16", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Slider active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive_s);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive_s)
                            {
                            nk_widget_disable_begin(ctx);
                            }

                        //static struct nk_color combo_color_e = {130, 50, 50, 255};
                        //float ratios_e[] = {0.15f, 0.85f};
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratios_e);
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        combo_color_e.r = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        combo_color_e.g = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        combo_color_e.b = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        combo_color_e.a = (nk_byte)nk_slide_int(ctx, 0, combo_color_e.a, 255, 5);
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //static nk_size prog_value_e = 40;
                        nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %u", (int)prog_value_e);
                        nk_progress(ctx, &prog_value_e, 100, NK_MODIFIABLE);


                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_PROPERTY", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_PROPERTY;
                    }
                //Group_NK_COLOR_PROPERTY
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_PROPERTY", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT17", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_PROPERTY]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_PROPERTY].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_PROPERTY].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_PROPERTY].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_PROPERTY].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_PROPERTY].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_PROPERTY].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_PROPERTY].a = *alpha_all;
                            }
                        table[0][NK_COLOR_PROPERTY].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_PROPERTY].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_PROPERTY].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_PROPERTY].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_PROPERTY].r, 255, 1,1);
                        table[0][NK_COLOR_PROPERTY].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_PROPERTY].g, 255, 1,1);
                        table[0][NK_COLOR_PROPERTY].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_PROPERTY].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_PROPERTY].a = *alpha_all;
                            }
                        table[0][NK_COLOR_PROPERTY].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_PROPERTY].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_PROPERTY].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View17", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_EDIT", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_EDIT;
                    }
                //Group_NK_COLOR_EDIT
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_EDIT", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT18", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_EDIT]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_EDIT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_EDIT].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_EDIT].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_EDIT].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_EDIT].r, 255, 1,1);
                        table[0][NK_COLOR_EDIT].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_EDIT].g, 255, 1,1);
                        table[0][NK_COLOR_EDIT].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_EDIT].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_EDIT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_EDIT].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_EDIT].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_EDIT].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View18", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            // I don't know what this does. nk_rgba(175,175,175,255)
            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_EDIT_CURSOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_EDIT_CURSOR;
                    }
                //Group_NK_COLOR_EDIT_CURSOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_EDIT_CURSOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT19", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_EDIT_CURSOR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT_CURSOR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT_CURSOR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT_CURSOR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT_CURSOR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_EDIT_CURSOR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT_CURSOR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_EDIT_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_EDIT_CURSOR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_EDIT_CURSOR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_EDIT_CURSOR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_EDIT_CURSOR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_EDIT_CURSOR].r, 255, 1,1);
                        table[0][NK_COLOR_EDIT_CURSOR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_EDIT_CURSOR].g, 255, 1,1);
                        table[0][NK_COLOR_EDIT_CURSOR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_EDIT_CURSOR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_EDIT_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_EDIT_CURSOR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_EDIT_CURSOR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_EDIT_CURSOR].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View19", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_COMBO", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_COMBO;
                    }
                //Group_NK_COLOR_COMBO
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_COMBO", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT20", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_COMBO]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_COMBO].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_COMBO].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_COMBO].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_COMBO].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_COMBO].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_COMBO].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_COMBO].a = *alpha_all;
                            }
                        table[0][NK_COLOR_COMBO].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_COMBO].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_COMBO].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_COMBO].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_COMBO].r, 255, 1,1);
                        table[0][NK_COLOR_COMBO].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_COMBO].g, 255, 1,1);
                        table[0][NK_COLOR_COMBO].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_COMBO].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_COMBO].a = *alpha_all;
                            }
                        table[0][NK_COLOR_COMBO].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_COMBO].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_COMBO].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View20", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);


                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // START Example COMBO
                        static float chart_selection = 8.0f;
                        static int current_weapon = 0;
                        static int check_values[5];
                        static float position[3];
                        static struct nk_color combo_color = {130, 50, 50, 255};
                        static struct nk_colorf combo_color2 = {0.509f, 0.705f, 0.2f, 1.0f};
                        static size_t prog_a =  20, prog_b = 40, prog_c = 10, prog_d = 90;
                        static const char *weapons[] = {"Fist","Pistol","Shotgun","Plasma","BFG"};

                        char buffer[64];
                        size_t sum = 0;

                        /* default combobox */
                        nk_layout_row_static(ctx, 25, 200, 1);
                        current_weapon = nk_combo(ctx, weapons, NK_LEN(weapons), current_weapon, 25, nk_vec2(200,200));

                        /* slider color combobox */
                        if (nk_combo_begin_color(ctx, combo_color, nk_vec2(200,200)))
                            {
                            float ratios[] = {0.15f, 0.85f};
                            nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios);
                            nk_label(ctx, "R:", NK_TEXT_LEFT);
                            combo_color.r = (nk_byte)nk_slide_int(ctx, 0, combo_color.r, 255, 5);
                            nk_label(ctx, "G:", NK_TEXT_LEFT);
                            combo_color.g = (nk_byte)nk_slide_int(ctx, 0, combo_color.g, 255, 5);
                            nk_label(ctx, "B:", NK_TEXT_LEFT);
                            combo_color.b = (nk_byte)nk_slide_int(ctx, 0, combo_color.b, 255, 5);
                            nk_label(ctx, "A:", NK_TEXT_LEFT);
                            combo_color.a = (nk_byte)nk_slide_int(ctx, 0, combo_color.a, 255, 5);
                            nk_combo_end(ctx);
                            }
                        /* complex color combobox */
                        if (nk_combo_begin_color(ctx, nk_rgb_cf(combo_color2), nk_vec2(200,400)))
                            {
                            enum color_mode {COL_RGB, COL_HSV};
                            static int col_mode = COL_RGB;
#ifndef DEMO_DO_NOT_USE_COLOR_PICKER
                            nk_layout_row_dynamic(ctx, 120, 1);
                            combo_color2 = nk_color_picker(ctx, combo_color2, NK_RGBA);
#endif

                            nk_layout_row_dynamic(ctx, 25, 2);
                            col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                            col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                            nk_layout_row_dynamic(ctx, 25, 1);
                            if (col_mode == COL_RGB)
                                {
                                combo_color2.r = nk_propertyf(ctx, "#R:", 0, combo_color2.r, 1.0f, 0.01f,0.005f);
                                combo_color2.g = nk_propertyf(ctx, "#G:", 0, combo_color2.g, 1.0f, 0.01f,0.005f);
                                combo_color2.b = nk_propertyf(ctx, "#B:", 0, combo_color2.b, 1.0f, 0.01f,0.005f);
                                combo_color2.a = nk_propertyf(ctx, "#A:", 0, combo_color2.a, 1.0f, 0.01f,0.005f);
                                }
                            else
                                {
                                float hsva[4];
                                nk_colorf_hsva_fv(hsva, combo_color2);
                                hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f,0.05f);
                                hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f,0.05f);
                                hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f,0.05f);
                                hsva[3] = nk_propertyf(ctx, "#A:", 0, hsva[3], 1.0f, 0.01f,0.05f);
                                combo_color2 = nk_hsva_colorfv(hsva);
                                }
                            nk_combo_end(ctx);
                            }
                        /* progressbar combobox */
                        sum = prog_a + prog_b + prog_c + prog_d;
                        sprintf(buffer, "%lu", (unsigned long)sum);  // (unsigned long) Altered by Axle
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                            {
                            nk_layout_row_dynamic(ctx, 30, 1);
                            nk_progress(ctx, &prog_a, 100, NK_MODIFIABLE);
                            nk_progress(ctx, &prog_b, 100, NK_MODIFIABLE);
                            nk_progress(ctx, &prog_c, 100, NK_MODIFIABLE);
                            nk_progress(ctx, &prog_d, 100, NK_MODIFIABLE);
                            nk_combo_end(ctx);
                            }

                        /* checkbox combobox */
                        sum = (size_t)(check_values[0] + check_values[1] + check_values[2] + check_values[3] + check_values[4]);
                        sprintf(buffer, "%lu", (unsigned long)sum);  // (unsigned long) Altered by Axle
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                            {
                            nk_layout_row_dynamic(ctx, 30, 1);
                            nk_checkbox_label(ctx, weapons[0], &check_values[0]);
                            nk_checkbox_label(ctx, weapons[1], &check_values[1]);
                            nk_checkbox_label(ctx, weapons[2], &check_values[2]);
                            nk_checkbox_label(ctx, weapons[3], &check_values[3]);
                            nk_combo_end(ctx);
                            }

                        /* complex text combobox */
                        sprintf(buffer, "%.2f, %.2f, %.2f", position[0], position[1],position[2]);
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                            {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            nk_property_float(ctx, "#X:", -1024.0f, &position[0], 1024.0f, 1,0.5f);
                            nk_property_float(ctx, "#Y:", -1024.0f, &position[1], 1024.0f, 1,0.5f);
                            nk_property_float(ctx, "#Z:", -1024.0f, &position[2], 1024.0f, 1,0.5f);
                            nk_combo_end(ctx);
                            }

                        /* chart combobox */
                        sprintf(buffer, "%.1f", chart_selection);
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,250)))
                            {
                            size_t i = 0;
                            static const float values[]= {26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f, 12.0f, 8.0f, 22.0f, 28.0f, 5.0f};
                            nk_layout_row_dynamic(ctx, 150, 1);
                            nk_chart_begin(ctx, NK_CHART_COLUMN, NK_LEN(values), 0, 50);
                            for (i = 0; i < NK_LEN(values); ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, values[i]);
                                if (res & NK_CHART_CLICKED)
                                    {
                                    chart_selection = values[i];
                                    nk_combo_close(ctx);
                                    }
                                }
                            nk_chart_end(ctx);
                            nk_combo_end(ctx);
                            }  // END Example COMBO

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_CHART", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_CHART;
                    }
                //Group_NK_COLOR_CHART
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_CHART", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT21", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_CHART]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_CHART].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_CHART].r, 255, 1,1);
                        table[0][NK_COLOR_CHART].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_CHART].g, 255, 1,1);
                        table[0][NK_COLOR_CHART].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_CHART].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_CHART].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View21", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        /* Chart Widgets
                         * This library has two different rather simple charts. The line and the
                         * column chart. Both provide a simple way of visualizing values and
                         * have a retained mode and immediate mode API version. For the retain
                         * mode version `nk_plot` and `nk_plot_function` you either provide
                         * an array or a callback to call to handle drawing the graph.
                         * For the immediate mode version you start by calling `nk_chart_begin`
                         * and need to provide min and max values for scaling on the Y-axis.
                         * and then call `nk_chart_push` to push values into the chart.
                         * Finally `nk_chart_end` needs to be called to end the process. */
                        float id = 0;
                        static int col_index = -1;
                        static int line_index = -1;
                        float step = (2*3.141592654f) / 32;

                        int i;
                        int index = -1;

                        /* line chart */
                        id = 0;
                        index = -1;
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)cos(id));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    line_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }

                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index*step));
                        if (line_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index*step));
                            }

                        /* column chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)fabs(sin(id)));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    col_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }
                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)fabs(sin(step * (float)index)));
                        if (col_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)fabs(sin(step * (float)col_index)));
                            }

                        /* mixed colored chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                            {
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,255,0), nk_rgb(0,150,0), 32, -1.0f, 1.0f);
                            for (id = 0, i = 0; i < 32; ++i)
                                {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                nk_chart_push_slot(ctx, (float)cos(id), 1);
                                nk_chart_push_slot(ctx, (float)sin(id), 2);
                                id += step;
                                }
                            }

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_CHART_COLOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_CHART_COLOR;
                    }
                //Group_NK_COLOR_CHART_COLOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_CHART_COLOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT22", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_CHART_COLOR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART_COLOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART_COLOR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART_COLOR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_CHART_COLOR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_CHART_COLOR].r, 255, 1,1);
                        table[0][NK_COLOR_CHART_COLOR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_CHART_COLOR].g, 255, 1,1);
                        table[0][NK_COLOR_CHART_COLOR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_CHART_COLOR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART_COLOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART_COLOR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_CHART_COLOR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART_COLOR].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View22", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);


                        /* Chart Widgets
                         * This library has two different rather simple charts. The line and the
                         * column chart. Both provide a simple way of visualizing values and
                         * have a retained mode and immediate mode API version. For the retain
                         * mode version `nk_plot` and `nk_plot_function` you either provide
                         * an array or a callback to call to handle drawing the graph.
                         * For the immediate mode version you start by calling `nk_chart_begin`
                         * and need to provide min and max values for scaling on the Y-axis.
                         * and then call `nk_chart_push` to push values into the chart.
                         * Finally `nk_chart_end` needs to be called to end the process. */
                        float id = 0;
                        static int col_index = -1;
                        static int line_index = -1;
                        float step = (2*3.141592654f) / 32;

                        int i;
                        int index = -1;

                        /* line chart */
                        id = 0;
                        index = -1;
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)cos(id));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    line_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }

                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index*step));
                        if (line_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index*step));
                            }

                        /* column chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)fabs(sin(id)));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    col_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }
                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)fabs(sin(step * (float)index)));
                        if (col_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)fabs(sin(step * (float)col_index)));
                            }

                        /* mixed colored chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                            {
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,255,0), nk_rgb(0,150,0), 32, -1.0f, 1.0f);
                            for (id = 0, i = 0; i < 32; ++i)
                                {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                nk_chart_push_slot(ctx, (float)cos(id), 1);
                                nk_chart_push_slot(ctx, (float)sin(id), 2);
                                id += step;
                                }
                            }

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_CHART_COLOR_HIGHLIGHT", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_CHART_COLOR_HIGHLIGHT;
                    }
                //Group_NK_COLOR_CHART_COLOR_HIGHLIGHT
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_CHART_COLOR_HIGHLIGHT", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT23", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].r, 255, 1,1);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].g, 255, 1,1);
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a = *alpha_all;
                            }
                        table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_CHART_COLOR_HIGHLIGHT].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View23", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);


                        /* Chart Widgets
                         * This library has two different rather simple charts. The line and the
                         * column chart. Both provide a simple way of visualizing values and
                         * have a retained mode and immediate mode API version. For the retain
                         * mode version `nk_plot` and `nk_plot_function` you either provide
                         * an array or a callback to call to handle drawing the graph.
                         * For the immediate mode version you start by calling `nk_chart_begin`
                         * and need to provide min and max values for scaling on the Y-axis.
                         * and then call `nk_chart_push` to push values into the chart.
                         * Finally `nk_chart_end` needs to be called to end the process. */
                        float id = 0;
                        static int col_index = -1;
                        static int line_index = -1;
                        float step = (2*3.141592654f) / 32;

                        int i;
                        int index = -1;

                        /* line chart */
                        id = 0;
                        index = -1;
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)cos(id));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    line_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }

                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index*step));
                        if (line_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index*step));
                            }

                        /* column chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f))
                            {
                            for (i = 0; i < 32; ++i)
                                {
                                nk_flags res = nk_chart_push(ctx, (float)fabs(sin(id)));
                                if (res & NK_CHART_HOVERING)
                                    index = (int)i;
                                if (res & NK_CHART_CLICKED)
                                    col_index = (int)i;
                                id += step;
                                }
                            nk_chart_end(ctx);
                            }
                        if (index != -1)
                            nk_tooltipf(ctx, "Value: %.2f", (float)fabs(sin(step * (float)index)));
                        if (col_index != -1)
                            {
                            nk_layout_row_dynamic(ctx, 20, 1);
                            nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)fabs(sin(step * (float)col_index)));
                            }

                        /* mixed colored chart */
                        nk_layout_row_dynamic(ctx, 100, 1);
                        if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                            {
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                            nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,255,0), nk_rgb(0,150,0), 32, -1.0f, 1.0f);
                            for (id = 0, i = 0; i < 32; ++i)
                                {
                                nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                nk_chart_push_slot(ctx, (float)cos(id), 1);
                                nk_chart_push_slot(ctx, (float)sin(id), 2);
                                id += step;
                                }
                            }



                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SCROLLBAR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SCROLLBAR;
                    }
                //Group_NK_COLOR_SCROLLBAR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SCROLLBAR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT24", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SCROLLBAR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SCROLLBAR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SCROLLBAR].r, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SCROLLBAR].g, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SCROLLBAR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SCROLLBAR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View24", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SCROLLBAR_CURSOR", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SCROLLBAR_CURSOR;
                    }
                //Group_NK_COLOR_SCROLLBAR_CURSOR
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SCROLLBAR_CURSOR", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT25", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SCROLLBAR_CURSOR]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].r, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].g, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View25", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SCROLLBAR_CURSOR_HOVER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SCROLLBAR_CURSOR_HOVER;
                    }
                //Group_NK_COLOR_SCROLLBAR_CURSOR_HOVER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SCROLLBAR_CURSOR_HOVER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT26", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].r, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].g, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR_HOVER].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;
                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View26", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_SCROLLBAR_CURSOR_ACTIVE", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_SCROLLBAR_CURSOR_ACTIVE;
                    }
                //Group_NK_COLOR_SCROLLBAR_CURSOR_ACTIVE
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_SCROLLBAR_CURSOR_ACTIVE", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT27", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].r, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].g, 255, 1,1);
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a = *alpha_all;
                            }
                        table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE].a;
                            }
                        nk_layout_row_dynamic(ctx, 30, 2);
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View27", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_NODE, "NK_COLOR_TAB_HEADER", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 1);
                if (nk_button_label(ctx, "Reset this Color"))
                    {
                    // Reset the color to the original loaded style.
                    reset_nk_color = NK_COLOR_TAB_HEADER;
                    }
                //Group_NK_COLOR_TAB_HEADER
                nk_layout_row_dynamic(ctx, 360, 1);  // Height
                if (nk_group_begin(ctx, "Group_NK_COLOR_TAB_HEADER", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                    {

                    //nk_layout_row_static(ctx, 380, 250, 2);
                    nk_layout_row_begin(ctx, NK_STATIC, 350, 2);  // 2 columns at height 320
                    nk_layout_row_push(ctx, 250);  // Split columns by pixel width (col 1).
                    if (nk_group_begin(ctx, "Group_EDIT28", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);  // Height

                        nk_button_color(ctx, table[0][NK_COLOR_TAB_HEADER]);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        // Slider adjust
                        //float ratios1[] = {0.15f, 0.85f};  // Slider ratio. Must be float!
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios1);  // Must be float!
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TAB_HEADER].r = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TAB_HEADER].r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TAB_HEADER].g = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TAB_HEADER].g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        table[0][NK_COLOR_TAB_HEADER].b = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TAB_HEADER].b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TAB_HEADER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TAB_HEADER].a = (nk_byte)nk_slide_int(ctx, 0, table[0][NK_COLOR_TAB_HEADER].a, 255, 5);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TAB_HEADER].a;
                            }

                        // Properties adjust
                        nk_layout_row_dynamic(ctx, 30, 1);
                        table[0][NK_COLOR_TAB_HEADER].r = nk_propertyf(ctx, "#R:", 0, table[0][NK_COLOR_TAB_HEADER].r, 255, 1,1);
                        table[0][NK_COLOR_TAB_HEADER].g = nk_propertyf(ctx, "#G:", 0, table[0][NK_COLOR_TAB_HEADER].g, 255, 1,1);
                        table[0][NK_COLOR_TAB_HEADER].b = nk_propertyf(ctx, "#B:", 0, table[0][NK_COLOR_TAB_HEADER].b, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            table[0][NK_COLOR_TAB_HEADER].a = *alpha_all;
                            }
                        table[0][NK_COLOR_TAB_HEADER].a = nk_propertyf(ctx, "#A:", 0, table[0][NK_COLOR_TAB_HEADER].a, 255, 1,1);
                        if (alpha_op == ALL)
                            {
                            *alpha_all = table[0][NK_COLOR_TAB_HEADER].a;
                            }
                        if (nk_button_label(ctx, "Apply Color"))
                            {
                            // Apply the Altered color to the current theme|style
                            theme_apply = nk_true;
                            }
                        if (nk_button_label(ctx, "Cancel Change"))
                            {
                            // Clear the color sliders to current style.
                            clear_change = nk_true;

                            }
                        nk_group_end(ctx);  // END GROUP EDIT
                        }

                    // START GROUP VIEW
                    nk_layout_row_push(ctx, 380);  // Split columns by pixel width (col 2).
                    if (nk_group_begin(ctx, "Group_View28", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        //nk_layout_row_static(ctx, 30, 350, 1);

                        //nk_button_color(ctx, combo_color);  //nk_button_color(ctx, nk_rgb(0,0,255));
                        nk_button_label(ctx, "Sample button");
                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        nk_label(ctx, "Sample Label", NK_TEXT_LEFT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Symbols
                        nk_layout_row_static(ctx, 25, 25, 8);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                        nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                        nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator
                        // Button active|inactive example
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_checkbox_label(ctx, "Inactive", &inactive);

                        nk_layout_row_static(ctx, 25, 80, 1);
                        if (inactive)
                            {
                            nk_widget_disable_begin(ctx);
                            }
                        nk_button_label(ctx, "button");
                        nk_widget_disable_end(ctx);

                        nk_label(ctx, "", NK_TEXT_LEFT);  // Separator

                        // Text enter example
                        //static const float ratio2[] = {0.30f, 0.70f};  // Text enter examples
                        //static char text1[1][64];  // Text enter examples
                        //static int text_len[1];  // Text enter examples
                        nk_layout_row(ctx, NK_DYNAMIC, 25, 2, ratio2);
                        nk_label(ctx, "Default:", NK_TEXT_LEFT);
                        nk_edit_string(ctx, NK_EDIT_SIMPLE, text1[0], &text_len[0], 64, nk_filter_default);

                        nk_group_end(ctx);  // END GROUP VIEW
                        }
                    nk_layout_row_end(ctx);  // end split columns.
                    //
                    nk_group_end(ctx);  // END GROUP_NK_COLOR_TEXT
                    }
                nk_tree_pop(ctx);
                }


            // Reset all colors from loaded table.
            if (resetall == nk_true)
                {
                //nk_style_from_table(ctx, table[0]);
                for (int n = 0; n < NK_COLOR_COUNT; n++)
                    {


                    table[0][n].r = table[1][n].r;
                    table[0][n].g = table[1][n].g;
                    table[0][n].b = table[1][n].b;
                    table[0][n].a = table[1][n].a;
                    if (alpha_op == ALL)
                        {
                        table[0][n].a = *alpha_all;
                        }

                    table[2][n].r = table[1][n].r;
                    table[2][n].g = table[1][n].g;
                    table[2][n].b = table[1][n].b;
                    table[2][n].a = table[1][n].a;
                    // Keep alpha ALL when applied
                    if (alpha_op == ALL)
                        {
                        table[2][n].a = *alpha_all;
                        }
                    }
                //theme_apply = nk_true;
                nk_style_from_table(ctx, table[0]);
                resetall = nk_false;
                }

            // Reset individual color from loaded table.
            if (reset_nk_color >= 0)
                {
                table[0][reset_nk_color].r = table[1][reset_nk_color].r;
                table[0][reset_nk_color].g = table[1][reset_nk_color].g;
                table[0][reset_nk_color].b = table[1][reset_nk_color].b;
                table[0][reset_nk_color].a = table[1][reset_nk_color].a;
                // Keep alpha ALL when applied
                if (alpha_op == ALL)
                    {
                    table[0][reset_nk_color].a = *alpha_all;
                    }

                //theme_apply = nk_true;
                nk_style_from_table(ctx, table[0]);
                reset_nk_color = -1;
                }

            if (clear_change == nk_true)
                {
                for (int n = 0; n < NK_COLOR_COUNT; n++)
                    {
                    table[0][n].r = table[2][n].r;
                    table[0][n].g = table[2][n].g;
                    table[0][n].b = table[2][n].b;
                    table[0][n].a = table[2][n].a;
                    // Keep alpha ALL
                    if (alpha_op == ALL)
                        {
                        table[0][n].a = *alpha_all;
                        }
                    }
                clear_change = nk_false;
                }

            // Apply them and update backup slot.
            if (theme_apply == nk_true)
                {
                for (int n = 0; n < NK_COLOR_COUNT; n++)
                    {
                    table[2][n].r = table[0][n].r;
                    table[2][n].g = table[0][n].g;
                    table[2][n].b = table[0][n].b;
                    table[2][n].a = table[0][n].a;
                    // Keep alpha ALL when applied
                    if (alpha_op == ALL)
                        {
                        table[0][n].a = *alpha_all;
                        }
                    }
                nk_style_from_table(ctx, table[0]);
                theme_apply = nk_false;
                }

            // Background color and images operate indepenently from widget controls.
            // Reset background colors from loaded table.
            if (bg_resetall == nk_true)
                {
                nk_bk_color[0].r = nk_bk_color[1].r;
                nk_bk_color[0].g = nk_bk_color[1].g;
                nk_bk_color[0].b = nk_bk_color[1].b;
                nk_bk_color[0].a = nk_bk_color[1].a;

                nk_bk_color[2].r = nk_bk_color[1].r;
                nk_bk_color[2].g = nk_bk_color[1].g;
                nk_bk_color[2].b = nk_bk_color[1].b;
                nk_bk_color[2].a = nk_bk_color[1].a;

                bg_theme_apply = nk_true;
                bg_resetall = nk_false;
                }

            // Reset individual color from loaded table.
            if (bg_reset_nk_color == nk_true)
                {
                nk_bk_color[0].r = nk_bk_color[1].r;
                nk_bk_color[0].g = nk_bk_color[1].g;
                nk_bk_color[0].b = nk_bk_color[1].b;
                nk_bk_color[0].a = nk_bk_color[1].a;
                bg_theme_apply = nk_true;
                bg_reset_nk_color = nk_false;
                }

            // Clear current color slides befor apply.
            if (bg_clear_change == nk_true)
                {
                nk_bk_color[0].r = nk_bk_color[2].r;
                nk_bk_color[0].g = nk_bk_color[2].g;
                nk_bk_color[0].b = nk_bk_color[2].b;
                nk_bk_color[0].a = nk_bk_color[2].a;

                bg_clear_change = nk_false;
                }

            // Apply background to backup slot
            if (bg_theme_apply == nk_true)
                {
                nk_bk_color[2].r = nk_bk_color[0].r;
                nk_bk_color[2].g = nk_bk_color[0].g;
                nk_bk_color[2].b = nk_bk_color[0].b;
                nk_bk_color[2].a = nk_bk_color[0].a;

                bg_theme_apply = nk_false;
                }

            nk_tree_pop(ctx);
            }  // ### END TREE EDIT THEME <-###


        // ###-> START overview.c EXAMPLES ###
        if (nk_tree_push(ctx, NK_TREE_TAB, "Theme Tests", NK_MINIMIZED))
            {

            if (show_app_about)
                {
                /* about popup */
                static struct nk_rect s = {20, 100, 300, 190};
                if (nk_popup_begin(ctx, NK_POPUP_STATIC, "About", NK_WINDOW_CLOSABLE, s))
                    {
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_label(ctx, "Nuklear", NK_TEXT_LEFT);
                    nk_label(ctx, "By Micha Mettke", NK_TEXT_LEFT);
                    nk_label(ctx, "nuklear is licensed under the public domain License.",  NK_TEXT_LEFT);
                    nk_popup_end(ctx);
                    }
                else show_app_about = nk_false;
                }

            /* window flags */
            if (nk_tree_push(ctx, NK_TREE_TAB, "Window", NK_MINIMIZED))
                {
                nk_layout_row_dynamic(ctx, 30, 2);
                nk_checkbox_label(ctx, "Menu", &show_menu);
                nk_checkbox_flags_label(ctx, "Titlebar", &window_flags, NK_WINDOW_TITLE);
                nk_checkbox_flags_label(ctx, "Border", &window_flags, NK_WINDOW_BORDER);
                nk_checkbox_flags_label(ctx, "Resizable", &window_flags, NK_WINDOW_SCALABLE);
                nk_checkbox_flags_label(ctx, "Movable", &window_flags, NK_WINDOW_MOVABLE);
                nk_checkbox_flags_label(ctx, "No Scrollbar", &window_flags, NK_WINDOW_NO_SCROLLBAR);
                nk_checkbox_flags_label(ctx, "Minimizable", &window_flags, NK_WINDOW_MINIMIZABLE);
                nk_checkbox_flags_label(ctx, "Scale Left", &window_flags, NK_WINDOW_SCALE_LEFT);
                nk_checkbox_label(ctx, "Disable widgets", &disable_widgets);
                nk_tree_pop(ctx);
                }

            if (disable_widgets)
                nk_widget_disable_begin(ctx);

            if (nk_tree_push(ctx, NK_TREE_TAB, "Widgets", NK_MINIMIZED))
                {
                enum options {A,B,C};
                static int checkbox_left_text_left;
                static int checkbox_centered_text_right;
                static int checkbox_right_text_right;
                static int checkbox_right_text_left;
                static int option_left;
                static int option_right;
                if (nk_tree_push(ctx, NK_TREE_NODE, "Text", NK_MINIMIZED))
                    {
                    /* Text Widgets */
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_label(ctx, "Label aligned left", NK_TEXT_LEFT);
                    nk_label(ctx, "Label aligned centered", NK_TEXT_CENTERED);
                    nk_label(ctx, "Label aligned right", NK_TEXT_RIGHT);
                    nk_label_colored(ctx, "Blue text", NK_TEXT_LEFT, nk_rgb(0,0,255));
                    nk_label_colored(ctx, "Yellow text", NK_TEXT_LEFT, nk_rgb(255,255,0));
                    nk_text(ctx, "Text without /0", 15, NK_TEXT_RIGHT);

                    nk_layout_row_static(ctx, 100, 200, 1);
                    nk_label_wrap(ctx, "This is a very long line to hopefully get this text to be wrapped into multiple lines to show line wrapping");
                    nk_layout_row_dynamic(ctx, 100, 1);
                    nk_label_wrap(ctx, "This is another long text to show dynamic window changes on multiline text");
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Button", NK_MINIMIZED))
                    {
                    /* Buttons Widgets */
                    nk_layout_row_static(ctx, 30, 100, 3);
                    if (nk_button_label(ctx, "Button"))
                        fprintf(stdout, "Button pressed!\n");
                    nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
                    if (nk_button_label(ctx, "Repeater"))
                        fprintf(stdout, "Repeater is being pressed!\n");
                    nk_button_set_behavior(ctx, NK_BUTTON_DEFAULT);
                    nk_button_color(ctx, nk_rgb(0,0,255));

                    nk_layout_row_static(ctx, 25, 25, 8);
                    nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_SOLID);
                    nk_button_symbol(ctx, NK_SYMBOL_CIRCLE_OUTLINE);
                    nk_button_symbol(ctx, NK_SYMBOL_RECT_SOLID);
                    nk_button_symbol(ctx, NK_SYMBOL_RECT_OUTLINE);
                    nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_UP);
                    nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_DOWN);
                    nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT);
                    nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT);

                    nk_layout_row_static(ctx, 30, 100, 2);
                    nk_button_symbol_label(ctx, NK_SYMBOL_TRIANGLE_LEFT, "prev", NK_TEXT_RIGHT);
                    nk_button_symbol_label(ctx, NK_SYMBOL_TRIANGLE_RIGHT, "next", NK_TEXT_LEFT);

                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Basic", NK_MINIMIZED))
                    {
                    /* Basic widgets */
                    static int int_slider = 5;
                    static float float_slider = 2.5f;
                    static nk_size prog_value = 40;
                    static float property_float = 2;
                    static int property_int = 10;
                    static int property_neg = 10;

                    static float range_float_min = 0;
                    static float range_float_max = 100;
                    static float range_float_value = 50;
                    static int range_int_min = 0;
                    static int range_int_value = 2048;
                    static int range_int_max = 4096;
                    static const float ratio[] = {120, 150};

                    nk_layout_row_dynamic(ctx, 0, 1);
                    nk_checkbox_label(ctx, "CheckLeft TextLeft", &checkbox_left_text_left);
                    nk_checkbox_label_align(ctx, "CheckCenter TextRight", &checkbox_centered_text_right, NK_WIDGET_ALIGN_CENTERED | NK_WIDGET_ALIGN_MIDDLE, NK_TEXT_RIGHT);
                    nk_checkbox_label_align(ctx, "CheckRight TextRight", &checkbox_right_text_right, NK_WIDGET_LEFT, NK_TEXT_RIGHT);
                    nk_checkbox_label_align(ctx, "CheckRight TextLeft", &checkbox_right_text_left, NK_WIDGET_RIGHT, NK_TEXT_LEFT);

                    nk_layout_row_static(ctx, 30, 80, 3);
                    option_left = nk_option_label(ctx, "optionA", option_left == A) ? A : option_left;
                    option_left = nk_option_label(ctx, "optionB", option_left == B) ? B : option_left;
                    option_left = nk_option_label(ctx, "optionC", option_left == C) ? C : option_left;

                    nk_layout_row_static(ctx, 30, 80, 3);
                    option_right = nk_option_label_align(ctx, "optionA", option_right == A, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? A : option_right;
                    option_right = nk_option_label_align(ctx, "optionB", option_right == B, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? B : option_right;
                    option_right = nk_option_label_align(ctx, "optionC", option_right == C, NK_WIDGET_RIGHT, NK_TEXT_RIGHT) ? C : option_right;

                    nk_layout_row(ctx, NK_STATIC, 30, 2, ratio);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Slider int");
                    nk_slider_int(ctx, 0, &int_slider, 10, 1);

                    nk_label(ctx, "Slider float", NK_TEXT_LEFT);
                    nk_slider_float(ctx, 0, &float_slider, 5.0, 0.5f);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Progressbar: %u", (int)prog_value);
                    nk_progress(ctx, &prog_value, 100, NK_MODIFIABLE);

                    nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                    nk_label(ctx, "Property float:", NK_TEXT_LEFT);
                    nk_property_float(ctx, "Float:", 0, &property_float, 64.0f, 0.1f, 0.2f);
                    nk_label(ctx, "Property int:", NK_TEXT_LEFT);
                    nk_property_int(ctx, "Int:", 0, &property_int, 100, 1, 1);
                    nk_label(ctx, "Property neg:", NK_TEXT_LEFT);
                    nk_property_int(ctx, "Neg:", -10, &property_neg, 10, 1, 1);

                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_label(ctx, "Range:", NK_TEXT_LEFT);
                    nk_layout_row_dynamic(ctx, 25, 3);
                    nk_property_float(ctx, "#min:", 0, &range_float_min, range_float_max, 1.0f, 0.2f);
                    nk_property_float(ctx, "#float:", range_float_min, &range_float_value, range_float_max, 1.0f, 0.2f);
                    nk_property_float(ctx, "#max:", range_float_min, &range_float_max, 100, 1.0f, 0.2f);

                    nk_property_int(ctx, "#min:", INT_MIN, &range_int_min, range_int_max, 1, 10);
                    nk_property_int(ctx, "#neg:", range_int_min, &range_int_value, range_int_max, 1, 10);
                    nk_property_int(ctx, "#max:", range_int_min, &range_int_max, INT_MAX, 1, 10);

                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Inactive", NK_MINIMIZED))
                    {
                    static int inactive = 1;
                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_checkbox_label(ctx, "Inactive", &inactive);

                    nk_layout_row_static(ctx, 30, 80, 1);
                    if (inactive)
                        {
                        nk_widget_disable_begin(ctx);
                        }

                    if (nk_button_label(ctx, "button"))
                        fprintf(stdout, "button pressed\n");

                    nk_widget_disable_end(ctx);

                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Selectable", NK_MINIMIZED))
                    {
                    if (nk_tree_push(ctx, NK_TREE_NODE, "List", NK_MINIMIZED))
                        {
                        static int selected[4] = {nk_false, nk_false, nk_true, nk_false};
                        nk_layout_row_static(ctx, 18, 100, 1);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[0]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[1]);
                        nk_label(ctx, "Not Selectable", NK_TEXT_LEFT);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[2]);
                        nk_selectable_label(ctx, "Selectable", NK_TEXT_LEFT, &selected[3]);
                        nk_tree_pop(ctx);
                        }
                    if (nk_tree_push(ctx, NK_TREE_NODE, "Grid", NK_MINIMIZED))
                        {
                        int i;
                        static int selected[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
                        nk_layout_row_static(ctx, 50, 50, 4);
                        for (i = 0; i < 16; ++i)
                            {
                            if (nk_selectable_label(ctx, "Z", NK_TEXT_CENTERED, &selected[i]))
                                {
                                int x = (i % 4), y = i / 4;
                                if (x > 0) selected[i - 1] ^= 1;
                                if (x < 3) selected[i + 1] ^= 1;
                                if (y > 0) selected[i - 4] ^= 1;
                                if (y < 3) selected[i + 4] ^= 1;
                                }
                            }
                        nk_tree_pop(ctx);
                        }
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Combo", NK_MINIMIZED))
                    {
                    /* Combobox Widgets
                     * In this library comboboxes are not limited to being a popup
                     * list of selectable text. Instead it is a abstract concept of
                     * having something that is *selected* or displayed, a popup window
                     * which opens if something needs to be modified and the content
                     * of the popup which causes the *selected* or displayed value to
                     * change or if wanted close the combobox.
                     *
                     * While strange at first handling comboboxes in a abstract way
                     * solves the problem of overloaded window content. For example
                     * changing a color value requires 4 value modifier (slider, property,...)
                     * for RGBA then you need a label and ways to display the current color.
                     * If you want to go fancy you even add rgb and hsv ratio boxes.
                     * While fine for one color if you have a lot of them it because
                     * tedious to look at and quite wasteful in space. You could add
                     * a popup which modifies the color but this does not solve the
                     * fact that it still requires a lot of cluttered space to do.
                     *
                     * In these kind of instance abstract comboboxes are quite handy. All
                     * value modifiers are hidden inside the combobox popup and only
                     * the color is shown if not open. This combines the clarity of the
                     * popup with the ease of use of just using the space for modifiers.
                     *
                     * Other instances are for example time and especially date picker,
                     * which only show the currently activated time/data and hide the
                     * selection logic inside the combobox popup.
                     */
                    static float chart_selection = 8.0f;
                    static int current_weapon = 0;
                    static int check_values[5];
                    static float position[3];
                    static struct nk_color combo_color = {130, 50, 50, 255};
                    static struct nk_colorf combo_color2 = {0.509f, 0.705f, 0.2f, 1.0f};
                    static size_t prog_a =  20, prog_b = 40, prog_c = 10, prog_d = 90;
                    static const char *weapons[] = {"Fist","Pistol","Shotgun","Plasma","BFG"};

                    char buffer[64];
                    size_t sum = 0;

                    /* default combobox */
                    nk_layout_row_static(ctx, 25, 200, 1);
                    current_weapon = nk_combo(ctx, weapons, NK_LEN(weapons), current_weapon, 25, nk_vec2(200,200));

                    /* slider color combobox */
                    if (nk_combo_begin_color(ctx, combo_color, nk_vec2(200,200)))
                        {
                        float ratios[] = {0.15f, 0.85f};
                        nk_layout_row(ctx, NK_DYNAMIC, 30, 2, ratios);
                        nk_label(ctx, "R:", NK_TEXT_LEFT);
                        combo_color.r = (nk_byte)nk_slide_int(ctx, 0, combo_color.r, 255, 5);
                        nk_label(ctx, "G:", NK_TEXT_LEFT);
                        combo_color.g = (nk_byte)nk_slide_int(ctx, 0, combo_color.g, 255, 5);
                        nk_label(ctx, "B:", NK_TEXT_LEFT);
                        combo_color.b = (nk_byte)nk_slide_int(ctx, 0, combo_color.b, 255, 5);
                        nk_label(ctx, "A:", NK_TEXT_LEFT);
                        combo_color.a = (nk_byte)nk_slide_int(ctx, 0, combo_color.a, 255, 5);
                        nk_combo_end(ctx);
                        }
                    /* complex color combobox */
                    if (nk_combo_begin_color(ctx, nk_rgb_cf(combo_color2), nk_vec2(200,400)))
                        {
                        enum color_mode {COL_RGB, COL_HSV};
                        static int col_mode = COL_RGB;
#ifndef DEMO_DO_NOT_USE_COLOR_PICKER
                        nk_layout_row_dynamic(ctx, 120, 1);
                        combo_color2 = nk_color_picker(ctx, combo_color2, NK_RGBA);
#endif

                        nk_layout_row_dynamic(ctx, 25, 2);
                        col_mode = nk_option_label(ctx, "RGB", col_mode == COL_RGB) ? COL_RGB : col_mode;
                        col_mode = nk_option_label(ctx, "HSV", col_mode == COL_HSV) ? COL_HSV : col_mode;

                        nk_layout_row_dynamic(ctx, 25, 1);
                        if (col_mode == COL_RGB)
                            {
                            combo_color2.r = nk_propertyf(ctx, "#R:", 0, combo_color2.r, 1.0f, 0.01f,0.005f);
                            combo_color2.g = nk_propertyf(ctx, "#G:", 0, combo_color2.g, 1.0f, 0.01f,0.005f);
                            combo_color2.b = nk_propertyf(ctx, "#B:", 0, combo_color2.b, 1.0f, 0.01f,0.005f);
                            combo_color2.a = nk_propertyf(ctx, "#A:", 0, combo_color2.a, 1.0f, 0.01f,0.005f);
                            }
                        else
                            {
                            float hsva[4];
                            nk_colorf_hsva_fv(hsva, combo_color2);
                            hsva[0] = nk_propertyf(ctx, "#H:", 0, hsva[0], 1.0f, 0.01f,0.05f);
                            hsva[1] = nk_propertyf(ctx, "#S:", 0, hsva[1], 1.0f, 0.01f,0.05f);
                            hsva[2] = nk_propertyf(ctx, "#V:", 0, hsva[2], 1.0f, 0.01f,0.05f);
                            hsva[3] = nk_propertyf(ctx, "#A:", 0, hsva[3], 1.0f, 0.01f,0.05f);
                            combo_color2 = nk_hsva_colorfv(hsva);
                            }
                        nk_combo_end(ctx);
                        }
                    /* progressbar combobox */
                    sum = prog_a + prog_b + prog_c + prog_d;
                    sprintf(buffer, "%lu", (unsigned long)sum);  // (unsigned long) Altered by Axle
                    if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);
                        nk_progress(ctx, &prog_a, 100, NK_MODIFIABLE);
                        nk_progress(ctx, &prog_b, 100, NK_MODIFIABLE);
                        nk_progress(ctx, &prog_c, 100, NK_MODIFIABLE);
                        nk_progress(ctx, &prog_d, 100, NK_MODIFIABLE);
                        nk_combo_end(ctx);
                        }

                    /* checkbox combobox */
                    sum = (size_t)(check_values[0] + check_values[1] + check_values[2] + check_values[3] + check_values[4]);
                    sprintf(buffer, "%lu", (unsigned long)sum);  // (unsigned long) Altered by Axle
                    if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                        {
                        nk_layout_row_dynamic(ctx, 30, 1);
                        nk_checkbox_label(ctx, weapons[0], &check_values[0]);
                        nk_checkbox_label(ctx, weapons[1], &check_values[1]);
                        nk_checkbox_label(ctx, weapons[2], &check_values[2]);
                        nk_checkbox_label(ctx, weapons[3], &check_values[3]);
                        nk_combo_end(ctx);
                        }

                    /* complex text combobox */
                    sprintf(buffer, "%.2f, %.2f, %.2f", position[0], position[1],position[2]);
                    if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,200)))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_property_float(ctx, "#X:", -1024.0f, &position[0], 1024.0f, 1,0.5f);
                        nk_property_float(ctx, "#Y:", -1024.0f, &position[1], 1024.0f, 1,0.5f);
                        nk_property_float(ctx, "#Z:", -1024.0f, &position[2], 1024.0f, 1,0.5f);
                        nk_combo_end(ctx);
                        }

                    /* chart combobox */
                    sprintf(buffer, "%.1f", chart_selection);
                    if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,250)))
                        {
                        size_t i = 0;
                        static const float values[]= {26.0f,13.0f,30.0f,15.0f,25.0f,10.0f,20.0f,40.0f, 12.0f, 8.0f, 22.0f, 28.0f, 5.0f};
                        nk_layout_row_dynamic(ctx, 150, 1);
                        nk_chart_begin(ctx, NK_CHART_COLUMN, NK_LEN(values), 0, 50);
                        for (i = 0; i < NK_LEN(values); ++i)
                            {
                            nk_flags res = nk_chart_push(ctx, values[i]);
                            if (res & NK_CHART_CLICKED)
                                {
                                chart_selection = values[i];
                                nk_combo_close(ctx);
                                }
                            }
                        nk_chart_end(ctx);
                        nk_combo_end(ctx);
                        }

                        {
                        static int time_selected = 0;
                        static int date_selected = 0;
                        static struct tm sel_time;
                        static struct tm sel_date;
                        if (!time_selected || !date_selected)
                            {
                            /* keep time and date updated if nothing is selected */
                            time_t cur_time = time(0);
                            struct tm *n = localtime(&cur_time);
                            if (!time_selected)
                                memcpy(&sel_time, n, sizeof(struct tm));
                            if (!date_selected)
                                memcpy(&sel_date, n, sizeof(struct tm));
                            }

                        /* time combobox */
                        sprintf(buffer, "%02d:%02d:%02d", sel_time.tm_hour, sel_time.tm_min, sel_time.tm_sec);
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(200,250)))
                            {
                            time_selected = 1;
                            nk_layout_row_dynamic(ctx, 25, 1);
                            sel_time.tm_sec = nk_propertyi(ctx, "#S:", 0, sel_time.tm_sec, 60, 1, 1);
                            sel_time.tm_min = nk_propertyi(ctx, "#M:", 0, sel_time.tm_min, 60, 1, 1);
                            sel_time.tm_hour = nk_propertyi(ctx, "#H:", 0, sel_time.tm_hour, 23, 1, 1);
                            nk_combo_end(ctx);
                            }

                        /* date combobox */
                        sprintf(buffer, "%02d-%02d-%02d", sel_date.tm_mday, sel_date.tm_mon+1, sel_date.tm_year+1900);
                        if (nk_combo_begin_label(ctx, buffer, nk_vec2(350,400)))
                            {
                            int i = 0;
                            const char *month[] = {"January", "February", "March",
                                                   "April", "May", "June", "July", "August", "September",
                                                   "October", "November", "December"
                                                  };
                            const char *week_days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
                            const int month_days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
                            int year = sel_date.tm_year+1900;
                            int leap_year = (!(year % 4) && ((year % 100))) || !(year % 400);
                            int days = (sel_date.tm_mon == 1) ?
                                       month_days[sel_date.tm_mon] + leap_year:
                                       month_days[sel_date.tm_mon];

                            /* header with month and year */
                            date_selected = 1;
                            nk_layout_row_begin(ctx, NK_DYNAMIC, 20, 3);
                            nk_layout_row_push(ctx, 0.05f);
                            if (nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT))
                                {
                                if (sel_date.tm_mon == 0)
                                    {
                                    sel_date.tm_mon = 11;
                                    sel_date.tm_year = NK_MAX(0, sel_date.tm_year-1);
                                    }
                                else sel_date.tm_mon--;
                                }
                            nk_layout_row_push(ctx, 0.9f);
                            sprintf(buffer, "%s %d", month[sel_date.tm_mon], year);
                            nk_label(ctx, buffer, NK_TEXT_CENTERED);
                            nk_layout_row_push(ctx, 0.05f);
                            if (nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT))
                                {
                                if (sel_date.tm_mon == 11)
                                    {
                                    sel_date.tm_mon = 0;
                                    sel_date.tm_year++;
                                    }
                                else sel_date.tm_mon++;
                                }
                            nk_layout_row_end(ctx);

                            /* good old week day formula (double because precision) */
                                {
                                int year_n = (sel_date.tm_mon < 2) ? year-1: year;
                                int y = year_n % 100;
                                int c = year_n / 100;
                                int y4 = (int)((float)y / 4);
                                int c4 = (int)((float)c / 4);
                                int m = (int)(2.6 * (double)(((sel_date.tm_mon + 10) % 12) + 1) - 0.2);
                                int week_day = (((1 + m + y + y4 + c4 - 2 * c) % 7) + 7) % 7;

                                /* weekdays  */
                                nk_layout_row_dynamic(ctx, 35, 7);
                                for (i = 0; i < (int)NK_LEN(week_days); ++i)
                                    nk_label(ctx, week_days[i], NK_TEXT_CENTERED);

                                /* days  */
                                if (week_day > 0) nk_spacing(ctx, week_day);
                                for (i = 1; i <= days; ++i)
                                    {
                                    sprintf(buffer, "%d", i);
                                    if (nk_button_label(ctx, buffer))
                                        {
                                        sel_date.tm_mday = i;
                                        nk_combo_close(ctx);
                                        }
                                    }
                                }
                            nk_combo_end(ctx);
                            }
                        }

                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Input", NK_MINIMIZED))
                    {
                    static const float ratio[] = {120, 150};
                    static char field_buffer[64];
                    static char text[9][64];
                    static int text_len[9];
                    static char box_buffer[512];
                    static int field_len;
                    static int box_len;
                    nk_flags active;

                    nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                    nk_label(ctx, "Default:", NK_TEXT_LEFT);

                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[0], &text_len[0], 64, nk_filter_default);
                    nk_label(ctx, "Int:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[1], &text_len[1], 64, nk_filter_decimal);
                    nk_label(ctx, "Float:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[2], &text_len[2], 64, nk_filter_float);
                    nk_label(ctx, "Hex:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[4], &text_len[4], 64, nk_filter_hex);
                    nk_label(ctx, "Octal:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[5], &text_len[5], 64, nk_filter_oct);
                    nk_label(ctx, "Binary:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_SIMPLE, text[6], &text_len[6], 64, nk_filter_binary);

                    nk_label(ctx, "Password:", NK_TEXT_LEFT);
                        {
                        int i = 0;
                        int old_len = text_len[8];
                        char buffer[64];
                        for (i = 0; i < text_len[8]; ++i) buffer[i] = '*';
                        nk_edit_string(ctx, NK_EDIT_FIELD, buffer, &text_len[8], 64, nk_filter_default);
                        if (old_len < text_len[8])
                            memcpy(&text[8][old_len], &buffer[old_len], (nk_size)(text_len[8] - old_len));
                        }

                    nk_label(ctx, "Field:", NK_TEXT_LEFT);
                    nk_edit_string(ctx, NK_EDIT_FIELD, field_buffer, &field_len, 64, nk_filter_default);

                    nk_label(ctx, "Box:", NK_TEXT_LEFT);
                    nk_layout_row_static(ctx, 180, 278, 1);
                    nk_edit_string(ctx, NK_EDIT_BOX, box_buffer, &box_len, 512, nk_filter_default);

                    nk_layout_row(ctx, NK_STATIC, 25, 2, ratio);
                    active = nk_edit_string(ctx, NK_EDIT_FIELD|NK_EDIT_SIG_ENTER, text[7], &text_len[7], 64,  nk_filter_ascii);
                    if (nk_button_label(ctx, "Submit") ||
                            (active & NK_EDIT_COMMITED))
                        {
                        text[7][text_len[7]] = '\n';
                        text_len[7]++;
                        memcpy(&box_buffer[box_len], &text[7], (nk_size)text_len[7]);
                        box_len += text_len[7];
                        text_len[7] = 0;
                        }
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Horizontal Rule", NK_MINIMIZED))
                    {
                    nk_layout_row_dynamic(ctx, 12, 1);
                    nk_label(ctx, "Use this to subdivide spaces visually", NK_TEXT_LEFT);
                    nk_layout_row_dynamic(ctx, 4, 1);
                    nk_rule_horizontal(ctx, nk_white, nk_true);
                    nk_layout_row_dynamic(ctx, 75, 1);
                    nk_label_wrap(ctx, "Best used in 'Card'-like layouts, with a bigger title font on top. Takes on the size of the previous layout definition. Rounding optional.");
                    nk_tree_pop(ctx);
                    }

                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Chart", NK_MINIMIZED))
                {
                /* Chart Widgets
                 * This library has two different rather simple charts. The line and the
                 * column chart. Both provide a simple way of visualizing values and
                 * have a retained mode and immediate mode API version. For the retain
                 * mode version `nk_plot` and `nk_plot_function` you either provide
                 * an array or a callback to call to handle drawing the graph.
                 * For the immediate mode version you start by calling `nk_chart_begin`
                 * and need to provide min and max values for scaling on the Y-axis.
                 * and then call `nk_chart_push` to push values into the chart.
                 * Finally `nk_chart_end` needs to be called to end the process. */
                float id = 0;
                static int col_index = -1;
                static int line_index = -1;
                float step = (2*3.141592654f) / 32;

                int i;
                int index = -1;

                /* line chart */
                id = 0;
                index = -1;
                nk_layout_row_dynamic(ctx, 100, 1);
                if (nk_chart_begin(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f))
                    {
                    for (i = 0; i < 32; ++i)
                        {
                        nk_flags res = nk_chart_push(ctx, (float)cos(id));
                        if (res & NK_CHART_HOVERING)
                            index = (int)i;
                        if (res & NK_CHART_CLICKED)
                            line_index = (int)i;
                        id += step;
                        }
                    nk_chart_end(ctx);
                    }

                if (index != -1)
                    nk_tooltipf(ctx, "Value: %.2f", (float)cos((float)index*step));
                if (line_index != -1)
                    {
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)cos((float)index*step));
                    }

                /* column chart */
                nk_layout_row_dynamic(ctx, 100, 1);
                if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f))
                    {
                    for (i = 0; i < 32; ++i)
                        {
                        nk_flags res = nk_chart_push(ctx, (float)fabs(sin(id)));
                        if (res & NK_CHART_HOVERING)
                            index = (int)i;
                        if (res & NK_CHART_CLICKED)
                            col_index = (int)i;
                        id += step;
                        }
                    nk_chart_end(ctx);
                    }
                if (index != -1)
                    nk_tooltipf(ctx, "Value: %.2f", (float)fabs(sin(step * (float)index)));
                if (col_index != -1)
                    {
                    nk_layout_row_dynamic(ctx, 20, 1);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Selected value: %.2f", (float)fabs(sin(step * (float)col_index)));
                    }

                /* mixed chart */
                nk_layout_row_dynamic(ctx, 100, 1);
                if (nk_chart_begin(ctx, NK_CHART_COLUMN, 32, 0.0f, 1.0f))
                    {
                    nk_chart_add_slot(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f);
                    nk_chart_add_slot(ctx, NK_CHART_LINES, 32, -1.0f, 1.0f);
                    for (id = 0, i = 0; i < 32; ++i)
                        {
                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                        nk_chart_push_slot(ctx, (float)cos(id), 1);
                        nk_chart_push_slot(ctx, (float)sin(id), 2);
                        id += step;
                        }
                    }
                nk_chart_end(ctx);

                /* mixed colored chart */
                nk_layout_row_dynamic(ctx, 100, 1);
                if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                    {
                    nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                    nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,255,0), nk_rgb(0,150,0), 32, -1.0f, 1.0f);
                    for (id = 0, i = 0; i < 32; ++i)
                        {
                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                        nk_chart_push_slot(ctx, (float)cos(id), 1);
                        nk_chart_push_slot(ctx, (float)sin(id), 2);
                        id += step;
                        }
                    }
                nk_chart_end(ctx);
                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Popup", NK_MINIMIZED))
                {
                static struct nk_color color = {255,0,0, 255};
                static int select[4];
                static int popup_active;
                const struct nk_input *in = &ctx->input;
                struct nk_rect bounds;

                /* menu contextual */
                nk_layout_row_static(ctx, 30, 160, 1);
                bounds = nk_widget_bounds(ctx);
                nk_label(ctx, "Right click me for menu", NK_TEXT_LEFT);

                if (nk_contextual_begin(ctx, 0, nk_vec2(100, 300), bounds))
                    {
                    static size_t prog = 40;
                    static int slider = 10;

                    nk_layout_row_dynamic(ctx, 25, 1);
                    nk_checkbox_label(ctx, "Menu", &show_menu);
                    nk_progress(ctx, &prog, 100, NK_MODIFIABLE);
                    nk_slider_int(ctx, 0, &slider, 16, 1);
                    if (nk_contextual_item_label(ctx, "About", NK_TEXT_CENTERED))
                        show_app_about = nk_true;
                    nk_selectable_label(ctx, select[0]?"Unselect":"Select", NK_TEXT_LEFT, &select[0]);
                    nk_selectable_label(ctx, select[1]?"Unselect":"Select", NK_TEXT_LEFT, &select[1]);
                    nk_selectable_label(ctx, select[2]?"Unselect":"Select", NK_TEXT_LEFT, &select[2]);
                    nk_selectable_label(ctx, select[3]?"Unselect":"Select", NK_TEXT_LEFT, &select[3]);
                    nk_contextual_end(ctx);
                    }

                /* color contextual */
                nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
                nk_layout_row_push(ctx, 120);
                nk_label(ctx, "Right Click here:", NK_TEXT_LEFT);
                nk_layout_row_push(ctx, 50);
                bounds = nk_widget_bounds(ctx);
                nk_button_color(ctx, color);
                nk_layout_row_end(ctx);

                if (nk_contextual_begin(ctx, 0, nk_vec2(350, 60), bounds))
                    {
                    nk_layout_row_dynamic(ctx, 30, 4);
                    color.r = (nk_byte)nk_propertyi(ctx, "#r", 0, color.r, 255, 1, 1);
                    color.g = (nk_byte)nk_propertyi(ctx, "#g", 0, color.g, 255, 1, 1);
                    color.b = (nk_byte)nk_propertyi(ctx, "#b", 0, color.b, 255, 1, 1);
                    color.a = (nk_byte)nk_propertyi(ctx, "#a", 0, color.a, 255, 1, 1);
                    nk_contextual_end(ctx);
                    }

                /* popup */
                nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
                nk_layout_row_push(ctx, 120);
                nk_label(ctx, "Popup:", NK_TEXT_LEFT);
                nk_layout_row_push(ctx, 50);
                if (nk_button_label(ctx, "Popup"))
                    popup_active = 1;
                nk_layout_row_end(ctx);

                if (popup_active)
                    {
                    static struct nk_rect s = {20, 100, 220, 90};
                    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Error", 0, s))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_label(ctx, "A terrible error as occurred", NK_TEXT_LEFT);
                        nk_layout_row_dynamic(ctx, 25, 2);
                        if (nk_button_label(ctx, "OK"))
                            {
                            popup_active = 0;
                            nk_popup_close(ctx);
                            }
                        if (nk_button_label(ctx, "Cancel"))
                            {
                            popup_active = 0;
                            nk_popup_close(ctx);
                            }
                        nk_popup_end(ctx);
                        }
                    else popup_active = nk_false;
                    }

                /* tooltip */
                nk_layout_row_static(ctx, 30, 150, 1);
                bounds = nk_widget_bounds(ctx);
                nk_label(ctx, "Hover me for tooltip", NK_TEXT_LEFT);
                if (nk_input_is_mouse_hovering_rect(in, bounds))
                    nk_tooltip(ctx, "This is a tooltip");

                nk_tree_pop(ctx);
                }

            if (nk_tree_push(ctx, NK_TREE_TAB, "Layout", NK_MINIMIZED))
                {
                if (nk_tree_push(ctx, NK_TREE_NODE, "Widget", NK_MINIMIZED))
                    {
                    float ratio_two[] = {0.2f, 0.6f, 0.2f};
                    float width_two[] = {100, 200, 50};

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Dynamic fixed column layout with generated position and size:", NK_TEXT_LEFT);
                    nk_layout_row_dynamic(ctx, 30, 3);
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "static fixed column layout with generated position and size:", NK_TEXT_LEFT);
                    nk_layout_row_static(ctx, 30, 100, 3);
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Dynamic array-based custom column layout with generated position and custom size:",NK_TEXT_LEFT);
                    nk_layout_row(ctx, NK_DYNAMIC, 30, 3, ratio_two);
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Static array-based custom column layout with generated position and custom size:",NK_TEXT_LEFT );
                    nk_layout_row(ctx, NK_STATIC, 30, 3, width_two);
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Dynamic immediate mode custom column layout with generated position and custom size:",NK_TEXT_LEFT);
                    nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 3);
                    nk_layout_row_push(ctx, 0.2f);
                    nk_button_label(ctx, "button");
                    nk_layout_row_push(ctx, 0.6f);
                    nk_button_label(ctx, "button");
                    nk_layout_row_push(ctx, 0.2f);
                    nk_button_label(ctx, "button");
                    nk_layout_row_end(ctx);

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Static immediate mode custom column layout with generated position and custom size:", NK_TEXT_LEFT);
                    nk_layout_row_begin(ctx, NK_STATIC, 30, 3);
                    nk_layout_row_push(ctx, 100);
                    nk_button_label(ctx, "button");
                    nk_layout_row_push(ctx, 200);
                    nk_button_label(ctx, "button");
                    nk_layout_row_push(ctx, 50);
                    nk_button_label(ctx, "button");
                    nk_layout_row_end(ctx);

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Static free space with custom position and custom size:", NK_TEXT_LEFT);
                    nk_layout_space_begin(ctx, NK_STATIC, 60, 4);
                    nk_layout_space_push(ctx, nk_rect(100, 0, 100, 30));
                    nk_button_label(ctx, "button");
                    nk_layout_space_push(ctx, nk_rect(0, 15, 100, 30));
                    nk_button_label(ctx, "button");
                    nk_layout_space_push(ctx, nk_rect(200, 15, 100, 30));
                    nk_button_label(ctx, "button");
                    nk_layout_space_push(ctx, nk_rect(100, 30, 100, 30));
                    nk_button_label(ctx, "button");
                    nk_layout_space_end(ctx);

                    nk_layout_row_dynamic(ctx, 30, 1);
                    nk_label(ctx, "Row template:", NK_TEXT_LEFT);
                    nk_layout_row_template_begin(ctx, 30);
                    nk_layout_row_template_push_dynamic(ctx);
                    nk_layout_row_template_push_variable(ctx, 80);
                    nk_layout_row_template_push_static(ctx, 80);
                    nk_layout_row_template_end(ctx);
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");
                    nk_button_label(ctx, "button");

                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Group", NK_MINIMIZED))
                    {
                    static int group_titlebar = nk_false;
                    static int group_border = nk_true;
                    static int group_no_scrollbar = nk_false;
                    static int group_width = 320;
                    static int group_height = 200;

                    nk_flags group_flags = 0;
                    if (group_border) group_flags |= NK_WINDOW_BORDER;
                    if (group_no_scrollbar) group_flags |= NK_WINDOW_NO_SCROLLBAR;
                    if (group_titlebar) group_flags |= NK_WINDOW_TITLE;

                    nk_layout_row_dynamic(ctx, 30, 3);
                    nk_checkbox_label(ctx, "Titlebar", &group_titlebar);
                    nk_checkbox_label(ctx, "Border", &group_border);
                    nk_checkbox_label(ctx, "No Scrollbar", &group_no_scrollbar);

                    nk_layout_row_begin(ctx, NK_STATIC, 22, 3);
                    nk_layout_row_push(ctx, 50);
                    nk_label(ctx, "size:", NK_TEXT_LEFT);
                    nk_layout_row_push(ctx, 130);
                    nk_property_int(ctx, "#Width:", 100, &group_width, 500, 10, 1);
                    nk_layout_row_push(ctx, 130);
                    nk_property_int(ctx, "#Height:", 100, &group_height, 500, 10, 1);
                    nk_layout_row_end(ctx);

                    nk_layout_row_static(ctx, (float)group_height, group_width, 2);
                    if (nk_group_begin(ctx, "Group", group_flags))
                        {
                        int i = 0;
                        static int selected[16];
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 0; i < 16; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected": "Unselected", NK_TEXT_CENTERED, &selected[i]);
                        nk_group_end(ctx);
                        }
                    nk_tree_pop(ctx);
                    }
                if (nk_tree_push(ctx, NK_TREE_NODE, "Tree", NK_MINIMIZED))
                    {
                    static int root_selected = 0;
                    int sel = root_selected;
                    if (nk_tree_element_push(ctx, NK_TREE_NODE, "Root", NK_MINIMIZED, &sel))
                        {
                        static int selected[8];
                        int i = 0, node_select = selected[0];
                        if (sel != root_selected)
                            {
                            root_selected = sel;
                            for (i = 0; i < 8; ++i)
                                selected[i] = sel;
                            }
                        if (nk_tree_element_push(ctx, NK_TREE_NODE, "Node", NK_MINIMIZED, &node_select))
                            {
                            int j = 0;
                            static int sel_nodes[4];
                            if (node_select != selected[0])
                                {
                                selected[0] = node_select;
                                for (i = 0; i < 4; ++i)
                                    sel_nodes[i] = node_select;
                                }
                            nk_layout_row_static(ctx, 18, 100, 1);
                            for (j = 0; j < 4; ++j)
                                nk_selectable_symbol_label(ctx, NK_SYMBOL_CIRCLE_SOLID, (sel_nodes[j]) ? "Selected": "Unselected", NK_TEXT_RIGHT, &sel_nodes[j]);
                            nk_tree_element_pop(ctx);
                            }
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 1; i < 8; ++i)
                            nk_selectable_symbol_label(ctx, NK_SYMBOL_CIRCLE_SOLID, (selected[i]) ? "Selected": "Unselected", NK_TEXT_RIGHT, &selected[i]);
                        nk_tree_element_pop(ctx);
                        }
                    nk_tree_pop(ctx);
                    }
                if (nk_tree_push(ctx, NK_TREE_NODE, "Notebook", NK_MINIMIZED))
                    {
                    static int current_tab = 0;
                    float step = (2*3.141592654f) / 32;
                    enum chart_type {CHART_LINE, CHART_HISTO, CHART_MIXED};
                    const char *names[] = {"Lines", "Columns", "Mixed"};
                    float id = 0;
                    int i;

                    /* Header */
                    nk_style_push_vec2(ctx, &ctx->style.window.spacing, nk_vec2(0,0));
                    nk_style_push_float(ctx, &ctx->style.button.rounding, 0);
                    nk_layout_row_begin(ctx, NK_STATIC, 20, 3);
                    for (i = 0; i < 3; ++i)
                        {
                        /* make sure button perfectly fits text */
                        const struct nk_user_font *f = ctx->style.font;
                        float text_width = f->width(f->userdata, f->height, names[i], nk_strlen(names[i]));
                        float widget_width = text_width + 3 * ctx->style.button.padding.x;
                        nk_layout_row_push(ctx, widget_width);
                        if (current_tab == i)
                            {
                            /* active tab gets highlighted */
                            struct nk_style_item button_color = ctx->style.button.normal;
                            ctx->style.button.normal = ctx->style.button.active;
                            current_tab = nk_button_label(ctx, names[i]) ? i: current_tab;
                            ctx->style.button.normal = button_color;
                            }
                        else current_tab = nk_button_label(ctx, names[i]) ? i: current_tab;
                        }
                    nk_style_pop_float(ctx);

                    /* Body */
                    nk_layout_row_dynamic(ctx, 140, 1);
                    if (nk_group_begin(ctx, "Notebook", NK_WINDOW_BORDER))
                        {
                        nk_style_pop_vec2(ctx);
                        switch (current_tab)
                            {
                            default:
                                break;
                            case CHART_LINE:
                                nk_layout_row_dynamic(ctx, 100, 1);
                                if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                                    {
                                    nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                                    for (i = 0, id = 0; i < 32; ++i)
                                        {
                                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                        nk_chart_push_slot(ctx, (float)cos(id), 1);
                                        id += step;
                                        }
                                    }
                                nk_chart_end(ctx);
                                break;
                            case CHART_HISTO:
                                nk_layout_row_dynamic(ctx, 100, 1);
                                if (nk_chart_begin_colored(ctx, NK_CHART_COLUMN, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                                    {
                                    for (i = 0, id = 0; i < 32; ++i)
                                        {
                                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                        id += step;
                                        }
                                    }
                                nk_chart_end(ctx);
                                break;
                            case CHART_MIXED:
                                nk_layout_row_dynamic(ctx, 100, 1);
                                if (nk_chart_begin_colored(ctx, NK_CHART_LINES, nk_rgb(255,0,0), nk_rgb(150,0,0), 32, 0.0f, 1.0f))
                                    {
                                    nk_chart_add_slot_colored(ctx, NK_CHART_LINES, nk_rgb(0,0,255), nk_rgb(0,0,150),32, -1.0f, 1.0f);
                                    nk_chart_add_slot_colored(ctx, NK_CHART_COLUMN, nk_rgb(0,255,0), nk_rgb(0,150,0), 32, 0.0f, 1.0f);
                                    for (i = 0, id = 0; i < 32; ++i)
                                        {
                                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 0);
                                        nk_chart_push_slot(ctx, (float)fabs(cos(id)), 1);
                                        nk_chart_push_slot(ctx, (float)fabs(sin(id)), 2);
                                        id += step;
                                        }
                                    }
                                nk_chart_end(ctx);
                                break;
                            }
                        nk_group_end(ctx);
                        }
                    else nk_style_pop_vec2(ctx);
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Simple", NK_MINIMIZED))
                    {
                    nk_layout_row_dynamic(ctx, 300, 2);
                    if (nk_group_begin(ctx, "Group_Without_Border", 0))
                        {
                        int i = 0;
                        char buffer[64];
                        nk_layout_row_static(ctx, 18, 150, 1);
                        for (i = 0; i < 64; ++i)
                            {
                            sprintf(buffer, "0x%02x", i);
                            nk_labelf(ctx, NK_TEXT_LEFT, "%s: scrollable region", buffer);
                            }
                        nk_group_end(ctx);
                        }
                    if (nk_group_begin(ctx, "Group_With_Border", NK_WINDOW_BORDER))
                        {
                        int i = 0;
                        char buffer[64];
                        nk_layout_row_dynamic(ctx, 25, 2);
                        for (i = 0; i < 64; ++i)
                            {
                            sprintf(buffer, "%08d", ((((i%7)*10)^32))+(64+(i%2)*2));
                            nk_button_label(ctx, buffer);
                            }
                        nk_group_end(ctx);
                        }
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Complex", NK_MINIMIZED))
                    {
                    int i;
                    nk_layout_space_begin(ctx, NK_STATIC, 500, 64);
                    nk_layout_space_push(ctx, nk_rect(0,0,150,500));
                    if (nk_group_begin(ctx, "Group_left", NK_WINDOW_BORDER))
                        {
                        static int selected[32];
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 0; i < 32; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected": "Unselected", NK_TEXT_CENTERED, &selected[i]);
                        nk_group_end(ctx);
                        }

                    nk_layout_space_push(ctx, nk_rect(160,0,150,240));
                    if (nk_group_begin(ctx, "Group_top", NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                        }

                    nk_layout_space_push(ctx, nk_rect(160,250,150,250));
                    if (nk_group_begin(ctx, "Group_buttom", NK_WINDOW_BORDER))
                        {
                        nk_layout_row_dynamic(ctx, 25, 1);
                        nk_button_label(ctx, "#FFAA");
                        nk_button_label(ctx, "#FFBB");
                        nk_button_label(ctx, "#FFCC");
                        nk_button_label(ctx, "#FFDD");
                        nk_button_label(ctx, "#FFEE");
                        nk_button_label(ctx, "#FFFF");
                        nk_group_end(ctx);
                        }

                    nk_layout_space_push(ctx, nk_rect(320,0,150,150));
                    if (nk_group_begin(ctx, "Group_right_top", NK_WINDOW_BORDER))
                        {
                        static int selected[4];
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 0; i < 4; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected": "Unselected", NK_TEXT_CENTERED, &selected[i]);
                        nk_group_end(ctx);
                        }

                    nk_layout_space_push(ctx, nk_rect(320,160,150,150));
                    if (nk_group_begin(ctx, "Group_right_center", NK_WINDOW_BORDER))
                        {
                        static int selected[4];
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 0; i < 4; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected": "Unselected", NK_TEXT_CENTERED, &selected[i]);
                        nk_group_end(ctx);
                        }

                    nk_layout_space_push(ctx, nk_rect(320,320,150,150));
                    if (nk_group_begin(ctx, "Group_right_bottom", NK_WINDOW_BORDER))
                        {
                        static int selected[4];
                        nk_layout_row_static(ctx, 18, 100, 1);
                        for (i = 0; i < 4; ++i)
                            nk_selectable_label(ctx, (selected[i]) ? "Selected": "Unselected", NK_TEXT_CENTERED, &selected[i]);
                        nk_group_end(ctx);
                        }
                    nk_layout_space_end(ctx);
                    nk_tree_pop(ctx);
                    }

                if (nk_tree_push(ctx, NK_TREE_NODE, "Splitter", NK_MINIMIZED))
                    {
                    const struct nk_input *in = &ctx->input;
                    nk_layout_row_static(ctx, 20, 320, 1);
                    nk_label(ctx, "Use slider and spinner to change tile size", NK_TEXT_LEFT);
                    nk_label(ctx, "Drag the space between tiles to change tile ratio", NK_TEXT_LEFT);

                    if (nk_tree_push(ctx, NK_TREE_NODE, "Vertical", NK_MINIMIZED))
                        {
                        static float a = 100, b = 100, c = 100;
                        struct nk_rect bounds;

                        float row_layout[5];
                        row_layout[0] = a;
                        row_layout[1] = 8;
                        row_layout[2] = b;
                        row_layout[3] = 8;
                        row_layout[4] = c;

                        /* header */
                        nk_layout_row_static(ctx, 30, 100, 2);
                        nk_label(ctx, "left:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);

                        nk_label(ctx, "middle:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);

                        nk_label(ctx, "right:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);

                        /* tiles */
                        nk_layout_row(ctx, NK_STATIC, 200, 5, row_layout);

                        /* left space */
                        if (nk_group_begin(ctx, "left", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR))
                            {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }

                        /* scaler */
                        bounds = nk_widget_bounds(ctx);
                        nk_spacing(ctx, 1);
                        if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                                nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                                nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                            {
                            a = row_layout[0] + in->mouse.delta.x;
                            b = row_layout[2] - in->mouse.delta.x;
                            }

                        /* middle space */
                        if (nk_group_begin(ctx, "center", NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR))
                            {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }

                        /* scaler */
                        bounds = nk_widget_bounds(ctx);
                        nk_spacing(ctx, 1);
                        if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                                nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                                nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                            {
                            b = (row_layout[2] + in->mouse.delta.x);
                            c = (row_layout[4] - in->mouse.delta.x);
                            }

                        /* right space */
                        if (nk_group_begin(ctx, "right", NK_WINDOW_BORDER|NK_WINDOW_NO_SCROLLBAR))
                            {
                            nk_layout_row_dynamic(ctx, 25, 1);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }

                        nk_tree_pop(ctx);
                        }

                    if (nk_tree_push(ctx, NK_TREE_NODE, "Horizontal", NK_MINIMIZED))
                        {
                        static float a = 100, b = 100, c = 100;
                        struct nk_rect bounds;

                        /* header */
                        nk_layout_row_static(ctx, 30, 100, 2);
                        nk_label(ctx, "top:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &a, 200.0f, 10.0f);

                        nk_label(ctx, "middle:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &b, 200.0f, 10.0f);

                        nk_label(ctx, "bottom:", NK_TEXT_LEFT);
                        nk_slider_float(ctx, 10.0f, &c, 200.0f, 10.0f);

                        /* top space */
                        nk_layout_row_dynamic(ctx, a, 1);
                        if (nk_group_begin(ctx, "top", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                            {
                            nk_layout_row_dynamic(ctx, 25, 3);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }

                        /* scaler */
                        nk_layout_row_dynamic(ctx, 8, 1);
                        bounds = nk_widget_bounds(ctx);
                        nk_spacing(ctx, 1);
                        if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                                nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                                nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                            {
                            a = a + in->mouse.delta.y;
                            b = b - in->mouse.delta.y;
                            }

                        /* middle space */
                        nk_layout_row_dynamic(ctx, b, 1);
                        if (nk_group_begin(ctx, "middle", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                            {
                            nk_layout_row_dynamic(ctx, 25, 3);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }

                            {
                            /* scaler */
                            nk_layout_row_dynamic(ctx, 8, 1);
                            bounds = nk_widget_bounds(ctx);
                            if ((nk_input_is_mouse_hovering_rect(in, bounds) ||
                                    nk_input_is_mouse_prev_hovering_rect(in, bounds)) &&
                                    nk_input_is_mouse_down(in, NK_BUTTON_LEFT))
                                {
                                b = b + in->mouse.delta.y;
                                c = c - in->mouse.delta.y;
                                }
                            }

                        /* bottom space */
                        nk_layout_row_dynamic(ctx, c, 1);
                        if (nk_group_begin(ctx, "bottom", NK_WINDOW_NO_SCROLLBAR|NK_WINDOW_BORDER))
                            {
                            nk_layout_row_dynamic(ctx, 25, 3);
                            nk_button_label(ctx, "#FFAA");
                            nk_button_label(ctx, "#FFBB");
                            nk_button_label(ctx, "#FFCC");
                            nk_button_label(ctx, "#FFDD");
                            nk_button_label(ctx, "#FFEE");
                            nk_button_label(ctx, "#FFFF");
                            nk_group_end(ctx);
                            }
                        nk_tree_pop(ctx);
                        }
                    nk_tree_pop(ctx);
                    }
                nk_tree_pop(ctx);
                }
            if (disable_widgets)
                {
                nk_widget_disable_end(ctx);
                }

            nk_tree_pop(ctx);
            }  // ###-> START overview.c EXAMPLES ###

        }  // END main Nuklear window
    nk_end(ctx);  // END main Nuklear window

    return !nk_window_is_closed(ctx, "NK_Theme");
    }

