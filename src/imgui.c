#include <imgui.h>

void imgui_init(ImGui *imgui, GfxCoreContext *context, HidState *hid_state, HidState *hid_state_prev, SpriteFont *font, uint32_t shader_primitive, uint32_t shader_sprite_font, uint32_t shader_sprite)
{
    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    RendererSprite *renderer_sprite = &imgui->renderer_sprites;

    renderer_sprite_font_init(renderer_sprite_font, context, 1000);
    renderer_primitive_init(renderer_primitive_fills, 1000, context);
    renderer_primitive_init(renderer_primitive_outlines, 1000, context);
    renderer_sprite_init(renderer_sprite, 16, context);

    renderer_primitive_apply_shader(renderer_primitive_fills, shader_primitive);
    renderer_primitive_apply_shader(renderer_primitive_outlines, shader_primitive);
    renderer_sprite_font_apply_shader(renderer_sprite_font, shader_sprite_font);
    renderer_sprite_apply_shader(renderer_sprite,shader_sprite);

    imgui->window = context->window;
    imgui->hid_state = hid_state;
    imgui->hid_state_prev = hid_state_prev;
    imgui->font = font;
    imgui->collapsed = 1;
    imgui->hold_scroll_bar = 0;
    imgui->scroll_bar_delta_y = 0;

    imgui->dx = 0;
    imgui->active_id = 0;
    imgui->scroll_bar_y = 0;


    imgui->blink_timer = 0;
    imgui->caret_blink = 0;
    imgui->mp_select = 0;
    imgui->caret = 0;
    imgui->offset = 0;
    imgui->selection_pivot = 0;
    imgui->hold_timer = 0;
    imgui->selection = 0;
    imgui->hold_timer_backspace = 0;
    imgui->delete_interval = 0;

    memset(imgui->text_password,'*',sizeof(char)*32);
}
void imgui_begin(ImGui *imgui,char* char_buffer, uint8_t char_buffer_length)
{
    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    RendererSprite *renderer_sprites = &imgui->renderer_sprites;

    renderer_primitive_begin(renderer_primitive_fills, renderer_primitive_fills->projection, DELO_TRIANGLE_LIST);
    renderer_primitive_begin(renderer_primitive_outlines, renderer_primitive_outlines->projection, DELO_LINE_LIST);
    renderer_sprite_font_begin(renderer_sprite_font, renderer_sprite_font->projection);
    renderer_sprite_begin(renderer_sprites, renderer_sprites->projection);

    imgui->char_buffer = char_buffer;
    imgui->char_buffer_length = char_buffer_length;
}
void imgui_end(ImGui *imgui)
{
    renderer_primitive_end(&imgui->renderer_primitive_fills);
    renderer_primitive_end(&imgui->renderer_primitive_outlines);
    renderer_sprite_font_end(&imgui->renderer_sprite_font);
    renderer_sprite_end(&imgui->renderer_sprites);
}
ButtonEvent imgui_button(Rectangle_f rect, char *caption, ImGui *imgui)
{
    ButtonEvent event = {0, 0};
    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    uint8_t button_state = BUTTON_STATE_NORMAL;

    if (rectangle_within_bounds(&rect, mouse_position_x, mouse_position_y))
    {
        button_state = (mouse_button_left == GLFW_PRESS) ? BUTTON_STATE_ACTIVE : BUTTON_STATE_HOVER;
        if (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_RELEASE)
        {
            event.clicked = 1;
        }
        event.hover = 1;
    }

    Color color_fill, color_outline, color_text;

    switch (button_state)
    {
    case BUTTON_STATE_NORMAL:
        color_set_i(&color_fill, 50, 64, 91, 255);
        color_set_i(&color_outline, 77 * 0.5, 99 * 0.5, 140 * 0.5, 255);
        break;
    case BUTTON_STATE_HOVER:
        color_set_i(&color_fill, 50 * 1.1, 64 * 1.1, 91 * 1.1, 255);
        color_set_i(&color_outline, 77 * 1.0, 99 * 1.0, 140 * 1.0, 255);
        break;
    case BUTTON_STATE_ACTIVE:
        color_set_i(&color_fill, 50 * 1.3, 64 * 1.3, 91 * 1.3, 255);
        color_set_i(&color_outline, 77 * 1.5, 99 * 1.5, 140 * 1.5, 255);
        break;
    }

    renderer_primitive_add_rectangle(renderer_primitive_fills, rect, color_fill);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect, color_outline);

    FontMeasurement measurement;
    sprite_font_measure_string(caption, font, &measurement, -1, -1);

    Vector2f position;
    position.x = rect.x + (rect.width / 2) - measurement.width_in_px / 2;
    position.y = rect.y + (rect.height / 2) - (font->font_size + font->padding) / 2;

    color_set_i(&color_text, 255, 255, 255, 255);

    renderer_sprite_font_add_text(renderer_sprite_font, font, caption, 0, position, color_text, (Vector2f){0, 0});

    return event;
}

SliderEvent imgui_slider(ImGui *imgui, int32_t id, Rectangle_f rect_bounds, Rectangle_f rect_knob, int32_t min, int32_t max, int32_t *value)
{
    SliderEvent event = {0, 0, 0.0f};
    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    rect_knob.x = rect_bounds.x + (rect_bounds.width - rect_knob.width) * ((float)*value / ((float)max - (float)min));
    rect_knob.y = rect_bounds.y;

    if (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_RELEASE)
    {
        if (rectangle_within_bounds(&rect_knob, mouse_position_x, mouse_position_y))
        {
            imgui->dx = rect_knob.x - mouse_position_x;
            imgui->active_id = id;
        }
        else if (rectangle_within_bounds(&rect_bounds, mouse_position_x, mouse_position_y))
        {
            imgui->dx = -rect_knob.width / 2;
            imgui->active_id = id;
        }
    }
    else if (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_PRESS)
    {
        if (imgui->active_id == id)
        {
            rect_knob.x = mouse_position_x + imgui->dx;
            if (rect_knob.x < rect_bounds.x)
            {
                rect_knob.x = rect_bounds.x;
            }
            else if (rect_knob.x > rect_bounds.x + rect_bounds.width - rect_knob.width)
            {
                rect_knob.x = rect_bounds.x + rect_bounds.width - rect_knob.width;
            }

            float x = (float)rect_knob.x - (float)rect_bounds.x;

            float percentage = x / (rect_bounds.width - rect_knob.width);

            int16_t range = max - min;

            int16_t temp = ((float)range * percentage) + (float)min;
            if (temp != *value)
            {
                *value = temp;
                event.changed = 1;
            }
        }
    }
    else if (mouse_button_left == GLFW_RELEASE && mouse_button_left_prev == GLFW_PRESS)
    {
        if (imgui->active_id == id)
        {
            imgui->active_id = 0;
            event.released = 1;
        }
    }

    Color color_fills;
    color_set_i(&color_fills, 50, 64, 91, 255);
    renderer_primitive_add_rectangle(renderer_primitive_fills, rect_bounds, color_fills);

    Color color_outlines;
    color_set_i(&color_outlines, 101, 129, 184, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_bounds, color_outlines);
    color_set_i(&color_outlines, 255, 255, 255, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_knob, color_outlines);

    Color color_text;
    color_set_i(&color_text, 255, 255, 255, 255);
    char value_text[16];
    snprintf(&value_text[0], sizeof(char) * CAPTION_SIZE, "%i", *value);

    Vector2f position;
    position.x = rect_bounds.x + rect_bounds.width + 6;
    position.y = rect_bounds.y + rect_bounds.height * 0.5 - font->font_size * 0.5;

    renderer_sprite_font_add_text(renderer_sprite_font, font, value_text, 0, position, color_text, (Vector2f){0, 0});

    return event;
}

uint8_t imgui_tswitch(ImGui *imgui, int32_t id, Rectangle_f rect_bounds, uint8_t *value, char *text_on, char *text_off)
{
    uint8_t changed = 0;

    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    Rectangle_f rect_knob =
        {
            (*value) ? rect_bounds.x + rect_bounds.width - rect_bounds.width / 2 : rect_bounds.x,
            rect_bounds.y,
            rect_bounds.width / 2,
            rect_bounds.height};

    if (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_RELEASE)
    {
        if (rectangle_within_bounds(&rect_bounds, mouse_position_x, mouse_position_y))
        {
            *value = !*value;
            changed = 1;
        }
    }

    Color color_fills;
    color_set_i(&color_fills, 50, 64, 91, 255);
    renderer_primitive_add_rectangle(renderer_primitive_fills, rect_bounds, color_fills);

    color_set_i(&color_fills, 66, 84, 120, 255);
    renderer_primitive_add_rectangle(renderer_primitive_fills, rect_knob, color_fills);

    Color color_outline;
    color_set_i(&color_outline, 101, 129, 184, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_bounds, color_outline);

    color_set_i(&color_outline, 255, 255, 255, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_knob, color_outline);

    Color color_sprite_font;
    color_set_i(&color_sprite_font, 255, 255, 255, 255);

    Vector2f position =
        {
            rect_bounds.x + rect_bounds.width + 6,
            rect_bounds.y + rect_bounds.height * 0.5 - font->font_size * 0.5};

    char *caption = (*value) ? text_on : text_off;

    renderer_sprite_font_add_text(renderer_sprite_font, font, caption, 0, position, color_sprite_font, (Vector2f){0, 0});
    return changed;
}
uint8_t lol2(Rectangle_f *rect, float min, float max)
{
    if (rect->y > max)
    {
        return 0;
    }
    else
    {
        if (rect->y + 32 > max)
        {
            rect->height = max - rect->y;
        }
    }

    if (rect->y < min)
    {
        return 0;
    }
    else
    {
        if (rect->y < min + 32)
        {
            rect->height = rect->y - min;
            rect->y += 32 - rect->height;
        }
    }
}
uint8_t imgui_dropdown(ImGui *imgui, int32_t id, Rectangle_f rect_header, char *captions,uint8_t *selections, uint16_t children_count, uint16_t stride)
{
    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    uint8_t display_count = children_count <= 16 ? children_count : 16;

    Rectangle_f scroll_area = (Rectangle_f)
    {
        rect_header.x + rect_header.width,
        rect_header.y + rect_header.height,
        16, rect_header.height * display_count
    };
    Rectangle_f scroll_bar = (Rectangle_f)
    {
        scroll_area.x,
        scroll_area.y,
        16, scroll_area.height * 0.1
    };

    float children_offset = 0;
    uint8_t multi_select = imgui->hid_state->key_ctrl_left;

    uint8_t click = (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_RELEASE);
    uint8_t hold = (mouse_button_left == GLFW_PRESS && mouse_button_left_prev == GLFW_PRESS);
    uint8_t release = (mouse_button_left == GLFW_RELEASE && mouse_button_left_prev == GLFW_PRESS);

    if (click)
    {
        if (rectangle_within_bounds(&rect_header, mouse_position_x, mouse_position_y))
        {
            imgui->collapsed = !imgui->collapsed;

            if (imgui->collapsed)
            {
                if (imgui->active_id == id)
                {
                    imgui->active_id = 0;
                }
            }
            else
            {
                imgui->active_id = id;
                imgui->scroll_bar_y = scroll_area.y;
            }
        }
    }

    if (imgui->active_id == id)
    {
        scroll_bar.y = imgui->scroll_bar_y;

        if (click)
        {
            Rectangle_f rect_bounds = {rect_header.x, rect_header.y, rect_header.width, rect_header.height * display_count + 32};

            if (!rectangle_within_bounds(&rect_bounds, mouse_position_x, mouse_position_y))
            {
                if (!rectangle_within_bounds(&scroll_area, mouse_position_x, mouse_position_y))
                {
                    imgui->collapsed = 1;
                    imgui->active_id = 0;
                }
            }
            if (rectangle_within_bounds(&scroll_bar, mouse_position_x, mouse_position_y))
            {
                imgui->hold_scroll_bar = 1;
                imgui->scroll_bar_delta_y = scroll_bar.y - mouse_position_y;
                printf("lol på dig davi\n");
            }
        }
        else if (hold)
        {
            if (imgui->hold_scroll_bar)
            {
                scroll_bar.y = mouse_position_y + imgui->scroll_bar_delta_y;
                if (scroll_bar.y < scroll_area.y)
                {
                    scroll_bar.y = scroll_area.y;
                }
                else if (scroll_bar.y + scroll_bar.height > scroll_area.y + scroll_area.height)
                {
                    scroll_bar.y = scroll_area.y + scroll_area.height - scroll_bar.height;
                }
                imgui->scroll_bar_y = scroll_bar.y;
            }
        }
        else if (release)
        {
            imgui->hold_scroll_bar = 0;
            imgui->scroll_bar_delta_y = 0;
        }

        if (!imgui->collapsed)
        {
            float min = scroll_area.y;
            float max = (scroll_area.y + scroll_area.height - scroll_bar.height);
            float current = scroll_bar.y;
            float norm = (current - min) / (max - min);

            children_offset = (children_count - display_count) * 32 * norm;

            for (int i = 0; i < children_count; i++)
            {
                uint8_t hover = 0;
                Rectangle_f rect =
                {
                    rect_header.x,
                    rect_header.y + 32 * (i + 1),
                    rect_header.width,
                    rect_header.height
                };
                rect.y -= children_offset;

                float temp_posy = rect.y;

                uint8_t within_bounds = lol2(&rect, rect_header.y, rect_header.y + (32 * (display_count + 1)));

                printf("within_bounds = %i\n",within_bounds);
                if (within_bounds)
                {
                    if (rectangle_within_bounds(&rect, mouse_position_x, mouse_position_y))
                    {
                        hover = 1;
                        if (click)
                        {
                            if (!multi_select)
                            {
                                for (int i = 0; i < children_count; i++)
                                {
                                    selections[i] = 0;
                                }
                                imgui->collapsed = 1;
                                imgui->active_id = 0;
                            }
                            selections[i] = !selections[i];
                        }
                    }

                    Color color_fills;
                    if (selections[i])
                    {
                        color_set_i(&color_fills, 66 * 1.5, 84 * 1.5, 120 * 1.5, 255);
                    }
                    else if (hover)
                    {
                        color_set_i(&color_fills, 66 * 0.5, 84 * 0.5, 120 * 0.5, 255);
                    }
                    else
                    {
                        color_set_i(&color_fills, 50, 64, 91, 255);
                    }

                    renderer_primitive_add_rectangle(renderer_primitive_fills, rect, color_fills);

                    Color color_outlines;
                    if (selections[i])
                    {
                        color_set_i(&color_outlines, 77 * 1.5, 99 * 1.5, 140 * 1.5, 255);
                    }
                    else
                    {
                        color_set_i(&color_outlines, 77, 99, 140, 255);
                    }
                    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect, color_outlines);

                    Color color_text;
                    if (selections[i])
                    {
                        color_set_i(&color_text, 255, 255, 255, 255);
                    }
                    else if (hover)
                    {
                        color_set_i(&color_text, 255, 255, 255, 255);
                    }
                    else
                    {
                        color_set_i(&color_text, 250, 250, 250, 255);
                    }

                    char *caption = &captions[i * sizeof(char) * stride];

                    Vector2f position;
                    position.x = rect.x + 6;
                    position.y = temp_posy + (rect_header.height / 2) - (font->font_size + font->padding) / 2;

                    Vector2f limit_y = {0, 0};
                    limit_y.x = rect_header.y + 32;
                    limit_y.y = rect_header.y + (rect_header.height * (display_count + 1));

                    renderer_sprite_font_add_text(renderer_sprite_font, font, caption, 12, position, color_text, limit_y);
                }
            }
        }

        if (children_count > 16)
        {
            scroll_bar.y = imgui->scroll_bar_y;

            Color color;
            color_set_i(&color, 50, 64, 91, 255);
            renderer_primitive_add_rectangle(renderer_primitive_fills, scroll_area, color);
            color_set_i(&color, 66 * 0.5, 84 * 0.5, 120 * 0.5, 255);
            renderer_primitive_add_rectangle(renderer_primitive_fills, scroll_bar, color);

            color_set_i(&color, 255, 255, 255, 255);
            renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, scroll_area, color);
            renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, scroll_bar, color);
        }
    }

    Color color;
    color_set_i(&color, 50, 64, 91, 255);
    renderer_primitive_add_rectangle(renderer_primitive_fills, rect_header, color);
    color_set_i(&color, 255, 255, 255, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_header, color);

    Color color_text;
    Vector2f position;
    position.x = rect_header.x + 6;
    position.y = rect_header.y + (rect_header.height / 2) - (font->font_size + font->padding) / 2;
    //renderer_sprite_font_add_text(renderer_sprite_font, font, "lol", 12, position, color_text, (Vector2f){0, 0});
}
void imgui_textbox_insert(char *buffer_source, uint16_t *caret, uint16_t *length, const char *buffer_input, uint16_t buffer_input_length)
{
    if (*caret == *length)
    {
        memcpy(&buffer_source[*caret], buffer_input, sizeof(char) * buffer_input_length);
        *caret += buffer_input_length;
        *length += buffer_input_length;
    }
    else
    {
        int len = strlen(&buffer_source[*caret]);
        memcpy(&buffer_source[*caret + buffer_input_length], &buffer_source[*caret], sizeof(char) * len);
        memcpy(&buffer_source[*caret], buffer_input, sizeof(char) * buffer_input_length);
        *caret += buffer_input_length;
        *length += buffer_input_length;
    }
    buffer_source[*length] = '\0';
}
void imgui_textbox_remove(char *buffer_source, uint16_t *caret, uint16_t *length)
{
    if (*caret == *length)
    {
        *caret -= 1;
        *length -= 1;
        buffer_source[*length] = '\0';
    }
    else
    {
        int32_t len = strlen(&buffer_source[*caret]);
        memcpy(&buffer_source[*caret - 1], &buffer_source[*caret], sizeof(char) * len);
        *caret -= 1;
        *length -= 1;
        buffer_source[*length] = '\0';
    }
}

void imgui_textbox_remove_segment(char *buffer_source, uint16_t *caret, uint16_t *length, uint16_t segment_start, uint16_t segment_end)
{
    int32_t len = strlen(&buffer_source[segment_end]);

    if (len == 0)
    {
        *caret = segment_start;
        *length -= segment_end - segment_start;
        buffer_source[*length] = '\0';
    }
    else
    {
        memcpy(&buffer_source[segment_start], &buffer_source[segment_end], sizeof(char) * len);
        *caret = segment_start;
        *length -= segment_end - segment_start;
        buffer_source[*length] = '\0';
    }
}
TextboxEvent imgui_textbox(ImGui *imgui, int32_t id, Rectangle_f rect_bounds, float dt, char* text, uint8_t input_mode,uint32_t max_display_length)
{
    TextboxEvent event = {0};
    
    RendererSpriteFont *renderer_sprite_font       = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills    = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left      = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    uint8_t click   = (mouse_button_left == GLFW_PRESS   && mouse_button_left_prev == GLFW_RELEASE);
    uint8_t hold    = (mouse_button_left == GLFW_PRESS   && mouse_button_left_prev == GLFW_PRESS);
    uint8_t release = (mouse_button_left == GLFW_RELEASE && mouse_button_left_prev == GLFW_PRESS);

    uint8_t key_backspace      = imgui->hid_state->key_backspace;
    uint8_t key_backspace_prev = imgui->hid_state_prev->key_backspace;

    uint8_t key_enter      = imgui->hid_state->key_enter;
    uint8_t key_enter_prev = imgui->hid_state_prev->key_enter;

    uint8_t key_escape      = imgui->hid_state->key_escape;
    uint8_t key_escape_prev = imgui->hid_state_prev->key_escape;

    uint8_t key_arrow_left = imgui->hid_state->key_arrow_left;
    uint8_t key_arrow_left_prev = imgui->hid_state_prev->key_arrow_left;

    uint8_t key_arrow_right = imgui->hid_state->key_arrow_right;
    uint8_t key_arrow_right_prev = imgui->hid_state_prev->key_arrow_right;

    uint8_t key_a = imgui->hid_state->key_a;
    uint8_t key_a_prev = imgui->hid_state_prev->key_a;

    uint8_t key_c = imgui->hid_state->key_c;
    uint8_t key_c_prev = imgui->hid_state_prev->key_c;

    uint8_t key_v = imgui->hid_state->key_v;
    uint8_t key_v_prev = imgui->hid_state_prev->key_v;

    uint8_t key_ctrl_left = imgui->hid_state->key_ctrl_left;

    uint16_t length = strlen(text);
    uint16_t length_prev = length;
    
    if (click)
    {
        imgui->mp_select = 0;

        if(rectangle_within_bounds(&rect_bounds, mouse_position_x, mouse_position_y))
        {
            imgui->active_id = id;
            
            Vector2f mp = (Vector2f)
            {
                mouse_position_x - (rect_bounds.x + 6),
                mouse_position_y - (rect_bounds.y)
            };
            imgui->caret = sprite_font_set_caret_mouse(text, imgui->offset, font, mp);
            imgui->selection_pivot = imgui->caret;
            imgui->hold_timer = 0;
        }
    }

    if(imgui->active_id == id)
    {
        imgui->blink_timer += dt;
        if (imgui->blink_timer < 0.6)
        {
            imgui->caret_blink = 1;
        }
        else
        {
            imgui->caret_blink = 0;
        }
        if (imgui->blink_timer > 1.2)
        {
            imgui->blink_timer = 0;
        }

        if (imgui->char_buffer_length > 0)
        {
            imgui->blink_timer = -0.2;
            imgui->caret_blink = 1;
            imgui_textbox_insert(text, &imgui->caret, &length, imgui->char_buffer, imgui->char_buffer_length);
        }
        else
        {
            if (key_backspace == GLFW_PRESS && key_backspace_prev == GLFW_RELEASE)
            {
                if (imgui->selection || imgui->mp_select)
                {
                    int start = (imgui->caret < imgui->selection_pivot) ? imgui->caret : imgui->selection_pivot;
                    int end = (imgui->caret > imgui->selection_pivot) ? imgui->caret : imgui->selection_pivot;
                    imgui_textbox_remove_segment(text, &imgui->caret, &length, start, end);
                }
                else if (imgui->caret > 0)
                {
                    imgui->blink_timer = -0.2;
                    imgui->caret_blink = 1;
                    imgui_textbox_remove(text, &imgui->caret, &length);
                }

                imgui->selection = 0;
                imgui->mp_select = 0;
                imgui->hold_timer_backspace = 0;
                imgui->delete_interval = 0;
            }
            else if (key_backspace == GLFW_PRESS && key_backspace_prev == GLFW_PRESS)
            {
                if (imgui->hold_timer_backspace > 0.5)
                {
                    if (imgui->delete_interval <= 0)
                    {
                        if (imgui->caret > 0)
                        {
                            imgui->blink_timer = -0.2;
                            imgui->caret_blink = 1;
                            imgui_textbox_remove(text, &imgui->caret, &length);
                        }
                        imgui->delete_interval = 0.1;
                    }
                    imgui->delete_interval -= dt;
                }
                imgui->hold_timer_backspace += dt;
            }
            if (key_enter == GLFW_PRESS && key_enter_prev == GLFW_RELEASE)
            {
                imgui->active_id = 0;
            }
            else if (key_escape == GLFW_PRESS && key_escape_prev == GLFW_RELEASE)
            {
                imgui->active_id = 0;
            }
            else if (key_arrow_left == GLFW_PRESS && key_arrow_left_prev == GLFW_RELEASE)
            {
                imgui->blink_timer = -0.2;
                imgui->caret_blink = 1;

                if (key_ctrl_left == GLFW_PRESS)
                {
                    if (!imgui->selection)
                    {
                        imgui->selection_pivot = imgui->caret;
                        imgui->selection = 1;
                    }
                }
                else
                {
                    imgui->selection = 0;
                }
                if (imgui->caret > 0)
                {
                    imgui->caret--;
                }
            }
            else if (key_arrow_right == GLFW_PRESS && key_arrow_right == GLFW_RELEASE)
            {
                imgui->blink_timer = -0.2;
                imgui->caret_blink = 1;

                if (key_ctrl_left == GLFW_PRESS)
                {
                    if (!imgui->selection)
                    {
                        imgui->selection_pivot = imgui->caret;
                        imgui->selection = 1;
                    }
                }
                else
                {
                    imgui->selection = 0;
                }

                if (imgui->caret < length)
                {
                    imgui->caret++;
                }
            }
            else if (key_ctrl_left == GLFW_PRESS  && key_a == GLFW_PRESS && key_a_prev == GLFW_RELEASE)
            {
                imgui->selection_pivot = 0;
                imgui->caret = length;
                imgui->mp_select = 1;
            }
        }

        uint8_t paste = (key_ctrl_left == GLFW_PRESS && key_v == GLFW_PRESS && key_v_prev == GLFW_RELEASE);

        if (paste)
        {
            const char *clipboard_text = glfwGetClipboardString(NULL);
            int32_t len = strlen(clipboard_text);
            imgui_textbox_insert(text, &imgui->caret, &length, clipboard_text, len);
        }

        uint8_t copy = (key_ctrl_left == GLFW_PRESS && key_c == GLFW_PRESS && key_c_prev == GLFW_RELEASE);

        if (copy)
        {
            uint16_t start = (imgui->caret < imgui->selection_pivot) ? imgui->caret : imgui->selection_pivot;
            uint16_t end = (imgui->caret > imgui->selection_pivot) ? imgui->caret : imgui->selection_pivot;
            char buffer[256];
            memcpy(buffer, &text[start], sizeof(char) * (end - start));
            buffer[end] = '\0';
            glfwSetClipboardString(imgui->window, buffer);
        }

        if (hold)
        {
            if (imgui->hold_timer > 0.5)
            {
                if (!imgui->mp_select)
                {
                    imgui->mp_select = 1;
                }
                else
                {
                    Vector2f mp = (Vector2f)
                    {
                        mouse_position_x - (rect_bounds.x + 6),
                        mouse_position_y - (rect_bounds.y)
                    };

                    imgui->caret = sprite_font_set_caret_mouse(&text[0], imgui->offset, font, mp);
                }
            }
            imgui->hold_timer += dt;
        }
    }

    Color color;
    color_set_i(&color, 50, 64, 91, 255);
    renderer_primitive_add_rectangle(renderer_primitive_fills, rect_bounds, color);
    color_set_i(&color, 101, 129, 184, 255);
    renderer_primitive_add_rectangle_outline(renderer_primitive_outlines, rect_bounds, color);

    if(imgui->active_id == id)
    {
        if (imgui->selection || imgui->mp_select)
        {
            color_set_i(&color, 137, 153, 190, 255);
            Rectangle_f rect = {0,0,0,0};

            FontMeasurement m_caret;
            FontMeasurement m_pivot;
            float c = 0;
            float p = 0;

            sprite_font_measure_string(&text[imgui->offset],font,&m_caret,imgui->caret-imgui->offset,-1);
            c = m_caret.width_in_px;

            if(imgui->selection_pivot > imgui->offset)
            {
                sprite_font_measure_string(&text[imgui->offset],font,&m_pivot,imgui->selection_pivot-imgui->offset,-1);
                p = m_pivot.width_in_px;
            }

            rect.width = fabs(c-p);
            rect.height = font->font_size;
            rect.x = (c < p) ? c:p;
            rect.x += rect_bounds.x  + font->padding;
            rect.y = rect_bounds.y + rect.height*0.5;


            renderer_primitive_add_rectangle(renderer_primitive_fills, rect, color);
            
        }

        Vector2f dim = {0,0};

        char *display_text = (input_mode == 0) ? &text[imgui->offset] : &imgui->text_password[imgui->offset];

        FontMeasurement measurement;
        sprite_font_measure_string(display_text, font,&measurement,  imgui->caret - imgui->offset,-1);
    
        Vector2f p = 
        {
            rect_bounds.x + measurement.width_in_px + font->padding,
            rect_bounds.y + rect_bounds.height*0.5 - font->font_size*0.5
        };

        if (imgui->caret_blink)
        {
            color_set_i(&color, 255, 255, 255, 255);
        }
        else
        {
            color_set_i(&color, 255, 255, 255, 0);
        }

        renderer_primitive_add_rectangle(renderer_primitive_fills, (Rectangle_f){p.x, p.y, 2, font->font_size}, color);


    }
    Vector2f position;
    position.x = rect_bounds.x + font->padding;
    position.y = rect_bounds.y + rect_bounds.height * 0.5 - (font->font_size  + font->padding)/2;

    Color color_text;
    color_set_i(&color_text, 255, 255, 255, 255);

    int16_t offset = (imgui->active_id == id) ? imgui->offset:0;
    int16_t caret  = (imgui->active_id == id) ? imgui->caret:0;


    if(input_mode == 0)
    {
        if(caret > max_display_length)
        {
            offset = caret -  max_display_length;
        }
        else
        {
            offset = 0;
        }
        if(length > 0)
        {   

            renderer_sprite_font_add_text(renderer_sprite_font
                                         ,font
                                         ,&text[offset]
                                         ,max_display_length
                                         ,position
                                         ,color_text,(Vector2f){0,0}
                                         );
        }
    }
    else
    {
        if(length > 0)
        {
            uint16_t len = length+1;
            memset(imgui->text_password,'*',sizeof(char)*len);
            imgui->text_password[len-1] = '\0';

            renderer_sprite_font_add_text(renderer_sprite_font
                                         ,font
                                         ,&imgui->text_password[offset]
                                         ,max_display_length
                                         ,position
                                         ,color_text,(Vector2f){0,0}
                                         );
        }
        
    }

    event.changed = (length != length_prev);

    return event;
    
}

void imgui_label(ImGui *imgui, int32_t id, Vector2f position, char* caption)
{
    Color color_text;
    color_set_i(&color_text, 255, 255, 255, 255);

    RendererSpriteFont *renderer_sprite_font = &imgui->renderer_sprite_font;
    renderer_sprite_font_add_text(renderer_sprite_font, imgui->font, caption, 0, position, color_text,(Vector2f){0,0});
}

void imgui_tabbar(ImGui *imgui, int32_t id,int8_t* selected, Rectangle_f rect_bounds, uint8_t tab_count, Texture *texture)
{
    RendererPrimitive *renderer_primitive_fills    = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    RendererSprite *renderer_sprites               = &imgui->renderer_sprites;

    TabBarEvent event = {0};
    int32_t mpx = imgui->hid_state->mouse_position_x;
    int32_t mpy = imgui->hid_state->mouse_position_y;
    int8_t mb   = imgui->hid_state->mouse_button_left;
    int8_t mbp  = imgui->hid_state_prev->mouse_button_left;

    int8_t hover = -2;

    for (int i = 0; i < tab_count; i++)
    {
        Rectangle_f tab_bounds = 
        {
            rect_bounds.x + (i * 64),
            rect_bounds.y + 8,
            64,
            64
        };
        if (rectangle_within_bounds(&tab_bounds, mpx, mpy))
        {
            hover = i;
            if (mb == GLFW_PRESS && mbp == GLFW_RELEASE)
            {
                *selected = i;
                event.changed = 1;
                event.index = 1;
            }
        }
    }
    
        for (int i = 0; i < tab_count; i++)
        {
            Rectangle_f tab_bounds = 
            {
                rect_bounds.x + (i * 64),
                rect_bounds.y + 8,
                64,
                64
            };
        
            int8_t top_left = 0;
            int8_t bottom_left = 0;
            int8_t top_right = 0;
            int8_t bottom_right = 0;

            float pl = 1.0;
            float pr = 1.0;
            
            Color color_fills;
            Color color_outlines;
            Color color_sprite;

            if (hover == i)
            {
                color_set_i(&color_fills, 66, 84, 120, 255);
                color_set_i(&color_outlines, 255, 255, 255, 100);
                color_set_i(&color_sprite, 255, 255, 255, 255);
                top_left = 6;
                bottom_left = 6;
                top_right = 6;
                bottom_right = 6;
            }
            else if (i == (hover - 1))
            {
                color_set_i(&color_fills, 40, 51, 72, 255);
                color_set_i(&color_outlines, 50, 64, 91, 100);
                color_set_i(&color_sprite, 200, 200, 200, 255);
                top_left = 0;
                bottom_left = 0;
                top_right = 5;
                bottom_right = 5;
            }
            else if (i == (hover + 1))
            {
                color_set_i(&color_fills, 40, 51, 72, 255);
                color_set_i(&color_outlines, 50, 64, 91, 100);
                color_set_i(&color_sprite, 200, 200, 200, 255);
                top_left = 5;
                bottom_left = 5;
                top_right = 0;
                bottom_right = 0;
            }
            else
            {
                color_set_i(&color_fills, 50, 64, 91, 100);
                color_set_i(&color_outlines, 50, 64, 91, 100);
                color_set_i(&color_sprite, 200, 200, 200, 255);
            }

            if (*selected == i)
            {
                color_set_i(&color_sprite, 255, 157, 0, 255);
            }

            Rectangle_f *b = &tab_bounds;
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x,            b->y - top_left}, color_fills);
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x + b->width, b->y - top_right}, color_fills);
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x + b->width, b->y + b->height + bottom_right}, color_fills);
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x + b->width, b->y + b->height + bottom_right}, color_fills);
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x,            b->y + b->height + bottom_left}, color_fills);
            renderer_primitive_add(renderer_primitive_fills,    (Vector2f){b->x,            b->y - top_left}, color_fills);

            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x, b->y - bottom_left}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x + b->width, b->y - bottom_right}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x + b->width, b->y - bottom_right}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x + b->width, b->y + b->height + top_right}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x + b->width, b->y + b->height + top_right}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x, b->y + b->height + top_left}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x, b->y + b->height + top_left}, color_fills);
            renderer_primitive_add(renderer_primitive_outlines, (Vector2f){b->x, b->y - bottom_left}, color_fills);

            Rectangle_f src = 
            {
                64*i,0,64,64
            };
            Rectangle_f des = 
            {
                64*i,0,64,64
            };

            renderer_sprite_add(renderer_sprites,32+(64*i),64/2+8,64,64,64*i,64,64,64,texture, &color_sprite);
        }
    
}
int days_in_month(int year, int month) {
    // Array of days in each month (for non-leap years)
    int days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Check for leap year and adjust February's days
    if (month == 2 && (year % 400 == 0 || (year % 100 != 0 && year % 4 == 0))) {
        return 29;
    }
    return days_in_months[month - 1];
}
DatePickerEvent imgui_datepicker(ImGui* imgui, int32_t id,uint16_t selected_year,uint8_t selected_month,uint8_t selected_day)
{
    uint16_t year  = selected_year  + imgui->offset_year;
    uint8_t  month = selected_month + imgui->offset_month;
    uint8_t  day   = selected_day;

    DatePickerEvent event;
    event.changed = 0;
    event.year = year;
    event.month = month;
    event.day = day;

    RendererSpriteFont *renderer_sprite_font       = &imgui->renderer_sprite_font;
    RendererPrimitive *renderer_primitive_fills    = &imgui->renderer_primitive_fills;
    RendererPrimitive *renderer_primitive_outlines = &imgui->renderer_primitive_outlines;
    SpriteFont *font = imgui->font;

    float mouse_position_x = imgui->hid_state->mouse_position_x;
    float mouse_position_y = imgui->hid_state->mouse_position_y;

    uint8_t mouse_button_left      = imgui->hid_state->mouse_button_left;
    uint8_t mouse_button_left_prev = imgui->hid_state_prev->mouse_button_left;

    uint8_t key_ctrl_left = imgui->hid_state->key_ctrl_left;


    Rectangle_f bounds_header = {100,100,256,32};
    Color color_header = (Color){0.3,0.3,0.3,1};

    if(rectangle_within_bounds(&bounds_header,mouse_position_x,mouse_position_y))
    {
        color_set_f(&color_header,0.5,0.5,0.5,1.0);

        if(mouse_button_left == 1 && mouse_button_left_prev == 0)
        {
            imgui->active_id = (imgui->active_id == id) ? 0 : id;
        }
    }

    struct tm time_info = {0};
    mktime(&time_info);

    int count = days_in_month(year,month);
    int x = 0;
    int y = 0;

    char selected_date_string[128];
    
    snprintf(selected_date_string, sizeof(char)*128,"%02d-%02d-%02d",selected_year,selected_month,selected_day);
    renderer_sprite_font_add_text(renderer_sprite_font
                                 ,imgui->font,selected_date_string
                                 ,200
                                 ,(Vector2f){bounds_header.x + 82,bounds_header.y + 2}
                                 ,(Color){1,1,1,1}
                                 ,(Vector2f){0,0}
                                 );
    
    renderer_primitive_add_rectangle(renderer_primitive_fills
                                    ,bounds_header
                                    ,(Color){0.15,0.15,0.15,1}
                                    );

    if(imgui->active_id == id)
    {
        Color color_text = (Color){1,1,1,1};
        
        Vector2f margin = {10,10};

        Rectangle_f bounds_calendar_top = (Rectangle_f)
        {
            bounds_header.x,
            bounds_header.y,
            256,
            96
        };

        Rectangle_f bounds_calendar_bottom = 
        {
            bounds_header.x,
            bounds_header.y,
            256,
            256 + 64
        };

        Rectangle_f bounds_area_interactive = 
        {
            bounds_header.x,
            bounds_header.y,
            256,
            bounds_calendar_top.height + bounds_calendar_bottom.height
        };

        if(!rectangle_within_bounds(&bounds_area_interactive,mouse_position_x,mouse_position_y))
        {
            if(mouse_button_left == 1 && mouse_button_left_prev == 0)
            {
                imgui->active_id = 0;
            }
        }

        renderer_primitive_add_rectangle(renderer_primitive_fills,
                                         bounds_calendar_bottom,
                                         (Color){0.1,0.1,0.1,1}
                                        );
                                        
        renderer_primitive_add_rectangle(renderer_primitive_fills,
                                         bounds_calendar_top, 
                                         (Color){0.15,0.15,0.15,1}
                                        );
        
        renderer_sprite_font_add_text(renderer_sprite_font
                                     ,imgui->font
                                     ,"Mo  Tu  We  Th  Fr  Sa  Su"
                                     ,200
                                     ,(Vector2f){margin.x + bounds_calendar_top.x + x,margin.y + bounds_calendar_top.y + y + 58}
                                     ,color_text
                                     ,(Vector2f){0,0}
                                     );

        time_info.tm_year = year - 1900; 
        time_info.tm_mon = month - 1;    
        time_info.tm_mday = 1;    
        mktime(&time_info);

        int weekday = time_info.tm_wday-1;
        if(weekday == -1) weekday = 6; //Americans are retarded sometimes

        x = weekday * 36;
        int stride_count = weekday;

    
        char month_str[128];
        char *month_names[] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

        snprintf(month_str,sizeof(char)*128,"%i %s",year,month_names[month-1]);

        renderer_sprite_font_add_text(renderer_sprite_font
                                     ,imgui->font
                                     ,month_str
                                     ,200
                                     ,(Vector2f){margin.x + bounds_calendar_top.x + 64,margin.y + bounds_calendar_top.y+26}
                                     ,color_text
                                     ,(Vector2f){0,0}
                                     );

        for (size_t i = 0; i < count; i++)
        {
            char day_str[3];
            snprintf(day_str,3,"%i",(i+1));
            
            Vector2f position = (Vector2f)
            {
                margin.x + bounds_calendar_top.x + x,
                margin.y + bounds_calendar_top.y + y + 96
            };

            renderer_sprite_font_add_text(renderer_sprite_font
                                         ,imgui->font
                                         ,day_str
                                         ,200
                                         ,position
                                         ,color_text
                                         ,(Vector2f){0,0}
                                         );
            
            x+= 36;

            if(selected_day == i+1 && month == selected_month && year == selected_year)
            {
                renderer_primitive_add_rectangle(renderer_primitive_fills
                                                ,(Rectangle_f){position.x-6,position.y-2,32,28}
                                                ,(Color){0,1,0,0.4}
                                                ); 
            }
            else if(mouse_position_x > position.x - 16 && mouse_position_x < position.x + 16)
            {
                if(mouse_position_y > position.y - 16 && mouse_position_y < position.y + 16)
                {
                    renderer_primitive_add_rectangle(renderer_primitive_fills
                                                    ,(Rectangle_f){position.x-6,position.y-2,32,28}
                                                    ,(Color){0.4,0.4,0.4,0.4}
                                                    ); 
                    
                    if(mouse_button_left == 1 && mouse_button_left_prev == 0)
                    {
                        event.changed = 1;
                        event.year = selected_year + imgui->offset_year;
                        event.month = selected_month + imgui->offset_month;
                        event.day = i+1;
                        imgui->offset_year = 0;
                        imgui->offset_month = 0;
                    }       
                }   
            }
            
            stride_count ++;
            if(stride_count == 7)
            {
                x = 0;
                y += 36;
                stride_count = 0;
            }
        }
    
        Rectangle_f button_month_previous =
        {
            bounds_calendar_top.x,
            bounds_calendar_top.y + 32,
            32,
            32
        };
        Rectangle_f button_month_next = 
        {
            bounds_calendar_top.x + bounds_calendar_top.width - 32, 
            bounds_calendar_top.y +32,
            32,
            32
        };
        
        if(rectangle_within_bounds(&button_month_previous,mouse_position_x,mouse_position_y))
        {
            if(mouse_button_left == 1 && mouse_button_left_prev == 0)
            {
                if(imgui->offset_month > -1200)
                {
                    if(!key_ctrl_left)
                    {
                        imgui->offset_month -= 1;
                        if(selected_month + imgui->offset_month < 1)
                        {
                            imgui->offset_month = 12-selected_month;
                            imgui->offset_year --;
                        }
                    }
                    else
                    {
                        imgui->offset_year --;
                    }
                }
            }
            renderer_primitive_add_rectangle(renderer_primitive_fills
                                            ,button_month_previous
                                            ,(Color){0.4,0.4,0.4,1.0}
                                            ); 
        }
        else if(rectangle_within_bounds(&button_month_next,mouse_position_x,mouse_position_y))
        {
            if(mouse_button_left == 1 && mouse_button_left_prev == 0)
            {
                if(imgui->offset_month < 1200)
                {
                    if(!key_ctrl_left)
                    {
                        imgui->offset_month += 1;
                        if(selected_month + imgui->offset_month > 12)
                        {
                            imgui->offset_month = -selected_month+1;
                            imgui->offset_year ++;
                        }
                    }
                    else
                    {
                        imgui->offset_year ++;
                    }
                    
                }
            }
            renderer_primitive_add_rectangle(renderer_primitive_fills
                                            ,button_month_next
                                            ,(Color){0.4,0.4,0.4,1.0}
                                            ); 
        }
        

        Color color_arrow = {0.9,0.9,0.9,1};

        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_previous.x+ 20 ,button_month_previous.y + 6 }, color_arrow);
        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_previous.x+ 6  ,button_month_previous.y + 16}, color_arrow);
        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_previous.x+ 20 ,button_month_previous.y + 24}, color_arrow);

        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_next.x + 10  ,button_month_next.y + 6 },color_arrow);
        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_next.x + 32-6,button_month_next.y + 16},color_arrow);
        renderer_primitive_add(renderer_primitive_fills,(Vector2f){button_month_next.x + 10  ,button_month_next.y + 24},color_arrow);

    }

    return event;
}