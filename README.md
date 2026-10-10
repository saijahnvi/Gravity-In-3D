# Relativistic-Gravity-In-3D
Interactive 3D N-body gravitational simulation and real-time spacetime grid deformation engine built in C++ using Modern OpenGL, GLFW, and GLM.

An interactive, high-performance **3D gravitational simulation engine** built from scratch in C++ and Modern OpenGL (`Core Profile 3.3`). The application renders celestial bodies orbiting a central mass while dynamically deforming a 3D wireframe mesh in real-time to simulate general relativistic spacetime curvature (gravitational wells).

---
## PREVIEW:
<img width="767" height="575" alt="Screenshot 2026-10-08 at 7 00 46 PM" src="https://github.com/user-attachments/assets/5117c4ef-5fa1-45f6-a445-48317ef3dbe2" />

<p align="center">
  <video width="100%" controls autoplay loop muted>
    <source src="https://github.com/user-attachments/assets/c7ca6622-65d7-453d-8199-7a84bd1db32b" type="video/mp4">
    Your browser does not support the video tag.
  </video>
</p>

<p align="center">
  <img width="767" alt="Simulation Screenshot" src="https://github.com/user-attachments/assets/5117c4ef-5fa1-45f6-a445-48317ef3dbe2" />
</p>
(The above video is recorded from a different device hence the resolution of the actual project is higher)

## Key Features

* **Real-Time Spacetime Grid Deformation:** Procedurally generated ground grid vertices dynamically warp downwards based on the gravitational potential ($V(r) \propto -M / r$) of all active bodies in the system.
* **Symplectic Integration & Sub-Stepping:** Employs a semi-implicit (symplectic) Euler integrator with fixed sub-stepping ($8\times$ per frame) to preserve phase-space volume and eliminate numerical energy drift over long orbital horizons.
* **Keplerian Orbital Mechanics:** Precise initial conditions and velocity vectors ($v = \sqrt{GM/r}$) ensuring stable circular orbits for multi-body configurations.
* **Interactive 3D Camera:** First-person free-roam camera implementation using GLM look-at matrices and mouse/keyboard callbacks (`WASD`, `Space`, `Shift`).
* **Rigid-Body Collision Resolution:** Elastic collision response handling distance overlaps and impulse exchange between dynamic bodies.

---
## Physics & Numerical Architecture

Standard explicit Euler integration methods suffer from severe energy drift in orbital mechanics, causing planets to either spiral into the star or fly off into space. This engine addresses numerical stability through:

1. **Sub-Stepped Integration:** Each frame's delta time ($\Delta t$) is subdivided into 8 discrete micro-steps. This drastically increases numerical accuracy for close-encounter gravitational accelerations without bottlenecking frame rates.
 
2. **Softened Gravitational Potential:** To avoid division-by-zero singularities when bodies pass close to grid vertices or each other, a softening factor ($\epsilon$) is incorporated into the distance denominator:
   $$\vec{a}_i = \sum_{j \neq i} \frac{G \cdot m_j \cdot (\vec{r}_j - \vec{r}_i)}{\left(\vert{}\vec{r}_j - \vec{r}_i\vert{}^2 + \epsilon^2\right)^{3/2}}$$

3. Real-world SI gravitational constants ($G = 6.67430 \times 10^{-11}$) and astronomical units introduce extreme exponents that can strain single-precision floating-point arithmetic (`float`) and require cumbersome time-scaling factors. To ensure smooth real-time performance and predictable orbital geometry, the engine utilizes **normalized simulation units** paired with a tuned gravitational constant: G=300.0f

---
## Graphics & Rendering Pipeline

* **Modern OpenGL Core Profile (3.3+):** Utilizes Vertex Array Objects (VAOs), Vertex Buffer Objects (VBOs), and custom GLSL vertex/fragment shaders.
* **Procedural Wireframe Mesh Generation:** The spacetime grid is constructed on the CPU as a dense vertex grid and dynamically deformed every frame or via vertex shader uniform transformations.
* **Matrix Transformations:** Handled via GLM (`glm::perspective`, `glm::lookAt`, `glm::translate`, `glm::scale`) to construct clean Model-View-Projection (MVP) pipelines.

---
## Controls & Navigation

| Key | Action |
| :--- | :--- |


| **`W` / `S`** | Move camera Forward / Backward |
| **`A` / `D`** | Move camera Left / Right |
| **`Space` / `Shift`** | Move camera Up / Down |
| **`Mouse`** | Look around (Free-roam FPS view) |
| **`Esc`** | Close window |

---

## Tech Stack & Dependencies

* **Language:** C++
* **Graphics API:** Modern OpenGL (3.3 Core)
* **Windowing & Input:** GLFW
* **Extension Loading:** GLEW
* **Linear Algebra:** OpenGL Mathematics (GLM)


Some of the code Implementations and Inspiration was taken from the following youtube video:https://www.youtube.com/watch?v=_YbGWoUaZg0
