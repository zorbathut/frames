
#include <gtest/gtest.h>

#include <SDL.h>
#include <frames/os_gl.h>
#include <frames/environment.h>
#include <frames/frame.h>
#include <frames/stream.h>
#include <frames/loader.h>
#include <frames/texture_config.h>

#include <png.h>

#include <cstdio>

int s_width = 0;
int s_height = 0;

// Sets up a working test environment
class TestSDLEnvironment {
public:
  TestSDLEnvironment() : m_win(0), m_glContext(0) {
    EXPECT_EQ(SDL_Init(SDL_INIT_VIDEO), 0);

    m_win = SDL_CreateWindow("Frames test harness", 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    EXPECT_TRUE(m_win != NULL);

    s_width = 1280;
    s_height = 720;

    m_glContext = SDL_GL_CreateContext(m_win);
    EXPECT_TRUE(m_glContext != 0);

    glewInit();
  }

  ~TestSDLEnvironment() {
    if (!m_glContext) {
      SDL_GL_DeleteContext(m_glContext);
    }

    if (m_win) {
      SDL_DestroyWindow(m_win);
    }

    SDL_Quit();
  }

private:
  SDL_Window *m_win;
  SDL_GLContext m_glContext;
};

class TestEnvironment {
public:
  TestEnvironment() : m_env(0) {
    m_env = new Frames::Environment();
    m_env->ResizeRoot(s_width, s_height); // set this up so we can check coordinates
  }

  ~TestEnvironment() {
    EXPECT_EQ(glGetError(), GL_NO_ERROR); // verify no GL issues
    delete m_env;
  }

  Frames::Environment *operator*() {
    return m_env;
  }
  Frames::Environment *operator->() {
    return m_env;
  }

private:
  TestSDLEnvironment m_sdl; // mostly taken care of with constructor/destructor
  Frames::Environment *m_env;
};

void TestSnapshot(TestEnvironment &env) {
  // Do the render
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  env->ResizeRoot(s_width, s_height);
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
  std::vector<unsigned char> pixels; pixels.resize(4 * s_width * s_height);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, s_width, s_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
  EXPECT_EQ(glGetError(), GL_NO_ERROR);

  // Annoyingly, OpenGL reads coordinates in math quadrant order, not scanline order like the rest of the civilized computer world
  // So . . . go ahead and invert the entire array
  for (int y = 0; y < s_height / 2; ++y) {
    for (int x = 0; x < s_width * 4; ++x) {
      std::swap(pixels[y * s_width * 4 + x], pixels[(s_height - 1 - y) * s_width * 4 + x]);
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

    png_set_IHDR(png_ptr, info_ptr, s_width, s_height,
      8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    std::vector<unsigned char *> rows;
    for (int i = 0; i < s_height; ++i) {
      rows.push_back(pixels.data() + i * s_width * 4);
    }

    png_write_image(png_ptr, rows.data());

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
  }

  EXPECT_EQ(pixels.size(), reference.size());
  EXPECT_TRUE(pixels == reference);
}

TEST(Layout, SetBackground) {
  TestEnvironment env;

  Frames::Frame *frame = Frames::Frame::CreateTagged(env->GetRoot());
  frame->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(1.f, 1.f, 1.f, 1.0f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.2f));
  TestSnapshot(env);

  frame->SetBackground(Frames::Color(0.2f, 0.3f, 0.7f, 0.8f));
  TestSnapshot(env);
};

TEST(Layout, SetPoint) {
  TestEnvironment env;

  Frames::Frame *red = Frames::Frame::CreateTagged(env->GetRoot());
  red->SetBackground(Frames::Color(1.f, 0.f, 0.f, 0.5f)); // Partially transparent so we can see frame intersections

  red->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::BOTTOMRIGHT);
  EXPECT_EQ(red->GetLeft(), 0);
  EXPECT_EQ(red->GetRight(), s_width);
  EXPECT_EQ(red->GetTop(), 0);
  EXPECT_EQ(red->GetBottom(), s_height);

  TestSnapshot(env);

  red->ClearAllPoints();
  red->SetPoint(Frames::TOPLEFT, env->GetRoot(), Frames::TOPLEFT);
  red->SetPoint(Frames::BOTTOMRIGHT, env->GetRoot(), Frames::CENTER);

  Frames::Frame *green = Frames::Frame::CreateTagged(env->GetRoot());
  green->SetBackground(Frames::Color(0.f, 1.f, 0.f, 0.5f));
  green->SetPoint(Frames::CENTER, env->GetRoot(), 0.75f, 0.75f);
  green->SetPoint(Frames::TOPRIGHT, env->GetRoot(), Frames::CENTERRIGHT);

  Frames::Frame *blue = Frames::Frame::CreateTagged(env->GetRoot());
  blue->SetBackground(Frames::Color(0.f, 0.f, 1.f, 0.5f));
  blue->SetPoint(Frames::BOTTOMLEFT, env->GetRoot(), Frames::CENTER);
  blue->SetPoint(Frames::TOPRIGHT, env->GetRoot(), Frames::TOPRIGHT);

  Frames::Frame *gray = Frames::Frame::CreateTagged(env->GetRoot());
  gray->SetBackground(Frames::Color(0.5f, 0.5f, 0.5f, 0.5f));
  gray->SetWidth((float)s_width / 2);
  gray->SetHeight((float)s_height / 2);
  gray->SetPoint(Frames::CENTER, 0, Frames::Nil, Frames::Nil, (float)s_width / 4, (float)s_height / 4 * 3);

  TestSnapshot(env);

  red->ClearConstraints();
  green->ClearConstraints();
  blue->ClearConstraints();
  gray->ClearConstraints();

  red->SetPoint(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  green->SetPoint(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  blue->SetPoint(Frames::LEFT, env->GetRoot(), Frames::LEFT);
  gray->SetPoint(Frames::LEFT, env->GetRoot(), Frames::LEFT);

  red->SetPoint(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  green->SetPoint(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  blue->SetPoint(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);
  gray->SetPoint(Frames::RIGHT, env->GetRoot(), Frames::RIGHT);

  red->SetPoint(Frames::TOP, env->GetRoot(), Frames::TOP);
  red->SetHeight(100.f);

  green->SetPoint(Frames::TOP, red, Frames::BOTTOM);
  green->SetHeight(100.f);

  blue->SetPoint(Frames::TOP, green, Frames::BOTTOM);
  blue->SetHeight(100.f);

  gray->SetPoint(Frames::TOP, blue, Frames::BOTTOM);
  gray->SetHeight(100.f);
  
  TestSnapshot(env);
};

TEST(Layout, Layer) {
  TestEnvironment env;

  const int testFrameCount = 20;
  Frames::Frame *frames[testFrameCount];
  const float order[testFrameCount] = { 0, 6, 3.14159f, 3, 5, 9, 12, -5, 0, 0, 3.14159f, 2, 2, 2, 5000, 4999, 5001, 0, 3, 3.14159f };  // Numbers are not magic in any way, just trying to provide an interesting cross-section
  {
    Frames::Layout *anchor = env->GetRoot();
    for (int i = 0; i < testFrameCount; ++i) {
      frames[i] = Frames::Frame::CreateTagged(env->GetRoot());
      frames[i]->SetWidth(400.f);
      frames[i]->SetHeight(400.f);
      frames[i]->SetBackground(Frames::Color((float)i / testFrameCount, (float)i / testFrameCount, (float)i / testFrameCount, 0.2f));
      frames[i]->SetPoint(Frames::TOPLEFT, anchor, Frames::TOPLEFT, 10.f, 10.f);
      frames[i]->SetLayer(order[i]);
      anchor = frames[i];
    }
  }

  TestSnapshot(env);

  // Re-order to do a new test
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->SetLayer(order[(i + 3) % testFrameCount]);
  }

  TestSnapshot(env);

  // Test strata
  for (int i = 0; i < testFrameCount; ++i) {
    frames[i]->SetStrata((float)(i % 2));
  }

  TestSnapshot(env);
}