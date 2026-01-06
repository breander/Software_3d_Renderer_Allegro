# Software 3D Renderer - main.cpp Documentation

## Overview

This is a software-based 3D renderer built using the Allegro 5 library. The program loads and renders 3D objects (specifically a teapot model) with basic lighting and transformation capabilities. It implements a complete 3D graphics pipeline entirely in software, including mesh loading, 3D transformations, projection, backface culling, and painter's algorithm for depth sorting.

## Data Structures

### `vec3d`
A 3D vector structure with homogeneous coordinates.
- **Members:**
  - `x`, `y`, `z`: 3D coordinates (float)
  - `w`: Homogeneous coordinate (default: 1)

### `triangle`
Represents a triangle in 3D space.
- **Members:**
  - `p[3]`: Array of three `vec3d` points
  - `color`: Lighting color value (float)

### `mesh`
Represents a 3D mesh object.
- **Members:**
  - `tris`: Vector of triangles
- **Methods:**
  - `LoadFromObjectFile(string sFileName)`: Loads mesh data from a Wavefront .obj file. Returns true on success, false on failure.

### `mat4x4`
A 4x4 transformation matrix.
- **Members:**
  - `m[4][4]`: 2D array of floats initialized to zero

## Functions

### Matrix Operations

#### `Matrix_MultiplyVector(mat4x4 &m, vec3d &i)`
Multiplies a 4x4 matrix by a vector to transform a 3D point.
- **Parameters:** 
  - `m`: The transformation matrix
  - `i`: Input vector
- **Returns:** Transformed vector

#### `Matrix_MakeIdentify()`
Creates a 4x4 identity matrix.
- **Returns:** Identity matrix

#### `Matrix_MakeRotationX(float fAngleRad)`
Creates a rotation matrix for rotation around the X-axis.
- **Parameters:** `fAngleRad`: Rotation angle in radians
- **Returns:** Rotation matrix

#### `Matrix_MakeRotationY(float fAngleRad)`
Creates a rotation matrix for rotation around the Y-axis.
- **Parameters:** `fAngleRad`: Rotation angle in radians
- **Returns:** Rotation matrix

#### `Matrix_MakeRotationZ(float fAngleRad)`
Creates a rotation matrix for rotation around the Z-axis.
- **Parameters:** `fAngleRad`: Rotation angle in radians
- **Returns:** Rotation matrix

#### `Matrix_MakeTranslation(float x, float y, float z)`
Creates a translation matrix.
- **Parameters:** `x`, `y`, `z`: Translation distances along each axis
- **Returns:** Translation matrix

#### `Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)`
Creates a perspective projection matrix to convert 3D coordinates to 2D screen space.
- **Parameters:**
  - `fFovDegrees`: Field of view in degrees
  - `fAspectRatio`: Aspect ratio of the viewport
  - `fNear`: Near clipping plane distance
  - `fFar`: Far clipping plane distance
- **Returns:** Projection matrix

#### `Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)`
Multiplies two 4x4 matrices together.
- **Parameters:** `m1`, `m2`: Input matrices
- **Returns:** Resulting matrix

### Vector Operations

#### `Vector_Add(vec3d &v1, vec3d &v2)`
Adds two vectors component-wise.
- **Parameters:** `v1`, `v2`: Input vectors
- **Returns:** Sum of the two vectors

#### `Vector_Sub(vec3d &v1, vec3d &v2)`
Subtracts the second vector from the first.
- **Parameters:** `v1`, `v2`: Input vectors
- **Returns:** Difference vector

#### `Vector_Mul(vec3d &v1, float k)`
Multiplies a vector by a scalar.
- **Parameters:**
  - `v1`: Input vector
  - `k`: Scalar multiplier
- **Returns:** Scaled vector

#### `Vector_Div(vec3d &v1, float k)`
Divides a vector by a scalar.
- **Parameters:**
  - `v1`: Input vector
  - `k`: Scalar divisor
- **Returns:** Scaled vector

#### `Vector_DotProduct(vec3d &v1, vec3d &v2)`
Calculates the dot product of two vectors.
- **Parameters:** `v1`, `v2`: Input vectors
- **Returns:** Dot product (float)

#### `Vector_Length(vec3d &v)`
Calculates the length (magnitude) of a vector.
- **Parameters:** `v`: Input vector
- **Returns:** Length (float)

#### `Vector_Normalise(vec3d &v)`
Normalizes a vector to unit length.
- **Parameters:** `v`: Input vector
- **Returns:** Normalized vector

#### `Vector_CrossProduct(vec3d &v1, vec3d &v2)`
Calculates the cross product of two vectors (perpendicular vector).
- **Parameters:** `v1`, `v2`: Input vectors
- **Returns:** Cross product vector

## Main Function

### `main(int argc, char *argv[])`

The main entry point of the program. It performs the following operations:

#### 1. Command Line Argument Parsing
Processes command line arguments to configure the renderer:
- `-debug`: Enables debug mode (shows triangle wireframes)
- `-width [value]`: Sets screen width (default: 1920)
- `-height [value]`: Sets screen height (default: 1080)
- `-fps [value]`: Sets target frames per second (default: 60)

#### 2. Allegro Initialization
Initializes the Allegro 5 library and its subsystems:
- Core Allegro system
- Keyboard input
- Timer for frame rate control
- Event queue for handling events
- Display window
- Font rendering (TTF)
- Primitives drawing addon

#### 3. Mesh Loading
Loads the 3D model from "teapot.obj" file using the `LoadFromObjectFile` method.

#### 4. Projection Matrix Setup
Creates a perspective projection matrix with:
- 60-degree field of view
- Aspect ratio based on screen dimensions
- Near plane at 0.1
- Far plane at 1000.0

#### 5. Main Rendering Loop
The core game loop that runs continuously until the program exits:

##### Event Handling
- **ALLEGRO_EVENT_TIMER**: Updates camera position based on keyboard input
  - Arrow keys: Move forward/backward (Z-axis) and left/right (X-axis)
  - W/S keys: Move up/down (Y-axis)
  - ESC key: Exit program
- **ALLEGRO_EVENT_KEY_DOWN**: Registers key presses
- **ALLEGRO_EVENT_KEY_UP**: Registers key releases
- **ALLEGRO_EVENT_DISPLAY_CLOSE**: Handles window close event

##### Rendering Pipeline
1. **FPS Calculation**: Measures and displays frame rate
2. **Screen Clear**: Fills screen with black
3. **Transformation Setup**: Creates world transformation matrix (translation)
4. **For Each Triangle in Mesh**:
   - Transform triangle to world space
   - Calculate surface normal using cross product
   - **Backface Culling**: Check if triangle faces camera (dot product with camera ray)
   - **Lighting Calculation**: Compute diffuse lighting based on light direction
   - **Projection**: Transform 3D coordinates to 2D screen space
   - Perform perspective divide (divide by w)
   - Invert X and Y coordinates
   - Offset and scale to screen coordinates
   - Add triangle to render list with its lighting color
5. **Depth Sorting**: Sort triangles by average Z-depth (painter's algorithm)
6. **Rasterization**: Draw all triangles as filled polygons
7. **Debug Mode**: Optionally draw wireframe outlines if debug mode is enabled
8. **Display Update**: Flip the display buffer

#### 6. Cleanup
Properly destroys all Allegro resources:
- Font
- Display
- Timer
- Primitives addon
- Font addon
- TTF addon
- Event queue

## Controls

- **Up Arrow**: Move forward (decrease Z position)
- **Down Arrow**: Move backward (increase Z position)
- **Left Arrow**: Move left (decrease X position)
- **Right Arrow**: Move right (increase X position)
- **W Key**: Move up (increase Y position)
- **S Key**: Move down (decrease Y position)
- **ESC Key**: Exit the program

## Rendering Features

1. **3D Transformations**: Full matrix-based transformation system
2. **Perspective Projection**: Realistic 3D-to-2D projection
3. **Backface Culling**: Only renders triangles facing the camera
4. **Diffuse Lighting**: Simple directional lighting model
5. **Depth Sorting**: Painter's algorithm for proper triangle ordering
6. **Debug Wireframe**: Optional wireframe overlay for debugging

## Dependencies

- Allegro 5 (core library)
- Allegro 5 Font addon
- Allegro 5 TTF addon
- Allegro 5 Primitives addon
- Allegro 5 Color addon
- Standard C++ libraries (vector, list, fstream, algorithm, string, sstream)
