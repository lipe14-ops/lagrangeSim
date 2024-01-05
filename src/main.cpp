#include <iostream>
#include <time.h>
#include <raylib.h>
#include <cstdio>

#define SCALE 10
#define TARGET_FPS 60

class Particle {

protected:
  Vector3 velocity, acceleration;

public:
  Vector3 position; 
  Color color;

  explicit Particle(Vector3 _position, Color _color) : color(_color), position(_position) {
    this->velocity = { 0 };
    this->acceleration = { 0 };
  } 

  void draw() {
    DrawSphere(position, 2, color);
  }
};

class RandomParticle : public Particle {

public:

  explicit RandomParticle(Vector3 _position, Color _color) : Particle(_position, _color) {}

  void update() {
    velocity.x += acceleration.x;
    velocity.y += acceleration.y;
    velocity.z += acceleration.z;

    position.x += velocity.x;
    position.y += velocity.y;
    position.z += velocity.z;

    if ((rand() % 2) == 1) {
      this->acceleration.x = ((double) rand() / (RAND_MAX)) * 2 - 1;
      this->acceleration.y = ((double) rand() / (RAND_MAX)) * 2 - 1;
      this->acceleration.z = ((double) rand() / (RAND_MAX)) * 2 - 1;

      this->acceleration.x /= 150;
      this->acceleration.y /= 150;
      this->acceleration.z /= 150;
    }
  }
};

int main(void) {
  srand((unsigned) time(NULL));
  InitWindow(800, 600, "Lagrange Particle Simulation");
  
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 150.0f, 150.0f, 150.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };    
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        
  camera.fovy = 45.0f;                              
  camera.projection = CAMERA_PERSPECTIVE;  

  RandomParticle particle = RandomParticle({ 0.0f, 0.0f, 0.0f }, RED);

  SetTargetFPS(TARGET_FPS);

  while (!WindowShouldClose()) {
    
    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    particle.draw();
    particle.update();

    DrawGrid(25, SCALE);

    EndMode3D();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
