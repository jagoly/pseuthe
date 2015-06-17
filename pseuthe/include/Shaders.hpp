/*********************************************************************
Matt Marchant 2015
http://trederia.blogspot.com

pseuthe Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

//source for shaders

#ifndef SHADERS_HPP_
#define SHADERS_HPP_

#include <string>

namespace Shader
{
    enum class Type
    {
        PostBrightnessExtract,
        PostDownSample,
        PostGaussianBlur,
        PostAdditiveBlend,
        PostChromeAb,
        LightRay
    };

    namespace FullPass
    {
        static const std::string vertex =
            "#version 120\n" \
            "void main()\n" \
            "{\n" \
            "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
            "    gl_TexCoord[0] = gl_MultiTexCoord0;\n" \
            "}";
    }

    namespace PostBrightness
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "const float threshold = 0.35;\n" \
            "const float factor = 4.0;\n" \

            "void main()\n" \
            "{\n" \
            "    vec4 sourceColour = texture2D(u_sourceTexture, gl_TexCoord[0].xy);\n" \
            "    float luminance = sourceColour.r * 0.2126 + sourceColour.g * 0.7152 + sourceColour.b * 0.0722;\n" \
            "    sourceColour *= clamp(luminance - threshold, 0.0 , 1.0) * factor;\n" \
            "    gl_FragColor = sourceColour;"
            "}\n";
    }

    //buns
    namespace PostDownSample
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform vec2 u_sourceSize;\n" \

            "void main()\n" \
            "{\n" \
            "    vec2 pixelSize = 1.0 / u_sourceSize;\n" \
            "    vec2 texCoords = gl_TexCoord[0].xy;\n" \
            "    vec4 colour = texture2D(u_sourceTexture, texCoords);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, 0.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, 0.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(0.0, 1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(0.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, 1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(1.0, -1.0) * pixelSize);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + vec2(-1.0, 1.0) * pixelSize);\n" \
            "    gl_FragColor = colour / 9.0;\n" \
            "}";
    }

    namespace PostGaussianBlur
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform vec2 u_offset;\n" \

            "void main()\n" \
            "{\n " \
            "    vec2 texCoords = gl_TexCoord[0].xy;\n" \
            "    vec4 colour = vec4(0.0);\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 4.0 * u_offset) * 0.0162162162;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 3.0 * u_offset) * 0.0540540541;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - 2.0 * u_offset) * 0.1216216216;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords - u_offset) * 0.1945945946;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords) * 0.2270270270;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + u_offset) * 0.1945945946;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 2.0 * u_offset) * 0.1216216216;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 3.0 * u_offset) * 0.0540540541;\n" \
            "    colour += texture2D(u_sourceTexture, texCoords + 4.0 * u_offset) * 0.0162162162;\n" \
            "    gl_FragColor = colour;\n" \
            "}";
    }

    namespace PostAdditiveBlend
    {
        static const std::string fragment =
            "#version 120\n" \
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform sampler2D u_bloomTexture;\n" \

            "void main()\n" \
            "{\n" \
            "    gl_FragColor = texture2D(u_sourceTexture, gl_TexCoord[0].xy) + texture2D(u_bloomTexture, gl_TexCoord[0].xy);\n" \
            "}";
    }

    namespace PostChromeAb
    {
        static const std::string fragment =
            "#version 120\n" \
            /*"#define BLUR\n" \*/
            "uniform sampler2D u_sourceTexture;\n" \
            "uniform float u_time;\n" \

            "const float lineCount = 10000.0;\n" \
            "const float noiseStrength = 0.7;\n" \

            "#if defined(BLUR)\n" \
            "const float maxOffset = 1.0 / 35.0;\n" \
            "#define KERNEL_SIZE 9\n" \
            "const vec2 kernel[KERNEL_SIZE] = vec2[KERNEL_SIZE]\n" \
            "(\n" \
            "    vec2(0.0000000000000000000, 0.04416589065853191),\n" \
            "    vec2(0.10497808951021347),\n" \
            "    vec2(0.0922903086524308425, 0.04416589065853191),\n" \
            "    vec2(0.0112445223775533675, 0.10497808951021347)," \
            "    vec2(0.40342407932501833),\n" \
            "    vec2(0.1987116566428735725, 0.10497808951021347),\n" \
            "    vec2(0.0000000000000000000, 0.04416589065853191),\n" \
            "    vec2(0.10497808951021347),\n" \
            "    vec2(0.0922903086524308425, 0.04416589065853191)\n" \
            ");\n" \
            "#else\n" \
            "const float maxOffset = 1.0 / 450.0;\n" \
            "#endif\n" \


            "void main()\n" \
            "{\n" \
            "    vec2 texCoord = gl_TexCoord[0].xy;\n" \
            "    vec2 offset = vec2((maxOffset / 2.0) - (texCoord.x * maxOffset), (maxOffset / 2.0) - (texCoord.y * maxOffset));\n"
            "    vec3 colour = vec3(0.0);\n" \
            "#if defined(BLUR)\n" \
            "    for(int i = 0; i < KERNEL_SIZE; ++i)\n" \
            "    {\n" \
            "        colour.r += texture2D(u_sourceTexture, texCoord + (offset * kernel[i])).r;\n" \
            "        colour.g += texture2D(u_sourceTexture, texCoord).g;\n" \
            "        colour.b += texture2D(u_sourceTexture, texCoord - (offset * kernel[i])).b;\n" \
            "    }\n" \
            "    colour /= KERNEL_SIZE;\n"
            "#else\n" \
            "    colour.r = texture2D(u_sourceTexture, texCoord + offset).r;\n" \
            "    colour.g = texture2D(u_sourceTexture, texCoord).g;\n" \
            "    colour.b = texture2D(u_sourceTexture, texCoord - offset).b;\n" \
            "#endif\n" \
            /*noise*/
            "    float x = (texCoord.x + 4.0) * texCoord.y * u_time * 10.0;\n" \
            "    x = mod(x, 13.0) * mod(x, 123.0);\n" \
            "    float grain = mod(x, 0.01) - 0.005;\n" \
            "    vec3 result = colour + vec3(clamp(grain * 100.0, 0.0, 0.07));\n" \
            /*scanlines*/
            "    vec2 sinCos = vec2(sin(texCoord.y * lineCount), cos(texCoord.y * lineCount + u_time));\n" \
            "    result += colour * vec3(sinCos.x, sinCos.y, sinCos.x) * (noiseStrength * 0.08);\n" \
            "    colour += (result - colour) * noiseStrength;\n" \
            "    gl_FragColor = vec4(colour, 1.0);" \
            "}";
    }

    namespace LightRay
    {
        static const std::string vertex =
            "#version 120\n" \
            "uniform vec3 u_lightPosition;\n" \
            "varying vec2 v_vertPosition;\n" \

            "void main()\n" \
            "{\n" \
            "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
            "    v_vertPosition = gl_Vertex.xy;\n" \
            "    gl_FrontColor = gl_Color;\n" \
            "}";

        static const std::string fragment =
            "#version 120\n" \
            "uniform float u_alpha = 1.0;\n" \

            "varying vec2 v_vertPosition;\n" \

            "const float falloffStart = 800.0;\n" \
            "const float falloffDistance = 700.0;\n" \

            "void main()\n " \
            "{\n" \
            "    float falloffAlpha = 1.0;\n" \
            "    float length = length(v_vertPosition);\n" \
            "    if(length > falloffStart)\n" \
            "    {\n" \
            "        falloffAlpha -= min((length - falloffStart) / falloffDistance , 1.0);\n" \
            "    }\n" \
            /*"    gl_FragColor = vec4(falloffAlpha);\n" \*/
            "    gl_FragColor = gl_Color;\n" \
            "    gl_FragColor.a *= u_alpha * falloffAlpha;\n" \
            "}";
    }
}

#endif //SHADERS_HPP_