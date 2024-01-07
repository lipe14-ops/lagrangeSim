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
#define TARGET_FPS 100

class Particle {

protected:
  Vector3 velocity, acceleration;

public:
  Vector3 position; 
  Color color;

  explicit Particle(Vector3 _position, Color _color) : color(_color), position(_position) {
    this->velocity = { 10, 10, 0  };
    this->acceleration = { .01, -0.9, -0.003 };
  } 

  void draw() {
    DrawSphere(position, 2, color);
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
  }
};


class RandomParticle : public Particle {

public:

  explicit RandomParticle(Vector3 _position, Color _color) : Particle(_position, _color) {}

  void update() {
    float step = 1;
    position.x += ((double) rand() / (RAND_MAX) * (step + 1) - step);
    position.y += ((double) rand() / (RAND_MAX) * (step + 1) - step);
    position.z += ((double) rand() / (RAND_MAX) * (step + 1) - step);
  }
};

class LagrangeTheorem {

protected:
  std::vector<double> cache;
  std::vector<Vector2> points;

public: 
  LagrangeTheorem(std::vector<Vector2> _points) : points(_points) {}
  
  void addPoint(Vector2 point) {
    points.push_back(point);
  }

  int pointsAmount() {
    return points.size();
  }

  double const interpolate(double const value) {
    double result = 0;
    for (int i = 0; i < points.size(); ++i) {
      double accumulator = points.at(i).y;
      for (int j = 0; j < points.size(); ++j) {
        if (i != j) {
          accumulator *= (value - points.at(j).x) / (points.at(i).x - points.at(j).x);  
        }
      }
      result += accumulator;
    }
    return result;
  }
};

int main(void) {
  srand((unsigned) time(NULL));
  InitWindow(900, 600, "Particle Simulation - Lagrange Theorem");
  
  Camera3D camera = { 0 };
  camera.position = (Vector3){ 200.0f, 200.0f, 200.0f };
  camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };    
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        
  camera.fovy = 45.0f;                              
  camera.projection = CAMERA_PERSPECTIVE;  

  Vector3 initialPos = { SCALE * -12, 0.0f, 0.0f };
  Particle particle = Particle(initialPos, ORANGE);

  std::vector<Vector4> particleTrace;

  LagrangeTheorem xInterpolation = LagrangeTheorem({});
  LagrangeTheorem yInterpolation = LagrangeTheorem({});
  LagrangeTheorem zInterpolation = LagrangeTheorem({});

  bool isSimulationOver = false;
  bool isPaused = true;
  bool isPlotMode = false;

  struct timeval timedate;
  gettimeofday(&timedate, NULL);

  long int initialTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000;
  long int currentTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000;

  float sliderMarkerPos = 0;
  float sliderValue = 0;
  float velocityValue = 0;
  float accelerationValue = 0;

  SetTargetFPS(TARGET_FPS);

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

    if (isPlotMode) {
      for (int i = 1; i < particleTrace.size(); ++i) {
        Vector4 previousPoint = particleTrace.at(i - 1);
        Vector4 currentVector = particleTrace.at(i);
        DrawLine3D({ previousPoint.x, previousPoint.y, previousPoint.z }, { currentVector.x, currentVector.y, currentVector.z }, PURPLE);
      }
    }

    if (!isSimulationOver && !isPaused) {
   
      gettimeofday(&timedate, NULL);
      currentTime = (timedate.tv_sec * 1000 + timedate.tv_usec / 1000);

      float dt = (currentTime - initialTime) / 100000000;

      particle.update();

      particleTrace.push_back({ 
        particle.position.x,
        particle.position.y,
        particle.position.z,
        (float) ((timedate.tv_sec * 1000 + (int) (timedate.tv_usec / 1000)) - initialTime),
      });

      // particle.applyDeltaTime(dt);
    }

    DrawGrid(25, SCALE);

    EndMode3D();

    float sliderY = (GetRenderHeight() / 2.5) - (((GetRenderHeight() / 2.5) - (FONTSIZE * 6 + PADDING * 3))) + (((GetRenderHeight() / 2.5) - (FONTSIZE * 6 + PADDING * 3))) / 2 + 2;
    float leftSliderLimit = (GetScreenWidth()  - (int) (GetScreenWidth() / 4)) + PADDING;
    float rightSliderLimit = GetScreenWidth() - PADDING;

     if (isSimulationOver) {
      if (xInterpolation.pointsAmount() == 0 || yInterpolation.pointsAmount() == 0 || zInterpolation.pointsAmount() == 0) {
        for (auto vector : particleTrace) {
          float deltaTime = (particleTrace.back().w - (particleTrace.back().w - vector.w)) / 1000;
          xInterpolation.addPoint({ deltaTime, vector.x });
          yInterpolation.addPoint({ deltaTime, vector.y });
          zInterpolation.addPoint({ deltaTime, vector.z });
        }
      }

      std::string posx = "  x: " + std::to_string(particle.position.x);
      std::string posy = "  y: " + std::to_string(particle.position.y);
      std::string posz = "  z: " + std::to_string(particle.position.z);

      particle.position.x = xInterpolation.interpolate(sliderValue );
      particle.position.y = yInterpolation.interpolate(sliderValue);
      particle.position.z = zInterpolation.interpolate(sliderValue);

      std::string timeMsg = "Time: " + std::to_string(sliderValue) + "s";
      std::string veloMsg = "Velo: " + std::to_string(velocityValue) + "m/s";
      std::string acceMsg = "Acce: " + std::to_string(accelerationValue) + "m/s²";

      float eps = 0.0001;

      auto veloFn = [eps, particle](LagrangeTheorem interpolation, float x) -> float {
        return (interpolation.interpolate(x + eps) - interpolation.interpolate(x)) / eps;
      };

      Vector3 velocityVector = {
        .x = (float) veloFn(xInterpolation, sliderValue),
        .y = (float) veloFn(yInterpolation, sliderValue),
        .z = (float) veloFn(zInterpolation, sliderValue),
      };

      velocityValue = std::sqrt(std::pow(velocityVector.x, 2) + std::pow(velocityVector.y, 2) + std::pow(velocityVector.z, 2));


      auto accFn = [eps, veloFn](LagrangeTheorem interpolation, float x) -> float {
        return (veloFn(interpolation, x + eps) - veloFn(interpolation, x)) / eps;
      };


      Vector3 accelerationVector = {
        .x = (float) accFn(xInterpolation, sliderValue),
        .y = (float) accFn(yInterpolation, sliderValue),
        .z = (float) accFn(zInterpolation, sliderValue),
      };

      accelerationValue = std::sqrt(std::pow(accelerationVector.x, 2) + std::pow(accelerationVector.y, 2) + std::pow(accelerationVector.z, 2));

      DrawText("simulation: OFF  [N] - new simulation  [P] - plot/unplot graph", PADDING, PADDING, FONTSIZE, DARKGRAY);
      
      DrawRectangle(GetScreenWidth() - (GetScreenWidth()  / 4), 0, 2 + GetScreenWidth() / 4, 2 + GetScreenHeight() / 2.5, DARKGRAY);
      DrawRectangle(GetScreenWidth() - (GetScreenWidth()  / 4) + 2, 0, GetScreenWidth() / 4, GetScreenHeight() / 2.5, RAYWHITE);

      DrawText("Pos:", GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 0 + PADDING, FONTSIZE, DARKGRAY);
      DrawText(posx.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 1 + PADDING, FONTSIZE * 0.98, GREEN);
      DrawText(posy.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 2 + PADDING, FONTSIZE * 0.98, RED);
      DrawText(posz.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 3 + PADDING, FONTSIZE * 0.98, BLUE);
      DrawText(timeMsg.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 4 + PADDING * 2, FONTSIZE, DARKGRAY);
      DrawText(veloMsg.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 5 + PADDING * 2, FONTSIZE, DARKGRAY);
      DrawText(acceMsg.c_str(), GetScreenWidth() - (GetScreenWidth()  / 4) + PADDING, FONTSIZE * 6 + PADDING * 2, FONTSIZE, DARKGRAY);

      DrawLineEx(
        { rightSliderLimit, sliderY}, 
        { leftSliderLimit, sliderY}, 
        4,
        GRAY);

      if (sliderMarkerPos == NULL)
        sliderMarkerPos = leftSliderLimit;

      int radius = 8;

      DrawCircleV({sliderMarkerPos, sliderY}, (radius + 1), DARKGRAY);
      DrawCircleV({sliderMarkerPos, sliderY}, radius, RAYWHITE);

      if (CheckCollisionPointCircle(GetMousePosition(), { sliderMarkerPos, sliderY }, radius * 2)) {
        if (GetMouseX() >= leftSliderLimit && GetMouseX() <= rightSliderLimit) {
          sliderMarkerPos = GetMouseX();
          sliderValue = (particleTrace.front().w / 1000) - ((((GetMouseX() - leftSliderLimit) / (rightSliderLimit - leftSliderLimit)) * ((particleTrace.front().w - particleTrace.back().w) / 1000)));

          if (sliderValue < particleTrace.front().w / 1000) sliderValue = particleTrace.front().w / 1000;
          if (sliderValue > particleTrace.back().w / 1000) sliderValue = particleTrace.back().w / 1000;

          particle.position.x = xInterpolation.interpolate(sliderValue);
          particle.position.y = yInterpolation.interpolate(sliderValue);
          particle.position.z = zInterpolation.interpolate(sliderValue);
        }
      }

      if (IsKeyPressed(KEY_P)) {
        isPlotMode = !isPlotMode;
      }
      
      if (IsKeyPressed(KEY_N)) {
        isSimulationOver = false;
        particle =  RandomParticle(initialPos, ORANGE);
        initialTime = timedate.tv_sec * 1000 + timedate.tv_usec / 1000;
        isPaused = true;
        particleTrace.clear();
        sliderMarkerPos = leftSliderLimit;
        sliderValue = 0;
        xInterpolation = LagrangeTheorem({});
        yInterpolation = LagrangeTheorem({});
        zInterpolation = LagrangeTheorem({});
        isPlotMode = false;
      }
    } else {
    if (isPaused) {
      DrawText("simulation: PAUSED  [SPACE] - resume  [N] - new simulation  [ENTER]", PADDING, PADDING, FONTSIZE, DARKGRAY);
      if (IsKeyPressed(KEY_N)) {
        gettimeofday(&timedate, NULL);
        isSimulationOver = false;
        particle =  RandomParticle(initialPos, ORANGE);
        initialTime = (timedate.tv_sec * 1000 + timedate.tv_usec / 1000);
        isPaused = true;
        particleTrace.clear();
        sliderMarkerPos = leftSliderLimit;
        sliderValue = 0;
        xInterpolation = LagrangeTheorem({});
        yInterpolation = LagrangeTheorem({});
        zInterpolation = LagrangeTheorem({});
        isPlotMode = false;
      }
    } else
      DrawText("simulation: ON  [SPACE] - pause  [ENTER] - end simulation", PADDING, PADDING, FONTSIZE, DARKGRAY);
    std::string posText = "particle position: (" + std::to_string(particle.position.x) + ", " + std::to_string(particle.position.y) + ", " + std::to_string(particle.position.z) + ")";
    DrawText(posText.c_str(), PADDING, FONTSIZE * 1 + PADDING * 2, FONTSIZE, DARKGRAY);
  }
    EndDrawing();
  }

  CloseWindow();
  return 0;
  
}
