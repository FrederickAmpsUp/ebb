use image;
use std::fs;

use crate::Instance;

// TODO: texture formats
//    and error stuff

struct Texture2D {
    size: wgpu::Extent3d,
    tex: wgpu::Texture
}

impl Texture2D {
    pub fn new(inst: &Instance<'static>, (width, height): (u32, u32)) -> Self {
        let size = wgpu::Extent3d {
            width, height,
            depth_or_array_layers: 1
        };

        let desc = wgpu::TextureDescriptor {
            label: Some("Ebb Texture2D"),
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        };

        Self {
            size,
            tex: inst.raw_device().create_texture(&desc),
        }
    }

    pub fn load(inst: &Instance<'static>, filename: &str) -> Result<Self, String> {
        let img_data = match fs::read(filename) {
            Ok(it) => it,
            Err(err) => return Err("Failed to load image as Texture2D".to_string()),
        };
        let img = match
            image::load_from_memory(&img_data) {
            Ok(img) => img,
            Err(e) => return Err("Failed to load image as Texture2D.".to_string())
        };

        let self_ = Self::new(inst, (img.width(), img.height()));

        inst.raw_queue().write_texture(
            wgpu::ImageCopyTexture {
                texture: &self_.tex,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
                aspect: wgpu::TextureAspect::All
            },
            &img.to_rgba8(),

            wgpu::ImageDataLayout {
                offset: 0,
                bytes_per_row: Some(4 * img.width()),
                rows_per_image: Some(img.height()),
            },
            self_.size
        );
        Ok(self_)
    }
}
