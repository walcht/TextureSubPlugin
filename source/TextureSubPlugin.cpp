#include <assert.h>
#include <math.h>

#include <vector>

#include "PlatformBase.h"
#include "RenderAPI.h"

static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces* unityInterfaces) {
  s_UnityInterfaces = unityInterfaces;
  s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
  s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

  // Run OnGraphicsDeviceEvent(initialize) manually on plugin load
  OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload() {
  s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

// GraphicsDeviceEvent
static RenderAPI* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static void UNITY_INTERFACE_API
OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType) {
  // Create graphics API implementation upon initialization
  if (eventType == kUnityGfxDeviceEventInitialize) {
    assert(s_CurrentAPI == NULL);
    s_DeviceType = s_Graphics->GetRenderer();
    s_CurrentAPI = CreateRenderAPI(s_DeviceType);
  }

  // Let the implementation process the device related events
  if (s_CurrentAPI) {
    s_CurrentAPI->ProcessDeviceEvent(eventType, s_UnityInterfaces);
  }

  // Cleanup graphics API implementation upon shutdown
  if (eventType == kUnityGfxDeviceEventShutdown) {
    delete s_CurrentAPI;
    s_CurrentAPI = NULL;
    s_DeviceType = kUnityGfxRendererNull;
  }
}

struct TextureSubImage2DParams {
  void* texture_handle;
  int32_t xoffset;
  int32_t yoffset;
  int32_t width;
  int32_t height;
  void* data_ptr;
  int32_t level;
  int32_t format;
  int32_t type;
  int32_t row_pitch;
};
static TextureSubImage2DParams g_TextureSubImage2DParams;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
SetTextureSubImage2DParams(void* texture_handle, int32_t xoffset,
                           int32_t yoffset, int32_t width, int32_t height,
                           void* data_ptr, int32_t level, int32_t format,
                           int32_t type, int32_t row_pitch) {
  g_TextureSubImage2DParams.texture_handle = texture_handle;
  g_TextureSubImage2DParams.xoffset = xoffset;
  g_TextureSubImage2DParams.yoffset = yoffset;
  g_TextureSubImage2DParams.width = width;
  g_TextureSubImage2DParams.height = height;
  g_TextureSubImage2DParams.data_ptr = data_ptr;
  g_TextureSubImage2DParams.level = level;
  g_TextureSubImage2DParams.format = format;
  g_TextureSubImage2DParams.type = type;
  g_TextureSubImage2DParams.row_pitch = row_pitch;
}

struct TextureSubImage3DParams {
  void* texture_handle;
  int32_t xoffset;
  int32_t yoffset;
  int32_t zoffset;
  int32_t width;
  int32_t height;
  int32_t depth;
  void* data_ptr;
  int32_t level;
  int32_t format;
  int32_t type;
  int32_t row_pitch;
  int32_t depth_pitch;
};
static TextureSubImage3DParams g_TextureSubImage3DParams;

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
SetTextureSubImage3DParams(void* texture_handle, int32_t xoffset,
                           int32_t yoffset, int32_t zoffset, int32_t width,
                           int32_t height, int32_t depth, void* data_ptr,
                           int32_t level, int32_t format, int32_t type,
                           int32_t row_pitch, int32_t depth_pitch) {
  g_TextureSubImage3DParams.texture_handle = texture_handle;
  g_TextureSubImage3DParams.xoffset = xoffset;
  g_TextureSubImage3DParams.yoffset = yoffset;
  g_TextureSubImage3DParams.zoffset = zoffset;
  g_TextureSubImage3DParams.width = width;
  g_TextureSubImage3DParams.height = height;
  g_TextureSubImage3DParams.depth = depth;
  g_TextureSubImage3DParams.data_ptr = data_ptr;
  g_TextureSubImage3DParams.level = level;
  g_TextureSubImage3DParams.format = format;
  g_TextureSubImage3DParams.type = type;
  g_TextureSubImage3DParams.row_pitch = row_pitch;
  g_TextureSubImage3DParams.depth_pitch = depth_pitch;
}

enum Event {
  TextureSubImage2D = 0,
  TextureSubImage3D = 1,
};

static void UNITY_INTERFACE_API OnRenderEvent(int eventID) {
  // Unknown / unsupported graphics device type? Do nothing
  if (s_CurrentAPI == NULL) return;

  switch ((Event)eventID) {
    case Event::TextureSubImage2D: {
      s_CurrentAPI->TextureSubImage2D(
          g_TextureSubImage2DParams.texture_handle,
          g_TextureSubImage2DParams.xoffset, g_TextureSubImage2DParams.yoffset,
          g_TextureSubImage2DParams.width, g_TextureSubImage2DParams.height,
          g_TextureSubImage2DParams.data_ptr, g_TextureSubImage2DParams.level,
          g_TextureSubImage2DParams.format, g_TextureSubImage2DParams.type,
          g_TextureSubImage2DParams.row_pitch);
      break;
    }
    case Event::TextureSubImage3D: {
      s_CurrentAPI->TextureSubImage3D(
          g_TextureSubImage3DParams.texture_handle,
          g_TextureSubImage3DParams.xoffset, g_TextureSubImage3DParams.yoffset,
          g_TextureSubImage3DParams.zoffset, g_TextureSubImage3DParams.width,
          g_TextureSubImage3DParams.height, g_TextureSubImage3DParams.depth,
          g_TextureSubImage3DParams.data_ptr, g_TextureSubImage3DParams.level,
          g_TextureSubImage3DParams.format, g_TextureSubImage3DParams.type,
          g_TextureSubImage3DParams.row_pitch,
          g_TextureSubImage3DParams.depth_pitch);
      break;
    }
    default:
      break;
  }
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
GetRenderEventFunc() {
  return OnRenderEvent;
}
