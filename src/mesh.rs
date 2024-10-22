use crate::rendering::*;
use crate::ecs;
use std::rc::Rc;
use crate::Instance;
use wgpu;
use wgpu::util::DeviceExt;
pub trait Vertex: Copy + Clone + bytemuck::Pod + bytemuck::Zeroable {
    const LAYOUT: wgpu::VertexBufferLayout<'static>;
}

pub struct RenderMesh {
    renderer: Rc<RenderPipeline>,
    vertex_buffer: wgpu::Buffer,
    num_vertices: usize,
    index_buffer: wgpu::Buffer,
    num_indices: usize
}
impl ecs::Component for RenderMesh {}

impl RenderMesh {
    pub fn new<V: Vertex>(instance: &Instance, renderer: Rc<RenderPipeline>, vertices: Vec<V>, indices: Vec<u32>) -> Self {
        let vert_slice = vertices.as_slice();
        let ind_slice = indices.as_slice();

        let vertex_buffer = instance.raw_device().create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Ebb Builtin RenderMesh - Vertex Buffer"),
            contents: bytemuck::cast_slice(vert_slice),
            usage: wgpu::BufferUsages::VERTEX
        });

        let index_buffer = instance.raw_device().create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("Ebb Builtin RenderMesh - Index Buffer"),
            contents: bytemuck::cast_slice(ind_slice),
            usage: wgpu::BufferUsages::INDEX
        });

        Self { renderer, vertex_buffer, num_vertices: vertices.len(), index_buffer, num_indices: indices.len() }
    }

    pub fn entity<V: Vertex>(instance: &Instance, renderer: Rc<RenderPipeline>, vertices: Vec<V>, indices: Vec<u32>) -> ecs::Entity {
        let mut e = ecs::Entity::new();
        e.add_component(Self::new(instance, renderer, vertices, indices));
        e
    }

    pub fn get_renderer(&self) -> Rc<RenderPipeline> {
        self.renderer.clone()
    }

    pub fn get_vertex_buffer(&self) -> &wgpu::Buffer {
        &self.vertex_buffer
    }

    pub fn get_num_vertices(&self) -> usize {
        self.num_vertices
    }

    pub fn get_index_buffer(&self) -> &wgpu::Buffer {
        &self.index_buffer
    }

    pub fn get_num_indices(&self) -> usize {
        self.num_indices
    }
}