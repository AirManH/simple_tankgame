#ifndef TANKGAME_CONTROLLER_HPP
#define TANKGAME_CONTROLLER_HPP

#include <learnopengl/camera.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tankgame/model.hpp>
#include <tankgame/display.hpp>

namespace tankgame::control {

typedef void (*BSkeyfun)(GLFWwindow *);

void close_window_on_esc(GLFWwindow *window);

void interact_tank_bottom(model::TankBottom &tank_bottom, GLFWwindow *window, GLfloat delta_time);

void interact_tank_up(model::TankUp &tank_up, GLFWwindow *window, GLfloat delta_time);

void set_tank_up_yaw_by_camera(GLFWwindow *window,
                               model::TankUp &tank_up,
                               model::TankBottom &tank_bottom,
                               display::TPSCamera &tps_camera);
// TODO ugly design
glm::vec3 get_tank_up_front(const model::TankUp &tank_up,
                            const model::TankBottom &tank_bottom);

void move_camera(GLFWwindow *window, Camera &camera, const float &deltaTime);

std::vector<glm::vec3> generate_grass_positions(GLuint n_x, GLuint n_z, GLfloat y_pos, GLfloat half_side_length);

class Setting {
 public:
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;
  GLuint window_width = 1080;
  GLuint window_height = 720;
  GLfloat lastX = 0.0f;
  GLfloat lastY = 0.0f;
  GLboolean firstMouse = true; // if it is first time we get mouse position
  Camera &camera;

  Setting(GLuint window_width,
          GLuint window_height,
          Camera &camera)
      : camera(camera) {
    this->lastX = this->window_width / 2.0f;
    this->lastY = this->window_height / 2.0f;
  };

};

// TODO create a "Manager" Interface

class MouseManger {
 public:
  explicit MouseManger(GLFWwindow *window) : window(window) {}

  void add(GLFWcursorposfun mouse_fun) {
    this->mouse_callbacks.push_back(mouse_fun);
  }

  void run_all() {
    double x, y;
    glfwGetCursorPos(this->window, &x, &y);
    for (auto mouse_callback : this->mouse_callbacks) {
      mouse_callback(this->window, x, y);
    }
  }
 private:
  GLFWwindow *window;
  std::vector<GLFWcursorposfun> mouse_callbacks;

};

class KeyManager {
 public:
  std::vector<BSkeyfun> key_callbacks;

  explicit KeyManager(GLFWwindow *window) : window(window) {}

  void add(BSkeyfun key_fun) {
    this->key_callbacks.push_back(key_fun);
  }

  void run_all() {
    for (auto key_fun : this->key_callbacks) {
      key_fun(this->window);
    }
  }

 private:
  GLFWwindow *window;
};

class MissileManager {
 public:
  std::vector<model::Missile> missiles;
  std::vector<model::FireParticle> fires;
  GLfloat fire_generate_speed, fire_life_time, fire_speed;
  GLFWwindow *window;
  double shoot_interval, last_shoot;
  model::TankUp &tank_up;
  model::TankBottom &tank_bottom;
  model::AttachableObject &tank_shoot_point;

  MissileManager(GLFWwindow *window,
                 model::TankUp &tank_up,
                 model::TankBottom &tank_bottom,
                 model::AttachableObject &tank_shoot_point,
                 double shoot_interval = 0.5f,
                 float fire_generate_speed = 10.0f,
                 float fire_remain_time = 0.1f,
                 float fire_speed = 10.0f);

  void wait_click_fire();

  /**
   * update missiles' location and generate fire
   * @param delta_time
   */
  void update(float delta_time);

  void clean();
};
}
#endif // TANKGAME_CONTROLLER_HPP
