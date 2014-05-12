
#include "lib.h"

#include <gtest/gtest.h>

#include <SDL.h>
#include <frames/os_gl.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/event.h>
#include <frames/stream.h>
#include <frames/layout.h>
#include <frames/loader.h>
#include <frames/texture.h>

#include <png.h>

#include <cstdio>
#include <fstream>

struct TestNames {
  std::string testName;
  std::string resultName;
};

TestNames GetTestNames(const std::string &family, const std::string &extension) {
  TestNames rv;

  std::string baseName = Frames::detail::Format("ref/%s_%s", ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name(), ::testing::UnitTest::GetInstance()->current_test_info()->name());

  static std::string s_testNameLast = "";
  static std::map<std::string, int> s_familyIds;

  if (baseName != s_testNameLast) {
    s_testNameLast = baseName;
    s_familyIds.clear();
  }

  std::string testFilePrefix = Frames::detail::Format("%s_%s_%d", baseName, family, s_familyIds[family]++);

  rv.testName = testFilePrefix + extension;
  rv.resultName = testFilePrefix + "_result" + extension;

  // write to the "input" file if that file doesn't exist
  if (!std::ifstream(rv.testName.c_str())) {
    rv.resultName = rv.testName;
  }

  return rv;
}

void TestCompareStrings(const std::string &family, const std::string &data) {
  TestNames testNames = GetTestNames(family, ".txt");

  // Grab our source file (or try to)
  std::string testsrc;
  {
    std::ifstream in(testNames.testName.c_str(), std::ios::binary); // Binary so we don't have to worry about \r\n's showing up in our event results
    if (in) {
      testsrc = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
  }

  // Write result to disk
  {
    std::ofstream out(testNames.resultName.c_str(), std::ios::binary);
    out << data;
  }

  EXPECT_TRUE(data == testsrc);
}

TestSDLEnvironment::TestSDLEnvironment(int width, int height) : m_win(0), m_glContext(0) {
  EXPECT_EQ(0, SDL_Init(SDL_INIT_VIDEO));

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  m_win = SDL_CreateWindow("Frames test harness", 100, 100, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  EXPECT_TRUE(m_win != NULL);

  m_width = width;
  m_height = height;

  m_glContext = SDL_GL_CreateContext(m_win);
  EXPECT_TRUE(m_glContext != 0);
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

int TestSDLEnvironment::WidthGet() const {
  return m_width;
}
int TestSDLEnvironment::HeightGet() const {
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

class TestLogger : public Frames::Configuration::Logger {
public:
  TestLogger() : m_allowErrors(false) { }
  ~TestLogger() {
    if (m_allowErrors) {
      EXPECT_FALSE(m_loggedErrors.empty());

      TestCompareStrings("error", m_loggedErrors);
    }
  }

  virtual void LogError(const std::string &log) {
    printf("[ERR] %s\n", log.c_str());
    if (!m_allowErrors) {
      GTEST_FAIL();
    }

    m_loggedErrors += log;
    m_loggedErrors += "\n";
  }
  virtual void LogDebug(const std::string &log) {
    printf("[DBG] %s\n", log.c_str());
  }

  void AllowErrors() {
    m_allowErrors = true;
  }

private:
  std::string m_loggedErrors;

  bool m_allowErrors;
};

TestEnvironment::TestEnvironment(bool startSDL, int width, int height) : m_env(0), m_sdl(0) {
  if (startSDL) {
    m_sdl = new TestSDLEnvironment(width, height);
  }

  Frames::Configuration config;
  config.FontDefaultIdSet("LindenHill.otf");
  m_logger = Frames::Ptr<TestLogger>(new TestLogger());
  config.LoggerSet(m_logger);
  m_env = new Frames::Environment(config);

  if (startSDL) {
    m_env->ResizeRoot(WidthGet(), HeightGet()); // set this up so we can check coordinates, otherwise we'll currently assume there are no coordinates
  }
}

TestEnvironment::~TestEnvironment() {
  EXPECT_EQ(GL_NO_ERROR, glGetError()); // verify no GL issues
  delete m_env;
  delete m_sdl;
}

void TestEnvironment::AllowErrors() {
  m_logger->AllowErrors();
}

VerbLog::VerbLog(const std::string &suffix) {
  if (!suffix.empty()) {
    m_suffix = "_" + suffix;
  }

  m_records += "Begin log\n"; // this is honestly just for some code laziness in VerbLog, an empty record vector is treated specially
}

VerbLog::~VerbLog() {
  if (!m_records.empty()) {
    Snapshot();
  }

  for (int i = 0; i < (int)m_detachers.size(); ++i) {
    delete m_detachers[i];
  }
}

void VerbLog::Snapshot() {
  TestCompareStrings("event" + m_suffix, m_records);

  m_records.clear();
}

void VerbLog::RecordEvent(Frames::Handle *handle) {
  RecordResult(handle, "");
}

void VerbLog::RecordEvent(Frames::Handle *handle, int p1) {
  RecordResult(handle, Frames::detail::Format("%d", p1));
}

void VerbLog::RecordEvent(Frames::Handle *handle, const Frames::Vector &p1) {
  RecordResult(handle, Frames::detail::Format("%s", p1));
}

void VerbLog::RecordEvent(Frames::Handle *handle, Frames::Input::Key p1) {
  RecordResult(handle, Frames::detail::Format("%s", Frames::Input::StringFromKey(p1)));
}

void VerbLog::RecordEvent(Frames::Handle *handle, const std::string &p1) {
  RecordResult(handle, Frames::detail::Format("%s", p1));
}

void VerbLog::RecordResult(Frames::Handle *handle, const std::string &params) {
  std::string current;
  if (handle->TargetGet() != handle->TargetContextGet() || handle->VerbGet() != handle->VerbContextGet()) {
    current = Frames::detail::Format(" (currently %s on %s)", handle->VerbContextGet()->NameGet(), handle->TargetContextGet()->DebugNameGet());
  }

  std::string param;
  if (!params.empty()) {
    param = Frames::detail::Format(" (%s)", params);
  }

  m_records += Frames::detail::Format("Event %s%s on %s%s\n", handle->VerbGet()->NameGet(), param, handle->TargetGet()->DebugNameGet(), current);
}

void TestSnapshot(TestEnvironment &env, std::string fname /*= ""*/) {
  // Do the render
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  env->ResizeRoot(env.WidthGet(), env.HeightGet());
  env->Render();

  TestNames testNames;
  if (fname.empty()) {
    testNames = GetTestNames("screen", ".png");
  } else {
    testNames.testName = fname + ".png";
    // write to the "input" file if that file doesn't exist
    if (!std::ifstream(testNames.testName.c_str())) {
      testNames.resultName = testNames.testName;
    } else {
      testNames.resultName = fname + "_result" + ".png";
    }
  }
  
  // We now have our test filename

  // Grab a screenshot
  std::vector<unsigned char> pixels; pixels.resize(4 * env.WidthGet() * env.HeightGet());

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, env.WidthGet(), env.HeightGet(), GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
  EXPECT_EQ(GL_NO_ERROR, glGetError());

  // Annoyingly, OpenGL reads coordinates in math quadrant order, not scanline order like the rest of the civilized computer world
  // So . . . go ahead and invert the entire array
  for (int y = 0; y < env.HeightGet() / 2; ++y) {
    for (int x = 0; x < env.WidthGet() * 4; ++x) {
      std::swap(pixels[y * env.WidthGet() * 4 + x], pixels[(env.HeightGet() - 1 - y) * env.WidthGet() * 4 + x]);
    }
  }

  // Grab our source file (or try to)
  std::vector<unsigned char> reference;
  {
    Frames::StreamPtr stream = Frames::StreamFile::Create(testNames.testName);
    if (stream)
    {
      Frames::TexturePtr tex = Frames::Loader::PNG::Load(*env, stream);
      EXPECT_EQ(Frames::Texture::RAW, tex->TypeGet());
      EXPECT_EQ(Frames::Texture::FORMAT_RGBA_8, tex->FormatGet());
      EXPECT_EQ(4, Frames::Texture::RawBPPGet(Frames::Texture::FORMAT_RGBA_8));
      EXPECT_EQ(tex->WidthGet() * 4, tex->RawStrideGet());
      reference.resize(tex->WidthGet() * tex->HeightGet() * 4);
      memcpy(&reference[0], tex->RawDataGet(), tex->WidthGet() * tex->HeightGet() * 4);
    }
  }

  {
    // Write result to disk
    // Don't need this anywhere in Frames so we'll just hack it in here
    FILE *fp = fopen(testNames.resultName.c_str(), "wb");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);

    png_set_IHDR(png_ptr, info_ptr, env.WidthGet(), env.HeightGet(),
      8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    std::vector<unsigned char *> rows;
    for (int i = 0; i < env.HeightGet(); ++i) {
      rows.push_back(&pixels[0] + i * env.WidthGet() * 4);
    }

    png_write_image(png_ptr, &rows[0]);

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
  }

  EXPECT_EQ(reference.size(), pixels.size());

  if (reference.size() == pixels.size()) {
    int different = 0;
    int outsidebounds = 0;
    for (int i = 0; i < (int)pixels.size(); ++i) {
      int diff = abs((int)pixels[i] - (int)reference[i]);
      if (diff > 0) {
        different++;
      }
      if (diff > 2) {
        outsidebounds++;
      }
    }
    
    EXPECT_EQ(0, outsidebounds);
    if (different) {
      GTEST_LOG_(WARNING) << "Mismatched pixels within bounds: " << different;
    }
  }
  
}

void HaltAndRender(TestEnvironment &env) {
  while (true) {
    // Do the render
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    env->ResizeRoot(env.WidthGet(), env.HeightGet());
    env->Render();
    env.Swap();
    env.HandleEvents();
  }
}
