
#include "lib.h"

#include <gtest/gtest.h>

#include <SDL.h>
#include <frames/os_gl.h>
#include <frames/environment.h>
#include <frames/stream.h>
#include <frames/loader.h>
#include <frames/texture_config.h>

#include <png.h>

#include <cstdio>

TestSDLEnvironment::TestSDLEnvironment() : m_win(0), m_glContext(0) {
  EXPECT_EQ(SDL_Init(SDL_INIT_VIDEO), 0);

  m_win = SDL_CreateWindow("Frames test harness", 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  EXPECT_TRUE(m_win != NULL);

  m_width = 1280;
  m_height = 720;

  m_glContext = SDL_GL_CreateContext(m_win);
  EXPECT_TRUE(m_glContext != 0);

  glewInit();
}

TestSDLEnvironment::~TestSDLEnvironment() {
  if (!m_glContext) {
    SDL_GL_DeleteContext(m_glContext);
  }

  if (m_win) {
    SDL_DestroyWindow(m_win);
  }

  SDL_Quit();
}

int TestSDLEnvironment::GetWidth() const {
  return m_width;
}
int TestSDLEnvironment::GetHeight() const {
  return m_height;
}

void TestSDLEnvironment::Swap() {
  SDL_GL_SwapWindow(m_win);
}

void TestSDLEnvironment::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // We actually just ignore all events
  }
}

TestEnvironment::TestEnvironment() : m_env(0) {
  Frames::Configuration config;
  config.fontDefaultId = "test/LindenHill.otf";
  m_env = new Frames::Environment(config);
  m_env->ResizeRoot(GetWidth(), GetHeight()); // set this up so we can check coordinates
}

TestEnvironment::~TestEnvironment() {
  EXPECT_EQ(glGetError(), GL_NO_ERROR); // verify no GL issues
  delete m_env;
}

void TestSnapshot(TestEnvironment &env) {
  // Do the render
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  env->ResizeRoot(env.GetWidth(), env.GetHeight());
  env->Render();

  std::string testName = Frames::Utility::Format("test/ref/%s_%s", ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name(), ::testing::UnitTest::GetInstance()->current_test_info()->name());

  static std::string s_testNameLast = "";
  static int s_testIdLast = 0;

  if (testName != s_testNameLast) {
    s_testNameLast = testName;
    s_testIdLast = 0;
  }

  std::string testFilePrefix = Frames::Utility::Format("%s_%d", testName.c_str(), s_testIdLast++);
  std::string testFileName = testFilePrefix + ".png";
  std::string failureFileName = testFilePrefix + "_result.png";

  // We now have our test filename

  // Grab a screenshot
  std::vector<unsigned char> pixels; pixels.resize(4 * env.GetWidth() * env.GetHeight());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, env.GetWidth(), env.GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  EXPECT_EQ(glGetError(), GL_NO_ERROR);

  // Annoyingly, OpenGL reads coordinates in math quadrant order, not scanline order like the rest of the civilized computer world
  // So . . . go ahead and invert the entire array
  for (int y = 0; y < env.GetHeight() / 2; ++y) {
    for (int x = 0; x < env.GetWidth() * 4; ++x) {
      std::swap(pixels[y * env.GetWidth() * 4 + x], pixels[(env.GetHeight() - 1 - y) * env.GetWidth() * 4 + x]);
    }
  }

  // Grab our source file (or try to)
  std::vector<unsigned char> reference;
  {
    Frames::StreamFile *stream = Frames::StreamFile::Create(testFileName);
    if (stream)
    {
      Frames::TextureConfig tex = Frames::Loader::PNG::Load(*env, stream);
      EXPECT_EQ(tex.GetMode(), Frames::TextureConfig::RAW);
      EXPECT_EQ(tex.Raw_GetType(), Frames::TextureConfig::MODE_RGBA);
      EXPECT_EQ(Frames::TextureConfig::GetBPP(Frames::TextureConfig::MODE_RGBA), 4);
      EXPECT_EQ(tex.Raw_GetStride(), tex.GetWidth() * 4);
      reference.resize(tex.GetWidth() * tex.GetHeight() * 4);
      memcpy(reference.data(), tex.Raw_GetData(), tex.GetWidth() * tex.GetHeight() * 4);
      delete stream;
    }
  }

  std::string writeFileName;
  if (reference.empty()) {
    writeFileName = testFileName;
  } else {
    writeFileName = failureFileName;
  }

  if (!writeFileName.empty()) {
    // For now, we just write to disk
    // Don't need this anywhere in Frames so we'll just hack it in here

    FILE *fp = fopen(writeFileName.c_str(), "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);

    png_set_IHDR(png_ptr, info_ptr, env.GetWidth(), env.GetHeight(),
      8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    std::vector<unsigned char *> rows;
    for (int i = 0; i < env.GetHeight(); ++i) {
      rows.push_back(pixels.data() + i * env.GetWidth() * 4);
    }

    png_write_image(png_ptr, rows.data());

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
  }

  EXPECT_EQ(pixels.size(), reference.size());
  EXPECT_TRUE(pixels == reference);
}

void HaltAndRender(TestEnvironment &env) {
  while (true) {
    // Do the render
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    env->ResizeRoot(env.GetWidth(), env.GetHeight());
    env->Render();
    env.Swap();
    env.HandleEvents();
  }
}
