#include <iostream>
#include <math.h>
#include <unistd.h>
#include <vector>
#include <sys/time.h>
#include <raylib.h>
#include <cstdio>

#define SCALE 10
#define FONTSIZE 20
#define PADDING 15
#define TARGET_FPS 200

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

  explicit RandomParticle(Vector3 _position, Color _color) : Particle(_position, _color) {
     this->acceleration.x = ((double) rand() / (RAND_MAX)) * 2 - 1;
     this->acceleration.y = ((double) rand() / (RAND_MAX)) * 2 - 1;
     this->acceleration.z = ((double) rand() / (RAND_MAX)) * 2 - 1;  
 
    this->acceleration.x /= 50;
    this->acceleration.y /= 50;
    this->acceleration.z /= 50;
  }

  void applyDeltaTime(float dt) {
    acceleration.x *= dt;
    acceleration.y *= dt;
    acceleration.z *= dt;
  }

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

      this->acceleration.x /= 50;
      this->acceleration.y /= 50;
      this->acceleration.z /= 50;
    }
  }
};

int main(void) {
  srand((unsigned) time(NULL));
  InitWindow(900, 600, "Lagrange Particle Simulation");
  
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 150.0f, 150.0f, 150.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };    
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        
  camera.fovy = 45.0f;                              
  camera.projection = CAMERA_PERSPECTIVE;  

  RandomParticle particle = RandomParticle({ 0.0f, 0.0f, 0.0f }, ORANGE);

  SetTargetFPS(TARGET_FPS);

  std::vector<Vector4> particleTrace;

  bool isSimulationOver = false;
  bool isPaused = true;

  struct timeval timedate;
  gettimeofday(&timedate, NULL);

  long int initialTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000;
  long int currentTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000;

  float sliderMarkerPos = 0;
  float traceIndex = 0;

  while (!WindowShouldClose()) {
     BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    particle.draw();

    if (IsKeyPressed(KEY_SPACE)) {
      isPaused = !isPaused;
    }

    if (IsKeyPressed(KEY_ENTER)) {
      isSimulationOver = true;
    }

    if (!isSimulationOver && !isPaused) {
   
      gettimeofday(&timedate, NULL);
      int dt = (currentTime - (timedate.tv_sec * 1000 + timedate.tv_usec / 1000)) / 1000;
      currentTime = (timedate.tv_sec * 1000 + timedate.tv_usec / 1000);

      particle.update();
      particle.applyDeltaTime(dt);

      particleTrace.push_back({ 
        particle.position.x,
        particle.position.y,
        particle.position.z,
        (float) ((timedate.tv_sec * 1000 + (int) (timedate.tv_usec / 1000)) - initialTime),
      });
    }

    DrawGrid(25, SCALE);

    EndMode3D();

    float sliderY = (GetRenderHeight() / 2.5) - (((GetRenderHeight() / 2.5) - (FONTSIZE * 6 + PADDING * 3))) + (((GetRenderHeight() / 2.5) - (FONTSIZE * 6 + PADDING * 3))) / 2 + 2;
    float leftSliderLimit = (GetScreenWidth()  - (int) (GetScreenWidth() / 4)) + PADDING;
    float rightSliderLimit = GetScreenWidth() - PADDING;

     if (isSimulationOver) {

      std::string posx = "  x: " + std::to_string(particle.position.x);
      std::string posy = "  y: " + std::to_string(particle.position.y);
      std::string posz = "  z: " + std::to_string(particle.position.z);

      std::string timeMsg = "Time: " + std::to_string(traceIndex) + "s";

      DrawText("simulation: OFF  [N] - new simulation", PADDING, PADDING, FONTSIZE, DARKGRAY);
      
      DrawRectangle(GetScreenWidth() - (GetScreenWidth()  / 4), 0, 2 + GetScreenWidth() / 4, 2 + GetScreenHeight() / 2.5, DARKGRAY);
      DrawRectangle(GetScreenWidth() - (GetScreenWidth()  / 4) + 2, 0, GetScreenWidth() / 4, GetScreenHeight() / 2.5, RAYWHITE);

      DrawText("Pos:", GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 0 + PADDING, FONTSIZE, DARKGRAY);
      DrawText(posx.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 1 + PADDING, FONTSIZE * 0.98, GREEN);
      DrawText(posy.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 2 + PADDING, FONTSIZE * 0.98, RED);
      DrawText(posz.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 3 + PADDING, FONTSIZE * 0.98, BLUE);
      DrawText(timeMsg.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 4 + PADDING * 2, FONTSIZE, DARKGRAY);
      DrawText("Velo: 0m/s", GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 5 + PADDING * 2, FONTSIZE, DARKGRAY);
      DrawText("Acce: 0m/s²", GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 6 + PADDING * 2, FONTSIZE, DARKGRAY);

      DrawLineEx(
        { rightSliderLimit, sliderY}, 
        { leftSliderLimit, sliderY}, 
        4,
        GRAY);

      if (sliderMarkerPos == NULL)
        sliderMarkerPos = leftSliderLimit;

      DrawCircleV({sliderMarkerPos, sliderY}, 7, DARKGRAY);
      DrawCircleV({sliderMarkerPos, sliderY}, 6, RAYWHITE);

      if (std::sqrt(std::pow(sliderMarkerPos - GetMouseX(), 2) + std::pow(sliderY - GetMouseY(), 2)) <= 12) {
        if (GetMouseX() >= leftSliderLimit && GetMouseX() <= rightSliderLimit) {
          sliderMarkerPos = GetMouseX();
          traceIndex = ((GetMouseX() - leftSliderLimit) / (rightSliderLimit - leftSliderLimit)) * particleTrace.back().w / 1000;
        }
      }

      if (IsKeyPressed(KEY_N)) {
        isSimulationOver = false;
        particle =  RandomParticle({ 0, 0, 0 }, ORANGE);
        initialTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000,
        isPaused = true;
        particleTrace.clear();
        sliderMarkerPos = leftSliderLimit;
        traceIndex = 0;
      }
    } else {
      if (isPaused) {
        DrawText("simulation: PAUSED  [SPACE] - resume  [N] - new simulation  [ENTER]", PADDING, PADDING, FONTSIZE, DARKGRAY);
        if (IsKeyPressed(KEY_N)) {
          gettimeofday(&timedate, NULL);
          isSimulationOver = false;
          particle =  RandomParticle({ 0, 0, 0 }, ORANGE);
          initialTime = (timedate.tv_sec * 1000 + timedate.tv_usec / 1000),
          isPaused = true;
          particleTrace.clear();
          sliderMarkerPos = leftSliderLimit;
          traceIndex = 0;
        }
      } else
        DrawText("simulation: ON  [SPACE] - pause  [ENTER] - end simulation", PADDING, PADDING, FONTSIZE, DARKGRAY);
      std::string posText = "particle position: (" + std::to_string(particle.position.x) + ", " + std::to_string(particle.position.y) + ", " + std::to_string(particle.position.z) + ")";
      DrawText(posText.c_str(), PADDING, FONTSIZE * 1 + PADDING * 2, FONTSIZE, DARKGRAY);
      UpdateCamera(&camera, CAMERA_PERSPECTIVE);
    }

    EndDrawing();

    camera.target.x = particle.position.x;
    camera.target.y = particle.position.y;
    camera.target.z = particle.position.z;
  }

  CloseWindow();
  return 0;
  
}
