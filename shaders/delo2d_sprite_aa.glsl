#VERT_BEGIN
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in float tex_index;
layout(location = 3) in vec4 color;

out vec2 v_tex_coord;
out float v_tex_index;
out vec4 v_color;

uniform mat4 u_mvp;

void main()
{      
    gl_Position = position * u_mvp;
    v_tex_coord = tex_coord;
    v_tex_index = tex_index;
    v_color = color;
}

#VERT_END
#FRAG_BEGIN
#version 330 core

uniform sampler2D u_textures[3];
layout(location = 0) out vec4 color;
in vec2 v_tex_coord;
in float v_tex_index;
in vec4 v_color;

#define FXAA_REDUCE_MUL     (1.0/16.0)
#define FXAA_REDUCE_MIN     (1.0/256.0)
#define FXAA_SPAN_MAX       8.0
#define FXAA_SPAN_THRESHOLD 0.03
#define FXAA_CORNER_RCPDIR  0.125
#define FXAA_EDGE_RCPDIR    0.25

struct Luma 
{
    float nw;
    float ne;
    float sw;
    float se;
    float w;
    float e;
    float s;
    float n;
    float m;
};

struct RGB 
{
    vec3 nw;
    vec3 ne;
    vec3 sw;
    vec3 se;
    vec3 w;
    vec3 e;
    vec3 s;
    vec3 n;
    vec3 m;
};

void main() 
{
    vec2 texel_size = 1.0 / textureSize(u_textures[0], 0);

    vec4 tex_color = texture2D(u_textures[0], v_tex_coord); 
    
    RGB rgb_data;
    rgb_data.m  = tex_color.rgb;
    rgb_data.nw = texture2D(u_textures[0], v_tex_coord + vec2(-1.0,-1.0) * texel_size).rgb;
    rgb_data.ne = texture2D(u_textures[0], v_tex_coord + vec2( 1.0,-1.0) * texel_size).rgb;
    rgb_data.sw = texture2D(u_textures[0], v_tex_coord + vec2(-1.0, 1.0) * texel_size).rgb;
    rgb_data.se = texture2D(u_textures[0], v_tex_coord + vec2( 1.0, 1.0) * texel_size).rgb;
    rgb_data.w  = texture2D(u_textures[0], v_tex_coord + vec2(-1.0, 0.0) * texel_size).rgb;
    rgb_data.e  = texture2D(u_textures[0], v_tex_coord + vec2( 1.0, 0.0) * texel_size).rgb;
    rgb_data.s  = texture2D(u_textures[0], v_tex_coord + vec2( 0.0, 1.0) * texel_size).rgb;
    rgb_data.n  = texture2D(u_textures[0], v_tex_coord + vec2( 0.0,-1.0) * texel_size).rgb;

    Luma luma_data;
    vec3 luma = vec3(0.299, 0.587, 0.114);
    luma_data.nw = dot(rgb_data.nw, luma);
    luma_data.ne = dot(rgb_data.ne, luma);
    luma_data.sw = dot(rgb_data.sw, luma);
    luma_data.se = dot(rgb_data.se, luma);
    luma_data.w  = dot(rgb_data.w , luma);
    luma_data.e  = dot(rgb_data.e , luma);
    luma_data.s  = dot(rgb_data.s , luma);
    luma_data.n  = dot(rgb_data.n , luma);
    luma_data.m  = dot(rgb_data.m,  luma);

    float rangeMin = min(luma_data.m, min(min(min(luma_data.nw, luma_data.ne), luma_data.sw), luma_data.se));
    float rangeMax = max(luma_data.m, max(max(max(luma_data.nw, luma_data.ne), luma_data.sw), luma_data.se));

    vec2 dir;
    dir.x = -((luma_data.nw + luma_data.ne) - (luma_data.sw + luma_data.se));
    dir.y =  ((luma_data.nw + luma_data.sw) - (luma_data.ne + luma_data.se));

    float dir_reduce = max((luma_data.nw + luma_data.ne + luma_data.sw + luma_data.se) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);

    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcp_dir_min)) * texel_size;

    vec3 rgb_a = 0.5 * (
        texture2D(u_textures[0], v_tex_coord + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture2D(u_textures[0], v_tex_coord + dir * (2.0 / 3.0 - 0.5)).rgb);

    vec3 rgb_b = rgb_a * 0.5 + 0.25 * (
        texture2D(u_textures[0], v_tex_coord + dir * -0.5).rgb +
        texture2D(u_textures[0], v_tex_coord + dir * 0.5).rgb);

    float luma_end = dot(rgb_b, luma);
    float luma_local_contrast = abs(luma_data.m - luma_end);

    bool span1 = luma_local_contrast > FXAA_SPAN_THRESHOLD;

    dir *= (span1) ? FXAA_CORNER_RCPDIR : FXAA_EDGE_RCPDIR;

    vec3 rgb_c = 0.5 * (
        texture2D(u_textures[0], v_tex_coord + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture2D(u_textures[0], v_tex_coord + dir * (2.0 / 3.0 - 0.5)).rgb);

    vec3 rgb_d = rgb_c * 0.5 + 0.25 * (
        texture2D(u_textures[0], v_tex_coord + dir * -0.5).rgb +
        texture2D(u_textures[0], v_tex_coord + dir * 0.5).rgb);

    vec4 final_color = mix(
        mix(vec4(rgb_data.m, tex_color.a), vec4(rgb_a, tex_color.a), span1), 
        mix(vec4(rgb_b, tex_color.a), vec4(rgb_c, tex_color.a), span1), luma_local_contrast);

    final_color = mix(final_color, vec4(rgb_d, tex_color.a), luma_local_contrast * luma_local_contrast);

    color = final_color*v_color;
}

#FRAG_END

