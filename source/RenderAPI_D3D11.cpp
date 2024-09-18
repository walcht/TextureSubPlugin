#include "PlatformBase.h"
#include "RenderAPI.h"

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>

#include "Unity/IUnityGraphicsD3D11.h"

class RenderAPI_D3D11 : public RenderAPI {
 public:
  RenderAPI_D3D11();
  virtual ~RenderAPI_D3D11() {}

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
  ID3D11Device* m_Device;
};

RenderAPI* CreateRenderAPI_D3D11() { return new RenderAPI_D3D11(); }

RenderAPI_D3D11::RenderAPI_D3D11() : m_Device(NULL) {}

void RenderAPI_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type,
                                         IUnityInterfaces* interfaces) {
  switch (type) {
    case kUnityGfxDeviceEventInitialize: {
      IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
      m_Device = d3d->GetDevice();
      break;
    }
  }
}

void RenderAPI_D3D11::TextureSubImage2D(void* texture_handle, int32_t xoffset,
                                        int32_t yoffset, int32_t width,
                                        int32_t height, void* data_ptr,
                                        int32_t level, Format format) {
  ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)texture_handle;
  assert(d3dtex);
  ID3D11DeviceContext* ctx = NULL;
  m_Device->GetImmediateContext(&ctx);

  // determine row pitch/depth from provided format
  uint32_t row_pitch;
  switch (format) {
    case Format::R8:
      row_pitch = width;  // width * sizeof(uint8_t)
      break;
    case Format::RHalf:
      row_pitch = width * 2;  // width * sizeof(uint16_t)
      break;
    default:
      break;
  }

  // make sure to set front and back such that front < back
  // even if they are not needed so that this works
  D3D11_BOX box;
  box.left = xoffset;
  box.top = yoffset;
  box.front = 0;
  box.right = xoffset + width;
  box.bottom = yoffset + height;
  box.back = 1;

  ctx->UpdateSubresource(d3dtex, 0, &box, data_ptr, row_pitch, 0);
  ctx->Release();
}

void RenderAPI_D3D11::TextureSubImage3D(void* texture_handle, int32_t xoffset,
                                        int32_t yoffset, int32_t zoffset,
                                        int32_t width, int32_t height,
                                        int32_t depth, void* data_ptr,
                                        int32_t level, Format format) {
  ID3D11Texture2D* d3dtex = (ID3D11Texture2D*)texture_handle;
  assert(d3dtex);
  ID3D11DeviceContext* ctx = NULL;
  m_Device->GetImmediateContext(&ctx);

  D3D11_BOX box;
  box.left = xoffset;
  box.top = yoffset;
  box.front = zoffset;
  box.right = xoffset + width;
  box.bottom = yoffset + height;
  box.back = zoffset + depth;

  // determine row pitch/depth from provided format
  uint32_t row_pitch;
  switch (format) {
    case Format::R8:
      row_pitch = width;  // width * sizeof(uint8_t)
      break;
    case Format::RHalf:
      row_pitch = width * 2;  // width * sizeof(uint16_t)
      break;
    default:
      break;
  }
  uint32_t depth_pitch = height * row_pitch;

  ctx->UpdateSubresource(d3dtex, 0, &box, data_ptr, row_pitch, depth_pitch);
  ctx->Release();
}

#endif  // #if SUPPORT_D3D11
