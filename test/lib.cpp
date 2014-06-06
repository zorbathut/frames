
#include "lib.h"

#include "lib_opengl.h"
#include "lib_dx11.h"
#include "lib_null.h"

#include <gtest/gtest.h>

#include <SDL.h>
#include <frames/detail_format.h>
#include <frames/environment.h>
#include <frames/event.h>
#include <frames/layout.h>
#include <frames/loader.h>
#include <frames/stream.h>
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

TestLogger::TestLogger() : m_allowErrors(false) { }
TestLogger::~TestLogger() {
  if (m_allowErrors) {
    EXPECT_FALSE(m_loggedErrors.empty());

    TestCompareStrings("error", m_loggedErrors);
  }
}

void TestLogger::LogError(const std::string &log) {
  printf("[ERR] %s\n", log.c_str());
  if (!m_allowErrors) {
    GTEST_FAIL();
  }

  m_loggedErrors += log;
  m_loggedErrors += "\n";
}

void TestLogger::LogDebug(const std::string &log) {
  printf("[DBG] %s\n", log.c_str());
}

void TestLogger::AllowErrors() {
  m_allowErrors = true;
}

class TestPathMunger : public Frames::Configuration::PathFromId {
public:
  virtual std::string Process(Frames::Environment *env, const std::string &id) {
    return "resources/" + id;
  }
};

TestEnvironment::TestEnvironment(bool startUI, int width, int height) : m_env(0), m_tenv(0) {
  if (startUI) {
    if (RendererIdGet().empty() || RendererIdGet() == "ogl2_1") {
      m_tenv = new TestWindowSDL(width, height);
    } else if (RendererIdGet() == "dx11_fl11") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_HAL);
    } else if (RendererIdGet() == "dx11_fl11_dbg") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_DEBUG);
    } else if (RendererIdGet() == "dx11_fl11_ref") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_REFERENCE);
    } else if (RendererIdGet() == "null") {
      m_tenv = new TestWindowNull(width, height);
    } else {
      printf("Invalid gtest renderer flag %s", RendererIdGet().c_str());
      ADD_FAILURE();
    }
  } else {
    m_tenv = new TestWindowNull(width, height);
  }

  m_logger = Frames::Ptr<TestLogger>(new TestLogger());

  Frames::Configuration::Global cglobal;
  cglobal.LoggerSet(m_logger);
  Frames::Configuration::Set(cglobal);

  Frames::Configuration::Local config;
  config.FontDefaultIdSet("LindenHill.otf");
  config.LoggerSet(m_logger);
  config.PathFromIdSet(Frames::Ptr<TestPathMunger>(new TestPathMunger()));
  config.RendererSet(m_tenv->RendererGet());
  m_env = Frames::Environment::Create(config);

  m_env->ResizeRoot(WidthGet(), HeightGet()); // set this up so we can check coordinates, otherwise we'll currently assume there are no coordinates
}

TestEnvironment::~TestEnvironment() {
  m_env.Reset();
  delete m_tenv;

  // Reset the configuration system so the logger goes out of scope
  Frames::Configuration::Set(Frames::Configuration::Global());
}

int TestEnvironment::WidthGet() const {
  return m_tenv->WidthGet();
}

int TestEnvironment::HeightGet() const {
  return m_tenv->HeightGet();
}

void TestEnvironment::Swap() {
  return m_tenv->Swap();
}

void TestEnvironment::HandleEvents() {
  return m_tenv->HandleEvents();
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
  env.ClearRenderTarget();
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
  std::vector<unsigned char> pixels = env.Screenshot();

  if (pixels.empty() && RendererIdGet() == "null") {
    return; // jolly good, then
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

  if (reference.size() != pixels.size()) {
    GTEST_LOG_(WARNING) << testNames.testName << " issues detected";
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
    
    if (outsidebounds || different) {
      GTEST_LOG_(WARNING) << testNames.testName << " issues detected";
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
    env.ClearRenderTarget();
    env->ResizeRoot(env.WidthGet(), env.HeightGet());
    env->Render();
    env.Swap();
    env.HandleEvents();
  }
}

static std::string s_renderer;
void RendererIdSet(const std::string &renderer) {
  s_renderer = renderer;
}
std::string &RendererIdGet() {
  return s_renderer;
}
