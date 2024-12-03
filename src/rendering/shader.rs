use wgsl_parser::{ParseResult, ParseStream, ParseStreamer, SyntaxTree};
use anyhow::{anyhow, Result, Context};
use std::{env::var, fs};

pub struct ShaderModule {
    processed_source: String,
        // could possibly store the exact type and check it later
    uniforms: Vec<(String, usize)>
}

impl ShaderModule {
    pub fn new(processed_source: String, uniforms: Vec<(String, usize)>) -> ShaderModule {
        Self {
            processed_source, uniforms
        }
    }

    pub fn source(&self) -> &String {
        &self.processed_source
    }

    pub fn uniforms(&self) -> &Vec<(String, usize)> {
        &self.uniforms
    }
}

trait TypeUtils {
    fn size(&self) -> Result<usize>;
}

trait TreeUtils {
    fn stringify(&self) -> String;
}

impl TypeUtils for wgsl_parser::common::TypeDecl {
    fn size(&self) -> Result<usize> {
        match &self.name {
            wgsl_parser::expr::IdentExpr::Namespaced(_) => { return Err(anyhow!("unimplemented sizeof namespaced type")) },
            wgsl_parser::expr::IdentExpr::Leaf(token) => {
                if let wgsl_parser::Token::Type(name, _) = token {
                    return Ok(match name.as_str() {
                        "bool" => 1,
                        "f16" => 2,
                        "u32" | "i32" | "f32" => 4,
                        "vec2" | "vec3" | "vec4" => {
                            if let Some(child_ty) = &self.child_ty {
                                let child_size = child_ty.size()?;
                                let nelems = match name.as_str() {
                                    "vec2" => 2,
                                    "vec3" => 3,
                                    "vec4" => 4,
                                    _ => unreachable!()
                                };
                                return Ok(child_size * nelems);
                            } else {
                                return Err(anyhow!("failed to calculate type size due to missing vector inner type"));
                            }
                        },
                        _ => { return Err(anyhow!("unimplemented sizeof type {}", name)); }
                    })
                } else {
                    return Err(anyhow!("unimplemented"));
                }
            }
        };
    }
}

impl TreeUtils for wgsl_parser::common::TypeDecl {
    fn stringify(&self) -> String {
        match &self.name {
            wgsl_parser::expr::IdentExpr::Namespaced(_) => todo!(),
            wgsl_parser::expr::IdentExpr::Leaf(token) => {
                if let wgsl_parser::Token::Type(name, _) = token {
                    if let Some(child_ty) = &self.child_ty {
                        format!("{}<{}>", name.to_string(), child_ty.stringify())
                    } else {
                        name.to_string()
                    }
                } else {
                    unimplemented!()
                }
            },
        }
    }
}

impl TreeUtils for wgsl_parser::decl::VarDecl {
    fn stringify(&self) -> String {
        self.storage.to_string()
    }
}

impl TreeUtils for wgsl_parser::decl::TypeAliasDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::StructDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::FieldDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::FunctionDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::ParamDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::legacy::ExtensionDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::ImportPathDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::ImportDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::legacy::ModuleDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::Decl {
    fn stringify(&self) -> String {
        match self {
            wgsl_parser::decl::Decl::Var(var_decl) => var_decl.stringify(),
            wgsl_parser::decl::Decl::Const(var_decl) => var_decl.stringify(),
            wgsl_parser::decl::Decl::TypeAlias(type_alias_decl) => type_alias_decl.stringify(),
            wgsl_parser::decl::Decl::Struct(struct_decl) => struct_decl.stringify(),
            wgsl_parser::decl::Decl::Field(field_decl) => field_decl.stringify(),
            wgsl_parser::decl::Decl::Function(function_decl) => function_decl.stringify(),
            wgsl_parser::decl::Decl::Param(param_decl) => param_decl.stringify(),
            wgsl_parser::decl::Decl::Extension(extension_decl) => extension_decl.stringify(),
            wgsl_parser::decl::Decl::ImportPath(import_path_decl) => import_path_decl.stringify(),
            wgsl_parser::decl::Decl::Import(import_decl) => import_decl.stringify(),
            wgsl_parser::decl::Decl::Module(module_decl) => module_decl.stringify(),
        }
    }
}

pub fn load_shader(filename: &str) -> Result<ShaderModule> {
    let contents = fs::read_to_string(filename).context("Failed to open shader file")?;

    let mut parser = ParseStream::from(contents);

    let tree = parser.parse::<SyntaxTree>().context("Failed to parse shader code.")?;

    let ParseResult {
        source,
        tokens: _,
        comments: _,
        errors,
    } = parser.into_inner();

    if !errors.is_empty() {
        let mut err_string = format!("Error(s) parsing shader file {filename}: \n");

        for err in errors {
            err_string += "\n";
            err_string += &err.to_string();
        }

        return Err(anyhow!(err_string));
    }

    let mut uniforms = Vec::new();

    for decl in tree.inner {
        use wgsl_parser::decl::Decl::*;
        match decl.clone() {
            Var(ref var_decl) => {
                let mut  is_uniform = false;
                if let Some(qualifiers) = &var_decl.storage_qualifiers {
                    for qual in qualifiers.iter() {
                        if qual.as_inner().0.to_string() == "uniform" {
                            is_uniform = true;
                            break;
                        }
                    }
                }
                if is_uniform {
                    if let Some(_) = var_decl.assignment {
                        return Err(anyhow!("Unexpected initializer on shader uniform '{}'", var_decl.name.as_inner().0.as_str()));
                    }
                    if let Some(ty) = &var_decl.ty {
                        uniforms.push((var_decl.name.as_inner().0.to_string(), ty.size()?));
                    } else {
                        return Err(anyhow!("Missing type for shader uniform '{}'", var_decl.name.as_inner().0.as_str()));
                    }
                } else {
                    // TODO: emit
                }
            },
            // TypeAlias(type_alias_decl) => todo!(),
            // Struct(struct_decl) => todo!(),
            // Field(field_decl) => todo!(),
            // Function(function_decl) => todo!(),
            // Param(param_decl) => todo!(),
            // Extension(extension_decl) => todo!(),
            // ImportPath(import_path_decl) => todo!(),
            // Import(import_decl) => todo!(),
            // Module(module_decl) => todo!(),
            other => {
                println!("{}", other.stringify());
            }
        }
    }

    Ok(
        ShaderModule::new(source.to_string(), uniforms)
    )
}
