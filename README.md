# OpenGL 3D Interactive Scene

This project is a 3D graphics assignment built using OpenGL and GLUT. It features a fully interactive 3D environment with camera control, object transformations (translation, rotation, scaling), and auto-rotation for a pyramid. Users can select between a cube, pyramid, or sphere and manipulate them in real time using keyboard controls.

## Features

- Interactive camera movement (WASD)
- Object selection (cube, pyramid, sphere)
- Translation in 3D space
- Rotation along X and Y axes
- Object scaling
- Auto-rotation toggle for pyramid
- Clean UI with keyboard-only interaction
- Exit the application with `ESC`

## Keyboard Controls

| Key             | Function                                   |
|-----------------|--------------------------------------------|
| **ESC**         | Exit the application                       |
| **W**           | Move camera forward                        |
| **S**           | Move camera backward                       |
| **A**           | Move camera left                           |
| **D**           | Move camera right                          |
| **1**           | Select Cube                                |
| **2**           | Select Pyramid                             |
| **3**           | Select Sphere                              |
| **↑ (Up Arrow)**| Move object forward (−Z)                   |
| **↓ (Down Arrow)**| Move object backward (+Z)               |
| **← (Left Arrow)**| Move object left (−X)                   |
| **→ (Right Arrow)**| Move object right (+X)                 |
| **Z**           | Move object upward (+Y)                    |
| **X**           | Move object downward (−Y)                  |
| **R**           | Rotate object clockwise (Y-axis)           |
| **T**           | Rotate object counter-clockwise (Y-axis)   |
| **F**           | Tilt object upward (X-axis)                |
| **G**           | Tilt object downward (X-axis)              |
| **+ (= key)**   | Increase object scale                      |
| **-**           | Decrease object scale                      |
| **N**           | Enable auto-rotation of pyramid            |
| **M**           | Disable auto-rotation of pyramid           |

## Requirements

- OpenGL
- GLUT (FreeGLUT)
- C++ compiler (G++ / Clang)

## Build & Run Instructions

1. Clone the repository:
   ```bash
   git clone git@github.com:Mesbah5411/OpenGL3D-project.git
   cd OpenGL3D-project
