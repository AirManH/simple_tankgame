#ifndef TANKGAME_UTILS_HPP
#define TANKGAME_UTILS_HPP

#include <string>
#include <random>
#include <learnopengl/model.h>
#include <glm/glm.hpp>

namespace tankgame::utils {

static const glm::vec4 origin_point = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
static const glm::mat4 unit_mat4 = glm::mat4(1.0f);
static const GLfloat SCALE = 1.0f;
static const GLfloat VECTOR = 0.0f;

Model load_model(const std::string& path);

GLint uniform_int(GLint a, GLint b);

GLuint BindTexture(const std::string &path);

std::string abs_path(const std::string &path);

void replace_inplace(std::string &subject, const std::string &search, const std::string &replace);

void cout_vec3(const glm::vec3 &v);

unsigned int loadTexture(char const *path);

class range {
 public:
  class iterator {
    friend class range;
   public:
    long int operator*() const { return i_; }
    const iterator &operator++() {
      ++i_;
      return *this;
    }
    iterator operator++(int) {
      iterator copy(*this);
      ++i_;
      return copy;
    }

    bool operator==(const iterator &other) const { return i_ == other.i_; }
    bool operator!=(const iterator &other) const { return i_ != other.i_; }

   protected:
    iterator(long int start) : i_(start) {}

   private:
    unsigned long i_;
  };

  iterator begin() const { return begin_; }
  iterator end() const { return end_; }
  range(long int begin, long int end) : begin_(begin), end_(end) {}
 private:
  iterator begin_;
  iterator end_;
};

unsigned int loadCubemap(std::vector<std::string> faces);

/**
 * rotate transform of rotate a to b
 * @param a
 * @param b
 * @return
 */
glm::mat4 rotate_to(const glm::vec3& a, const glm::vec3& b);
}
#endif //TANKGAME_UTILS_HPP
