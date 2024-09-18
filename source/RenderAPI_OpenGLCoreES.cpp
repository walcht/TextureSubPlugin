#include "PlatformBase.h"
#include "RenderAPI.h"

// OpenGL Core profile (desktop) or OpenGL ES (mobile) implementation of
// RenderAPI. Supports several flavors: Core, ES2, ES3

#if SUPPORT_OPENGL_UNIFIED

#include <assert.h>
#if UNITY_IOS || UNITY_TVOS
#include <OpenGLES/ES2/gl.h>
#elif UNITY_ANDROID || UNITY_WEBGL
#include <GLES2/gl2.h>
#elif UNITY_OSX
#include <OpenGL/gl3.h>
#elif UNITY_WIN
// On Windows, use gl3w to initialize and load OpenGL Core functions. In
// principle any other library (like GLEW, GLFW etc.) can be used; here we use
// gl3w since it's simple and straightforward.
#include "gl3w/gl3w.h"
#elif UNITY_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#elif UNITY_EMBEDDED_LINUX
#include <GLES2/gl2.h>
#if SUPPORT_OPENGL_CORE
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif
#elif UNITY_QNX
#include <GLES2/gl2.h>
#else
#error Unknown platform
#endif

class RenderAPI_OpenGLCoreES : public RenderAPI {
 public:
  RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType);
  virtual ~RenderAPI_OpenGLCoreES() {}

  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                  IUnityInterfaces* interfaces);
  virtual void TextureSubImage2D(void* texture_handle, int32_t xoffset,
                                 int32_t yoffset, int32_t width, int32_t height,
                                 void* data_ptr, int32_t level, Format format);
  virtual void TextureSubImage3D(void* texture_handle, int32_t xoffset,
                                 int32_t yoffset, int32_t zoffset,
                                 int32_t width, int32_t height, int32_t depth,
                                 void* data_ptr, int32_t level, Format format);

 private:
  UnityGfxRenderer m_APIType;
};

RenderAPI* CreateRenderAPI_OpenGLCoreES(UnityGfxRenderer apiType) {
  return new RenderAPI_OpenGLCoreES(apiType);
}

RenderAPI_OpenGLCoreES::RenderAPI_OpenGLCoreES(UnityGfxRenderer apiType)
    : m_APIType(apiType) {}

void RenderAPI_OpenGLCoreES::ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                                IUnityInterfaces* interfaces) {}

void RenderAPI_OpenGLCoreES::TextureSubImage3D(void* texture_handle,
                                               int32_t xoffset, int32_t yoffset,
                                               int32_t zoffset, int32_t width,
                                               int32_t height, int32_t depth,
                                               void* data_ptr, int32_t level,
                                               Format format) {
  GLuint gltex = (GLuint)(size_t)(texture_handle);

  GLenum gltype;
  switch (format) {
    case Format::R8:
      gltype = GL_UNSIGNED_BYTE;
      break;
    case Format::RHalf:
      gltype = GL_HALF_FLOAT;
      break;
    default:
      break;
  }

  glBindTexture(GL_TEXTURE_3D, gltex);
  glTexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, width,
                  height, depth, GL_RED, gltype, data_ptr);
}

void RenderAPI_OpenGLCoreES::TextureSubImage2D(void* texture_handle,
                                               int32_t xoffset, int32_t yoffset,
                                               int32_t width, int32_t height,
                                               void* data_ptr, int32_t level,
                                               Format format) {
  GLuint gltex = (GLuint)(size_t)(texture_handle);

  GLenum gltype;
  switch (format) {
    case Format::R8:
      gltype = GL_UNSIGNED_BYTE;
      break;
    case Format::RHalf:
      gltype = GL_HALF_FLOAT;
      break;
    default:
      break;
  }

  glBindTexture(GL_TEXTURE_2D, gltex);
  glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, GL_RED,
                  gltype, data_ptr);
}

#endif  // #if SUPPORT_OPENGL_UNIFIED
