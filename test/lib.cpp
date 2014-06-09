
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

void ClampScreenshotAlpha(std::vector<unsigned char> *pixels) {
  for (int i = 3; i < (int)pixels->size(); i += 4) {
    (*pixels)[i] = 255;
  }
}

struct TestNames {
  std::string testName;
  std::string resultName;
  std::string diffName;
};

static std::string s_testNameLast = "";
static std::map<std::string, int> s_familyIds;

TestNames GetTestNames(const std::string &family, const std::string &extension) {
  TestNames rv;

  std::string baseName = Frames::detail::Format("ref/%s_%s", ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name(), ::testing::UnitTest::GetInstance()->current_test_info()->name());

  if (baseName != s_testNameLast) {
    s_testNameLast = baseName;
    s_familyIds.clear();
  }

  std::string testFilePrefix = Frames::detail::Format("%s_%s_%d", baseName, family, s_familyIds[family]++);

  rv.testName = testFilePrefix + "_ref%d" + extension;
  rv.resultName = testFilePrefix + "_result" + extension;
  rv.diffName = testFilePrefix + "_diff" + extension;

  // write to the "input" file if that file doesn't exist
  std::string master = Frames::detail::Format(rv.testName.c_str(), 0);
  if (!std::ifstream(master.c_str())) {
    rv.resultName = master;
  }

  return rv;
}

void ResetTestNames() {
  s_testNameLast = "";
  s_familyIds.clear();
}

void TestCompareStrings(const std::string &family, const std::string &data) {
  TestNames testNames = GetTestNames(family, ".txt");

  // Multiple valid refs, NYI
  std::string tname = Frames::detail::Format(testNames.testName.c_str(), 0);

  // Grab our source file (or try to)
  std::string testsrc;
  {
    std::ifstream in(tname.c_str(), std::ios::binary); // Binary so we don't have to worry about \r\n's showing up in our event results
    if (in) {
      testsrc = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    }
  }

  // Write result to disk
  {
    std::ofstream out(testNames.resultName.c_str(), std::ios::binary);
    if (out) {
      out << data;
    } else {
      ADD_FAILURE() << "Cannot write result " << testNames.resultName;
    }
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
  if (!m_allowErrors) {
    GTEST_FAIL() << log;
  } else {
    printf("[ERR-EXPCT] %s\n", log.c_str());
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
    if (RendererIdGet() == "ogl3_2_core") {
      m_tenv = new TestWindowSDL(width, height, 3, 2, SDL_GL_CONTEXT_PROFILE_CORE);
    } else if (RendererIdGet() == "ogl3_2_compat") {
      m_tenv = new TestWindowSDL(width, height, 3, 2, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    } else if (RendererIdGet() == "dx11_fl11") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_HAL);
    } else if (RendererIdGet() == "dx11_fl11_dbg") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_DEBUG);
    } else if (RendererIdGet() == "dx11_fl11_ref") {
      m_tenv = new TestWindowDX11(width, height, TestWindowDX11::MODE_REFERENCE);
    } else if (RendererIdGet() == "null") {
      m_tenv = new TestWindowNull(width, height);
    } else {
      ADD_FAILURE() << Frames::detail::Format("Invalid gtest renderer flag %s", RendererIdGet().c_str());
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

void WritePng(const std::string &filename, const std::vector<unsigned char> &data, int width, int height) {
  FILE *fp = fopen(filename.c_str(), "wb");

  if (fp) {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io(png_ptr, fp);
    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
    png_set_filter(png_ptr, 0, PNG_ALL_FILTERS);

    png_set_IHDR(png_ptr, info_ptr, width, height,
      8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    std::vector<const unsigned char *> rows;
    for (int i = 0; i < height; ++i) {
      rows.push_back(&data[0] + i * width * 4);
    }

    png_write_image(png_ptr, (png_bytepp)&rows[0]); // come on, why couldn't png_bytepp have been const

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
  } else {
    ADD_FAILURE() << "Cannot write result " << filename;
  }
}

void TestSnapshot(TestEnvironment &env, const SnapshotConfig &csf) {
  // Do the render
  env.ClearRenderTarget();
  env->ResizeRoot(env.WidthGet(), env.HeightGet());
  env->Render();

  TestNames testNames;
  if (csf.FileGet().empty()) {
    testNames = GetTestNames("screen", ".png");
  } else {
    testNames.testName = csf.FileGet() + "_ref%d.png";
    std::string master = Frames::detail::Format(testNames.testName.c_str(), 0);
    // write to the "input" file if that file doesn't exist
    if (!std::ifstream(master.c_str())) {
      testNames.resultName = master;
    } else {
      testNames.resultName = csf.FileGet() + "_result" + ".png";
    }
  }
  
  // We now have our test filename

  // Grab a screenshot
  std::vector<unsigned char> pixels = env.Screenshot();

  if (pixels.empty() && RendererIdGet() == "null") {
    return; // jolly good, then
  }

  // Write result to disk
  WritePng(testNames.resultName, pixels, env.WidthGet(), env.HeightGet());

  // Grab our source file (or try to)
  int epsilon = std::numeric_limits<int>::max();
  int critical = std::numeric_limits<int>::max();
  std::multiset<int> diffs;
  std::vector<unsigned char> diffdata;
  std::string match;
  {
    int refid = 0;
    while (true) {
      std::string fname = Frames::detail::Format(testNames.testName.c_str(), refid);
      std::vector<unsigned char> reference;
      Frames::StreamPtr stream = Frames::StreamFile::Create(fname);
      if (stream) {
        Frames::TexturePtr tex = Frames::Loader::PNG::Load(*env, stream);
        EXPECT_EQ(Frames::Texture::RAW, tex->TypeGet());
        EXPECT_EQ(Frames::Texture::FORMAT_RGBA_8, tex->FormatGet());
        EXPECT_EQ(4, Frames::Texture::RawBPPGet(Frames::Texture::FORMAT_RGBA_8));
        EXPECT_EQ(tex->WidthGet() * 4, tex->RawStrideGet());
        reference.resize(tex->WidthGet() * tex->HeightGet() * 4);
        memcpy(&reference[0], tex->RawDataGet(), tex->WidthGet() * tex->HeightGet() * 4);
      }

      if (reference.empty()) {
        break;
      }

      EXPECT_EQ(pixels.size(), reference.size()) << " - wrong size on " << fname;

      if (reference.size() == pixels.size()) {
        int tepsilon = 0;
        int tcritical = 0;
        std::multiset<int> tdiffs;
        std::vector<unsigned char> tdiffdata;
        tdiffdata.resize(pixels.size());

        for (int i = 0; i < (int)pixels.size(); ++i) {
          int diff = abs((int)pixels[i] - (int)reference[i]);
          bool shifted = false;
          if (diff && csf.NearestGet()) {
            const int dx[] = {0, 0, 1, -1};
            const int dy[] = {1, -1, 0, 0};
            for (int ofs = 0; ofs < 4; ++ofs) {
              int target = i + (dx[ofs] + dy[ofs] * env.WidthGet()) * 4;
              if (target >= 0 && target < (int)reference.size()) {
                int tdiff = abs((int)pixels[i] - (int)reference[target]);
                if (tdiff < diff) {
                  diff = tdiff;
                  shifted = true;
                }
              }
            }
          }
          tdiffdata[i] = (unsigned char)std::min(diff * 4, 255);
          if (diff > csf.DeltaGet()) {
            tcritical++;
            tdiffs.insert(diff);
            if (tdiffs.size() > 10) {
              tdiffs.erase(tdiffs.begin());
            }
          } else if (diff || shifted) {
            tepsilon++;
          }
        }

        if (tcritical < critical || tcritical == critical && tepsilon < epsilon) {
          epsilon = tepsilon;
          critical = tcritical;
          diffs = tdiffs;
          diffdata = tdiffdata;
          match = fname;
        }
      }

      ++refid;
    }
  }

  EXPECT_FALSE(match.empty());

  if (epsilon || critical) {
    ClampScreenshotAlpha(&diffdata);
    WritePng(testNames.diffName, diffdata, env.WidthGet(), env.HeightGet());
  }

  if (epsilon) {
    GTEST_LOG_(WARNING) << epsilon << " epsilon pixels compared to " << match;
  }
  EXPECT_EQ(0, critical) << "Critical pixels detected, compared to " << match;
  if (!diffs.empty()) {
    std::string tdiffs = "Pixel diffs: ";
    for (std::multiset<int>::const_iterator itr = diffs.begin(); itr != diffs.end(); ++itr) {
      if (itr != diffs.begin()) {
        tdiffs += ", ";
      }
      tdiffs += Frames::detail::Format("%d", *itr);
    }
    printf("%s", tdiffs.c_str());
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
  ResetTestNames();
}
std::string &RendererIdGet() {
  return s_renderer;
}
