#ifndef TANKGAME_DISPLAY_HPP
#define TANKGAME_DISPLAY_HPP

#include <learnopengl/camera.h>
#include <learnopengl/shader.h>
#include <vector>
#include <tuple>
#include <tankgame/model.hpp>

namespace tankgame::display {
//void apply_FPS_camera_to_shader(blockshoot::control::Setting &setting,
//                                Camera &camera,
//                                Shader &shader);

class TPSCamera {
 public:
  // Camera Attributes
  model::AttachableObject &target;
  /** <b>normalized</b> vector, its length is <code>this->dis</code> */
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 world_up;
  GLfloat dis; // distance to target
  // Euler Angles
  float yaw;
  float pitch;
  // Camera options
  float move_speed;
  float mouse_sensitivity;
  float fov;
  //
  static float DEFAULT_SPEED,
      DEFAULT_SENSITIVITY,
      DEFAULT_YAW,
      DEFAULT_PITCH,
      DEFAULT_ZOOM;
  vector<float> FOV_RANGE = {1.0f, 70.0f};
  vector<float> PITCH_RANGE = {-89.0f, 89.0f};

  /**
   * To use this, make sure that the center of object
   * is (0,0,0) after first load
   * @param target
   * @param init_pos
   */
  TPSCamera(model::AttachableObject &target,
            glm::vec3 init_pos,
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

  void reset_target(model::AttachableObject &tgt);

  glm::mat4 get_view_matrix();

  glm::vec3 get_eye_pos();

  void process_scroll(float y_offset);

  void process_mouse(float x_offset, float y_offset, GLboolean constrain_pitch = true);

 protected:
  glm::vec3 get_target_center();
  /**
   * update <b>up, right, front</b> based on <b>pitch, yaw</b>
   */
  void update_camera_vector();
};

}
#endif //TANKGAME_DISPLAY_HPP
