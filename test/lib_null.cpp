
#include "lib_null.h"

#include <frames/renderer_null.h>

TestWindowNull::TestWindowNull(int width, int height) : TestWindow(width, height) { }

TestWindowNull::~TestWindowNull() {}

void TestWindowNull::Swap() {}

void TestWindowNull::HandleEvents() {}

Frames::Configuration::RendererPtr TestWindowNull::RendererGet() {
  return Frames::Configuration::RendererNull();
}

void TestWindowNull::ClearRenderTarget() {}

std::vector<unsigned char> TestWindowNull::Screenshot() {
  return std::vector<unsigned char>();
}
