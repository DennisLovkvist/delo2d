#include "delo2d.h"

#define COUNT_TREES 17
#define COUNT_SHADOWS 5
#define COUNT_CLOUDS 3
#define COUNT_TALL_GRASS 18
#define COUNT_PARTICLES 40

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

typedef struct Fish Fish;
struct Fish
{
    unsigned int sprite_index, active;
    float x, y, spawn_delay;
};
typedef struct Raven Raven;
struct Raven
{
    unsigned int sprite_index, active;
    float x, y, spawn_delay,speed_x,speed_y;
};

typedef struct Particle Particle;
struct Particle
{
    float x,y,x_prev,y_prev,vx,vy,lifetime,speed;
};

typedef struct Scene Scene;
struct Scene
{
    Particle particles[COUNT_PARTICLES];
    Tree trees[COUNT_TREES];
    Shadow shadows[COUNT_SHADOWS];
    Cloud clouds[COUNT_CLOUDS];
    TallGrass tall_grass[COUNT_TALL_GRASS];
    Dog dog;
    Fish fish;
    Raven raven;
};

void setup_dog(Dog *dog);
void setup_tall_grass(TallGrass *tall_grass);
void setup_clouds(Cloud *clouds);
void setup_shadows(Shadow *shadows);
void setup_particles(Particle *particles);
void setup_trees(Tree *trees);
void update_dog(Dog *dog,Sprite *sprites);
void update_tall_grass(TallGrass *tall_grass,Sprite *sprites);
void update_clouds(Cloud *clouds,Sprite *sprites);
void update_shadows(Shadow *shadows,Sprite *sprites);
void update_particles(float t,float dt, Particle *particles,Sprite *sprite);
void update_trees(Tree *trees,Sprite *sprites);

int game_setup(Scene *scene,Texture *sprite_sheets,unsigned int screen_width,unsigned int screen_height);
void game_update_controls(KeyboardInput *ki,KeyboardInput *ki_prev, Projection projection);
void game_update_logic(float t,float dt,Scene *scene);
void game_render(float t,Scene *scene, unsigned int *shaders,Texture *sprite_sheets,Projection projection,int screen_width,unsigned int screen_height);

