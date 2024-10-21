use wgpu;

pub struct Surface<'a> {
    surf: &'a wgpu::Surface<'a>
}

impl<'a> Surface<'a> {
    pub fn new(surf: &'a wgpu::Surface<'a>) -> Self {
        Self { surf }
    }

    pub fn wgpu_surface(&self) -> &'a wgpu::Surface<'a> {
        self.surf
    }
}