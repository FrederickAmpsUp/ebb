use wgpu;

/// A surface for rendering to
pub trait Surface {
    /// Gets the internal [wgpu::Surface] object.
    /// 
    /// # Returns
    /// 
    /// The internal reference to a [wgpu::Texture].
    fn get_texture(&self) -> wgpu::Texture;

    /// Gets a [wgpu::TextureView] that one can render to.
    /// 
    /// # Returns
    /// 
    /// A [wgpu::TextureView] corresponding to the internal texture.
    fn get_view(&mut self) -> wgpu::TextureView;

    /// Gets the width of the surface
    /// 
    /// # Returns
    /// 
    /// The width of the surface (in pixels), or 0 on failure.
    fn width(&self) -> u32;

    /// Gets the  of the surface
    /// 
    /// # Returns
    /// 
    /// The height of the surface (in pixels), or 0 on failure.
    fn height(&self) -> u32;
}

pub struct WindowSurface<'a> {
    surf: wgpu::Surface<'a>,
    curr_tex: Option<wgpu::SurfaceTexture>
}

impl<'a> Surface for WindowSurface<'a> {
    fn get_texture(&self) -> wgpu::Texture {
        unimplemented!()
    }

    fn get_view(&mut self) -> wgpu::TextureView {
        self.curr_tex = Some(self.surf.get_current_texture().expect("Failed to retrieve window surface."));
        self.curr_tex.as_ref().unwrap().texture.create_view(&Default::default())
    }
    
    fn width(&self) -> u32 {
        if let Some(t) = self.curr_tex.as_ref() {
            t.texture.width()
        } else { 0 }
    }
    
    fn height(&self) -> u32 {
        if let Some(t) = self.curr_tex.as_ref() {
            t.texture.height()
        } else { 0 }
    }
}

impl<'a> WindowSurface<'a> {
    pub fn new(surf: wgpu::Surface<'a>) -> Self {
        Self {
            surf,
            curr_tex: None
        }
    }

    pub fn raw_surface(&self) -> &wgpu::Surface {
        &self.surf
    }

    pub fn present(&mut self) {
        if let Some(t) = self.curr_tex.take() {
            t.present();
        }
        self.curr_tex = None;
    }
}