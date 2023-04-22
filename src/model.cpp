//
// Created by AirMa on 2019/12/20.
//

#include "tankgame/model.hpp"

#include <glm/gtx/matrix_decompose.hpp>

namespace tankgame::model {

AttachableObject::AttachableObject(const glm::mat4 &default_model_matrix)
    : self_trans(default_model_matrix),
      anchor_trans(1.0f),
      result_trans(default_model_matrix) {
  this->init();
}

void AttachableObject::transform(const glm::mat4 &trans) {
  this->self_trans = trans;
  this->update_result_trans();
  this->recursive_transform(this->result_trans);
}

void AttachableObject::update_result_trans() {
  this->result_trans = this->anchor_trans * this->self_trans;
}

void AttachableObject::recursive_transform(glm::mat4 parent_result_trans) {
  for (auto &child: children) {
    child->anchor_trans = parent_result_trans;
    child->update_result_trans();
    child->recursive_transform(child->result_trans);
  }
}

glm::mat4 AttachableObject::get_model_matrix() {
  this->update_result_trans();
  return this->result_trans;
}

void AttachableObject::init() {
  this->recursive_transform(this->result_trans);
}
void AttachableObject::append(AttachableObject *child) {
  this->children.push_back(child);
  this->recursive_transform(this->result_trans);
}

GLfloat NewtonObject::g = 9.8f;

void NewtonObject::update(GLfloat delta_time) {
  this->v += this->a * delta_time;
  this->x += this->v * delta_time;
}
NewtonObject::NewtonObject(const glm::vec3 &x, const glm::vec3 &a, const glm::vec3 &v) : x(x), a(a), v(v) {}

TankBottom::TankBottom(const glm::vec3 &init_pos,
                       float yaw,
                       GLfloat move_speed,
                       GLfloat rotate_speed,
                       const glm::vec3 &rotate_axis)
    : model_calculator(glm::mat4(1.0f)),
      move_speed(move_speed),
      rotate_speed(rotate_speed),
      rotate_axis(rotate_axis),
      pos(init_pos),
      yaw(yaw) {
  this->update_front();
  this->update_model_matrix();
}

void TankBottom::move(const std::string &dir, float delta_time) {
  float speed = this->move_speed * delta_time;
  if (dir == "u") {
    this->pos = this->pos + (this->front * speed);
  } else if (dir == "d") {
    this->pos = this->pos - (this->front * speed);
  }
  this->update_model_matrix();

//  std::cout << "pos" << std::endl;
//  utils::cout_vec3(this->pos);
//  std::cout << "front" << std::endl;
//  utils::cout_vec3(this->front);
}

void TankBottom::rotate(const std::string &dir, float delta_time) {
  float delta_degree = this->rotate_speed * delta_time;
  if (dir == "l") {
    this->yaw += delta_degree;
  } else if (dir == "r") {
    this->yaw -= delta_degree;
  }
  this->update_front();
  this->update_model_matrix();
}

void TankBottom::update_front() {
  glm::vec3 new_front(glm::sin(glm::radians(this->yaw)),
                      0.0f,
                      glm::cos(glm::radians(this->yaw)));
  this->front = glm::normalize(new_front);
}
void TankBottom::update_model_matrix() {
  glm::mat4 model(1.0f);
  model = glm::translate(model, this->pos);
  model = glm::rotate(model, glm::radians(this->yaw), this->rotate_axis);
  this->model_calculator.transform(model);
}

glm::mat4 TankBottom::get_model_matrix() {
  return this->model_calculator.get_model_matrix();
}

TankUp::TankUp(const glm::vec3 &init_pos,
               GLfloat yaw,
               GLfloat rotate_speed,
               const glm::vec3 &rotate_axis)
    : pos(init_pos),
      yaw(yaw),
      rotate_speed(rotate_speed),
      model_calculator(glm::mat4(1.0f)),
      rotate_axis(rotate_axis) {
  this->update_model_matrix();
}

void TankUp::set_yaw(GLfloat yaw) {
  this->yaw = yaw;
  this->update_model_matrix();
}

glm::mat4 TankUp::get_model_matrix() {
  return this->model_calculator.get_model_matrix();
}

void TankUp::update_model_matrix() {
  auto model = glm::mat4(1.0f);
  model = glm::translate(model, this->pos);
  model = glm::rotate(model, glm::radians(this->yaw), this->rotate_axis);
  this->model_calculator.transform(model);
}

void TankUp::rotate(const std::string &dir, GLfloat delta_time) {
  GLfloat delta_degree = this->rotate_speed * delta_time;
  if (dir == "l") {
    this->yaw += delta_degree;
  } else if (dir == "r") {
    this->yaw -= delta_degree;
  }
  this->update_model_matrix();
}

Grass::Grass(const glm::vec3 &pos) : pos(pos), model_matrix(1.0f) {
  this->degree = this->rotate_degrees[utils::uniform_int(0, this->rotate_degrees.size() - 1)];
  glm::mat4 m(1.0f);
  m = glm::translate(m, this->pos);
  m = glm::rotate(m, glm::radians(this->degree), glm::vec3(0.0f, 1.0f, 0.0f));
  this->model_matrix = m;
}

SkyBox::SkyBox(const std::vector<std::string> &faces, Shader &shader) : shader(shader) {

  float skyboxVertices[] = {
      // positions
      -1.0f, 1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, -1.0f, 1.0f,
      -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, -1.0f,
      1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, -1.0f,
      1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, 1.0f
  };
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
  this->cubemapTexture = utils::loadCubemap(faces);
}
void SkyBox::draw(const glm::mat4 &camera_view, const glm::mat4 &camera_projection) {
  // draw skybox as last
  glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
  this->shader.use();
  auto view = glm::mat4(glm::mat3(camera_view)); // remove translation from the view matrix
  this->shader.setMat4("view", view);
  this->shader.setMat4("projection", camera_projection);
  // skybox cube
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default
}

Particle::Particle(GLfloat life_time, GLfloat remain_time) : life_time(life_time), remain_time(remain_time) {}

void Particle::update(GLfloat delta_time) {
  if (!is_dead()) {
    this->remain_time -= delta_time;
  }
}
GLboolean Particle::is_dead() {
  return remain_time < 0.0f;
}

Missile::Missile(const glm::vec3 &x, const glm::vec3 &v)
    : NewtonObject(x, {0.0f, -NewtonObject::g, 0.0f}, v) {}

glm::mat4 Missile::get_model_matrix() {

  auto rotate = utils::rotate_to({0.0f, 0.0f, 1.0f}, this->v);
  auto translate = glm::translate(glm::mat4(1.0f), this->x);
  return translate * rotate;
}
GLboolean Missile::is_under_ground() {
  return this->x.y < 0.0f;
}

FireParticle::FireParticle(const glm::vec3 &x, const glm::vec3 &v,
                           GLfloat life_time, GLfloat remain_time,
                           const glm::vec4 &color_start, const glm::vec4 &color_end)
    : Particle(life_time, remain_time),
      NewtonObject(x, {0.0f, 0.0f, 0.0f}, v),
      color_start(color_start),
      color_end(color_end) {}

glm::vec4 FireParticle::color() {
  auto diff = this->color_end - this->color_start;
  return this->color_end - diff * (this->remain_time / this->life_time);
}

float FireParticle::scale() {
  return this->remain_time / this->life_time;
}
glm::mat4 FireParticle::get_model_matrix() {
  auto rotate = utils::rotate_to({0.0f, 0.0f, 1.0f}, this->v);
  auto scale = glm::scale(utils::unit_mat4, glm::vec3(this->scale()));
  auto translate = glm::translate(glm::mat4(1.0f), this->x);
  return translate * rotate * scale;
}

}