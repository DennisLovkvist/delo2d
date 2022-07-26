//glfw crosd-platform windows
//glew fetches function pointers from graphics card
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libs/delo2d.h"
#include <time.h>

#define GLEW_STATIC 1
#define SHADERS_COUNT 2
#define TEXTURES_COUNT 2
#define SPRITES_COUNT 49
#define SPRITEBATCH_CAPACITY 49
#define WINDOW_TITLE "OpenGl Test"


typedef struct Graphics Graphics;
struct Graphics
{
    GLFWwindow *window;
    VertexArray vertex_array;
    VertexArray vertex_array2;
    SpriteBatch sprite_batch;
    SpriteBatch sprite_batch2;
    Sprite sprites[SPRITES_COUNT];  
    Sprite sprites2[1];  
    Texture textures[TEXTURES_COUNT]; 
    unsigned int shaders[SHADERS_COUNT];
    float ortho_proj[4][4];
    RenderTarget render_target;
};

const unsigned int screen_width = 1920;
const unsigned int screen_height = 1080;

int init(Graphics *graphics)
{    
    if (!glfwInit()){return -1;}    
    delo2d_render_setup(&graphics->window, screen_width, screen_height,WINDOW_TITLE);  
    if(delo2d_render_initialize() == -1){return -1;} 
    
    delo2d_matrix_orthographic_projection(graphics->ortho_proj,0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);    
    
    delo2d_vertex_array_create(&graphics->vertex_array,DELO_QUAD_LIST,SPRITES_COUNT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    return 0;
}
int load(Texture *textures,unsigned int *shaders)
{
    shaders[0] = delo2d_shader_from_file("default_shader.glsl");  
    shaders[1] = delo2d_shader_from_file("shader_render_target.glsl");    
    delo2d_load_texture(&textures[0],"textures/sprite_sheet_fire.png");  
    delo2d_load_texture(&textures[1],"textures/sprite_sheet_scene.png"); 
    return 0;
}
int game_setup(Graphics *graphics)
{
    float SD = 0.5f;
    float SS = 1.0f; 

    Color palette_orange[9];
    delo2d_color_set_i(&palette_orange[0],49,17,38,255);
    delo2d_color_set_i(&palette_orange[1],70,21,42,255);
    delo2d_color_set_i(&palette_orange[2],106,32,45,255);
    delo2d_color_set_i(&palette_orange[3],174,68,52,255);
    delo2d_color_set_i(&palette_orange[4],224,109,52,255);
    delo2d_color_set_i(&palette_orange[5],241,130,51,255);
    delo2d_color_set_i(&palette_orange[6],255,168,52,255);
    delo2d_color_set_i(&palette_orange[7],255,213,165,255);
    delo2d_color_set_i(&palette_orange[8],255,255,217,255);

    Color palette_green[7];
    delo2d_color_set_i(&palette_green[0],130, 149, 75,255);
    delo2d_color_set_i(&palette_green[1],158,169,70,255);
    delo2d_color_set_i(&palette_green[2],186, 189, 66,255);
    delo2d_color_set_i(&palette_green[3],213,200,68,255);
    delo2d_color_set_i(&palette_green[4],239, 211, 69,255);
    delo2d_color_set_i(&palette_green[5],247,225,100,255);
    delo2d_color_set_i(&palette_green[6],255, 239, 130,255);


    Color color_white;
    delo2d_color_set_f(&color_white,1,1,1,1);
    
    delo2d_define_sprite(&graphics->sprites[0], 906,138,610*SD,562*SD,2098*SS,910*SS,610*SS,562*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,color_white);
    delo2d_define_sprite(&graphics->sprites[1], 136,532,2872*SD,918*SD,0*SS,3082*SS,2872*SS,918*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[2]);//Ground
    delo2d_define_sprite(&graphics->sprites[2], 470,242,406*SD,86*SD,1624*SS,2130*SS,406*SS,86*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[3], 754,212,238*SD,146*SD,1386*SS,2130*SS,238*SS,146*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[4], 1012,202,344*SD,164*SD,1042*SS,2130*SS,344*SS,164*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[5], 400,200,2092*SD,452*SD,0*SS,1218*SS,2092*SS,452*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Mountain
    delo2d_define_sprite(&graphics->sprites[6], 602,254,116*SD,338*SD,274*SS,2254*SS,116*SS,338*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Wind turbine
    delo2d_define_sprite(&graphics->sprites[7], 476,244,116*SD,338*SD,274*SS,2254*SS,116*SS,338*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Wind turbine
    delo2d_define_sprite(&graphics->sprites[8], 400,300,2092*SD,308*SD,0*SS,910*SS,2092*SS,308*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[5]);//Tree line
    delo2d_define_sprite(&graphics->sprites[9], 370,320,1750*SD,460*SD,0*SS,1670*SS,1750*SS,460*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[4]);//Tree line
    delo2d_define_sprite(&graphics->sprites[10], 442,360,262*SD,490*SD,542*SS,2590*SS,262*SS,490*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[2]);//Tree   
    delo2d_define_sprite(&graphics->sprites[11], 390,360,268*SD,490*SD,274*SS,2590*SS,268*SS,490*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[1]);//Tree 
    delo2d_define_sprite(&graphics->sprites[12], 310,260,274*SD,692*SD,0*SS,2390*SS,274*SS,692*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[13], 262,248,274*SD,692*SD,0*SS,2390*SS,274*SS,692*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[14], 942,328,256*SD,490*SD,806*SS,2590*SS,256*SS,490*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[2]);//Tree
    delo2d_define_sprite(&graphics->sprites[15], 816,340,268*SD,490*SD,1062*SS,2590*SS,268*SS,490*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[2]);//Tree
    delo2d_define_sprite(&graphics->sprites[16], 1046,224,294*SD,698*SD,1330*SS,2384*SS,294*SS,698*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[1]);//Tree
    delo2d_define_sprite(&graphics->sprites[17], 1138,132,406*SD,866*SD,1624*SS,2216*SS,406*SS,866*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[1]);//Tree
    delo2d_define_sprite(&graphics->sprites[18], 1188,78,402*SD,1004*SD,2030*SS,2078*SS,402*SS,1004*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[19], 1290,78,438*SD,1004*SD,2432*SS,2078*SS,438*SS,1004*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[20], 1378,78,402*SD,1004*SD,2030*SS,2078*SS,402*SS,1004*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree

    delo2d_define_sprite(&graphics->sprites[21], 1476,36,372*SD,1092*SD,2872*SS,1990*SS,372*SS,1092*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[22], 1552,36,438*SD,1092*SD,3244*SS,1990*SS,438*SS,1092*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Tree

    delo2d_define_sprite(&graphics->sprites[23], 292,544,652*SD,134*SD,356*SS,776*SS,652*SS,134*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Grass
    delo2d_define_sprite(&graphics->sprites[24], 1000,512,1102*SD,134*SD,1008*SS,776*SS,1102*SS,134*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[0]);//Grass
    delo2d_define_sprite(&graphics->sprites[25], 886,312,610*SD,604*SD,2098*SS,1472*SS,610*SS,604*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Tree leafless
    delo2d_define_sprite(&graphics->sprites[26], 480,380,342*SD,460*SD,1750*SS,1670*SS,342*SS,460*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[27], 484,482,158*SD,318*SD,642*SS,2130*SS,158*SS,318*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[28], 520,588,864*SD,204*SD,0*SS,538*SS,864*SS,204*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[3]);//Ground
    delo2d_define_sprite(&graphics->sprites[29], 546,404,864*SD,538*SD,0*SS,0*SS,864*SS,538*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//VAN
    delo2d_define_sprite(&graphics->sprites[30], 560,494,252*SD,402*SD,390*SS,2130*SS,252*SS,402*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[31], 972,526,242*SD,318*SD,800*SS,2130*SS,242*SS,318*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[32], 342,564,328*SD,296*SD,1000*SS,2294*SS,328*SS,296*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[33], 480,722,116*SD,120*SD,0*SS,742*SS,116*SS,120*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Rock
    delo2d_define_sprite(&graphics->sprites[34], 616,716,118*SD,102*SD,116*SS,742*SS,118*SS,102*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Rock
    delo2d_define_sprite(&graphics->sprites[35], 502,626,90*SD,48*SD,0*SS,862*SS,90*SS,48*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[5]);//Rock
    delo2d_define_sprite(&graphics->sprites[36], 618,672,88*SD,66*SD,116*SS,844*SS,88*SS,66*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[5]);//Rock
    delo2d_define_sprite(&graphics->sprites[37], 380,688,152*SD,66*SD,204*SS,844*SS,152*SS,66*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[5]);//Rock
    delo2d_define_sprite(&graphics->sprites[38], 912,570,320*SD,236*SD,864*SS,418*SS,320*SS,236*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dog
    delo2d_define_sprite(&graphics->sprites[39], 898,652,386*SD,78*SD,864*SS,654*SS,386*SS,78*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[2]);//Dog grass
    delo2d_define_sprite(&graphics->sprites[40], 748,542,404*SD,418*SD,866*SS,0*SS,404*SS,418*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[41], 750,486,178*SD,194*SD,1742*SS,0*SS,178*SS,194*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[42], 764,542,470*SD,296*SD,1270,0*SS,470*SS,296*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[43], 826,596,86*SD,68*SD,1270*SS,298*SS,86*SS,68*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[44], 752,606,178*SD,102*SD,1742*SS,194*SS,178*SS,102*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[45], 910,562,76*SD,92*SD,1356*SS,298*SS,76*SS,92*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[46], 792,682,250*SD,236*SD,1182*SS,418*SS,250*SS,236*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Bonfire back
    delo2d_define_sprite(&graphics->sprites[47], 758,388,400*SD,770*SD,0*SS,0*SS,400*SS,770*SS,0,graphics->textures[0].width,graphics->textures[0].height,16,56,0.7,palette_orange[7]);//Fire 
    delo2d_define_sprite(&graphics->sprites[48], 738,726,486*SD,208*SD,1434*SS,298*SS,486*SS,208*SS,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette_orange[6]);//Bonfire front 

 
 




    delo2d_create_sprite_batch(&graphics->sprite_batch,SPRITES_COUNT);

    for (int i = 0; i < SPRITES_COUNT; i++)
    {        
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[i],i);
    }   

    delo2d_sprite_batch_to_vertex_array(&graphics->sprite_batch,&graphics->vertex_array); 

    return 0;
}

    int n = 22;
void game_render(Graphics *graphics)
{   
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->render_target.fbo);
    glClearColor(0.94,0.80,0.2,1);
    glClear(GL_COLOR_BUFFER_BIT);  

    delo2d_vertex_array_draw(graphics->render_target.fbo,&graphics->vertex_array,graphics->shaders[0],&graphics->textures,2,&graphics->ortho_proj);

    delo2d_render_target_draw(0,&graphics->render_target,graphics->shaders[1]);

    glfwSwapBuffers(graphics->window);
}
void game_update_logic(float dt,VertexArray *vertex_array,Sprite *sprites)
{
    //delo2d_sprite_rotate(&sprites[1],0.01f,vertex_array);
    //delo2d_sprite_translate(&sprites[0],1,0,vertex_array);
    delo2d_sprite_animate(&sprites[47],dt,vertex_array);

    //delo2d_sprite_animate(&sprites[26],dt,vertex_array);
}
void unload(Texture *textures,unsigned int *shaders)
{ 
    for (int i = 0; i < TEXTURES_COUNT; i++)
    {
        //not implemented
        //delo2d_unload_texture(&textures[i]);  
    }
    for (int i = 0; i < SHADERS_COUNT; i++)
    {
        glDeleteProgram(shaders[i]);
    }        
    glfwTerminate();
}
void game_update_render_state(Graphics *graphics)
{
    for (int i = 0; i < SPRITES_COUNT; i++)
    {
        if(graphics->sprites[i].tex_coords_updated)
        {
            delo2d_sprite_batch_update_tex_coords(&graphics->vertex_array,&graphics->sprite_batch,&graphics->sprites[i],i);
            graphics->sprites[i].tex_coords_updated = 0;
        }
    }
}
void game_update_controls(KeyboardInput *ki,KeyboardInput *ki_prev, float *ortho_proj,Graphics *graphics)
{
    if (ki->move_up == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,0,-1.0f,screen_width,screen_height);
        delo2d_sprite_translate(&graphics->sprites[n],0,-2,&graphics->vertex_array);
        printf("%.0f,%.0f\n",graphics->sprites[n].position.x,graphics->sprites[n].position.y);
    }
    if (ki->move_dn == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,0,1.0f,screen_width,screen_height);

        delo2d_sprite_translate(&graphics->sprites[n],0,2,&graphics->vertex_array);
        printf("%.0f,%.0f\n",graphics->sprites[n].position.x,graphics->sprites[n].position.y);
    }
    if (ki->move_l == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,-1,0,screen_width,screen_height);

        delo2d_sprite_translate(&graphics->sprites[n],-2,0,&graphics->vertex_array);
        printf("%.0f,%.0f\n",graphics->sprites[n].position.x,graphics->sprites[n].position.y);
    }
    if (ki->move_r == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,1,0,screen_width,screen_height);

        delo2d_sprite_translate(&graphics->sprites[n],2,0,&graphics->vertex_array);        
        printf("%.0f,%.0f\n",graphics->sprites[n].position.x,graphics->sprites[n].position.y);
    }
} 

int main(void)
{  
    //variables
    Graphics graphics;
    KeyboardInput ki;
    KeyboardInput ki_prev;

    float dt = 0;
    float t = 0;
    //init graphics
    if(init(&graphics) == -1)return -1;
    //init input
    delo2d_input_init(&ki,&ki_prev);
    //load game resources
    load(&graphics.textures,&graphics.shaders);
    //setup game
    game_setup(&graphics);

    clock_t initial_time = clock();      
    
    int n = 0;

    glViewport(0, 0, screen_width,screen_height);

    delo2d_render_target_create(&graphics.render_target, screen_width,screen_height);

    while (!glfwWindowShouldClose(graphics.window))
    {
        t = (float)clock()/CLOCKS_PER_SEC;

        game_update_controls(&ki,&ki_prev,&graphics.ortho_proj,&graphics);   

        game_update_logic(dt,&graphics.vertex_array,&graphics.sprites);

        game_update_render_state(&graphics);

        game_render(&graphics);

        dt = (float)clock()/CLOCKS_PER_SEC - t;

        glfwPollEvents();

        delo2d_input_update(graphics.window,&ki,&ki_prev);
    }

    //unload and exit
    unload(&graphics.textures,&graphics.shaders);

    return 0;
}