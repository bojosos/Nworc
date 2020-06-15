emcc -s USE_FREETYPE=1 "distance-field.c" "utf8-utils.c" "edtaa3func.c" "platform.c" "texture-atlas.c" "texture-font.c" "vector.c" --memory-init-file 0 -O3 -o bin/Web/freetype-gl.bc
