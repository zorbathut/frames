
#include "SDL.h"
#include "frame/os_gl.h"

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

      SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(glcontext);
  }
    
  SDL_DestroyWindow(win);

  SDL_Quit();

  return 0;
}
