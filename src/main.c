
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
    uint32_t alpha_bg_shader;
    SpriteFont font_default;

    sprite_font_load(&font_default,"fonts/white-rabbit.regular.ttf",16);
    shader_load("shaders/gl300/sprite.vert"   ,"shaders/gl300/sprite.frag"     ,&shader_sprite);
    shader_load("shaders/gl300/sprite.vert"   ,"shaders/gl300/sprite_font.frag",&shader_sprite_font);
    shader_load("shaders/gl300/primitive.vert","shaders/gl300/primitive.frag"  ,&shader_primitive);

    shader_load("shaders/gl300/primitive.vert","shaders/gl300/default_canvas_bg.frag"  ,&alpha_bg_shader);

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

    float canvas_width = 128;
    float canvas_height = 128;

    imgui.offset_month = 0;
    imgui.offset_year = 0;

    float display_width = 128;
    float display_height = 128;

    RenderTarget rt_layer_0;
    render_target_init(&rt_layer_0,1920,1080,0,0,canvas_width,canvas_height);

    RenderTarget rt_layer_1;
    render_target_init(&rt_layer_1,1920,1080,0,0,canvas_width,canvas_height);

    Sprite canvas_bg;
    sprite_define(&canvas_bg,display_width,display_height,(Rectangle_f){0,0,canvas_width,canvas_height});
   

    Sprite canvas;
    sprite_define(&canvas,display_width,display_height,(Rectangle_f){0,0,canvas_width,canvas_height});
    canvas.position.x    = display_width/2;
    canvas.position.y    = display_height/2;
    canvas_bg.position.x = display_width/2;
    canvas_bg.position.y = display_height/2;

    renderer_sprite_add2(&renderer_sprite,&canvas_bg,&rt_layer_0.texture);

    renderer_sprite_add2(&renderer_sprite,&canvas,&rt_layer_1.texture);

    glBindFramebuffer(GL_FRAMEBUFFER, rt_layer_0.fbo);
    glViewport(0, 0, canvas_width, canvas_height);
    renderer_primitive_begin(&renderer_primitive,&rt_layer_0.projection,&alpha_bg_shader,DELO_TRIANGLE_LIST);
    renderer_primitive_add_rectangle(&renderer_primitive,(Rectangle_f){0,0,canvas_width,canvas_height},(Color){1,1,1,1});
    renderer_primitive_end(&renderer_primitive);

    glBindFramebuffer(GL_FRAMEBUFFER, rt_layer_1.fbo);
    glViewport(0, 0, canvas_width, canvas_height);
    renderer_primitive_begin(&renderer_primitive,&rt_layer_1.projection,NULL,DELO_TRIANGLE_LIST);
    renderer_primitive_add_rectangle(&renderer_primitive,(Rectangle_f){0,0,canvas_width,canvas_height},(Color){1,0,1,0.5});
    renderer_primitive_end(&renderer_primitive);


 Camera2D camera;
 camera2d_init(&camera,&context,context.back_buffer_width, context.back_buffer_height);
 camera2d_move(&camera,100,0);

camera2d_zoom(&camera,4);

 struct Matrix44 mb = 
 {
    2,0,0,0
    ,0,2,0,0
    ,0,0,1,0
    ,4,3,0,1

 };

 Matrix44 ma;
 matrix44_invert2(&camera.view,&ma);

 print_matrix44(&camera.view);
 print_matrix44(&ma);


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

        camera2d_update(&camera);

        Vector2f mp = {

context.hid_state.mouse_position_x,context.hid_state.mouse_position_y
};

       // Convert mouse position to normalized device coordinates (NDC)
        Vector2f ndc = {
            (2.0f * mp.x / context.back_buffer_width) - 1.0f,  // X in [-1, 1]
            1.0f - (2.0f * mp.y / context.back_buffer_height)   // Y in [-1, 1], assuming top-left origin for screen space
        };

        // Invert the view-projection matrix to get the world space transformation
        Matrix44 inverse_view_projection;
        matrix44_invert2(&camera.view_projection, &inverse_view_projection);

        // Convert NDC to a 4D vector (homogeneous coordinates)
        Vector4f ndc_h = {ndc.x, ndc.y, 0.0f, 1.0f};

        // Transform NDC to world space using the inverse view-projection matrix
        Vector4f world_pos_h = matrix44_multiply_vector4f(inverse_view_projection, ndc_h);

        // Divide by w to get the final 2D world position (for orthographic projections, w should be 1)
        Vector2f mp_world = {world_pos_h.x / world_pos_h.w, world_pos_h.y / world_pos_h.w};




        glBindFramebuffer(GL_FRAMEBUFFER, rt_layer_1.fbo);

        glViewport(0, 0, canvas_width, canvas_height);
        renderer_primitive_begin(&renderer_primitive,&rt_layer_1.projection,NULL,DELO_TRIANGLE_LIST);
        renderer_primitive_add_rectangle(&renderer_primitive,(Rectangle_f){mp_world.x,mp_world.y,1,1},(Color){1,1,1,1});
        renderer_primitive_end(&renderer_primitive);


    
        frame_begin(&context);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        renderer_sprite.projection = camera.view_projection;
        renderer_sprite_update(&renderer_sprite);
        renderer_sprite_render(&renderer_sprite);


        imgui_begin(&imgui,&glfw_callback_data->key_buffer[0], glfw_callback_data->key_buffer_length);
        
        DatePickerEvent dpe = imgui_datepicker(&imgui,UNIQUE_ID,year,month,day);
        if(dpe.changed)
        {
            year = dpe.year;
            month = dpe.month;
            day = dpe.day;
        }



        imgui_end(&imgui);



        renderer_primitive_begin(&renderer_primitive,&renderer_primitive.projection_default,NULL,DELO_LINE_LIST);
        renderer_primitive_add_line(&renderer_primitive,(Vector2f){0,0},(Vector2f){mp.x,mp.y},(Color){1,1,1,1});
        renderer_primitive_end(&renderer_primitive);
        

        frame_end(&context);
    }
}