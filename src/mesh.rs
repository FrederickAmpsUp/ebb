use crate::rendering::*;
use crate::ecs;
use std::rc::Rc;
use crate::Instance;
use wgpu;
use wgpu::util::DeviceExt;

/// Any type that may be used as a vertex in a mesh.
pub trait Vertex: Copy + Clone + bytemuck::Pod + bytemuck::Zeroable {
    /// The [wgpu::VertexBufferLayout] of the data in the vertex.
    const LAYOUT: wgpu::VertexBufferLayout<'static>;
}

/// An [ecs::Component] that allows rendering of an indexed triangle mesh.
pub struct RenderMesh {
    renderer: Rc<RenderPipeline>,
    vertex_buffer: wgpu::Buffer,
    num_vertices: usize,
    index_buffer: wgpu::Buffer,
    num_indices: usize
}
impl ecs::Component for RenderMesh {}

impl RenderMesh {
    /// Creates a new [RenderMesh] using the specified [Instance], [RenderPipeline], vertex and index buffer.
    /// 
    /// # Type Parameters
    /// 
    /// * `V` - the [Vertex] type to use.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - the [Instance] to use when creating WGPU objects. Borrowed for the duration of the function call.
    /// * `renderer` - a handle to the [RenderPipeline] to be used for rendering.
    /// * `vertices` - a [Vec] of the vertices in the mesh.
    /// * `indices`  - a [Vec] of indices into the vertex buffer, every 3 form a triangle.
    /// 
    /// # Returns
    /// 
    /// The [RenderMesh] component, which may be added to an [ecs::Entity] for rendering.
    pub fn new<V: Vertex>(instance: &Instance, renderer: Rc<RenderPipeline>, vertices: &Vec<V>, indices: &Vec<u32>) -> Self {
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
    /// Creates a new entity with a [RenderMesh] component using the specified [Instance], [RenderPipeline], vertex and index buffer.
    /// 
    /// # Type Parameters
    /// 
    /// * `V` - the [Vertex] type to use.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - the [Instance] to use when creating WGPU objects. Borrowed for the duration of the function call.
    /// * `renderer` - a handle to the [RenderPipeline] to be used for rendering.
    /// * `vertices` - a [Vec] of the vertices in the mesh.
    /// * `indices`  - a [Vec] of indices into the vertex buffer, every 3 form a triangle.
    /// 
    /// # Returns
    /// 
    /// An [ecs::Entity] with a [RenderMesh] component, which may be added to an [ecs::World].
    pub fn entity<V: Vertex>(instance: &Instance, renderer: Rc<RenderPipeline>, vertices: &Vec<V>, indices: &Vec<u32>) -> ecs::Entity {
        let mut e = ecs::Entity::new();
        e.add_component(Self::new(instance, renderer, vertices, indices));
        e
    }

    /// Gets a handle to the internal [RenderPipeline].
    /// 
    /// # Returns
    /// 
    /// A handle to the internal [RenderPipeline].
    pub fn get_renderer(&self) -> Rc<RenderPipeline> {
        self.renderer.clone()
    }

    /// Gets a handle to the internal vertex buffer.
    /// 
    /// # Returns
    /// 
    /// A handle to the internal [wgpu::Buffer] (vertex buffer).
    pub fn get_vertex_buffer(&self) -> &wgpu::Buffer {
        &self.vertex_buffer
    }

    /// Gets the number of vertices in the vertex buffer.
    /// 
    /// # Returns
    /// 
    /// The number of vertices in the vertex buffer.
    pub fn get_num_vertices(&self) -> usize {
        self.num_vertices
    }

    /// Gets a handle to the internal index buffer.
    /// 
    /// # Returns
    /// 
    /// A handle to the internal [wgpu::Buffer] (index buffer).
    pub fn get_index_buffer(&self) -> &wgpu::Buffer {
        &self.index_buffer
    }

    /// Gets the number of indices in the index buffer.
    /// 
    /// # Returns
    /// 
    /// The number of indices in the index buffer.
    pub fn get_num_indices(&self) -> usize {
        self.num_indices
    }
}