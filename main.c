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
#define SPRITES_COUNT 71
#define SPRITEBATCH_CAPACITY 71
#define WINDOW_TITLE "OpenGl Test"
#define COUNT_TREES 17
#define COUNT_SHADOWS 5
#define COUNT_CLOUDS 3
#define COUNT_TALL_GRASS 18


typedef struct Graphics Graphics;
struct Graphics
{
    GLFWwindow *window;
    SpriteBatch sprite_batch;
    Sprite sprites[SPRITES_COUNT];  
    Texture textures[TEXTURES_COUNT]; 
    unsigned int shaders[SHADERS_COUNT];
    float ortho_proj[4][4];
    RenderTarget render_target;
};

typedef struct Tree Tree;
struct Tree
{
    unsigned int sprite_index;
    float sway, min_sway, max_sway,speed;
    unsigned int flag;
};

typedef struct TallGrass TallGrass;
struct TallGrass
{
    unsigned int sprite_index;
    float sway, min_sway, max_sway,speed;
    unsigned int flag;
};

typedef struct Cloud Cloud;
struct Cloud
{
    unsigned int sprite_index;
    float scale_x, min_scale_x, max_scale_x;
    float scale_y, min_scale_y, max_scale_y;
    float speed;
    unsigned int flag_x,flag_y;
};

typedef struct Shadow Shadow;
struct Shadow
{
    unsigned int sprite_index;
    float scale_x, min_scale_x, max_scale_x;
    float scale_y, min_scale_y, max_scale_y;
    float speed;
    unsigned int flag_x,flag_y;
};
typedef struct Dog Dog;
struct Dog
{
    unsigned int sprite_index;
    float scale_y, min_scale_y, max_scale_y;
    unsigned int flag_y;
};

typedef struct Scene Scene;
struct Scene
{
    Tree trees[COUNT_TREES];
    Shadow shadows[COUNT_SHADOWS];
    Cloud clouds[COUNT_CLOUDS];
    TallGrass tall_grass[COUNT_TALL_GRASS];
    Dog dog;
};

const unsigned int screen_width = 1920;
const unsigned int screen_height = 1080;

int init(Graphics *graphics)
{    
    if (!glfwInit()){return -1;}    
    delo2d_render_setup(&graphics->window, screen_width, screen_height,WINDOW_TITLE);  
    if(delo2d_render_initialize() == -1){return -1;} 
    
    delo2d_matrix_orthographic_projection(graphics->ortho_proj,0.0f,(float)screen_width,0.0f,(float)screen_height,1,-1);    
    
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
void setup_dog(Dog *dog,Sprite *sprites)
{  
    dog->sprite_index = 58;
    dog->min_scale_y = 1;
    dog->max_scale_y = 1.04;
    dog->scale_y = 1;
    dog->flag_y = 0;    
}
void setup_tall_grass(TallGrass *tall_grass,Sprite *sprites)
{
    for (int i = 0; i < COUNT_TALL_GRASS; i++)
    { 
        tall_grass[i].sprite_index = 35+i;
        tall_grass[i].min_sway = -0.01f;
        tall_grass[i].max_sway = 0.01f;
        tall_grass[i].speed = 5;
        tall_grass[i].sway = sin(rand());
        tall_grass[i].flag = 0;
    }
}
void setup_clouds(Cloud *clouds,Sprite *sprites)
{
    for (int i = 0; i < COUNT_CLOUDS; i++)
    { 
        clouds[i].sprite_index = 2+i;
        clouds[i].min_scale_x = 0.97f;
        clouds[i].max_scale_x = 1.03f;
        clouds[i].min_scale_y = 0.97f;
        clouds[i].max_scale_y = 1.03f;
        clouds[i].speed = 1;
        clouds[i].scale_x = sin(rand());
        clouds[i].scale_y = sin(rand());
        clouds[i].flag_x = 0;
        clouds[i].flag_y = 0;
    }
}
void setup_shadows(Shadow *shadows,Sprite *sprites)
{
    shadows[0].sprite_index = 31;
    shadows[1].sprite_index = 32;
    shadows[2].sprite_index = 33;
    shadows[3].sprite_index = 34;
    shadows[4].sprite_index = 66;

    for (int i = 0; i < COUNT_SHADOWS; i++)
    { 
        shadows[i].min_scale_x = 0.97f;
        shadows[i].max_scale_x = 1.03f;
        shadows[i].min_scale_y = 0.97f;
        shadows[i].max_scale_y = 1.03f;
        shadows[i].speed = 1;
        shadows[i].scale_x = sin(rand());
        shadows[i].scale_y = sin(rand());
        shadows[i].flag_x = 0;
        shadows[i].flag_y = 0;
    }
}
void setup_trees(Tree *trees,Sprite *sprites)
{
    for (int i = 0; i < COUNT_TREES; i++)
    { 
        trees[i].sprite_index = 8+i;
        trees[i].min_sway = -0.5f;
        trees[i].max_sway = 0.5f;
        trees[i].speed = 5;
        trees[i].sway = sin(rand());
        trees[i].flag = 0;
    }

    trees[0].speed = 3;
    trees[0].min_sway = -0.5f;
    trees[0].max_sway = 0.5f;

    trees[1].speed = 4;
    trees[1].min_sway = -0.5f;
    trees[1].max_sway = 0.5f;
}
int game_setup(Graphics *graphics,Scene *scene)
{
    Color palette[9];
    delo2d_color_set_i(&palette[0],49,17,38,255);
    delo2d_color_set_i(&palette[1],70,21,42,255);
    delo2d_color_set_i(&palette[2],106,32,45,255);
    delo2d_color_set_i(&palette[3],174,68,52,255);
    delo2d_color_set_i(&palette[4],224,109,52,255);
    delo2d_color_set_i(&palette[5],241,130,51,255);
    delo2d_color_set_i(&palette[6],255,168,52,255);
    delo2d_color_set_i(&palette[7],255,213,165,255);
    delo2d_color_set_i(&palette[8],255,255,217,255);

    Color color_white;
    delo2d_color_set_f(&color_white,1,1,1,1);
    
    delo2d_define_sprite(&graphics->sprites[0], 906,138,610,562,2098,910,610,562,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,color_white);
    delo2d_define_sprite(&graphics->sprites[1], 4,540,3554,918,0,3082,3554,918,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Ground
    delo2d_define_sprite(&graphics->sprites[2], 470,242,406,86,1624,2130,406,86,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[3], 754,212,238,146,1386,2130,238,146,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[4], 1012,202,344,164,1042,2130,344,164,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Cloud
    delo2d_define_sprite(&graphics->sprites[5], 400,200,2092,452,0,1218,2092,452,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Mountain
    delo2d_define_sprite(&graphics->sprites[6], 602,254,116,338,274,2254,116,338,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Wind turbine
    delo2d_define_sprite(&graphics->sprites[7], 476,244,116,338,274,2254,116,338,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Wind turbine
    delo2d_define_sprite(&graphics->sprites[8], 400,300,2092,308,0,910,2092,308,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[5]);//Tree line
    delo2d_define_sprite(&graphics->sprites[9], 370,320,1750,460,0,1670,1750,460,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[4]);//Tree line
    delo2d_define_sprite(&graphics->sprites[10], 442,360,262,490,542,2590,262,490,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tree   
    delo2d_define_sprite(&graphics->sprites[11], 390,360,268,490,274,2590,268,490,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Tree 
    delo2d_define_sprite(&graphics->sprites[12], 310,260,274,692,0,2390,274,692,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[13], 262,248,274,692,0,2390,274,692,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[14], 942,328,256,490,806,2590,256,490,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tree
    delo2d_define_sprite(&graphics->sprites[15], 816,340,268,490,1062,2590,268,490,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tree
    delo2d_define_sprite(&graphics->sprites[16], 1046,224,294,698,1330,2384,294,698,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Tree
    delo2d_define_sprite(&graphics->sprites[17], 1138,132,406,866,1624,2216,406,866,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Tree
    delo2d_define_sprite(&graphics->sprites[18], 1188,78,402,1004,2030,2078,402,1004,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[19], 1290,78,438,1004,2432,2078,438,1004,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[20], 1378,78,402,1004,2030,2078,402,1004,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[21], 1476,36,372,1092,2872,1990,372,1092,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[22], 1552,36,438,1092,3244,1990,438,1092,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[23], 128,68,372,1092,2872,1990,372,1092,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[24], 24,70,438,1092,3244,1990,438,1092,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Tree
    delo2d_define_sprite(&graphics->sprites[25], 292,544,652,134,356,776,652,134,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Grass
    delo2d_define_sprite(&graphics->sprites[26], 1000,512,1102,134,1008,776,1102,134,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[0]);//Grass
    delo2d_define_sprite(&graphics->sprites[27], 886,312,610,604,2098,1472,610,604,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tree leafless
    delo2d_define_sprite(&graphics->sprites[28], 480,380,342,460,1750,1670,342,460,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tree leafless
    delo2d_define_sprite(&graphics->sprites[29], 512,588,864,204,0,538,864,204,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Ground    
    delo2d_define_sprite(&graphics->sprites[30], 546,404,864,538,0,0,864,538,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//VAN  
    delo2d_define_sprite(&graphics->sprites[31], 598,732,118,78,1248,654,118,78,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Rock shadow
    delo2d_define_sprite(&graphics->sprites[32], 596,686,118,44,1248,732,118,44,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Rock shadow
    delo2d_define_sprite(&graphics->sprites[33], 450,740,160,84,1366,692,160,84,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Rock shadow
    delo2d_define_sprite(&graphics->sprites[34], 362,678,160,84,1526,692,160,84,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Rock shadow 
    delo2d_define_sprite(&graphics->sprites[35], 1201,638,26,202,390,2130,26,202,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[36], 1206,672,20,132,416,2200,20,132,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[37], 335,586,20,70,436,2262,20,70,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[38], 346,600,18,40,456,2292,18,40 ,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[39], 419,610,26,202,390,2130,26,202,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[40], 429,642,20,132,416,2200,20,132 ,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[41], 627,650,20,70,436,2262,20,70,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[42], 632,591,26,202,390,2130,26,202,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[3]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[43], 639,624,20,132,416,2200,20,132,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[44], 953,571,26,202,390,2130,26,202,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[45], 1054,749,20,70,436,2262,20,70,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[46], 1057,683,26,202,390,2130,26,202,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[47], 1062,718,20,132,416,2200,20,132,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[48], 1132,581,20,70,436,2262,20,70,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[49], 1137,595,18,40,456,2292,18,40,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[50], 649,794,20,132,416,2200,20,132,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[51], 1348,578,18,40,456,2292,18,40,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[52], 1165,770,20,70,436,2262,20,70,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Tall grass
    delo2d_define_sprite(&graphics->sprites[53], 480,722,116,120,0,742,116,120,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Rock
    delo2d_define_sprite(&graphics->sprites[54], 616,716,118,102,116,742,118,102,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Rock
    delo2d_define_sprite(&graphics->sprites[55], 502,626,90,48,0,862,90,48,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[5]);//Rock
    delo2d_define_sprite(&graphics->sprites[56], 618,672,88,66,116,844,88,66,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[5]);//Rock
    delo2d_define_sprite(&graphics->sprites[57], 380,688,152,66,204,844,152,66,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[5]);//Rock    
    delo2d_define_sprite(&graphics->sprites[58], 912,570,320,236,864,418,320,236,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dog
    delo2d_define_sprite(&graphics->sprites[59], 898,652,386,88,864,654,386,88,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[2]);//Dog grass
    delo2d_define_sprite(&graphics->sprites[60], 748,542,404,418,866,0,404,418,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[61], 750,486,178,194,1742,0,178,194,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[62], 764,542,470,296,1270,0,470,296,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[63], 826,596,86,68,1270,298,86,68,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[64], 752,606,178,102,1742,194,178,102,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[65], 910,562,76,92,1356,298,76,92,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[7]);//Dave
    delo2d_define_sprite(&graphics->sprites[66], 738,772,486,128,1434,504,486,128,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[1]);//Bonfire shadow
    delo2d_define_sprite(&graphics->sprites[67], 792,682,250,236,1182,418,250,236,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Bonfire back
    delo2d_define_sprite(&graphics->sprites[68], 758,338,400,770,0,0,400,770,0,graphics->textures[0].width,graphics->textures[0].height,16,56,0.8,palette[7]);//Fire 
    delo2d_define_sprite(&graphics->sprites[69], 738,726,486,208,1434,298,486,208,1,graphics->textures[1].width,graphics->textures[1].height,1,1,0,palette[6]);//Bonfire front 
    delo2d_define_sprite(&graphics->sprites[70], 754,430,400,770,0,0,400,770,0,graphics->textures[0].width,graphics->textures[0].height,16,56,0.8,palette[7]);//Fire 

    for (size_t i = 0; i < SPRITES_COUNT; i++)
    {
        graphics->sprites[i].rect_des.width *= 0.5f;
        graphics->sprites[i].rect_des.height *= 0.5f;

        graphics->sprites[i].position.x = graphics->sprites[i].rect_des.x+graphics->sprites[i].rect_des.width*0.5f;
        graphics->sprites[i].position.y = graphics->sprites[i].rect_des.y+graphics->sprites[i].rect_des.height*0.5f;
    }
    
    graphics->sprites[45].flip_horizontally = 1;
    graphics->sprites[48].flip_horizontally = 1;
    graphics->sprites[41].flip_horizontally = 1;
    graphics->sprites[23].flip_horizontally = 1;
    graphics->sprites[24].flip_horizontally = 1;

    graphics->sprites[68].scale.x = 1.25f;
    graphics->sprites[68].scale.y = 1.25f;
    graphics->sprites[68].color.a = 1.0f;

    graphics->sprites[70].scale.x = 0.75f;
    graphics->sprites[70].scale.y = 0.75f;

    graphics->sprites[70].time = graphics->sprites[70].duration/2;
    graphics->sprites[70].color.a = 0.3f;
    srand(time(NULL));

    setup_dog(&scene->dog,&graphics->sprites);
    setup_tall_grass(&scene->tall_grass,&graphics->sprites);
    setup_shadows(&scene->shadows,&graphics->sprites);
    setup_trees(&scene->trees,&graphics->sprites);

    delo2d_create_sprite_batch(&graphics->sprite_batch,SPRITES_COUNT);

    for (int i = 0; i < SPRITES_COUNT; i++)
    {        
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[i],i);
    }   
    

    return 0;
}

    int n = 48;
void game_render(Graphics *graphics)
{   
    glBindFramebuffer(GL_FRAMEBUFFER, graphics->render_target.fbo);
    glClearColor(0.94,0.80,0.2,1);
    glClear(GL_COLOR_BUFFER_BIT);  

    delo2d_sprite_batch_draw(graphics->render_target.fbo,&graphics->sprite_batch,&graphics->textures,2,graphics->shaders[0],&graphics->ortho_proj);
    
    delo2d_render_target_draw(0,&graphics->render_target,graphics->shaders[1]);

    glfwSwapBuffers(graphics->window);
}
void update_dog(Dog *dog,SpriteBatch *sprite_batch,Sprite *sprites)
{
    dog->scale_y += (dog->flag_y) ? 0.0002f:-0.0005f;
    if(dog->scale_y < dog->min_scale_y)
    {
        dog->scale_y = dog->min_scale_y;
        dog->flag_y = 1;
    }
    else if (dog->scale_y > dog->max_scale_y)
    {
        dog->scale_y = dog->max_scale_y;
        dog->flag_y = 0;
    }

    int index = dog->sprite_index;
    sprites[index].scale.y = dog->scale_y;
    delo2d_sprite_batch_add(sprite_batch,&sprites[index],index);
}
void update_tall_grass(TallGrass *tall_grass,SpriteBatch *sprite_batch,Sprite *sprites)
{
    for (int i = 0; i < COUNT_TALL_GRASS; i++)  
    {          
        tall_grass[i].sway += (tall_grass[i].flag) ? 0.0001f:-0.0001f;

        if(tall_grass[i].sway < tall_grass[i].min_sway)
        {
            tall_grass[i].sway = tall_grass[i].min_sway;
            tall_grass[i].flag = 1;
        }
        else if (tall_grass[i].sway > tall_grass[i].max_sway)
        {
            tall_grass[i].sway = tall_grass[i].max_sway;
            tall_grass[i].flag = 0;
        }
        int index = tall_grass[i].sprite_index;

        sprites[index].pivot_point.y = sprites[index].rect_des.height/2;
        sprites[index].orientation = tall_grass[i].sway*tall_grass[i].speed;

        delo2d_sprite_batch_add(sprite_batch,&sprites[index],index);
    }
}
void update_clouds(Cloud *clouds,SpriteBatch *sprite_batch,Sprite *sprites)
{
    for (int i = 0; i < COUNT_CLOUDS; i++)
    {          
        float rnd_x = (rand()%((100+1)-1) + 1)/200000.0;
        float rnd_y = (rand()%((100+1)-1) + 1)/200000.0;

        clouds[i].scale_x += ((clouds[i].flag_x) ? rnd_x:-rnd_x);
        clouds[i].scale_y += ((clouds[i].flag_y) ? rnd_y:-rnd_y);

        if(clouds[i].scale_x < clouds[i].min_scale_x)
        {
            clouds[i].scale_x = clouds[i].min_scale_x;
            clouds[i].flag_x = 1;
        }
        else if (clouds[i].scale_x > clouds[i].max_scale_x)
        {
            clouds[i].scale_x = clouds[i].max_scale_x;
            clouds[i].flag_x = 0;
        }
        if(clouds[i].scale_y < clouds[i].min_scale_y)
        {
            clouds[i].scale_y = clouds[i].min_scale_y;
            clouds[i].flag_y = 1;
        }
        else if (clouds[i].scale_y > clouds[i].max_scale_y)
        {
            clouds[i].scale_y = clouds[i].max_scale_y;
            clouds[i].flag_y = 0;
        }

        int index = clouds[i].sprite_index;


        sprites[index].scale.x = clouds[i].scale_x*clouds[i].speed;
        sprites[index].scale.y = clouds[i].scale_y*clouds[i].speed;

        sprites[index].position.x += 0.05f;

        delo2d_sprite_batch_add(sprite_batch,&sprites[index],index);

    } 
}
void update_shadows(Shadow *shadows,SpriteBatch *sprite_batch,Sprite *sprites)
{
    for (int i = 0; i < COUNT_SHADOWS; i++)
    {          
        float rnd_x = (rand()%((100+1)-1) + 1)/10000.0;
        float rnd_y = (rand()%((100+1)-1) + 1)/10000.0;

        shadows[i].scale_x += ((shadows[i].flag_x) ? rnd_x:-rnd_x);
        shadows[i].scale_y += ((shadows[i].flag_y) ? rnd_y:-rnd_y);

        if(shadows[i].scale_x < shadows[i].min_scale_x)
        {
            shadows[i].scale_x = shadows[i].min_scale_x;
            shadows[i].flag_x = 1;
        }
        else if (shadows[i].scale_x > shadows[i].max_scale_x)
        {
            shadows[i].scale_x = shadows[i].max_scale_x;
            shadows[i].flag_x = 0;
        }
        if(shadows[i].scale_y < shadows[i].min_scale_y)
        {
            shadows[i].scale_y = shadows[i].min_scale_y;
            shadows[i].flag_y = 1;
        }
        else if (shadows[i].scale_y > shadows[i].max_scale_y)
        {
            shadows[i].scale_y = shadows[i].max_scale_y;
            shadows[i].flag_y = 0;
        }


        sprites[shadows[i].sprite_index].scale.x = shadows[i].scale_x*shadows[i].speed;
        sprites[shadows[i].sprite_index].scale.y = shadows[i].scale_y*shadows[i].speed;
        delo2d_sprite_batch_add(sprite_batch,&sprites[shadows[i].sprite_index],shadows[i].sprite_index);

    }
}
void update_trees(Tree *trees,SpriteBatch *sprite_batch,Sprite *sprites)
{
    for (int i = 0; i < COUNT_TREES; i++)
    {          
        trees[i].sway += (trees[i].flag) ? 0.005f:-0.005f;

        if(trees[i].sway < trees[i].min_sway)
        {
            trees[i].sway = trees[i].min_sway;
            trees[i].flag = 1;
        }
        else if (trees[i].sway > trees[i].max_sway)
        {
            trees[i].sway = trees[i].max_sway;
            trees[i].flag = 0;
        }


        sprites[trees[i].sprite_index].skew.x = trees[i].sway*trees[i].speed;
        delo2d_sprite_batch_add(sprite_batch,&sprites[trees[i].sprite_index],trees[i].sprite_index);

    }
}
void game_update_logic(float t,float dt,Scene *scene,SpriteBatch *sprite_batch,Sprite *sprites)
{
    update_dog(&scene->dog,sprite_batch,sprites);
    update_tall_grass(&scene->tall_grass,sprite_batch,sprites);
    update_shadows(&scene->shadows,sprite_batch,sprites);
    update_trees(&scene->trees,sprite_batch,sprites);     

    delo2d_sprite_animate(&sprites[68],dt,&sprite_batch->vertex_array);
    delo2d_sprite_animate(&sprites[70],dt,&sprite_batch->vertex_array);
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
        if(graphics->sprites[i].updated_tex_coords)
        {
            delo2d_sprite_batch_update_tex_coords(&graphics->sprite_batch.vertex_array,&graphics->sprite_batch,&graphics->sprites[i],i);
            graphics->sprites[i].updated_tex_coords = 0;
        }
    }
}
void game_update_controls(KeyboardInput *ki,KeyboardInput *ki_prev, float *ortho_proj,Graphics *graphics)
{
    if (ki->move_up == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,0,-1.0f,screen_width,screen_height);
        graphics->sprites[n].position.y -=2;
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[n],n);
    }
    if (ki->move_dn == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,0,1.0f,screen_width,screen_height);
        graphics->sprites[n].position.y +=2;
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[n],n);
    }
    if (ki->move_l == GLFW_PRESS)
    {
        //delo2d_camera_move(ortho_proj,-1,0,screen_width,screen_height);
        graphics->sprites[n].position.x -=2;
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[n],n);
    }
    if (ki->move_r == GLFW_PRESS)
    {
        graphics->sprites[n].position.x +=2;        
        delo2d_sprite_batch_add(&graphics->sprite_batch,&graphics->sprites[n],n);
    }
    Quad quad;
    delo2d_get_quad(&quad,&graphics->sprite_batch.vertex_array,n);
    Vector2f c;
    delo2d_quad_get_center(&quad,&c);
    //printf("%.0f,%.0f\n",*quad.v0.x,*quad.v0.y);
} 

int main(void)
{  
    //variables
    Graphics graphics;
    KeyboardInput ki;
    KeyboardInput ki_prev;

    Scene scene;

    float dt = 0;
    float t = 0;
    //init graphics
    if(init(&graphics) == -1)return -1;
    //init input
    delo2d_input_init(&ki,&ki_prev);
    //load game resources
    load(&graphics.textures,&graphics.shaders);
    //setup game
    game_setup(&graphics,&scene);

    clock_t initial_time = clock();      
    
    int n = 0;

    glViewport(0, 0, screen_width,screen_height);

    delo2d_render_target_create(&graphics.render_target, screen_width,screen_height);

    while (!glfwWindowShouldClose(graphics.window))
    {
        t = (float)clock()/CLOCKS_PER_SEC;

        game_update_controls(&ki,&ki_prev,&graphics.ortho_proj,&graphics);   

        game_update_logic(t,dt,&scene,&graphics.sprite_batch,&graphics.sprites);

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