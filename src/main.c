
#define STB_IMAGE_IMPLEMENTATION

#include <delo2d.h>  
#include <imgui.h>  
#include <stdio.h>

int main()
{
    GfxCoreContext     context;
    RendererSprite     renderer_sprite;
    RendererSpriteFont renderer_sprite_font;
    RendererPrimitive  renderer_primitive;

    context_init(&context,1920,1080,"IMI Aware");
    renderer_sprite_font_init(&renderer_sprite_font,&context,1000);
    renderer_sprite_init(&renderer_sprite,100,&context);
    renderer_primitive_init(&renderer_primitive,1000,&context);

    uint32_t shader_sprite;
    uint32_t shader_sprite_font;
    uint32_t shader_primitive;
    SpriteFont font_default;

    sprite_font_load(&font_default,"fonts/white-rabbit.regular.ttf",16);
    shader_load("shaders/gl300/sprite.vert"   ,"shaders/gl300/sprite.frag"     ,&shader_sprite);
    shader_load("shaders/gl300/sprite.vert"   ,"shaders/gl300/sprite_font.frag",&shader_sprite_font);
    shader_load("shaders/gl300/primitive.vert","shaders/gl300/primitive.frag"  ,&shader_primitive);

    renderer_sprite_apply_shader         (&renderer_sprite     ,shader_sprite);
    renderer_sprite_font_apply_shader    (&renderer_sprite_font,shader_sprite_font);
    renderer_primitive_apply_shader      (&renderer_primitive  ,shader_primitive);
    
    HidState         *hid_state          = &context.hid_state;
    HidState         *hid_state_prev     = &context.hid_state_prev;
    GlfwCallbackData *glfw_callback_data = &context.glfw_callback_data;
    GLFWwindow       *window             = context.window;

    hid_control_init(hid_state,      window);
    hid_control_init(hid_state_prev, window);

    glfwSetWindowUserPointer(window, glfw_callback_data);
    glfwSetCharCallback(window, callback_character);


    ImGui imgui;
    imgui.active_id = 0;
    uint8_t toggle_value = 0;
    imgui_init(&imgui,&context,hid_state,hid_state_prev,&font_default, shader_primitive,shader_sprite_font,shader_sprite);
    int32_t slider_value = 50;
    int32_t slider_value2 = 5;
    int8_t active_tab = 0;

    char captions[32][CAPTION_SIZE];
    uint8_t selections[32];

    for (size_t i = 0; i < 32; i++)
    {
        snprintf(&captions[i][0],sizeof(char)*CAPTION_SIZE,"[%i]%s", i, "Option");
        selections[i] = 0;
    }
    
    char text[256];
    text[0] = '\0';

        uint16_t year = 2024;
    uint8_t month = 10;
    uint8_t day = 18;

    imgui.offset_month = 0;
    imgui.offset_year = 0;

    while (!glfwWindowShouldClose(window)) 
    {
        hid_control_update(hid_state
                                 ,window
                                 ,context.screen_width
                                 ,context.screen_height
                                 ,context.back_buffer_width
                                 ,context.back_buffer_height
                                 );
        if(hid_state->key_escape == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        frame_begin(&context);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
/*
        Color c;
        color_set_i(&c, 50, 64, 91, 255);
        renderer_primitive_begin(&renderer_primitive,renderer_primitive.projection,DELO_TRIANGLE_LIST);
        renderer_primitive_add_rectangle(&renderer_primitive,(Rectangle_f){0,0,1920,100},c);
        renderer_primitive_end(&renderer_primitive);

        color_set_i(&c, 255, 255, 255, 255);
        renderer_primitive_begin(&renderer_primitive,renderer_primitive.projection,DELO_LINE_LIST);
        renderer_primitive_add_rectangle_outline(&renderer_primitive,(Rectangle_f){0,0,1920,100},c);
        renderer_primitive_end(&renderer_primitive);

        imgui_begin(&imgui,&glfw_callback_data->key_buffer[0], glfw_callback_data->key_buffer_length);


        imgui_label(&imgui,UNIQUE_ID,(Vector2f){512,32},"Areas");
        imgui_label(&imgui,UNIQUE_ID,(Vector2f){512 +32 + 128,32},"Pick Zone");
        imgui_label(&imgui,UNIQUE_ID,(Vector2f){512 + 64+ 128 + 128,32},"Status");


        imgui_dropdown(&imgui,UNIQUE_ID,(Rectangle_f){512,58,128, 32},&captions[0][0], selections,32, CAPTION_SIZE);

        imgui_dropdown(&imgui,UNIQUE_ID,(Rectangle_f){512 +32 + 128,58,128, 32},&captions[0][0], selections,32, CAPTION_SIZE);

        imgui_dropdown(&imgui,UNIQUE_ID,(Rectangle_f){512 + 64+ 128 + 128,58,128, 32},&captions[0][0], selections,32, CAPTION_SIZE);



        if(imgui_button((Rectangle_f){100,100,256,64},"Lol boi", &imgui).clicked)
        {
            printf("%s\n", "clicked");
        }

        imgui_slider(&imgui,UNIQUE_ID, (Rectangle_f){200,200,256,32},(Rectangle_f){0,0,32,32},0, 100, &slider_value);

        imgui_slider(&imgui,UNIQUE_ID, (Rectangle_f){400,400,256,32},(Rectangle_f){0,0,32,32},0, 50, &slider_value2);

        imgui_tswitch(&imgui,UNIQUE_ID, (Rectangle_f){400,700,64,32},&toggle_value, "On", "Off");


        imgui_dropdown(&imgui,UNIQUE_ID,(Rectangle_f){500,200,128,32},&captions[0][0],selections,32,CAPTION_SIZE);


        imgui_textbox(&imgui,UNIQUE_ID, (Rectangle_f){100,700,128,32}, context.dt, text, 0,25);

        imgui_tabbar(&imgui,UNIQUE_ID,&active_tab,(Rectangle_f){0,0,384,128},8,&tabbar_tex);

        
*/

        imgui_begin(&imgui,&glfw_callback_data->key_buffer[0], glfw_callback_data->key_buffer_length);
        
        DatePickerEvent dpe = imgui_datepicker(&imgui,UNIQUE_ID,year,month,day);
        if(dpe.changed)
        {
            year = dpe.year;
            month = dpe.month;
            day = dpe.day;
        }


        imgui_end(&imgui);

        frame_end(&context);
    }
}