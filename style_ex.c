/*----------------------------------------------------------------------------*/
/* Name:        Get Current Nuklear style table (style_ex.c)                  */
/* URL:                                                                       */
/* Purpose:     Visually create formatted Nuklear style tables                 */
/*              Load and save formatted nk table file.                         */
/*              [start_nk_style_v1] - [end_nk_style_v1]                       */
/*                                                                            */
/* Platform:    Win64, Ubuntu64                                               */
/*                                                                            */
/* Author:      Axle                                                          */
/* Created:     27/02/2024                                                    */
/* Updated:                                                                   */
/* Copyright:   (c) Axle 2024                                                 */
/* Licence:     MIT0 No Attribution                                           */
/*----------------------------------------------------------------------------*/
/* NOTES:                                                                     */
/* Compliments nuklear_style.c                                                */
/*----------------------------------------------------------------------------*/

/* Collects the current style colors from struct nk_context *ctx */
/* this correlates with the nk_color table[n] found in \demo\common\style.c */
/* Must be called after nk_allegro5_init() and set_style() */
/* Use: */
/* struct nk_color table[NK_COLOR_COUNT]; */
/* get_current_style_table(ctx, table); */
/* fprintf_table_dbg(table); */
void get_current_style_table(struct nk_context *ctx, struct nk_color table[3][NK_COLOR_COUNT])
    {
    /* Define the stile table. No current error handling. */
    /* table[0] == Current edit, table[1] == Reset|roll back, table[2] == temp undo */
    /* table[1] is never altered. */
    /* Creat an array with NK_COLOR_xxx */
    for (int n = 0; n < 3; n++)
        {
        table[n][NK_COLOR_TEXT] = ctx->style.text.color;  /* nk_rgba() */
        table[n][NK_COLOR_WINDOW] = ctx->style.window.background;  /* nk_rgba() */
        table[n][NK_COLOR_HEADER] = ctx->style.window.header.normal.data.color;  // nk_style_item{} */
        table[n][NK_COLOR_BORDER] = ctx->style.window.border_color;  /* nk_rgba() */
        table[n][NK_COLOR_BUTTON] = ctx->style.button.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_BUTTON_HOVER] = ctx->style.button.hover.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_BUTTON_ACTIVE] = ctx->style.button.active.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_TOGGLE] = ctx->style.option.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_TOGGLE_HOVER] = ctx->style.option.hover.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_TOGGLE_CURSOR] = ctx->style.option.cursor_normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SELECT] = ctx->style.selectable.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SELECT_ACTIVE] = ctx->style.selectable.normal_active.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SLIDER] = ctx->style.slider.bar_normal;  /* nk_rgba() */
        table[n][NK_COLOR_SLIDER_CURSOR] = ctx->style.slider.cursor_normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SLIDER_CURSOR_HOVER] = ctx->style.slider.cursor_hover.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SLIDER_CURSOR_ACTIVE] = ctx->style.slider.cursor_active.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_PROPERTY] = ctx->style.property.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_EDIT] = ctx->style.edit.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_EDIT_CURSOR] = nk_rgba(175,175,175,255);  /* Not used!? */
        table[n][NK_COLOR_COMBO] = ctx->style.combo.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_CHART] = ctx->style.chart.background.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_CHART_COLOR] = ctx->style.chart.color;  /* nk_rgba() */
        table[n][NK_COLOR_CHART_COLOR_HIGHLIGHT] = ctx->style.chart.selected_color;  /* nk_rgba() */
        table[n][NK_COLOR_SCROLLBAR] = ctx->style.scrollh.normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SCROLLBAR_CURSOR] = ctx->style.scrollh.cursor_normal.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SCROLLBAR_CURSOR_HOVER] = ctx->style.scrollh.cursor_hover.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = ctx->style.scrollh.cursor_active.data.color;  /* nk_style_item{} */
        table[n][NK_COLOR_TAB_HEADER] = ctx->style.tab.background.data.color;  /* nk_style_item{} */
        }
    }

/* Set new theme from the current table. */
/* Use nk_style_default(ctx); to recover the default theme */
void set_new_style(struct nk_context *ctx, struct nk_color *table)
    {
    nk_style_from_table(ctx, table);
    /* Do error handling */
    }


/* DEBUG print for struct nk_color table[NK_COLOR_COUNT] above. */
/* Console application must be enabled to view console outputs. */
void fprintf_table_dbg(struct nk_color *table)
    {
    for (int i = 0; i < NK_COLOR_COUNT; i++)
        {
        fprintf(stdout, "%s = RGBA(%d, %d, %d, %d)\n", nk_color_names[i], table[i].r, table[i].g, table[i].b, table[i].a);
        }
    }