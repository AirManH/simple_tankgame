//
// Created by AirMa on 12/15/2019.
//
#include <tankgame/display.hpp>
#include <tankgame/utils.hpp>

namespace tankgame::display {

//void apply_FPS_camera_to_shader(blockshoot::control::Setting &setting,
//                                Camera &camera,
//                                Shader &shader) {
//  auto projection_matrix = glm::perspective(
//      glm::radians(camera.Zoom),
//      (GLfloat) setting.window_width / (GLfloat) setting.window_height,
//      0.1f,
//      100.0f);
//  auto view_matrix = camera.GetViewMatrix();
//  shader.setMat4("projection", projection_matrix);
//  shader.setMat4("view", view_matrix);
//}



float TPSCamera::DEFAULT_YAW = 90.0f;
float TPSCamera::DEFAULT_PITCH = 0.0f;
float TPSCamera::DEFAULT_SPEED = 2.5f;
float TPSCamera::DEFAULT_SENSITIVITY = 0.1f;
float TPSCamera::DEFAULT_ZOOM = 45.0f;

TPSCamera::TPSCamera(model::AttachableObject &target,
                     glm::vec3 init_pos,
                     glm::vec3 up)
    : target(target),
      move_speed(DEFAULT_SPEED),
      mouse_sensitivity(DEFAULT_SENSITIVITY),
      yaw(DEFAULT_YAW),
      pitch(DEFAULT_PITCH),
      fov(DEFAULT_ZOOM),
      world_up(up) {
  this->front = glm::normalize(this->get_target_center() - init_pos);
  this->dis = glm::length(this->get_target_center() - init_pos);
  this->PITCH_RANGE[1] = glm::degrees(asin(this->get_target_center().y / this->dis));
}

void TPSCamera::reset_target(model::AttachableObject &tgt) {
  auto old_pos = this->get_target_center() - this->front * this->dis;
  this->target = tgt;
  this->front = glm::normalize(this->get_target_center() - old_pos);
  this->dis = glm::length(this->get_target_center() - old_pos);
  this->PITCH_RANGE[1] = glm::radians(atan2(this->get_target_center().y, this->dis));
  this->update_camera_vector();
}

glm::mat4 TPSCamera::get_view_matrix() {
  auto center = this->get_target_center();
  return glm::lookAt(center - this->front * this->dis, center, this->up);
}

void TPSCamera::process_scroll(float y_offset) {
  auto low = FOV_RANGE[0];
  auto high = FOV_RANGE[1];
  if (low <= fov && fov <= high) {
    fov -= y_offset;
  }
  if (fov <= low) {
    fov = low;
  }
  if (high <= fov) {
    fov = high;
  }
}

glm::vec3 TPSCamera::get_target_center() {
  // we only need translate matrix
  return target.get_model_matrix() * utils::origin_point;
}

void TPSCamera::process_mouse(float x_offset, float y_offset, GLboolean constrain_pitch) {
  x_offset *= this->mouse_sensitivity;
  y_offset *= this->mouse_sensitivity;

  this->yaw += x_offset;
  this->pitch += y_offset;

  if (constrain_pitch) {
    auto low = this->PITCH_RANGE[0];
    auto high = this->PITCH_RANGE[1];
    if (this->pitch > high) {
      this->pitch = high;
    }
    if (this->pitch < low) {
      this->pitch = low;
    }
  }

  this->update_camera_vector();
}

void TPSCamera::update_camera_vector() {
  glm::vec3 new_front;
  new_front.x = glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
  new_front.y = glm::sin(glm::radians(this->pitch));
  new_front.z = glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));

  this->front = glm::normalize(new_front);
  this->right = glm::normalize(glm::cross(this->front, this->world_up));
  this->up = glm::normalize(glm::cross(this->right, this->front));
}

glm::vec3 TPSCamera::get_eye_pos() {
  auto center = this->get_target_center();
  return center - this->front * this->dis;
}
}
