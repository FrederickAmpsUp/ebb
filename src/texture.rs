use image;
use std::fs;

use crate::Instance;

// TODO: texture formats
//    and error stuff
//    and not putting everything in its own bind group

pub trait Texture {
    fn raw_bind_group(&self) -> &wgpu::BindGroup;
    fn raw_bind_group_layout(&self) -> &wgpu::BindGroupLayout;

    fn width(&self) -> u32;
    fn height(&self) -> u32;
    fn size(&self) -> (u32, u32);
}

pub struct ImageTexture {
    size: wgpu::Extent3d,
    tex: wgpu::Texture,
    view: wgpu::TextureView,

    sampler: wgpu::Sampler,
    
    bind_group_layout: wgpu::BindGroupLayout,
    bind_group: wgpu::BindGroup
}

impl ImageTexture {
    pub fn new(inst: &Instance<'static>, (width, height): (u32, u32)) -> Self {
        let size = wgpu::Extent3d {
            width, height,
            depth_or_array_layers: 1
        };

        let desc = wgpu::TextureDescriptor {
            label: Some("Ebb ImageTexture"),
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Rgba8UnormSrgb,
            usage: wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
            view_formats: &[],
        };

        let tex = inst.raw_device().create_texture(&desc);
        let view = tex.create_view(&Default::default());

        let sampler = inst.raw_device().create_sampler(&wgpu::SamplerDescriptor {
            address_mode_u: wgpu::AddressMode::ClampToEdge,
            address_mode_v: wgpu::AddressMode::ClampToEdge,
            address_mode_w: wgpu::AddressMode::ClampToEdge,
            mag_filter: wgpu::FilterMode::Linear,
            min_filter: wgpu::FilterMode::Nearest,
            mipmap_filter: wgpu::FilterMode::Nearest,
            ..Default::default()
        });

        let bind_group_layout = inst.raw_device().create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("Ebb ImageTexture BG layout"),
            entries: &[
                wgpu::BindGroupLayoutEntry {
                    binding: 0,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Texture {
                        multisampled: false,
                        view_dimension: wgpu::TextureViewDimension::D2,
                        sample_type: wgpu::TextureSampleType::Float { filterable: true },
                    },
                    count: None,
                },
                wgpu::BindGroupLayoutEntry {
                    binding: 1,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    // This should match the filterable field of the
                    // corresponding Texture entry above.
                    ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering),
                    count: None,
                }
            ]
        });

        let bind_group = inst.raw_device().create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("Ebb ImageTexture BG"),
            layout: &bind_group_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&sampler),
                }
            ]
        });

        Self {
            size,
            tex, view,
            sampler,
            bind_group_layout,
            bind_group
        }
    }

    pub fn load(inst: &Instance<'static>, filename: &str) -> Result<Self, String> {
        let img_data = match fs::read(filename) {
            Ok(it) => it,
            Err(_) => return Err("Failed to load image as Texture2D".to_string()),
        };
        let img = match
            image::load_from_memory(&img_data) {
            Ok(img) => img,
            Err(_) => return Err("Failed to load image as Texture2D.".to_string())
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

impl Texture for ImageTexture {
    fn raw_bind_group(&self) -> &wgpu::BindGroup {
        &self.bind_group
    }

    fn raw_bind_group_layout(&self) -> &wgpu::BindGroupLayout {
        &self.bind_group_layout
    }

    fn width(&self) -> u32 {
        self.size.width
    }

    fn height(&self) -> u32 {
        self.size.height
    }

    fn size(&self) -> (u32, u32) {
        (self.width(), self.height())
    }
}
