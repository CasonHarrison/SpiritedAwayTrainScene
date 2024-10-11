# SpiritedAwayTrainScene
This is my repository for my Spirited away train scene, recreated using shaders in C++

Spirited Away is one of my favorite movies of all time. So during my graphics class, I was inspired to recreate the iconic train scene from the movie. In order to achieve this, I utilized multiple methods to achieve the final desired look.
Firstly, there is the skybox. A skybox in shaders is a large cube that surrounds the entire scene, with each face of the cube textured to represent the sky, creating the illusion of distant 3D surroundings. For this project, I used frames from the actual movie to recreate the beautiful pastel sky. The details of this implementation can be found in the skybox.vert file under shaders. How it works is a cube map texture is applied to a 3D cube, and the local positions of the cube serve as texture coordinates because each position vector from the cube's origin acts as a direction vector needed for sampling the texture. Consequently, the vertex shader outputs the local position vector as the texture coordinate, allowing the fragment shader to use these coordinates to sample from the samplerCube. This texture is also later used in the reflections in the water which will be described later.

The next feature implemented was a skysphere. In fact the background you see in the video is not the skybox, only the skysphere. With the perspective of the camera I couldn't fit the skybox in a way that matched the source image. Instead, the skybox is just used for the reflections in the water as using the internal normals of the skyphere wasn't giving the right look on the water surface. 

Then I create the water. I create the water surface using plane obj then modifying it with worley noise to look move wave like. The noise octaves also utilize the Time to move and animate over time. Then the water is colored using phong shading and the reflections of the skybox as mentioned earlier.

Here is the final product
<details open="" class="details-reset border rounded-2">
  <summary class="px-3 py-2 border-bottom">
    <svg aria-hidden="true" viewBox="0 0 16 16" version="1.1" data-view-component="true" height="16" width="16" class="octicon octicon-device-camera-video">
    <path fill-rule="evenodd" d="..."></path>
</svg>
    <span class="dropdown-caret"></span>
  </summary>

  <video src="https://github.com/user-attachments/assets/60e06096-99af-43f8-8038-8b25739e5a0b" data-canonical-src="https://github.com/user-attachments/assets/60e06096-99af-43f8-8038-8b25739e5a0b" controls="controls" muted="muted" class="d-block rounded-bottom-2 width-fit" style="max-height:640px;">

  </video>
</details>



