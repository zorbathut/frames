/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FramesEditorPCH.h"

#include "FramesFontFactory.h"

#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "IMainFrameModule.h"
#include "FramesFont.h"
#include "SlateCore.h"

UFramesFontFactory::UFramesFontFactory(const class FPostConstructInitializeProperties &PCIP)
  : Super(PCIP)
{
  SupportedClass = UFramesFont::StaticClass();

  bCreateNew = true;
  bEditorImport = true;
}

bool UFramesFontFactory::ConfigureProperties() {
  m_target = "";

  IDesktopPlatform *desktop = FDesktopPlatformModule::Get();
  if (!desktop)
  {
    return false;
  }

  void *windowHandle = 0;

  // why does OpenFileDialog not do this on its own
  IMainFrameModule &frame = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
  const TSharedPtr<SWindow> window = frame.GetParentWindow();
  if (window.IsValid() && window->GetNativeWindow().IsValid()) {
    windowHandle = window->GetNativeWindow()->GetOSWindowHandle();
  }

  TArray<FString> output;
  if (!desktop->OpenFileDialog(windowHandle, TEXT("Choose font file"), FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT), TEXT(""), TEXT("Supported fonts|*.ttf;*.otf"), EFileDialogFlags::None, output)) {
    return false;
  }

  if (output.Num() != 1) {
    return false;
  }

  m_target = output[0];

  return true;
}

UObject *UFramesFontFactory::FactoryCreateNew(UClass *classType, UObject *parent, FName name, EObjectFlags flags, UObject *context, FFeedbackContext *warn) {
  int64 size = IPlatformFile::GetPlatformPhysical().FileSize(m_target.GetCharArray().GetData());
  if (size < 0 || size > (1 << 30)) { // no, you cannot load a 2gb font. this is not a thing which is allowed.
    return 0;
  }

  UFramesFont *rv = ConstructObject<UFramesFont>(UFramesFont::StaticClass(), parent, name, flags);
  rv->Data.SetNumUninitialized((int32)size);

  IFileHandle *file = IPlatformFile::GetPlatformPhysical().OpenRead(m_target.GetCharArray().GetData());
  file->Read(rv->Data.GetData(), size);
  delete file;

  return rv;
}
