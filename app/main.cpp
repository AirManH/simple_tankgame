#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <tankgame/utils.hpp>
#include <tankgame/controller.hpp>
#include <tankgame/model.hpp>
#include <tankgame/display.hpp>

#include <iostream>

namespace tg = tankgame;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
// TODO
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// tps_camera
// TODO
// Camera tps_camera(glm::vec3(0.0f, 0.0f, 3.0f));

// set TPS tps_camera

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


// ----- GLOBAL -----

// wild point
tg::display::TPSCamera *tps_camera_global_pointer;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "tank_game", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // scroll for tps_camera
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // ----- configure global opengl state -----
  glEnable(GL_DEPTH_TEST);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // ----- Shaders -----
  Shader shader(tg::utils::abs_path("./resource/shader/shadow_mapping_v.glsl").c_str(),
                tg::utils::abs_path("./resource/shader/shadow_mapping_f.glsl").c_str());
  Shader simpleDepthShader(tg::utils::abs_path("./resource/shader/shadow_mapping_depth_v.glsl").c_str(),
                           tg::utils::abs_path("./resource/shader/shadow_mapping_depth_f.glsl").c_str());
  Shader skybox_shader(tg::utils::abs_path("./resource/shader/sky_box_v.glsl").c_str(),
                       tg::utils::abs_path("./resource/shader/sky_box_f.glsl").c_str());
  // vvvvv Models BEGIN vvvvv
  // tank up
  Model tank_bottom_model(tg::utils::abs_path("./resource/model/tank/bottom.obj"));
  auto tank_bottom_locator = tg::model::TankBottom(glm::vec3(0.0f, 0.8f, 0.0f),
                                                   0.0f,
                                                   5.0f,
                                                   40.0f);
  // tank bottom
  Model tank_up_model(tg::utils::abs_path("./resource/model/tank/up.obj"));
  auto tank_up_locator = tg::model::TankUp(glm::vec3(0.0f, 1.15f, 0.0f));
  tank_bottom_locator.model_calculator.append(&(tank_up_locator.model_calculator));
  // camera focus
  auto camera_focus = tg::model::AttachableObject(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f)));
  tank_up_locator.model_calculator.append(&camera_focus);
  // camera
  auto tps_camera = tg::display::TPSCamera(camera_focus, glm::vec3(0.0f, 10.0f, 10.0f));
  tps_camera_global_pointer = &tps_camera;
  // grass
  Model grass_model(tg::utils::abs_path("./resource/model/grass/grass_cube.obj"));
  auto grass_positions = tg::control::generate_grass_positions(20, 20, 0.0f, 2.0f);
  auto grass_model_getters = std::vector<tg::model::Grass>();
  for (auto &pos : grass_positions) {
    grass_model_getters.emplace_back(pos);
  }
  // fire particle
  auto fire_model = tg::utils::load_model("./resource/model/cube/cube.obj");
  auto fire_shader = Shader(tg::utils::abs_path("./resource/shader/particle_v.glsl").c_str(),
                            tg::utils::abs_path("./resource/shader/particle_f.glsl").c_str());
  // missile manager
  auto missile_model = tg::utils::load_model("./resource/model/bullet/grass_bullet.obj");
  auto tank_shoot_point = tg::model::AttachableObject(
      glm::translate(tg::utils::unit_mat4, {0.0f, 0.0f, 4.5f}));
  tank_up_locator.model_calculator.append(&tank_shoot_point);
  auto missile_manager = tg::control::MissileManager(window,
                                                     tank_up_locator,
                                                     tank_bottom_locator,
                                                     tank_shoot_point,
                                                     0.5f,
                                                     10.0f,
                                                     0.01f,
                                                     0.5f);
  // skybox
  tg::model::SkyBox sky_box(
      {tg::utils::abs_path("./resource/texture/skybox/right.png"),
       tg::utils::abs_path("./resource/texture/skybox/left.png"),
       tg::utils::abs_path("./resource/texture/skybox/up.png"),
       tg::utils::abs_path("./resource/texture/skybox/down.png"),
       tg::utils::abs_path("./resource/texture/skybox/front.png"),
       tg::utils::abs_path("./resource/texture/skybox/back.png")},
      skybox_shader);
  // ^^^^^ Models END ^^^^^

  // ----- configure depth map FBO -----

  const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  // create depth texture
  unsigned int depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_DEPTH_COMPONENT,
               SHADOW_WIDTH,
               SHADOW_HEIGHT,
               0,
               GL_DEPTH_COMPONENT,
               GL_FLOAT,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  // attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // ----- shader configuration -----

  shader.use();
  shader.setInt("diffuseTexture", 0);
  shader.setInt("shadowMap", 10);

  skybox_shader.use();
  skybox_shader.setInt("skybox", 0);

  // ----- lighting info -----

  glm::vec3 lightPos(20.0f, 20.0f, 20.0f);

  // ------ input ------

  tg::control::MouseManger mouse_manger(window);
  mouse_manger.add(mouse_callback);
  tg::control::KeyManager key_manager(window);
  key_manager.add(tg::control::close_window_on_esc);

  while (!glfwWindowShouldClose(window)) {

    // per-frame time logic

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // ----- input -----
    mouse_manger.run_all();
    key_manager.run_all();
    tg::control::interact_tank_bottom(tank_bottom_locator, window, deltaTime);
    tg::control::interact_tank_up(tank_up_locator, window, deltaTime);
    tg::control::set_tank_up_yaw_by_camera(window, tank_up_locator, tank_bottom_locator, tps_camera);
    missile_manager.wait_click_fire();
    missile_manager.update(deltaTime);
    missile_manager.clean();

    // ----- Render Shadow -----

    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = -100.0f, far_plane = 200.0f;
    //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
    lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    // render scene from light's point of view
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);

    // --- normal render BEGIN ---

    // tank
    simpleDepthShader.setMat4("model", tank_bottom_locator.get_model_matrix());
    tank_bottom_model.Draw(shader);
    simpleDepthShader.setMat4("model", tank_up_locator.get_model_matrix());
    tank_up_model.Draw(shader);
    // grass block
    for (const auto &model_getter: grass_model_getters) {
      simpleDepthShader.setMat4("model", model_getter.model_matrix);
      grass_model.Draw(shader);
    }
    // missiles
    for (auto &missile: missile_manager.missiles) {
      simpleDepthShader.setMat4("model", missile.get_model_matrix());
      missile_model.Draw(shader);
    }

    // --- normal render STOP ---

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // reset viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // vvvvv Normal Render vvvvv

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    // view/projection transformations
    glm::mat4 projection = glm::perspective(
        glm::radians(tps_camera.fov), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = tps_camera.get_view_matrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // set light
    shader.setVec3("viewPos", tps_camera.get_eye_pos());
    shader.setVec3("lightPos", lightPos);
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // render the loaded model
    // tank
    shader.setMat4("model", tank_bottom_locator.get_model_matrix());
    tank_bottom_model.Draw(shader);
    shader.setMat4("model", tank_up_locator.get_model_matrix());
    tank_up_model.Draw(shader);
    // grass
    for (const auto &model_getter: grass_model_getters) {
      shader.setMat4("model", model_getter.model_matrix);
      grass_model.Draw(shader);
    }
    // missile
    for (auto &missile: missile_manager.missiles) {
      shader.setMat4("model", missile.get_model_matrix());
      missile_model.Draw(shader);
    }
    // sky box
    sky_box.draw(tps_camera.get_view_matrix(), projection);
    // fires use different shader
    fire_shader.use();
    fire_shader.setMat4("projection", projection);
    fire_shader.setMat4("view", view);
    for (auto &fire: missile_manager.fires) {
      fire_shader.setMat4("model", fire.get_model_matrix());
      fire_shader.setVec4("particle_color", fire.color());
      fire_model.Draw(fire_shader);
    }

    // ^^^^^ Normal Render ^^^^^

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  // delete tps_camera_global_pointer;
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  tps_camera_global_pointer->process_mouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  tps_camera_global_pointer->process_scroll(yoffset);
}