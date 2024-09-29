# About
TextureSubPlugin is a Unity low-level native plugin for uploading data to chunks
(i.e., subregions or bricks) of a 2D/3D Texture.

This repository is adapted from [Unity native rendering plugin repository](https://github.com/Unity-Technologies/NativeRenderingPlugin/tree/master).

## Usage

1. build the plugin from source code
1. navigate to the compiled TextureSubPlugin.dll (TextureSubPlugin.so on Linux)
   in your build directory and copy it to your:
   `<unity-project-dir>/Assets/Plugins/` folder
1. add the attached `TextureSubPlugin.cs` to your Unity project for
   plugin-related enum definitions
1. in some C# script add the following declarations:

    ```csharp
    [DllImport("TextureSubPlugin")]
    private static extern IntPtr GetRenderEventFunc();

    [DllImport("TextureSubPlugin")]
    private static extern void UpdateTextureSubImage3DParams(
        System.IntPtr texture_handle,
        System.Int32 xoffset,
        System.Int32 yoffset,
        System.Int32 zoffset,
        System.Int32 width,
        System.Int32 height,
        System.Int32 depth,
        System.IntPtr data_ptr,
        System.Int32 level,
        System.Int32 format
    );
    ```

1. and to update a texture:

    ```csharp
    // wait until current frame rendering is done ...
    yield return new WaitForEndOfFrame();
    
    // the brick to be uploaded
    byte[] data = ...
    int bricksize = 128;
    
    // we are sending a managed object to unmanaged thread (i.e., C++)
    // the object has to be pinned to a fixed location in memory during
    // the plugin call
    GCHandle gc_data = GCHandle.Alloc(data, GCHandleType.Pinned);
    
    // update global state
    // m_tex_ptr is initialized as: m_tex_ptr = m_tex.GetNativeTexturePtr();
    // with m_tex a Texture3D object
    UpdateTextureSubImage3DParams(m_tex_ptr, 0, 0, 0, bricksize, bricksize,
        bricksize, gc_data.AddrOfPinnedObject(), level: 0,
        format: (int)TextureSubPlugin.Format.R8);
    
    // actual texture loading SHOULD be called from the render thread otherwise
    // we might change the current render state. This is still true even after
    // frame rendering has finished
    GL.IssuePluginEvent(GetRenderEventFunc(),
        (int)TextureSubPlugin.Event.TextureSubImage3D);
    
    // GC, you are now again free to manage the object
    gc_data.Free();
    ```

For textures larger than 2GBs and when using OpenGL or Vulkan, using Unity's
Texture3D/2D constructor outputs the following error:

```Texture3D (WIDTHxHEIGHTxDEPTH) is too large, currently up to 2GB is allowed```

Direct3D11/12 impose a 2GB limit per resource but OpenGL and Vulkan don't.
To bypass this, create the texture using the provided ```CreateTexture3D```
(see RenderAPI.h):

```csharp
// update global state
UpdateCreateTexture3DParams(m_tex_width, m_tex_height, m_tex_depth,
m_tex_plugin_format);  // see RenderAPI.h

// issue texture creation command in the render thread
GL.IssuePluginEvent(GetRenderEventFunc(), (int)TextureSubPlugin.Event.CreateTexture3D);

// wait one frame because IssuePluginEvent's callback may not get called immediately
yield return new WaitForEndOfFrame();

// retrieve the native texture handle
IntPtr tex_ptr = RetrieveCreatedTexture3D();

// finally, create the Texture3D object from the created native texture
Texture3D tex = Texture3D.CreateExternalTexture(m_tex_width, m_tex_height, m_tex_depth,
    tex_format, mipChain: false, nativeTex: m_tex_ptr);
```

Again, if the graphics API is Direct3D11/12, there is (probably) no good reason
to use this.

## License

MIT License. Read `license.txt` file.