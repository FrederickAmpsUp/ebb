use wgpu;

/// A surface for rendering to
pub struct Surface<'a> {
    surf: &'a wgpu::Surface<'a>
}

impl<'a> Surface<'a> {
    /// Create a new surface from the raw [wgpu::Surface] object.
    /// 
    /// # Arugments
    /// 
    /// * `surf` - A reference to a [wgpu::Surface], borrowed for the lifetime of the return value.
    /// 
    /// # Returns
    /// 
    /// The [Surface] object
    pub fn new(surf: &'a wgpu::Surface<'a>) -> Self {
        Self { surf }
    }

    /// Gets the internal [wgpu::Surface] object.
    /// 
    /// # Returns
    /// 
    /// The internal reference to a [wgpu::Surface]
    pub fn wgpu_surface(&self) -> &'a wgpu::Surface<'a> {
        self.surf
    }
}