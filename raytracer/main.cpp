#include "raytracerlib.h"
#include "camera.h"

// main gets as an argument the name of the .OBJ file
// Then it retreives the scene from the file (parsing part)
// And the second step is rendering itself

int main () {
    std::string name_of_file = ".obj";

    Scene scene = ReadScene(name_of_file);
    Camera camera{}
    Image image = Render(scene, )
}