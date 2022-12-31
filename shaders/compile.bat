glslc shader.frag -o frag.spv
glslc shader.vert -o vert.spv

glslc SkyBoxShader.frag -o SkyBoxFrag.spv
glslc SkyBoxShader.vert -o SkyBoxVert.spv

glslc hoverlayShader.frag -o hoverlayFrag.spv
glslc hoverlayShader.vert -o hoverlayVert.spv

pause
exit