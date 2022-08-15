#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "delo2d.h"
#include "game.h"


#define SPRITES_COUNT 76
#define SPRITEBATCH_CAPACITY 200

SpriteBatch sb;

Sprite sprite_particle;
Sprite sprite_sky;
Sprite sprites_scene_above_water[SPRITES_COUNT];  

//sprites used to define a distortion map which warps the scene depending on color (black = no distortion, white = full distortion)
Sprite sprite_distortion_map_mask_shoreline;//draws a black area on the distortion map, excluding most of the scenery above water from any distortions
Sprite sprite_distortion_map_mask_land;//excludes the remaining area above water from the distortion map, by drawing a black silouette of the shoreline on the distortion map
Sprite sprite_distortion_map_fire;//draws a blurred version of the fire animation in white on the distortion map   
Sprite sprite_distortion_map_water;//Sprite used to generate water like patern with the voronoi shader (same size as screen) 

Sprite sprite_rt_scene_above_water;//draws main scenery components
Sprite sprite_rt_scene_water_reflection;//draws a flipped version of the drawn scene
Sprite sprite_rt_scene_land_and_water;//combines the original scene and the reflection

//render targets for different stages of the rendering
RenderTarget render_target_scene_above_water;
RenderTarget render_target_disortion_map;
RenderTarget render_target_scene_with_water_reflection;
RenderTarget render_target_scene_with_distortions;

Texture render_target_textures[4];//textures holding the texture id of all render targets   

void setup_dog(Dog *dog)
{  
    dog->sprite_index = 58;
    dog->min_scale_y = 1;
    dog->max_scale_y = 1.04;
    dog->scale_y = 1;
    dog->flag_y = 0;    
}
void setup_tall_grass(TallGrass *tall_grass)
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
void setup_clouds(Cloud *clouds)
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
void setup_shadows(Shadow *shadows)
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
void setup_particles(Particle *particles)
{
    for (size_t i = 0; i < COUNT_PARTICLES; i++)
    {        
        particles[i].x = (rand() % (850 + 1 - 800)) + 800;
        particles[i].y = (rand() % (700 + 1 - 500)) + 500;

        particles[i].x = particles[i].x_prev;
        particles[i].y = particles[i].y_prev;
        
        particles[i].vx = (float)(((rand() % (8 + 1 - 0)) + 0)-4)/10.0;
        particles[i].vy = -(float)((rand() % (10 + 1 - 0)) + 0)/10.0;
        particles[i].lifetime = (rand() % (1800 + 1 - 0)) + 0;
        particles[i].speed = 0;

    }
}
void setup_trees(Tree *trees)
{
    for (int i = 0; i < COUNT_TREES; i++)
    { 
        trees[i].sprite_index = 8+i;
        trees[i].min_sway = -0.5f;
        trees[i].max_sway = 0.5f;
        trees[i].speed = 0.02;
        trees[i].sway = sin(rand());
        trees[i].flag = 0;
    }

    trees[0].speed = 0.01;
    trees[0].min_sway = -0.5f;
    trees[0].max_sway = 0.5f;

    trees[1].speed = 0.01;
    trees[1].min_sway = -0.5f;
    trees[1].max_sway = 0.5f;
}
int game_setup(Scene *scene,Texture *sprite_sheets,unsigned int screen_width,unsigned int screen_height)
{
    srand(time(NULL));
    delo2d_sprite_batch_create(&sb,SPRITEBATCH_CAPACITY);

    delo2d_render_target_create(&render_target_scene_above_water, screen_width,screen_height);
    delo2d_render_target_create(&render_target_disortion_map, screen_width,screen_height);
    delo2d_render_target_create(&render_target_scene_with_water_reflection, screen_width,screen_height);
    delo2d_render_target_create(&render_target_scene_with_distortions, screen_width,screen_height);


    for (size_t i = 0; i < 4; i++)
    {
        render_target_textures[i].width =  screen_width;
        render_target_textures[i].height =  screen_height;
        render_target_textures[i].bytes_per_pixel = 0;
    }  

    render_target_textures[0].renderer_id = render_target_scene_above_water.fbt;
    render_target_textures[1].renderer_id = render_target_scene_with_water_reflection.fbt;
    render_target_textures[2].renderer_id = render_target_disortion_map.fbt;
    render_target_textures[3].renderer_id = render_target_scene_with_distortions.fbt;


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
    Color color_black;
    delo2d_color_set_f(&color_black,0,0,0,1);

    int ss_width = sprite_sheets[1].width;
    int ss_height = sprite_sheets[1].height;
    float default_scale_x = 1,default_scale_y = 1;
    float default_skew_x = 0,default_skew_y = 0;
    
    delo2d_sprite_define(&sprites_scene_above_water[0], 1000,250,256,256,2110,776,256,256,1,ss_width,ss_height,4,8,0.2,color_white,2,2,default_skew_x,default_skew_y,0,0);//Sun
    delo2d_sprite_define(&sprites_scene_above_water[1], 0,540,3680,918,0,3082,3680,918,1,ss_width,ss_height,1,1,0,palette[2],1.1f,default_scale_y,default_skew_x,default_skew_y,0,0);//Ground
    delo2d_sprite_define(&sprites_scene_above_water[2], 0,242,406,86,1624,2130,406,86,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Cloud
    delo2d_sprite_define(&sprites_scene_above_water[3], 600,212,238,146,1386,2130,238,146,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Cloud
    delo2d_sprite_define(&sprites_scene_above_water[4], 1412,202,344,164,1042,2130,344,164,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Cloud
    delo2d_sprite_define(&sprites_scene_above_water[5], 400,200,2092,452,0,1218,2092,452,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Mountain
    delo2d_sprite_define(&sprites_scene_above_water[6], 602,254,116,338,274,2254,116,338,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Wind turbine
    delo2d_sprite_define(&sprites_scene_above_water[7], 476,244,116,338,274,2254,116,338,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Wind turbine
    delo2d_sprite_define(&sprites_scene_above_water[8], 400,300,2092,308,0,910,2092,308,1,ss_width,ss_height,1,1,0,palette[5],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree line
    delo2d_sprite_define(&sprites_scene_above_water[9], 370,340,1750,460,0,1670,1750,460,1,ss_width,ss_height,1,1,0,palette[4],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree line
    delo2d_sprite_define(&sprites_scene_above_water[10], 442,360,262,490,542,2590,262,490,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree   
    delo2d_sprite_define(&sprites_scene_above_water[11], 390,360,268,490,274,2590,268,490,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree 
    delo2d_sprite_define(&sprites_scene_above_water[12], 310,260,274,692,0,2390,274,692,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[13], 262,248,274,692,0,2390,274,692,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[14], 942,328,256,490,806,2590,256,490,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[15], 816,340,268,490,1062,2590,268,490,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[16], 1046,224,294,698,1330,2384,294,698,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[17], 1139,148,406,866,1624,2216,406,866,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[18], 1188,78,402,1004,2030,2078,402,1004,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[19], 1290,78,438,1004,2432,2078,438,1004,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[20], 1378,78,402,1004,2030,2078,402,1004,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[21], 1476,36,372,1092,2872,1990,372,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[22], 1552,36,438,1092,3244,1990,438,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[23], 128,68,372,1092,2872,1990,372,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[24], 24,68,438,1092,3244,1990,438,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[25], 292,544,652,134,356,776,652,134,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Grass
    delo2d_sprite_define(&sprites_scene_above_water[26], 1000,512,1102,134,1008,776,1102,134,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Grass
    delo2d_sprite_define(&sprites_scene_above_water[27], 886,312,610,604,2098,1472,610,604,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree leafless
    delo2d_sprite_define(&sprites_scene_above_water[28], 480,380,342,460,1750,1670,342,460,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree leafless
    delo2d_sprite_define(&sprites_scene_above_water[29], 512,588,864,204,0,538,864,204,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Ground    
    delo2d_sprite_define(&sprites_scene_above_water[30], 546,404,864,538,0,0,864,538,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//VAN  
    delo2d_sprite_define(&sprites_scene_above_water[31], 598,732,118,78,1248,654,118,78,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock shadow
    delo2d_sprite_define(&sprites_scene_above_water[32], 596,686,118,44,1248,732,118,44,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock shadow
    delo2d_sprite_define(&sprites_scene_above_water[33], 450,740,160,84,1366,692,160,84,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock shadow
    delo2d_sprite_define(&sprites_scene_above_water[34], 362,678,160,84,1526,692,160,84,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock shadow 
    delo2d_sprite_define(&sprites_scene_above_water[35], 1201,638,26,202,390,2130,26,202,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[36], 1206,672,20,132,416,2200,20,132,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[37], 335,586,20,70,436,2262,20,70,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[38], 346,600,18,40,456,2292,18,40 ,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[39], 419,610,26,202,390,2130,26,202,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[40], 429,642,20,132,416,2200,20,132 ,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[41], 627,650,20,70,436,2262,20,70,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[42], 632,591,26,202,390,2130,26,202,1,ss_width,ss_height,1,1,0,palette[3],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[43], 639,624,20,132,416,2200,20,132,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[44], 953,571,26,202,390,2130,26,202,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[45], 1054,749,20,70,436,2262,20,70,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[46], 1057,683,26,202,390,2130,26,202,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[47], 1062,718,20,132,416,2200,20,132,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[48], 1132,581,20,70,436,2262,20,70,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[49], 1137,595,18,40,456,2292,18,40,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[50], 649,794,20,132,416,2200,20,132,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[51], 1348,578,18,40,456,2292,18,40,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[52], 1165,770,20,70,436,2262,20,70,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tall grass
    delo2d_sprite_define(&sprites_scene_above_water[53], 480,722,116,120,0,742,116,120,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock
    delo2d_sprite_define(&sprites_scene_above_water[54], 616,716,118,102,116,742,118,102,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock
    delo2d_sprite_define(&sprites_scene_above_water[55], 502,626,90,48,0,862,90,48,1,ss_width,ss_height,1,1,0,palette[5],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock
    delo2d_sprite_define(&sprites_scene_above_water[56], 618,672,88,66,116,844,88,66,1,ss_width,ss_height,1,1,0,palette[5],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock
    delo2d_sprite_define(&sprites_scene_above_water[57], 380,688,152,66,204,844,152,66,1,ss_width,ss_height,1,1,0,palette[5],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Rock    
    delo2d_sprite_define(&sprites_scene_above_water[58], 912,570,320,236,864,418,320,236,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dog
    delo2d_sprite_define(&sprites_scene_above_water[59], 898,652,386,88,864,654,386,88,1,ss_width,ss_height,1,1,0,palette[2],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dog grass
    delo2d_sprite_define(&sprites_scene_above_water[60], 748,542,404,418,866,0,404,418,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[61], 750,486,178,194,1742,0,178,194,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[62], 764,542,470,296,1270,0,470,296,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[63], 826,596,86,68,1270,298,86,68,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[64], 752,606,178,102,1742,194,178,102,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[65], 910,562,76,92,1356,298,76,92,1,ss_width,ss_height,1,1,0,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Dave
    delo2d_sprite_define(&sprites_scene_above_water[66], 738,772,486,128,1434,504,486,128,1,ss_width,ss_height,1,1,0,palette[1],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Bonfire shadow
    delo2d_sprite_define(&sprites_scene_above_water[67], 792,682,250,236,1182,418,250,236,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Bonfire back
    delo2d_sprite_define(&sprites_scene_above_water[68], 758,338,400,770,0,0,400,770,0,sprite_sheets[0].width,sprite_sheets[0].height,16,56,0.8,palette[7],1.25f,1.25f,default_skew_x,default_skew_y,0,0);//Fire 
    delo2d_sprite_define(&sprites_scene_above_water[69], 738,726,486,208,1434,298,486,208,1,ss_width,ss_height,1,1,0,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Bonfire front 
    delo2d_sprite_define(&sprites_scene_above_water[70], 754,430,400,770,0,0,400,770,0,sprite_sheets[0].width,sprite_sheets[0].height,16,56,0.8,palette[7],0.75f,0.75f,default_skew_x,default_skew_y,0,0);//Fire 
    delo2d_sprite_define(&sprites_scene_above_water[71], 660,900,944,212,0,4000,944,212,1,ss_width,ss_height,7,28,0.3,palette[6],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Fish
    delo2d_sprite_define(&sprites_scene_above_water[72], 660,900,944,212,0,4848,944,212,1,ss_width,ss_height,7,28,0.3,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Fish distortion
    delo2d_sprite_define(&sprites_scene_above_water[73], 660,660,166,172,1917,0,166,172,1,ss_width,ss_height,10,23,0.07,palette[7],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Raven
    delo2d_sprite_define(&sprites_scene_above_water[74], -240,-469,438,1092,3244,1990,438,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,1,0);//Tree
    delo2d_sprite_define(&sprites_scene_above_water[75], 1610,-500,438,1092,3244,1990,438,1092,1,ss_width,ss_height,1,1,0,palette[0],default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);//Tree
   
    delo2d_sprite_define(&sprite_rt_scene_water_reflection, 0,510,screen_width,screen_height,0,0,screen_width,screen_height,0,screen_width,screen_height,1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);
    delo2d_sprite_define(&sprite_rt_scene_above_water, 0,0,screen_width,screen_height,0,0,screen_width,screen_height,0,screen_width,screen_height,1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,1);
    delo2d_sprite_define(&sprite_distortion_map_water, 0,0,screen_width,screen_height,0,0,screen_width,screen_height,1,screen_width,screen_height,1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0); 
    delo2d_sprite_define(&sprite_distortion_map_mask_shoreline, 80,530,3680,918,0,3082,3680,918,1,ss_width,ss_height,1,1,0,palette[2],1.1,default_scale_y,default_skew_x,default_skew_y,0,0);
    delo2d_sprite_define(&sprite_distortion_map_mask_land, 0,0,screen_width,590,1300,3400,10,10,1,ss_width,ss_height,1,1,0,color_black,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);
    delo2d_sprite_define(&sprite_distortion_map_fire,820,270,400,770,0,0,400,770,2,sprite_sheets[2].width,sprite_sheets[2].height,16,56,0.8,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);
    delo2d_sprite_define(&sprite_rt_scene_land_and_water, 0,0,screen_width,screen_height,0,0,screen_width,screen_height,1,screen_width,screen_height,1.1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,1);    
    delo2d_sprite_define(&sprite_particle, 0,0,3,3,1300,3400,5,5,1,ss_width,ss_height,1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);     
    delo2d_sprite_define(&sprite_sky, 0,0,screen_width*2,500,0,0,screen_width,screen_height,1,ss_width,ss_height,1,1,0,color_white,default_scale_x,default_scale_y,default_skew_x,default_skew_y,0,0);
    
    //SPRITES_COUNT-2 Omits the last 2 trees from scaling down
    for (size_t i = 0; i < SPRITES_COUNT-2; i++)
    {
        delo2d_sprite_scale_dest_rect(&sprites_scene_above_water[i],0.5f,0.5f);
    }    
    delo2d_sprite_scale_dest_rect(&sprite_distortion_map_mask_shoreline,0.5f,0.5f);
    delo2d_sprite_scale_dest_rect(&sprite_distortion_map_fire,0.5f,0.5f);

    sprites_scene_above_water[70].time = sprites_scene_above_water[70].duration/2;
    sprites_scene_above_water[70].color.a = 0.3f;

    delo2d_color_set_f(&sprite_distortion_map_mask_shoreline.color,0.5f,0.5f,0.5f,1.0f);
    delo2d_color_set_f(&sprite_distortion_map_mask_land.color,0.5f,0.5f,0.5f,1.0f);

    sprites_scene_above_water[0].offset.x = 2110;
    sprites_scene_above_water[0].offset.y = 776;

    delo2d_color_set_f(&sprite_rt_scene_water_reflection.color,0.6,0.5,0.3,1);


    setup_dog(&scene->dog);
    setup_tall_grass(&scene->tall_grass);
    setup_shadows(&scene->shadows);
    setup_trees(&scene->trees);
    setup_particles(&scene->particles);
    setup_clouds(&scene->clouds);

    scene->fish.active = 0;
    scene->fish.spawn_delay = 2;
    scene->fish.sprite_index = 71;
    
    scene->raven.active = 0;
    scene->raven.spawn_delay = 4;
    scene->raven.sprite_index = 73;
    scene->raven.speed_x = 0;


    sprites_scene_above_water[scene->fish.sprite_index].loop = 0;
    sprites_scene_above_water[scene->fish.sprite_index].offset.x = 0;
    sprites_scene_above_water[scene->fish.sprite_index].offset.y = 4000;

    sprites_scene_above_water[scene->fish.sprite_index+1].loop = 0;
    sprites_scene_above_water[scene->fish.sprite_index+1].offset.x = 0;
    sprites_scene_above_water[scene->fish.sprite_index+1].offset.y = 4848;


    sprites_scene_above_water[scene->raven.sprite_index].loop = 1;
    sprites_scene_above_water[scene->raven.sprite_index].offset.x = 1917;
    sprites_scene_above_water[scene->raven.sprite_index].offset.y = 0;
    sprites_scene_above_water[scene->raven.sprite_index].scale.x = 0.5;
    sprites_scene_above_water[scene->raven.sprite_index].scale.y = 0.5;

    return 0;
}
void game_update_controls(KeyboardInput *ki,KeyboardInput *ki_prev, Projection projection)
{
    if (ki->move_up == GLFW_PRESS)
    {
        //delo2d_camera_move(projection,0,-1.0f,screen_width,screen_height);
        sprite_distortion_map_mask_shoreline.position.y--;
    }
    if (ki->move_dn == GLFW_PRESS)
    {
        //delo2d_camera_move(projection,0,1.0f,screen_width,screen_height);
        sprite_distortion_map_mask_shoreline.position.y++;
    }
    if (ki->move_l == GLFW_PRESS)
    {
        //delo2d_camera_move(projection,-1,0,screen_width,screen_height);
        sprite_distortion_map_mask_shoreline.position.x--;
    }
    if (ki->move_r == GLFW_PRESS)
    {
        sprite_distortion_map_mask_shoreline.position.x++;
    }
} 
void update_dog(Dog *dog,Sprite *sprites)
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
}
void update_tall_grass(TallGrass *tall_grass,Sprite *sprites)
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
    }
}
void update_clouds(Cloud *clouds,Sprite *sprites)
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

        sprites[index].position.x += 0.1f;

        if(sprites[index].position.x > 2000)
        {
            sprites[index].position.x = -100;
        }
    } 
}
void update_shadows(Shadow *shadows,Sprite *sprites)
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
    }
}
void update_particles(float t,float dt, Particle *particles,Sprite *sprite)
{
    for (size_t i = 0; i < COUNT_PARTICLES; i++)
    {        
        particles[i].lifetime += dt;
        if(particles[i].lifetime > 1800)
        {
            particles[i].x = (rand() % (850 + 1 - 800)) + 800;
            particles[i].y = (rand() % (700 + 1 - 500)) + 500;
            particles[i].x_prev = particles[i].x;
            particles[i].y_prev = particles[i].y;
            particles[i].vx = (float)(((rand() % (8 + 1 - 0)) + 0)-4)/100.0;
            particles[i].vy = -(float)((rand() % (10 + 1 - 0)) + 0)/100.0;
            particles[i].lifetime  = 0;
        }
        else
        {
            particles[i].vx-=((float)((rand() % (10 + 1 - 0)) + 0)-5)/50.0;
            particles[i].vy-=((float)((rand() % (10 + 1 - 0)) + 0)-4.5)/50.0;

            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;           
        }        

        float dx = particles[i].x_prev - particles[i].x;
        float dy = particles[i].y_prev - particles[i].y;
        particles[i].speed = sqrt(dx*dx + dy*dy);

        particles[i].x_prev = particles[i].x;
        particles[i].y_prev = particles[i].y;
    }
    
}
void update_trees(Tree *trees,Sprite *sprites)
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


        sprites[trees[i].sprite_index].pivot_point.y = sprites[trees[i].sprite_index].rect_des.height/2;
    }
}
void game_update_logic(float t,float dt,Scene *scene)
{
    update_dog(&scene->dog,sprites_scene_above_water);
    update_tall_grass(&scene->tall_grass,sprites_scene_above_water);
    update_shadows(&scene->shadows,sprites_scene_above_water);
    update_trees(&scene->trees,sprites_scene_above_water);
    update_particles(t,dt,&scene->particles,&sprite_particle);     
    update_clouds(&scene->clouds,sprites_scene_above_water);

    delo2d_sprite_animate(&sprites_scene_above_water[0],dt);
    delo2d_sprite_animate(&sprites_scene_above_water[68],dt);
    delo2d_sprite_animate(&sprites_scene_above_water[70],dt); 

    delo2d_sprite_animate(&sprites_scene_above_water[scene->fish.sprite_index],dt);
    delo2d_sprite_animate(&sprites_scene_above_water[scene->fish.sprite_index+1],dt);
    delo2d_sprite_animate(&sprites_scene_above_water[scene->raven.sprite_index],dt); 


    if(sprites_scene_above_water[scene->fish.sprite_index].time >= sprites_scene_above_water[scene->fish.sprite_index].duration && scene->fish.active == 1)
    {
        scene->fish.active = 0;        
    }
    if(scene->fish.active == 0)
    {
        scene->fish.spawn_delay -= 0.016f;

        if(scene->fish.spawn_delay <= 0)
        {
            scene->fish.active = 1; 
            sprites_scene_above_water[scene->fish.sprite_index].position.x = sprites_scene_above_water[scene->fish.sprite_index+1].position.x = (rand() % (1920 + 1 - 0)) + 0;
            scene->fish.spawn_delay = (rand() % (38 + 1 - 18)) + 18;
            sprites_scene_above_water[scene->fish.sprite_index].time = sprites_scene_above_water[scene->fish.sprite_index+1].time = 0;

            int n = ((rand() % (10 + 1 - 0)) + 0);
            if(n > 5)
            {
                sprites_scene_above_water[scene->fish.sprite_index].flip_horizontally = 0;
                sprites_scene_above_water[scene->fish.sprite_index+1].flip_horizontally = 0;
            }
            else
            {
                sprites_scene_above_water[scene->fish.sprite_index].flip_horizontally = 1;
                sprites_scene_above_water[scene->fish.sprite_index+1].flip_horizontally = 1;
            }


        }
    }

    
    if(scene->raven.active == 1)
    {
        int reset = 0;

        if(scene->raven.speed_x > 0)
        {
            reset = (sprites_scene_above_water[scene->raven.sprite_index].position.x >= 1960);
        } 
        else if(scene->raven.speed_x < 0)
        {
            reset = (sprites_scene_above_water[scene->raven.sprite_index].position.x <= -40);        
        }

        if(reset)
        {
            scene->raven.spawn_delay = (rand() % (38 + 1 - 18)) + 18;
            scene->raven.active = 0;
        }
        sprites_scene_above_water[scene->raven.sprite_index].position.x += scene->raven.speed_x;
        sprites_scene_above_water[scene->raven.sprite_index].position.y += scene->raven.speed_y;
    }
    else
    {
        scene->raven.spawn_delay -= 0.016f;

        if(scene->raven.spawn_delay <= 0)
        {
            scene->raven.active = 1; 
            int n = ((rand() % (10 + 1 - 0)) + 0);
            if(n > 5)
            {
                sprites_scene_above_water[scene->raven.sprite_index].position.x = 1960;
                sprites_scene_above_water[scene->raven.sprite_index].flip_horizontally = 0;
                scene->raven.speed_x = -3;
            }
            else
            {
                sprites_scene_above_water[scene->raven.sprite_index].position.x = -40;
                sprites_scene_above_water[scene->raven.sprite_index].flip_horizontally = 1;
                scene->raven.speed_x = 3;
            }
            scene->raven.speed_y = 0;

            sprites_scene_above_water[scene->raven.sprite_index].position.y = (rand() % (500 + 1 - 50)) + 50;


            sprites_scene_above_water[scene->raven.sprite_index].time = 0;
        }

    }


    
        
    





    delo2d_sprite_animate(&sprite_distortion_map_fire,dt);
}
void game_render(float t,Scene *scene, unsigned int *shaders,Texture *sprite_sheets,Projection projection,int screen_width,unsigned int screen_height)
{       
    Color clear_color;
    delo2d_color_set_f(&clear_color,0,0,0,0);
    //draw sky
    delo2d_render_target_set(render_target_scene_above_water.fbo,0,0,0,0);
    delo2d_sprite_batch_begin(&sb,shaders[5],projection);
    {
        glUseProgram(shaders[5]); 
        float resolution[2] = {screen_width,screen_height};
        glUniform2fv(glGetUniformLocation(shaders[5],"u_resolution"),1,resolution); 
        delo2d_sprite_batch_add(&sb,&sprite_sky, &sprite_sheets[1]);
    }
    delo2d_sprite_batch_end(&sb);

    //draw scene
    delo2d_sprite_batch_begin(&sb,shaders[0],projection);


    for (size_t i = 0; i < 20; i++)
    {
        delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[i], &sprite_sheets[sprites_scene_above_water[i].texture_index]);
    }
    if(scene->raven.active)
    {
        delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[scene->raven.sprite_index], &sprite_sheets[sprites_scene_above_water[scene->raven.sprite_index].texture_index]);
    }
    for (size_t i = 20; i < SPRITES_COUNT-5; i++)
    {
        delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[i], &sprite_sheets[sprites_scene_above_water[i].texture_index]);
    }

    
    

    delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[74], &sprite_sheets[sprites_scene_above_water[74].texture_index]);
    delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[75], &sprite_sheets[sprites_scene_above_water[75].texture_index]);

  

    for (size_t i = 0; i < COUNT_PARTICLES; i++)
    {
        sprite_particle.position.x = scene->particles[i].x;
        sprite_particle.position.y = scene->particles[i].y;
        sprite_particle.skew.x = scene->particles[i].speed*2;
        sprite_particle.color.r = 1.0;
        sprite_particle.color.g = 0.90;
        sprite_particle.color.b = 0.3;
        sprite_particle.color.a = 1-(scene->particles[i].lifetime / 1800.0);
        sprite_particle.scale.x = sprite_particle.scale.y = 1-(scene->particles[i].lifetime / 1800.0);

        if(sprite_particle.scale.x < 0)
        {
            sprite_particle.scale.x = sprite_particle.scale.y = 0;
        }
        sprite_particle.orientation = atan2(scene->particles[i].vy,scene->particles[i].vx);

        sprite_particle.scale.x = 1;
        sprite_particle.scale.y = 1;
        delo2d_sprite_batch_add(&sb,&sprite_particle, &sprite_sheets[1]);
    }
    delo2d_sprite_batch_end(&sb);

    //draw scene water reflection
    delo2d_render_target_set(render_target_scene_with_water_reflection.fbo,0.84,0.40,0.1,1);

    delo2d_sprite_batch_begin(&sb,shaders[2],projection);
        glUseProgram(shaders[2]);          
        {
            int width[1] = {render_target_textures[0].width};
            int height[1] = {render_target_textures[0].height};
            glUniform1iv(glGetUniformLocation(shaders[2],"u_textures_width"),1,width); 
            glUniform1iv(glGetUniformLocation(shaders[2],"u_textures_height"),1,height);        
        }
        delo2d_sprite_batch_add(&sb,&sprite_rt_scene_water_reflection, &render_target_textures[0]);
    delo2d_sprite_batch_end(&sb);

    delo2d_sprite_batch_begin(&sb,shaders[0],projection);        
        delo2d_sprite_batch_add(&sb,&sprite_rt_scene_above_water, &render_target_textures[0]);
    delo2d_sprite_batch_end(&sb);

    //draw distortion pattern
    delo2d_render_target_set(render_target_disortion_map.fbo,0.5,0.5,0.5,1);
    
    delo2d_sprite_batch_begin(&sb,shaders[3],projection);   
    {
        glUseProgram(shaders[3]); 
        glUniform1f(glGetUniformLocation(shaders[3],"u_time"),t);
        float resolution[2] = {screen_width,screen_height};
        glUniform2fv(glGetUniformLocation(shaders[3],"u_resolution"),1,resolution); 
        delo2d_sprite_batch_add(&sb,&sprite_distortion_map_water,&sprite_sheets[0]);//generate voronoi pattern
    }
    delo2d_sprite_batch_end(&sb);

    delo2d_sprite_batch_begin(&sb,shaders[6],projection); 
        delo2d_sprite_batch_add(&sb,&sprite_distortion_map_mask_shoreline,&sprite_sheets[1]);//voronoi mask
        delo2d_sprite_batch_add(&sb,&sprite_distortion_map_mask_land,&sprite_sheets[1]);//voronoi mask
        delo2d_sprite_batch_add(&sb,&sprite_distortion_map_fire,&sprite_sheets[2]);//fire heat distortion
    delo2d_sprite_batch_end(&sb);
    
    delo2d_sprite_batch_begin(&sb,shaders[0],projection); 
    if(scene->fish.active)
    {
        delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[scene->fish.sprite_index+1], &sprite_sheets[sprites_scene_above_water[scene->fish.sprite_index+1].texture_index]);
    }
    delo2d_sprite_batch_end(&sb);
    //draw scene with distortions
    delo2d_render_target_set(render_target_scene_with_distortions.fbo,1,0,0,1);
    
    delo2d_sprite_batch_begin(&sb,shaders[4],projection); 
        glUseProgram(shaders[4]);         
        unsigned int texture_index = -1;
        delo2d_sprite_batch_add_texture(&sb,&render_target_textures[2],&texture_index);
        glUniform1i(glGetUniformLocation(shaders[4],"u_texture_distortion"),texture_index);//set texture slot
        {
            int width[2] = {render_target_textures[2].width,render_target_textures[1].width};
            int height[2] = {render_target_textures[2].height,render_target_textures[1].height};

            glUniform1iv(glGetUniformLocation(shaders[4],"u_textures_width"),2,width); 
            glUniform1iv(glGetUniformLocation(shaders[4],"u_textures_height"),2,height); 
        }
        delo2d_sprite_batch_add(&sb,&sprite_rt_scene_land_and_water,&render_target_textures[1]);
    delo2d_sprite_batch_end(&sb);

    //draw foreground elements
    delo2d_render_target_set(0,0.94,0.80,0.2,1);
        glUseProgram(shaders[1]); 
    delo2d_render_target_draw(&render_target_scene_with_distortions,shaders[1]);

    delo2d_sprite_batch_begin(&sb,shaders[0],projection); 
    if(scene->fish.active)
    {
        delo2d_sprite_batch_add(&sb,&sprites_scene_above_water[scene->fish.sprite_index], &sprite_sheets[sprites_scene_above_water[scene->fish.sprite_index].texture_index]);
    }
    delo2d_sprite_batch_end(&sb);

}