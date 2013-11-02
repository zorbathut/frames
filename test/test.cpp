
#include <gtest/gtest.h>

#include <SDL.h>
#include <frame/os_gl.h>
#include <frame/environment.h>
#include <frame/frame.h>
#include <frame/stream.h>
#include <frame/loader.h>
#include <frame/texture_config.h>

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
    m_env = new Frame::Environment();
  }

  ~TestEnvironment() {
    EXPECT_EQ(glGetError(), GL_NO_ERROR); // verify no GL issues
    delete m_env;
  }

  Frame::Environment *operator*() {
    return m_env;
  }
  Frame::Environment *operator->() {
    return m_env;
  }

private:
  TestSDLEnvironment m_sdl; // mostly taken care of with constructor/destructor
  Frame::Environment *m_env;
};

void TestSnapshot(TestEnvironment &env) {
  // Do the render
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  env->ResizeRoot(s_width, s_height);
  env->Render();

  std::string testName = Frame::Utility::Format("test/ref/%s_%s", ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name(), ::testing::UnitTest::GetInstance()->current_test_info()->name());

  static std::string s_testNameLast = "";
  static int s_testIdLast = 0;

  if (testName != s_testNameLast) {
    s_testNameLast = testName;
    s_testIdLast = 0;
  }

  std::string testFilePrefix = Frame::Utility::Format("%s_%d", testName.c_str(), s_testIdLast++);
  std::string testFileName = testFilePrefix + ".png";
  std::string failureFileName = testFilePrefix + "_failure.png";

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
    Frame::StreamFile *stream = Frame::StreamFile::Create(testFileName);
    if (stream)
    {
      Frame::TextureConfig tex = Frame::Loader::PNG::Load(*env, stream);
      EXPECT_EQ(tex.GetMode(), Frame::TextureConfig::RAW);
      EXPECT_EQ(tex.Raw_GetType(), Frame::TextureConfig::MODE_RGBA);
      EXPECT_EQ(Frame::TextureConfig::GetBPP(Frame::TextureConfig::MODE_RGBA), 4);
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

  Frame::Frame *frame = Frame::Frame::CreateTagged(env->GetRoot());
  frame->SetBackground(Frame::Color(1.f, 0.f, 0.f, 0.5f));
  
  TestSnapshot(env);
};

/*
int main(int argc, char** argv){
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    std::printf("%s", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_Window *win = SDL_CreateWindow("Frame test harness", 100, 100, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  
  if (!win) {
    std::printf("%s", SDL_GetError());
    return 1;
  }
  
  // Create an OpenGL context associated with the window.
  {
    SDL_GLContext glcontext = SDL_GL_CreateContext(win);

    glewInit();

    Frame::Environment *env = new Frame::Environment();

    env->ResizeRoot(1280, 720);

    {
      Frame::Frame *redrect = Frame::Frame::CreateTagged(env->GetRoot());
      redrect->SetBackground(Frame::Color(1, 0, 0));
      redrect->SetPoint(Frame::Axis::X, 0, env->GetRoot(), 0.4f, 0);
      redrect->SetPoint(Frame::Axis::X, 1, env->GetRoot(), 0.6f, 0);
      redrect->SetPoint(Frame::Axis::Y, 0, env->GetRoot(), 0.4f, 0);
      redrect->SetPoint(Frame::Axis::Y, 1, env->GetRoot(), 0.6f, 0);
    }

    bool quit = false;
    while (!quit)
    {
      {
        SDL_Event e;
        while (SDL_PollEvent(&e)){
          //If user closes he window
          if (e.type == SDL_QUIT)
            quit = true;
        }
      }

      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT);

      env->Render();

      SDL_GL_SwapWindow(win);
    }

    delete env;

    SDL_GL_DeleteContext(glcontext);
  }
    
  SDL_DestroyWindow(win);

  SDL_Quit();

  return 0;
}
*/
