use glam;

use crate::ecs;

/// A component which describes an entity's transformation in 3d space.
pub struct Transform3D {
    pub model_matrix: glam::Mat4
}

impl ecs::Component for Transform3D {}

// TODO: useful functions for Transform3D