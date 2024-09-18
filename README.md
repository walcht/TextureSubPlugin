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
    
    // actual texture loading should be called from the render thread
    GL.IssuePluginEvent(GetRenderEventFunc(),
        (int)TextureSubPlugin.Event.TextureSubImage3D);
    
    // GC, you are now again free to manage the object
    gc_data.Free();
    ```

## License

MIT License. Read `license.txt` file.