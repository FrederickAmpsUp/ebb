extern crate proc_macro;
use proc_macro::TokenStream;
use quote::quote;
use syn::{parse_macro_input, DeriveInput, Data, Fields};

#[proc_macro_derive(Vertex)]
pub fn vertex_layout_derive(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);
    let struct_name = &input.ident;

    let fields = if let Data::Struct(data_struct) = &input.data {
        if let Fields::Named(fields_named) = &data_struct.fields {
            &fields_named.named
        } else {
            panic!("VertexLayout only works on structs with named fields");
        }
    } else {
        panic!("VertexLayout only works on structs");
    };

    let mut vertex_attributes = Vec::new();
    let mut location_counter: u32 = 0;

    for field in fields {
        let field_name = &field.ident;
        let ty = &field.ty;

        let format = match quote! { #ty }.to_string().as_str() {
            "Vec2" => quote! { wgpu::VertexFormat::Float32x2 },
            "Vec3" => quote! { wgpu::VertexFormat::Float32x3 },
            "Vec4" => quote! { wgpu::VertexFormat::Float32x4 },
            "u32" => quote! { wgpu::VertexFormat::Uint32 },
            "f32" => quote! { wgpu::VertexFormat::Float32 },
            // TODO: other stuff
            fmt => panic!("Unsupported field type {fmt} for vertex layout"),
        };

        vertex_attributes.push(quote! {
            wgpu::VertexAttribute {
                offset: core::mem::offset_of!(#struct_name, #field_name) as wgpu::BufferAddress,
                shader_location: #location_counter,
                format: #format,
            }
        });

        location_counter += 1;
    }

    let output = quote! {
        impl ebb::mesh::Vertex for #struct_name {
            const LAYOUT: wgpu::VertexBufferLayout<'static> = wgpu::VertexBufferLayout {
                array_stride: std::mem::size_of::<#struct_name>() as wgpu::BufferAddress,
                step_mode: wgpu::VertexStepMode::Vertex,
                attributes: &[#(#vertex_attributes),*],
            };
        }

        unsafe impl bytemuck::Pod for #struct_name {}
        unsafe impl bytemuck::Zeroable for #struct_name {}
    };

    output.into()
}
