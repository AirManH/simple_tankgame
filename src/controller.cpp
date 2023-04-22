//
// Created by AirMa on 12/15/2019.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <tankgame/controller.hpp>

namespace tankgame::control {

void close_window_on_esc(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void move_camera(GLFWwindow *window, Camera &camera, const float &deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}
void interact_tank_bottom(model::TankBottom &tank_bottom, GLFWwindow *window, GLfloat delta_time) {

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      tank_bottom.rotate("r", delta_time);
    } else {
      tank_bottom.rotate("l", delta_time);
    }
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      tank_bottom.rotate("l", delta_time);
    } else {
      tank_bottom.rotate("r", delta_time);
    }
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    tank_bottom.move("u", delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    tank_bottom.move("d", delta_time);
  }
}
void interact_tank_up(model::TankUp &tank_up, GLFWwindow *window, GLfloat delta_time) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    tank_up.rotate("l", delta_time);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    tank_up.rotate("r", delta_time);
  }
}

void set_tank_up_yaw_by_camera(GLFWwindow *window,
                               model::TankUp &tank_up,
                               model::TankBottom &tank_bottom,
                               display::TPSCamera &tps_camera) {
  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
  if (state == GLFW_PRESS) {
    // TODO ugly here
    tank_up.set_yaw(-tps_camera.yaw - tank_bottom.yaw + 90.0f);
  }
}
std::vector<glm::vec3> generate_grass_positions(GLuint n_x, GLuint n_z, GLfloat y_pos, GLfloat half_side_length) {
  vector<glm::vec3> positions;
  for (auto i: utils::range(-n_x, n_x)) {
    for (auto k : utils::range(-n_z, n_z)) {
      positions.emplace_back(i * half_side_length, y_pos, k * half_side_length);
    }
  }
  return positions;
}

glm::vec3 get_tank_up_front(const model::TankUp &tank_up,
                            const model::TankBottom &tank_bottom) {
  // TODO ugly code
  auto final_yaw = tank_up.yaw + tank_bottom.yaw;
  auto rotate_matrix = glm::rotate(glm::mat4(1.0f),
                                   glm::radians(final_yaw),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
  auto init_front = glm::vec4(0.0f, 0.0f, -1.0f, utils::VECTOR);
  return -rotate_matrix * init_front;
}

void MissileManager::wait_click_fire() {
  if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
      || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    if (glfwGetTime() - this->last_shoot > this->shoot_interval) {
      // calculate speed
      float speed = 70.0f;
      // generate a missile
      auto init_pos = glm::vec3(tank_shoot_point.get_model_matrix() * utils::origin_point);
      auto front = glm::normalize(get_tank_up_front(tank_up, tank_bottom));
      auto missile = model::Missile(init_pos, front * speed);
      this->missiles.push_back(missile);
      // update time variables
      this->last_shoot = glfwGetTime();
    }
  }
}

void MissileManager::update(float delta_time) {
  for (auto &missile : this->missiles) {
    missile.update(delta_time);
  }
  for (auto &fire :this->fires) {
    fire.NewtonObject::update(delta_time);
    fire.Particle::update(delta_time);
  }
  for (auto &missile : this->missiles) {
    auto fire_number = max(GLuint(1), GLuint(fire_generate_speed * delta_time));
    // TODO ugly, magic code. Default fire set here.
    // color
    auto color_begin = glm::vec4(0.8f, 0.0f, 0.0f, 1.0f);
    auto color_end = glm::vec4(0.8f, 1.0f, 0.0f, 0.2f);
    // physics
    auto init_pos = missile.x;
    auto init_v = -glm::normalize(missile.v) * this->fire_speed;
    auto life_time = 0.5f;
    // new fire
    auto default_fire = model::FireParticle(init_pos, init_v,
                                            life_time, life_time,
                                            color_begin, color_end);
    for (auto i : utils::range(0, fire_number)) {
      this->fires.push_back(default_fire);
    }
  }
}

void MissileManager::clean() {
  while (!this->missiles.empty() && missiles[0].is_under_ground()) {
    this->missiles.erase(this->missiles.begin());
  }
  while (!this->fires.empty() && fires[0].is_dead()) {
    this->fires.erase(this->fires.begin());
  }
}

MissileManager::MissileManager(GLFWwindow *window,
                               model::TankUp &tank_up,
                               model::TankBottom &tank_bottom,
                               model::AttachableObject &tank_shoot_point,
                               double shoot_interval,
                               GLfloat fire_generate_speed,
                               float fire_remain_time,
                               float fire_speed)
    : window(window),
      shoot_interval(shoot_interval),
      tank_up(tank_up),
      tank_bottom(tank_bottom),
      tank_shoot_point(tank_shoot_point),
      fire_generate_speed(fire_generate_speed),
      fire_life_time(fire_remain_time),
      fire_speed(fire_speed) {
  last_shoot = glfwGetTime();
}
}