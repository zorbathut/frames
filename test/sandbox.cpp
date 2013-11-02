
#include "SDL.h"
#include "frame/os_gl.h"
#include "frame/environment.h"
#include "frame/frame.h"

#include <cstdio>

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
