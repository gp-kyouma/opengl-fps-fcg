#ifndef FCG_DRAWDATA
#define FCG_DRAWDATA

// Headers abaixo são específicos de C++
#include <map>
#include <string>
#include <vector>

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

//JSON
#include "vec_aux.h"//for vec3 json
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

/*
STRUCTS/CLASSES
*/

enum TexRepeat
{
    TEXREPEAT_NONE = 0,
    TEXREPEAT_TOP,
    TEXREPEAT_LARGER_SIDE,
};

NLOHMANN_JSON_SERIALIZE_ENUM( TexRepeat, {
    {TEXREPEAT_NONE, "none"},
    {TEXREPEAT_TOP, "top"},
    {TEXREPEAT_LARGER_SIDE, "larger_side"},
});

struct ColorData
{
    std::string tex_name;
    bool use_flat_color = false;
    glm::vec3 flat_color;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ColorData, tex_name, use_flat_color, flat_color);

struct SubObjectData
{
    std::string obj_name;

    float alpha = 1.0f;
    ColorData diffuse;
    ColorData specular;

    bool use_alpha_mask = false;
    glm::vec3 alpha_mask_color;

    TexRepeat tex_repeat = TEXREPEAT_NONE;

    void set_diffuse();
    void set_specular();
    void set_alpha();//remember to reset after anything that calls this
    void set_alpha_mask();//remember to reset after anything that calls this
    void set_tex_repeat(glm::vec3 scale);//remember to reset after anything that calls this
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SubObjectData, obj_name,
                                                alpha, diffuse, specular,
                                                use_alpha_mask, alpha_mask_color,
                                                tex_repeat);

struct WeaponAnimData
{
    bool melee = false;
    bool forcedAim = false;
    bool fadeAim = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WeaponAnimData,
                                                melee, forcedAim, fadeAim);

struct ProjectileAnimData
{
    bool spin_over_time = false;
    glm::vec3 spin;

    bool moving_proj = false;
    float wall_embed = 0.125f;
    float length = 1.0f;

    bool expanding_proj = false;
    ColorData initial_color;
    ColorData final_color;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ProjectileAnimData,
                                                spin_over_time, spin,
                                                moving_proj, wall_embed, length,
                                                expanding_proj, initial_color, final_color);

struct DrawData
{
    std::string key;

    //model matrix info

    //static
    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 translation;
    glm::vec3 original_scale;//for resize

    //specific
    WeaponAnimData wpn_anim;
    ProjectileAnimData proj_anim;

    //mesh/color info
    SubObjectData main_obj;
    std::vector<SubObjectData> sub_objs;

    //misc flags
    bool no_draw = false;//early exit
    bool may_use_alpha = false;//helper variable

    bool ignore_lighting = false;
    bool use_gouraud = false;
    bool use_spherical_uv = false;
    float line_width = 1.0f;

    void set_drawdata_flags();
    void reset_drawdata_flags();
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DrawData, key,
                                                scale, rotation, translation, original_scale,
                                                wpn_anim, proj_anim,
                                                main_obj, sub_objs,
                                                no_draw, may_use_alpha,
                                                ignore_lighting, use_gouraud, use_spherical_uv, line_width);

/*
DECLARAÇÃO DE VARIÁVEIS GLOBAIS
*/

// mapa de todos os drawdata, onde cada objeto acessa usando sua key
extern std::map<std::string, DrawData> g_DrawDataMap;

#endif // FCG_DRAWDATA
