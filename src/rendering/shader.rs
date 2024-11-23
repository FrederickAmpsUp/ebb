use wgsl_parser::{Parse, ParseResult, ParseStream, ParseStreamer, SyntaxTree};
use anyhow::{anyhow, Result, Context};
use std::fs;

pub struct ShaderModule {
    processed_source: String,
    uniforms: Vec<(String, wgsl_parser::common::TypeDecl)>
}

impl ShaderModule {
    pub fn new(processed_source: String, uniforms: Vec<(String, wgsl_parser::common::TypeDecl)>) -> ShaderModule {
        Self {
            processed_source, uniforms
        }
    }

    pub fn source(&self) -> &String {
        &self.processed_source
    }

    pub fn uniforms(&self) -> &Vec<(String, wgsl_parser::common::TypeDecl)> {
        &self.uniforms
    }
}

pub fn load_shader(filename: &str) -> Result<String> {
    let contents = fs::read_to_string(filename).context("Failed to open shader file")?;

    let mut parser = ParseStream::from(contents);

    let tree = parser.parse::<SyntaxTree>().context("Failed to parse shader code.")?;

    let ParseResult {
        source,
        tokens,
        comments,
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

    println!("{:?}", tree);

    Ok("".to_string())
}
