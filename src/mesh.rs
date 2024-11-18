use crate::rendering::*;
use crate::ecs;
use std::rc::Rc;
use crate::Instance;
use bytemuck::Pod;
use bytemuck::Zeroable;
use wgpu;
use wgpu::util::DeviceExt;
use tobj;

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

/// A basic vertex that stores only a 3d position
#[repr(C)]
#[derive(Clone, Copy)]
pub struct PositionVertex3D {
    position: [f32; 3]
}
impl Vertex for PositionVertex3D {
    const LAYOUT: wgpu::VertexBufferLayout<'static> = wgpu::VertexBufferLayout {
        array_stride: size_of::<PositionVertex3D>() as u64,
        step_mode: wgpu::VertexStepMode::Vertex,
        attributes: &wgpu::vertex_attr_array![0 => Float32x3],
    };
}

unsafe impl Pod for PositionVertex3D {}
unsafe impl bytemuck::Zeroable for PositionVertex3D {}

#[repr(C)]
#[derive(Clone, Copy)]
pub struct PositionNormalTexcoordVertex3D {
    position: [f32; 3],
    normal: [f32; 3],
    texcoord: [f32; 2]
}
impl Vertex for PositionNormalTexcoordVertex3D {
    const LAYOUT: wgpu::VertexBufferLayout<'static> = wgpu::VertexBufferLayout {
        array_stride: size_of::<PositionNormalTexcoordVertex3D>() as u64,
        step_mode: wgpu::VertexStepMode::Vertex,
        attributes: &wgpu::vertex_attr_array![0 => Float32x3, 1 => Float32x3, 2 => Float32x2]
    };
}

unsafe impl Pod for PositionNormalTexcoordVertex3D {}
unsafe impl bytemuck::Zeroable for PositionNormalTexcoordVertex3D {}

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

    /// Load a mesh from a Wavefront .OBJ file.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - the [Instance] to use when creating WGPU objects. Borrowed for the duration of the function call.
    /// * `renderer` - a handle to the [RenderPipeline] to be used for rendering.
    /// * `file` - the filename (relative to working directory) to load.
    /// 
    /// # Returns
    /// 
    /// The [RenderMesh] component, which may be added to an [ecs::Entity] for rendering.
    pub fn load_obj(instance: &Instance, renderer: Rc<RenderPipeline>, file: &str) -> Result<Self, tobj::LoadError> {
            // TODO: better errors

            // we don't care about materials
        let (models, _) = tobj::load_obj(file, &tobj::GPU_LOAD_OPTIONS)?;
        if models.len() == 0 {
            return Err(tobj::LoadError::GenericFailure);
        };

        let mesh = &models[0].mesh;
        
        let mut vertices: Vec<PositionNormalTexcoordVertex3D> = vec![];
        vertices.resize(mesh.positions.len(), PositionNormalTexcoordVertex3D::zeroed());

        let mut i: usize = 0;
        while i < mesh.positions.len() / 3 {
            
            let position = [
                mesh.positions[i * 3 + 0],
                mesh.positions[i * 3 + 1],
                mesh.positions[i * 3 + 2],
            ];
            let normal = [
                mesh.normals[i * 3 + 0],
                mesh.normals[i * 3 + 1],
                mesh.normals[i * 3 + 2],
            ];
            let texcoord = [
                mesh.texcoords[i * 2 + 0],
                mesh.texcoords[i * 2 + 1],
            ];

            vertices[i] = PositionNormalTexcoordVertex3D {
                position, normal, texcoord
            };

            i += 1;
        }

        Ok(Self::new(instance, renderer, &vertices, &mesh.indices))
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
