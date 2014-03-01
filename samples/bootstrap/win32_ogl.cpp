// Testbed example for Windows OpenGL rendering. This also acts as a general-purpose OpenGL framework.
// Windows rendering is reasonably complicated to set up and handle properly - the vast majority of this code is not Frames-related.
// This will probably be moved to a _complex version soon.

#include <Windows.h>

#include <frames/environment.h>
#include <frames/os_win32.h>

// Configuration here.
const bool cResizable = true;
const bool cFullscreenToggleable = true;
const int cDepthBits = 24; // This can be 0 if you're not planning to do any 3d rendering - Frames does not use the depth buffer.
const int cStencilBits = 8; // This can be 0 if you're not planning to do any 3d rendering - Frames does not use the stencil buffer. (Yet.)
const bool cVsync = false; // Left off by default for easy comparison of framerates.
const int cBpp = 8; // Values other than 8 are extremely uncommon.

// Default values here, also modified on-the-fly as needed.
static bool sFullscreen = false;
static int sWidth = 1280;
static int sHeight = 720;

// State here. We use globals for the sake of a simple example; if you're writing a game that is intended to have two different rendered windows, you'll obviously need something a bit more complicated.
static bool sMinimized = false;
static bool sShutdown = false; // Easiest way to get feedback from the message handler into the main loop.
static bool sFocused = true;
static Frames::Environment *sEnv = NULL;

// Code below this line

// Handles window messages that arrive by any means, message queue or by direct notification.
LRESULT CALLBACK HandleMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
  // Extract information from the parameters
  unsigned short wParam1 = LOWORD(wParam), wParam2 = HIWORD(wParam);
  unsigned short lParam1 = LOWORD(lParam), lParam2 = HIWORD(lParam);

  // Handle each message
  switch (message) {
  case WM_SYSCOMMAND:
    // Prevent screen saver and monitor power saving
    if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
      return 0;
    // Prevent accidental pausing by pushing F10 or what not
    if (wParam == SC_MOUSEMENU || wParam == SC_KEYMENU)
      return 0;
    break;
  case WM_CLOSE:
    sShutdown = true;
    return 0;
  case WM_MOVE:
    // This is fine, it can just happen on its own.
    break;
  case WM_SIZE:
    // Set the resolution if a full-screen window was alt-tabbed into or out of.
    if (sMinimized != (wParam == SIZE_MINIMIZED) && sFullscreen) {
      if (wParam == SIZE_MINIMIZED) {
        ChangeDisplaySettings(0, 0);
      } else {
        DEVMODE screen_settings;
        screen_settings.dmSize = sizeof(screen_settings);
        screen_settings.dmDriverExtra = 0;
        screen_settings.dmPelsWidth = lParam1;
        screen_settings.dmPelsHeight = lParam2;
        screen_settings.dmBitsPerPel = cBpp;
        screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);
      }
    }
    sMinimized = (wParam == SIZE_MINIMIZED);
    if (!sMinimized) {
      sWidth = lParam1;
      sHeight = lParam2;
      if (sEnv) {
        sEnv->ResizeRoot(sWidth, sHeight);  // Inform Frames that the window resolution has changed.
      }
    }
    break;
  case WM_SIZING: {
    RECT *rect = reinterpret_cast<RECT *>(lParam);
    // If you want to constrain resizing in a complicated manner, this spot is the right place to do it - setting the values in "rect" will change the new size of the window.
    // If you want to constrain resizing in a simple manner, consider handling WM_GETMINMAXINFO.
    glViewport(0, 0, rect->right - rect->left, rect->bottom - rect->top);
    break;
  }
  case WM_ACTIVATE:
    sFocused = (wParam1 == WA_ACTIVE || wParam1 == WA_CLICKACTIVE);
    // If the user alt-tabs out of a fullscreen window, the window will keep drawing and will
    // remain in full-screen mode. Here, we minimize the window, which fixes the drawing problem,
    // and then the WM_SIZE event fixes the full-screen problem.
    if (!sFocused && sFullscreen)
      ShowWindow(window_handle, SW_MINIMIZE);
    break;
  }

  // Frames includes a function to translate most immediately relevant Win32 messages into Frames input events.
  if (sEnv) {
    Frames::Input iev;
    if (Frames::InputGatherWin32(&iev, window_handle, message, wParam, lParam)) {
      sEnv->Input(iev);
    }
  }

  // Pass on remaining messages to the default handler
  return DefWindowProcW(window_handle, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // Register window class
  const wchar_t *const windowClassName = L"FrameTemplateWin32OGL";
  WNDCLASSW window_class;
  window_class.style = CS_OWNDC;  // CS_OWNDC can cause issues if other applications are trying to write to this window, but in a standard game model, that shouldn't ever happen
  window_class.lpfnWndProc = HandleMessage;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = GetModuleHandle(0);
  window_class.hIcon = 0;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hbrBackground = 0;
  window_class.lpszMenuName = 0;
  window_class.lpszClassName = windowClassName;
  if (!RegisterClassW(&window_class)) {
    MessageBoxW(NULL, L"Cannot initialize window class", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return EXIT_FAILURE;
  }
  
  // Figure out window styles
  DWORD window_style = WS_POPUP;
  DWORD window_style_ex = 0;
  if (!sFullscreen) {
    window_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    if (cResizable)
      window_style |= WS_THICKFRAME;
  }
  
  // Specify the window dimensions and get the border size
  RECT window_rectangle;
  window_rectangle.left = 0;
  window_rectangle.right = sWidth;
  window_rectangle.top = 0;
  window_rectangle.bottom = sHeight;
  if (!AdjustWindowRectEx(&window_rectangle, window_style, false, window_style_ex)) {
    MessageBoxW(NULL, L"Cannot calculate window size", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return EXIT_FAILURE;
  }
  
  // Create window
  HWND window = CreateWindowExW(
    window_style_ex,
    windowClassName,
    L"Frames sample application (Win32, OpenGL)",
    window_style,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    window_rectangle.right - window_rectangle.left,
    window_rectangle.bottom - window_rectangle.top,
    NULL,
    NULL,
    GetModuleHandle(0),
    NULL);
  
  if (!window) {
    MessageBoxW(NULL, L"Cannot create window", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return EXIT_FAILURE;
  }
  
  // Set icon here - TBD
  // SendMessage(s_window, WM_SETICON, ICON_BIG, (LPARAM)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(101), IMAGE_ICON, 32, 32, 0));
  // SendMessage(s_window, WM_SETICON, ICON_SMALL, (LPARAM)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(101), IMAGE_ICON, 16, 16, 0));
  
  // Get the device context
  HDC deviceContext = GetDC(window);
  if (!deviceContext) {
    MessageBoxW(NULL, L"Cannot get device context", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  
  // Set pixel format (assuming we're targeting standard 32bit)
  PIXELFORMATDESCRIPTOR pixel_format_request;
  memset(&pixel_format_request, 0, sizeof(pixel_format_request));
  pixel_format_request.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pixel_format_request.nVersion = 1;
  pixel_format_request.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pixel_format_request.iPixelType = PFD_TYPE_RGBA; // There's nothing mandatory about this for Frames, but it's rare to want a different pixel type
  pixel_format_request.cColorBits = 8; // There's nothing mandatory about this for Frames, but it's rare to want a different depth
  pixel_format_request.cStencilBits = cStencilBits;
  pixel_format_request.cDepthBits = cDepthBits;
  unsigned int pixel_format_id = ChoosePixelFormat(deviceContext, &pixel_format_request);
  if (!pixel_format_id) {
    MessageBoxW(NULL, L"Cannot choose pixel format", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  if (!SetPixelFormat(deviceContext, pixel_format_id, &pixel_format_request)) {
    MessageBoxW(NULL, L"Cannot set pixel format", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  
  // Switch to full-screen mode if appropriate
  if (sFullscreen) {
    DEVMODE screen_settings;
    screen_settings.dmSize = sizeof(screen_settings);
    screen_settings.dmDriverExtra = 0;
    screen_settings.dmPelsWidth = sWidth;
    screen_settings.dmPelsHeight = sHeight;
    screen_settings.dmBitsPerPel = cBpp;
    screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    if (ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
      MessageBoxW(NULL, L"Cannot switch to fullscreen", L"Warning", MB_ICONWARNING | MB_OK);
      sFullscreen = false;  // This, at least, we can recover from sensibly
    }
  }

  // Make an OpenGL rendering context for this thread
  HGLRC glContext = wglCreateContext(deviceContext);
  if (!glContext) {
    MessageBoxW(NULL, L"Cannot acquire OpenGL context", L"Fatal error", MB_ICONEXCLAMATION | MB_OK);
    return 0;
  }
  wglMakeCurrent(deviceContext, glContext);

  // Turn vsync on
  if (cVsync) {
    // Query for the appropriate OpenGL extension
    typedef BOOL(APIENTRY *WGLSwapProc)(int);
    WGLSwapProc sSwapProc = 0;

    const char *extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "WGL_EXT_swap_control") != 0)
      sSwapProc = (WGLSwapProc)wglGetProcAddress("wglSwapIntervalEXT");

    if (sSwapProc)
      sSwapProc(1);
  }

  // Show the window. Note that SetForegroundWindow can fail if the user is currently using another window, but this is fine and nothing to be alarmed about.
  ShowWindow(window, SW_SHOW);
  SetForegroundWindow(window);
  SetFocus(window);

  // Create our Frames instance
  sEnv = new Frames::Environment();
  sEnv->ResizeRoot(sWidth, sHeight);  // Initialize size

  // Here's our main loop!
  while (true) {
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }

    if (sShutdown) {
      break;
    }

    // Clear background to black
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render from Frames
    sEnv->Render();

    // Swap the back buffer to the front
    SwapBuffers(deviceContext);
  }

  // Start teardown
  delete sEnv;

  if (sFullscreen && !sMinimized) {
    ChangeDisplaySettings(NULL, 0);
  }

  if (glContext) {
    wglDeleteContext(glContext);
  }

  if (deviceContext) {
    ReleaseDC(window, deviceContext);
  }

  if (window) {
    DestroyWindow(window);
  }

  // And we're done!
  // Note: In real applications it's common to ignore cleanup and just TerminateProcess(GetCurrentProcess(), 0);. This isn't harmful to Frames in any way - choose whatever termination method is better for your application.
  return 0;
}
