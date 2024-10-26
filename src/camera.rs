use std::{cell::RefCell, f32::consts::PI};
use std::sync::Arc;
use crate::{ecs, surface};

/// A camera, which may be orthographic or perspective.
/// It stores a render target.
pub struct Camera {
    projection_matrix: glam::Mat4,
    render_target: Arc<RefCell<dyn surface::Surface>>
}
impl ecs::Component for Camera {}

impl Camera {
    /// Creates a new perspective camera.
    /// 
    /// # Arguments
    /// 
    /// * `render_target` - a [surface::Surface] to render to.
    /// * `vfov_degrees` - the vertical FOV of the camera, in degrees.
    /// * `z_near` - the view-space Z coordinate of the nearclip plane.
    /// * `z_far` - the view-space Z coordinate of the farclip plane.
    /// 
    /// # Returns
    /// 
    /// The [Camera] object. 
    pub fn perspective(render_target: Arc<RefCell<impl surface::Surface + 'static>>, vfov_degrees: f32, z_near: f32, z_far: f32) -> Self {
        Self {
            // aspect todo
            projection_matrix: glam::Mat4::perspective_lh(vfov_degrees * (PI / 180.0), 1.0, z_near, z_far),
            render_target
        }
    }

    pub fn get_render_target(&self) -> Arc<RefCell<dyn surface::Surface>> {
        self.render_target.clone()
    }

    pub fn get_projection_matrix(&self) -> &glam::Mat4 {
        &self.projection_matrix
    }
}