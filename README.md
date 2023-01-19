# Platformer

Renderer:
    RenderCmdBuffer
        pushQuads, pushText etc..

    IRenderDevice
        render(RenderCmdBuffer)

    // hidden to the rest of the program
    OGLRenderDevice : IRenderDevice

    //maybe a factory function

    font use pixel buffer and texture atlas
