
#include "doclib.h"

Frames::Text *CreateNameBar(Frames::Layout *root, const std::string &header) {
  Frames::Text *text = Frames::Text::Create(root, "Header");

  text->TextSet(header);
  text->PinSet(Frames::CENTERTOP, root, Frames::CENTERTOP, 0, 30);
  text->SizeSet(30);
  text->ColorTextSet(Frames::Color(1.f, 1.f, 0.f));
  text->FontSet("geo_1.ttf");

  return text;
}
