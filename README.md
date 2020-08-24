# pillow

Pillow is a software rendering engine built from scratch in C++. It intentionally uses no libraries or API's for rendering graphics. Consequently, all calculations and algorithms are implemented from the ground up. This was done for learning purposes and personal entertainment. So far the engine features:

- Wireframe rendering (perspective projection)
- Compatibility with .obj files
- A look at camera
- Clipping
- Rasterization and color interpolation using scanline algorithm
- Material rendering 
- Compatibility with .mtl files
- Depth-buffering
- Back-face culling
- Gouraud shading using the phong illumination model

## How to run
You will need linux to run. Please do the following to run the demo:

`git clone https://github.com/zzef/pillow.git`

`make`

`./pillow`

<p align="center">
  <img src="https://raw.githubusercontent.com/zzef/pillow/master/docs/sample3.gif">
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/zzef/pillow/master/docs/sample.gif">
</p>
