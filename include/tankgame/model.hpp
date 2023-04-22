#ifndef TANKGAME_MODEL_HPP
#define TANKGAME_MODEL_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/model.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <tankgame/utils.hpp>

namespace tankgame::model {

class Object {
 public:
  virtual glm::mat4 get_model_matrix() = 0;
};

class AttachableObject : public Object {
 public:

  AttachableObject(const glm::mat4 &default_model_matrix);

  /**
   * replace self transform matrix
   */
  void transform(const glm::mat4 &trans);

  glm::mat4 get_model_matrix() override;

  void append(AttachableObject *child);

 protected:
  glm::mat4 self_trans;
  glm::mat4 anchor_trans;
  glm::mat4 result_trans;
  std::vector<AttachableObject *> children;

  void init();
  void update_result_trans();

  /**
   * recursively update transform matrix of children
   * @param parent_result_trans
   */
  void recursive_transform(glm::mat4 parent_result_trans);
};

class NewtonObject {
 public:
  static GLfloat g;
  glm::vec3 x, a, v;
  NewtonObject(const glm::vec3 &x, const glm::vec3 &a, const glm::vec3 &v);
  void update(GLfloat delta_time);
};

class Particle {
 public:
  GLfloat life_time;
  GLfloat remain_time;

  Particle(GLfloat life_time, GLfloat remain_time);

  void update(GLfloat delta_time);

  GLboolean is_dead();
};

class FireParticle : public Particle, public NewtonObject, public Object {
 public:
  glm::vec4 color();
  float scale();
  FireParticle(const glm::vec3 &x, const glm::vec3 &v,
               GLfloat life_time, GLfloat remain_time,
               const glm::vec4 &color_start, const glm::vec4 &color_end);
  glm::mat4 get_model_matrix() override;
 protected:
  glm::vec4 color_start, color_end;
};

class Missile : public NewtonObject, public Object {
 public:

  Missile(const glm::vec3 &x, const glm::vec3 &v);

  glm::mat4 get_model_matrix() override;

  GLboolean is_under_ground();
};

class TankBottom {
 public:
  GLfloat move_speed, rotate_speed;
  glm::vec3 rotate_axis;
  glm::vec3 pos;
  glm::vec3 front;
  float yaw;
  AttachableObject model_calculator;

  explicit TankBottom(const glm::vec3 &init_pos = glm::vec3(0.0f, 0.0f, 0.0f),
                      float yaw = 0.0f,
                      GLfloat move_speed = 10.0f,
                      GLfloat rotate_speed = 20.0f,
                      const glm::vec3 &rotate_axis = glm::vec3(0.0f, 1.0f, 0.0f));

  void move(const std::string &dir, float delta_time);

  void rotate(const std::string &dir, float delta_time);

  glm::mat4 get_model_matrix();

  void update_model_matrix();

 protected:
  void update_front();

};

class TankUp {
  // TODO
 public:
  AttachableObject model_calculator;
  GLfloat rotate_speed, yaw;
  glm::vec3 rotate_axis;
  glm::vec3 pos;

  TankUp(const glm::vec3 &init_pos = glm::vec3(0.0f, 0.0f, 0.0f),
         GLfloat yaw = 0.0f,
         GLfloat rotate_speed = 20.0f,
         const glm::vec3 &rotate_axis = glm::vec3(0.0f, 1.0f, 0.0f));

  void set_yaw(GLfloat yaw);

  void rotate(const std::string &dir, GLfloat delta_time);

  glm::mat4 get_model_matrix();

  void update_model_matrix();

};

class Grass {
 public:
  std::vector<GLfloat> rotate_degrees = {0.0f}; //= {0.0f, 90.0f, 180.0f, 270.0f};
  GLfloat degree;
  glm::vec3 pos;
  glm::mat4 model_matrix;

  Grass(const glm::vec3 &pos);
};

class SkyBox {
 public:
  SkyBox(const std::vector<std::string> &faces, Shader &shader);

  unsigned int skyboxVAO, skyboxVBO, cubemapTexture;

  Shader &shader;

  void draw(const glm::mat4 &camera_view, const glm::mat4 &camera_projection);

};

}

#endif //TANKGAME_MODEL_HPP
