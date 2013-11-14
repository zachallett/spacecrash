#include "stdafx.h"
#include "base.h"
#include "sys.h"

//========================================================================================
// Has to be provided by the game
extern int Main(void);

//========================================================================================
// Platform layer implementation

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  int retval = -1;

  if (glfwInit() == GL_TRUE)
  {
    if (glfwOpenWindow(SYS_WIDTH, SYS_HEIGHT, 0, 0, 0, 0, 8, 0, SYS_FULLSCREEN ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_TRUE) /* rgba, depth, stencil */
    {
        retval = Main();
        glfwCloseWindow();
    }
    glfwTerminate();
  }
  return retval;
}

//-----------------------------------------------------------------------------
void SYS_Pump()
{
  // GLFW takes care...
}

//-----------------------------------------------------------------------------
void SYS_Show()
{
  glfwSwapBuffers();
}

//-----------------------------------------------------------------------------
bool SYS_GottaQuit()
{
  return !glfwGetWindowParam(GLFW_OPENED);
}

//-----------------------------------------------------------------------------
void SYS_Sleep(int ms)
{
    usleep(1000 * ms);
}

//-----------------------------------------------------------------------------
bool SYS_KeyPressed(int key)
{
  return glfwGetKey(key);
}

//-----------------------------------------------------------------------------
ivec2 SYS_MousePos()
{
  int x, y;
  ivec2 pos;
  glfwGetMousePos(&x, &y);
  pos.x = x;
  pos.y = SYS_HEIGHT - y;
  return pos;
}

//-----------------------------------------------------------------------------
bool SYS_MouseButonPressed(int button)
{
  return glfwGetMouseButton(button);
}
