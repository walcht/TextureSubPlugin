#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Unity/IUnityGraphics.h"

struct IUnityInterfaces;

class RenderAPI {
 public:
  virtual ~RenderAPI() {}

  /// <summary>
  ///	Loads provided data (i.e., sub-region of texture 2D) into a
  /// 	provided texture2D's GPU memory.
  /// </summary>
  ///
  /// <remarks>
  ///	For OpenGL this is simply a call to glTextureSubImage2D.
  /// </remarks>
  ///
  /// <param name="texture_handle">
  ///
  /// </param>
  ///
  /// <param name="xoffset"></param>
  /// <param name="yoffset"></param>
  /// <param name="width"></param>
  /// <param name="height"></param>
  ///
  /// <param name="data_ptr"></param>
  ///
  /// <param name="level">
  ///	Will be provided to glTextureSubImage2D's level parameter.
  ///	Only used in case graphics API is OpenGL.
  /// </param>
  ///
  /// <param name="format">
  ///	Will be provided to glTextureSubImage2D's format parameter.
  ///	Only used in case graphics API is OpenGL.
  /// </param>
  ///
  /// <param name="type">
  ///	Will be provided to glTextureSubImage2D's type parameter.
  ///	Only used in case graphics API is OpenGL.
  ///</param>
  ///
  /// <param name="row_pitch">
  ///	The size in bytes of one row of the source data. Will be
  ///	provided, as is, to UpdateSubresource's SrcRowPitch parameter.
  /// 	Only used in case graphics API is Direct3D11.
  /// </param>
  virtual void TextureSubImage2D(void* texture_handle, int32_t xoffset,
                                 int32_t yoffset, int32_t width, int32_t height,
                                 void* data_ptr, int32_t level = 0,
                                 int32_t format = -1, int32_t type = -1,
                                 int32_t row_pitch = -1) = 0;

  virtual void TextureSubImage3D(void* texture_handle, int32_t xoffset,
                                 int32_t yoffset, int32_t zoffset,
                                 int32_t width, int32_t height, int32_t depth,
                                 void* data_ptr, int32_t level = 0,
                                 int32_t format = -1, int32_t type = -1,
                                 int32_t row_pitch = -1,
                                 int32_t depth_pitch = -1) = 0;

  /// <summary>
  ///	Callback to process general events like initialization,	shutdown,
  ///	device loss/reset etc.
  /// </summary>
  ///
  /// <param name="type">
  /// </param>
  ///
  /// <param name="interfaces">
  /// </param>
  virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                  IUnityInterfaces* interfaces) = 0;
};

// Create a graphics API implementation instance for the given API type.
RenderAPI* CreateRenderAPI(UnityGfxRenderer apiType);
