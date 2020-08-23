# pillow

Pillow is a software rendering engine built from scratch in C++. It intentionally uses no libraries or API's for graphics. Consequently, all calculations and algorithms are implemented from the ground up. This was done for learning purposes and personal entertainment. So far the engine features:

- Wireframe rendering (perspective projection)
- Clipping
- Rasterization and color interpolation
- Depth-buffering (Now renders solid wireframes)
- Back-face culling
- Flat shading using the phong reflection model

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
