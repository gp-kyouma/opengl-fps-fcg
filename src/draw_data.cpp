#include "draw_data.h"
#include "renderer.h"

// mapa de todos os drawdata, onde cada objeto acessa usando sua key
std::map<std::string, DrawData> g_DrawDataMap;

void SubObjectData::set_diffuse()
{
    if (diffuse.use_flat_color)
        setDiffuseColor(diffuse.flat_color);
    else
        setDiffuseTexture(diffuse.tex_name);
}

void SubObjectData::set_specular()
{
    if (specular.use_flat_color)
        setSpecularColor(specular.flat_color);
    else
        setSpecularTexture(specular.tex_name);
}

void SubObjectData::set_alpha()
{
    if (alpha < 1.0f)
        setAlphaValue(alpha);
}

void SubObjectData::set_alpha_mask()
{
    if (use_alpha_mask)
        setAlphaMask(alpha_mask_color);
}

void SubObjectData::set_tex_repeat(glm::vec3 scale)
{
    float width  = scale.x;
    float length = scale.z;
    float height = scale.y;

    switch (tex_repeat){
        case TEXREPEAT_NONE:
            break;
        case TEXREPEAT_TOP:
            if (width > length)
                setTextureRepeat(width/length,1);
            else
                setTextureRepeat(1,length/width);
            break;
        case TEXREPEAT_LARGER_SIDE:
            if (width > length)
                setTextureRepeat(width,height);
            else
                setTextureRepeat(length,height);
            break;
    }
}

void DrawData::set_drawdata_flags()
{
    setIgnoreLighting(ignore_lighting);
    setUseGouraud(use_gouraud);
    setUseSphericalUV(use_spherical_uv);
    glLineWidth(line_width);
}

void DrawData::reset_drawdata_flags()
{
    setIgnoreLighting(false);
    setUseGouraud(false);
    setUseSphericalUV(false);
    glLineWidth(1.0f);

    resetAlphaValue();
    resetAlphaMask();
    resetTextureRepeat();
}
