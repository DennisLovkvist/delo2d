#pragma once 
#include <delo2d.h>
#include <stdint.h>

#define CAPTION_SIZE 128

#define BUTTON_STATE_NORMAL 0
#define BUTTON_STATE_HOVER  1
#define BUTTON_STATE_ACTIVE 2

#define UNIQUE_ID __LINE__

typedef struct ImGui ImGui;
struct ImGui
{
    GLFWwindow *window;
    HidState *hid_state;
    HidState *hid_state_prev;

    RendererSpriteFont renderer_sprite_font;            
    RendererPrimitive renderer_primitive_fills;   
    RendererPrimitive renderer_primitive_outlines;
    RendererSprite renderer_sprites;
    SpriteFont* font;

    float dx;
    int32_t active_id;
    uint8_t collapsed;
    uint8_t hold_scroll_bar;
    uint8_t dropdown_hover;
    float scroll_bar_delta_y;
    float scroll_bar_y;

    float blink_timer;
    uint8_t caret_blink;
    uint8_t mp_select;
    int16_t caret;
    int16_t offset;
    int16_t selection_pivot;
    float hold_timer;
    uint8_t selection;
    float hold_timer_backspace;
    float delete_interval;
    char text_password[32];

    char* char_buffer;
    uint8_t char_buffer_length;

    int8_t offset_year;
    int8_t offset_month;
};

typedef struct ButtonEvent ButtonEvent;
struct ButtonEvent
{
    uint8_t clicked;
    uint8_t hover;
};
typedef struct SliderEvent SliderEvent;
struct SliderEvent
{
    uint8_t released;
    uint8_t changed;
    float delta;
};

typedef struct TextboxEvent TextboxEvent;
struct TextboxEvent
{
    uint8_t changed;
};
typedef struct TabBarEvent TabBarEvent;
struct TabBarEvent
{
    uint8_t changed;
    uint8_t index;
};

typedef struct DatePickerEvent DatePickerEvent;
struct DatePickerEvent
{
    uint8_t changed;
    uint16_t year;
    uint8_t month;
    uint8_t day;
};


void imgui_init(ImGui* imgui, GfxCoreContext* context, HidState* hid_state,HidState* hid_state_prev, SpriteFont* font,uint32_t shader_primitive, uint32_t shader_sprite_font, uint32_t shader_sprite);
void imgui_begin(ImGui* imgui,char* char_buffer, uint8_t char_buffer_length);
void imgui_end(ImGui* imgui);
ButtonEvent imgui_button(Rectangle_f rect,char* caption, ImGui* imgui);
SliderEvent imgui_slider(ImGui* imgui,int32_t id, Rectangle_f rect_bounds,Rectangle_f rect_knob,int32_t min, int32_t max, int32_t* value);
uint8_t imgui_tswitch(ImGui* imgui,int32_t id, Rectangle_f rect_bounds,uint8_t* value, char *text_on, char *text_off);
uint8_t imgui_dropdown(ImGui* imgui,int32_t id, Rectangle_f rect_header,char* captions,uint8_t *selections,  uint16_t children_count, uint16_t stride);
TextboxEvent imgui_textbox(ImGui *imgui, int32_t id, Rectangle_f rect_bounds, float dt, char* text, uint8_t input_mode,uint32_t max_display_length);
void imgui_label(ImGui *imgui, int32_t id, Vector2f position, char* caption);
void imgui_tabbar(ImGui *imgui, int32_t id,int8_t* selected,Rectangle_f rect_bounds, uint8_t tab_count, Texture *texture);
DatePickerEvent imgui_datepicker(ImGui* imgui, int32_t id,uint16_t year,uint8_t month,uint8_t day);