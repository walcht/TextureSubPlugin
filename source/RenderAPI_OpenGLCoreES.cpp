#include <sstream>
#include <string>

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

  virtual void CreateTexture3D(uint32_t width, uint32_t height, uint32_t depth,
                               Format format, void*& texture);

  virtual void ClearTexture3D(void* texture_handle);

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
                                                IUnityInterfaces* interfaces) {
  if (type == kUnityGfxDeviceEventInitialize) {
#ifdef DEBUG
    UNITY_LOG(g_Log, "kUnityGfxDeviceEventInitialize");
#endif
#if UNITY_WIN && SUPPORT_OPENGL_CORE
    if (m_APIType == kUnityGfxRendererOpenGLCore) {
      gl3wInit();
      int version_major, version_minor;
      glGetIntegerv(GL_MAJOR_VERSION, &version_major);
      glGetIntegerv(GL_MINOR_VERSION, &version_minor);
      std::ostringstream ss;
      ss << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
      ss << "Supports " << version_major << "." << version_minor << ": "
         << gl3wIsSupported(version_major, version_minor);
      UNITY_LOG(g_Log, ss.str().c_str());
    }
#endif
    // Make sure that there are no GL error flags set before proceeding
    while (glGetError() != GL_NO_ERROR) {
    }
  } else if (type == kUnityGfxDeviceEventShutdown) {
#ifdef DEBUG
    UNITY_LOG(g_Log, "kUnityGfxDeviceEventShutdown");
#endif
  } else if (type == kUnityGfxDeviceEventAfterReset) {
#ifdef DEBUG
    UNITY_LOG(g_Log, "kUnityGfxDeviceEventAfterReset");
#endif
  }
}

void RenderAPI_OpenGLCoreES::TextureSubImage3D(void* texture_handle,
                                               int32_t xoffset, int32_t yoffset,
                                               int32_t zoffset, int32_t width,
                                               int32_t height, int32_t depth,
                                               void* data_ptr, int32_t level,
                                               Format format) {
  GLuint gltex = (GLuint)(size_t)(texture_handle);

  GLenum gltype;
  switch (format) {
    case Format::R8_UINT:
      gltype = GL_UNSIGNED_BYTE;
      break;
    case Format::R16_UINT:
      gltype = GL_UNSIGNED_SHORT;
      break;
    default:
      return;
      break;
  }

  glBindTexture(GL_TEXTURE_3D, gltex);
  glTexSubImage3D(GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, width,
                  height, depth, GL_RED, gltype, data_ptr);

  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::ostringstream ss;
    ss << __FUNCTION__ << " error(s): 0x" << std::hex << err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      ss << " 0x" << err;
    }
    UNITY_LOG_ERROR(g_Log, ss.str().c_str());
    return;
  }
}

void RenderAPI_OpenGLCoreES::TextureSubImage2D(void* texture_handle,
                                               int32_t xoffset, int32_t yoffset,
                                               int32_t width, int32_t height,
                                               void* data_ptr, int32_t level,
                                               Format format) {
  GLuint gltex = (GLuint)(size_t)(texture_handle);

  GLenum gltype;
  switch (format) {
    case Format::R8_UINT:
      gltype = GL_UNSIGNED_BYTE;
      break;
    case Format::R16_UINT:
      gltype = GL_UNSIGNED_SHORT;
      break;
    default:
      return;
      break;
  }

  glBindTexture(GL_TEXTURE_2D, gltex);
  glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, GL_RED,
                  gltype, data_ptr);
}

void RenderAPI_OpenGLCoreES::CreateTexture3D(uint32_t width, uint32_t height,
                                             uint32_t depth, Format format,
                                             void*& texture) {
  int MAX_DIM_SIZE;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &MAX_DIM_SIZE);
  {
    std::ostringstream ss;
    ss << "GL_MAX_3D_TEXTURE_SIZE: " << MAX_DIM_SIZE;
    UNITY_LOG(g_Log, ss.str().c_str());
  }

  GLint internal_format;
  GLenum type;
  switch (format) {
    case R8_UINT:
      internal_format = GL_R8;
      type = GL_UNSIGNED_BYTE;
      break;
    case R16_UINT:
      internal_format = GL_R16;
      type = GL_UNSIGNED_SHORT;
      break;
    default:
      texture = NULL;
      return;
      break;
  }

  GLuint gl_texture;
  glGenTextures(1, &gl_texture);
  glBindTexture(GL_TEXTURE_3D, gl_texture);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  {
    std::ostringstream ss;
    ss << "supplied width: " << width << " height: " << height
       << " depth: " << depth;
    UNITY_LOG(g_Log, ss.str().c_str());
  }

  glTexStorage3D(GL_TEXTURE_3D, 1, internal_format, width, height, depth);

  GLenum err;
  if ((err = glGetError()) != GL_NO_ERROR) {
    std::ostringstream ss;
    ss << __FUNCTION__ << " error(s): 0x" << std::hex << err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      ss << " 0x" << err;
    }
    UNITY_LOG_ERROR(g_Log, ss.str().c_str());
    texture = NULL;
    return;
  }

  {
    std::ostringstream ss;
    ss << "created texture 3D glTexImage3D texture handle: " << gl_texture;
    UNITY_LOG(g_Log, ss.str().c_str());
  }

  texture = (void*)gl_texture;
}

void RenderAPI_OpenGLCoreES::ClearTexture3D(void* texture_handle) {
  glDeleteTextures(1, (GLuint*)&texture_handle);
}

#endif  // #if SUPPORT_OPENGL_UNIFIED
