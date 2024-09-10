# SpiritedAwayTrainScene
This is my repository for my Spirited away train scene, recreated using shaders in C++

Spirited Away is one of my favorite movies of all time. So during my graphics class, I was inspired to recreate the iconic train scene from the movie. In order to achieve this, I utilized multiple methods to achieve the final desired look.
Firstly, there is the skybox. A skybox in shaders is a large cube that surrounds the entire scene, with each face of the cube textured to represent the sky, creating the illusion of distant 3D surroundings. For this project, I used frames from the actual movie to recreate the beautiful pastel sky. The details of this implementation can be found in the skybox.vert file under shaders. How it works is a cube map texture is applied to a 3D cube, and the local positions of the cube serve as texture coordinates because each position vector from the cube's origin acts as a direction vector needed for sampling the texture. Consequently, the vertex shader outputs the local position vector as the texture coordinate, allowing the fragment shader to use these coordinates to sample from the samplerCube.

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



