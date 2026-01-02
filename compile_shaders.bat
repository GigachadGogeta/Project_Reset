DEL "shaders\shader.vert.spv" /Q
DEL "shaders\shader.frag.spv" /Q
DEL "shaders\point_light.vert.spv" /Q
DEL "shaders\point_light.frag.spv" /Q
glslc.exe raw_shaders\shader.vert -o shaders\shader.vert.spv
glslc.exe raw_shaders\shader.frag -o shaders\shader.frag.spv
glslc.exe raw_shaders\point_light.vert -o shaders\point_light.vert.spv
glslc.exe raw_shaders\point_light.frag -o shaders\point_light.frag.spv
pause