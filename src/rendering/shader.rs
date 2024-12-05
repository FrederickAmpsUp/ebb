use wgsl_parser::{ParseResult, ParseStream, ParseStreamer, SyntaxTree};
use anyhow::{anyhow, Result, Context};
use std::{collections::HashMap, fs};

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

type StructSizeMap = HashMap<String, usize>;
trait TypeUtils {
    fn size(&self, structs: &StructSizeMap) -> Result<usize>;
}

trait TreeUtils {
    fn stringify(&self) -> String;
}

impl TypeUtils for wgsl_parser::common::TypeDecl {
    fn size(&self, structs: &StructSizeMap) -> Result<usize> {
        match &self.name {
            wgsl_parser::expr::IdentExpr::Namespaced(_) => { return Err(anyhow!("unimplemented sizeof namespaced type")) },
            wgsl_parser::expr::IdentExpr::Leaf(token) => {
                return Ok(match token.to_string().as_str() {
                    "bool" => 1,
                    "f16" => 2,
                    "u32" | "i32" | "f32" => 4,
                    "vec2" | "vec3" | "vec4" => {
                        if let Some(child_ty) = &self.child_ty {
                            let child_size = child_ty.size(structs)?;
                            let nelems = match token.to_string().as_str() {
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
                    name => {
                        if let Some(&sz) = structs.get(name) {
                            return Ok(sz);
                        } else {
                            return Err(anyhow!("unimplemented sizeof type {}", name));
                        }
                    }
                });
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
                    unimplemented!("unexpected token")
                }
            },
        }
    }
}

impl TreeUtils for wgsl_parser::decl::VarDecl {
    fn stringify(&self) -> String {
        let mut storage_quals = String::new();
        if let Some(quals) = &self.storage_qualifiers {
            storage_quals.push('<');
            for qual in quals.iter() {
                storage_quals.push_str(qual.to_string().as_str());
                storage_quals.push_str(", ");
            }
            let _ = storage_quals.split_off(storage_quals.len() - 2);
            storage_quals.push('>');
        }
        
        let prefix = format!("{}{} {}", self.storage, storage_quals, self.name.to_string());

        let pref_ty = if let Some(ty) = &self.ty {
            format!("{}: {}", prefix, ty.stringify())
        } else {
            prefix
        };

        let nopunc = if let Some(init) = &self.assignment {
            format!("{} = {}", pref_ty, init.stringify())
        } else {
            pref_ty
        };

        format!("{}{}", nopunc, self.semicolon.to_string())
    }
}

impl TreeUtils for wgsl_parser::decl::TypeAliasDecl {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::decl::StructDecl {
    fn stringify(&self) -> String {
        let mut out = format!("{} {} {}\n", self.storage, self.name, self.body.open_brace);
        for field in self.body.fields.iter() {
            out.push('\t');
            out.push_str(&field.stringify());
            out.push('\n');
        }
        out.push_str(&self.body.close_brace.to_string());
        out
    }
}

impl TreeUtils for wgsl_parser::decl::FieldDecl {
    fn stringify(&self) -> String {
        format!("{}: {}{}", self.name, self.ty.stringify(), if let Some(s) = &self.separator { s.to_string() } else { "".to_string() })
    }
}

impl TreeUtils for wgsl_parser::decl::FunctionDecl {
    fn stringify(&self) -> String {
        let mut args = String::new();
        for arg in self.params.iter() {
            args.push_str(&arg.stringify());
            args.push_str(", ");
        }
        let _ = args.split_off(args.len() - 2);

        let ret_ty = if let Some(ty) = &self.return_ty {
            format!(" -> {}", ty.stringify())
        } else {
            "".to_string()
        };

        format!("{} {}({}){} {}", self.storage.to_string(), self.name.to_string(), args, ret_ty, self.body.stringify())
    }
}

impl TreeUtils for wgsl_parser::decl::ParamDecl {
    fn stringify(&self) -> String {
        format!("{}: {}", self.name.to_string(), self.ty.stringify())
    }
}

impl TreeUtils for wgsl_parser::decl::legacy::ExtensionDecl {
    fn stringify(&self) -> String {
        unimplemented!("legacy::ExtensionDecl::stringify")
    }
}

impl TreeUtils for wgsl_parser::decl::ImportPathDecl {
    fn stringify(&self) -> String {
        unimplemented!("ImportPathDecl::stringify")
    }
}

impl TreeUtils for wgsl_parser::decl::ImportDecl {
    fn stringify(&self) -> String {
        unimplemented!("ImportDecl::stringify")
    }
}

impl TreeUtils for wgsl_parser::decl::legacy::ModuleDecl {
    fn stringify(&self) -> String {
        unimplemented!("legacy::ModuleDecl::stringify")
    }
}

impl TreeUtils for wgsl_parser::expr::UnaryPreExpr {
    fn stringify(&self) -> String {
        format!("{}{}", self.op.to_string(), self.expr.stringify())
    }
}

impl TreeUtils for wgsl_parser::expr::UnaryPostExpr {
    fn stringify(&self) -> String {
        format!("{}{}", self.expr.stringify(), self.op.to_string())
    }
}

impl TreeUtils for wgsl_parser::expr::BinaryExpr {
    fn stringify(&self) -> String {
        format!("{} {} {}", self.lhs.stringify(), self.op.to_string(), self.rhs.stringify())
    }
}

impl TreeUtils for wgsl_parser::expr::FnCallExpr {
    fn stringify(&self) -> String {
        let mut args = String::new();

        args.push_str(self.arguments.brace_open.to_string().as_str());
        for arg in self.arguments.arguments.iter() {
            args.push_str(&arg.stringify());
            args.push_str(", ");
        }
        if self.arguments.arguments.len() > 0 { let _ = args.split_off(args.len() - 2); }
        args.push_str(self.arguments.brace_close.to_string().as_str());

        format!("{}{}", self.ident.stringify(), args)
    }
}

impl TreeUtils for wgsl_parser::expr::TypeCtorExpr {
    fn stringify(&self) -> String {
        let mut args = String::new();

        args.push_str(self.arguments.brace_open.to_string().as_str());
        for arg in self.arguments.arguments.iter() {
            args.push_str(&arg.stringify());
            args.push_str(", ");
        }
        if self.arguments.arguments.len() > 0 { let _ = args.split_off(args.len() - 2); }
        args.push_str(self.arguments.brace_close.to_string().as_str());

        format!("{}{}", self.ty.stringify(), args)
    }
}

impl TreeUtils for wgsl_parser::expr::GroupExpr {
    fn stringify(&self) -> String {
        format!("{}{}{}", self.brace_open, self.expr.stringify(), self.brace_close)
    }
}

impl TreeUtils for wgsl_parser::expr::BitcastExpr {
    fn stringify(&self) -> String {
        format!("{}<{}>{}", self.keyword.to_string(), self.ty.stringify(), self.expr.stringify())
    }
}

impl TreeUtils for wgsl_parser::expr::IdentExpr {
    fn stringify(&self) -> String {
        match self {
            wgsl_parser::expr::IdentExpr::Namespaced(namespaced_ident) => {
                println!("namespaced ident");
                format!("{}.{}", namespaced_ident.namespace, namespaced_ident.ident.stringify())
            },
            wgsl_parser::expr::IdentExpr::Leaf(token) => {
                token.to_string()
            },
        }
    }
}

impl TreeUtils for wgsl_parser::expr::PostfixExpr {
    fn stringify(&self) -> String {
        let accessor = match &self.accessor {
            wgsl_parser::expr::Accessor::Dot(token) => format!("{}{}", token.to_string(), self.expr.stringify()),
            wgsl_parser::expr::Accessor::Index(braces) => {
                format!("{}{}{}", braces[0], self.expr.stringify(), braces[1])
            },
        };
        format!("{}{}", accessor, if let Some(postfix) = &self.postfix { postfix.stringify() } else { "".to_string() })
    }
}
impl TreeUtils for wgsl_parser::expr::PrimaryExpr {
    fn stringify(&self) -> String {
        format!("{}{}", self.expr.stringify(), if let Some(postfix) = &self.postfix { postfix.stringify() } else { "".to_string() })
    }
}

impl TreeUtils for wgsl_parser::stmt::BlockStmt {
    fn stringify(&self) -> String {
        let mut body = String::new();
        for stmt in self.stmts.iter() {
            let str_stmt = stmt.stringify();
            for line in str_stmt.lines() {
                body.push('\t');
                body.push_str(line);
                body.push_str("\n");
            }
        }
        format!("{}\n{}{}", self.brace_open, body, self.brace_close)
    }
}

impl TreeUtils for wgsl_parser::stmt::ReturnStmt {
    fn stringify(&self) -> String {
        format!("{} {}{}", self.keyword, if let Some(v) = &self.value { v.stringify() } else { "".to_string() }, self.semicolon)
    }
}

impl TreeUtils for wgsl_parser::stmt::IfStmt {
    fn stringify(&self) -> String {
        let then = format!("{} {} {}", self.keyword, self.condition.stringify(), self.then_branch.stringify());
        if let Some(else_branch) = &self.else_branch {
            then + &format!(" {} {}", else_branch.keyword, else_branch.body.stringify())
        } else {
            then
        }
    }
}

impl TreeUtils for wgsl_parser::stmt::SwitchStmt {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::stmt::LoopStmt {
    fn stringify(&self) -> String {
        format!("{} {}", self.keyword, self.body.stringify())
    }
}

impl TreeUtils for wgsl_parser::stmt::ContinuingStmt {
    fn stringify(&self) -> String {
        format!("{} {}", self.keyword, self.body.stringify())
    }
}

impl TreeUtils for wgsl_parser::stmt::ForStmt {
    fn stringify(&self) -> String {
        todo!()
    }
}

impl TreeUtils for wgsl_parser::stmt::KeywordStmt {
    fn stringify(&self) -> String {
        format!("{}{}", self.keyword, self.semicolon)
    }
}

impl TreeUtils for wgsl_parser::stmt::ExprStmt {
    fn stringify(&self) -> String {
        format!("{}{}", self.expr.stringify(), self.semicolon)
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
impl TreeUtils for wgsl_parser::expr::Expr {
    fn stringify(&self) -> String {
        match self {
            wgsl_parser::expr::Expr::UnaryPre(unary_pre_expr) => unary_pre_expr.stringify(),
            wgsl_parser::expr::Expr::UnaryPost(unary_post_expr) => unary_post_expr.stringify(),
            wgsl_parser::expr::Expr::Binary(binary_expr) => binary_expr.stringify(),
            wgsl_parser::expr::Expr::Assignment(binary_expr) => binary_expr.stringify(),
            wgsl_parser::expr::Expr::FnCall(fn_call_expr) => fn_call_expr.stringify(),
            wgsl_parser::expr::Expr::TypeCtor(type_ctor_expr) => type_ctor_expr.stringify(),
            wgsl_parser::expr::Expr::Group(group_expr) => group_expr.stringify(),
            wgsl_parser::expr::Expr::Bitcast(bitcast_expr) => bitcast_expr.stringify(),
            wgsl_parser::expr::Expr::Literal(token) => token.to_string(),
            wgsl_parser::expr::Expr::Ident(ident_expr) => ident_expr.stringify(),
            wgsl_parser::expr::Expr::Primary(primary_expr) => primary_expr.stringify(),
        }
    }
}
impl TreeUtils for wgsl_parser::stmt::Stmt {
    fn stringify(&self) -> String {
        match self {
            wgsl_parser::stmt::Stmt::Block(block_stmt) => block_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Return(return_stmt) => return_stmt.stringify(),
            wgsl_parser::stmt::Stmt::If(if_stmt) => if_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Switch(switch_stmt) => switch_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Loop(loop_stmt) => loop_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Continuing(continuing_stmt) => continuing_stmt.stringify(),
            wgsl_parser::stmt::Stmt::For(for_stmt) => for_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Var(var_decl) => var_decl.stringify(),
            wgsl_parser::stmt::Stmt::Break(keyword_stmt) => keyword_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Continue(keyword_stmt) => keyword_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Discard(keyword_stmt) => keyword_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Fallthrough(keyword_stmt) => keyword_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Expr(expr_stmt) => expr_stmt.stringify(),
            wgsl_parser::stmt::Stmt::Empty(token) => token.to_string(),
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
    let mut structs = StructSizeMap::new();

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
                        uniforms.push((var_decl.name.as_inner().0.to_string(), ty.size(&structs)?));
                    } else {
                        return Err(anyhow!("Missing type for shader uniform '{}'", var_decl.name.as_inner().0.as_str()));
                    }
                } else {
                    println!("{}", var_decl.stringify());
                }
            },
            // TypeAlias(type_alias_decl) => todo!(),
            Struct(struct_decl) => {
                let mut size: usize = 0;
                for field in struct_decl.body.fields.iter() {

                }
            },
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
