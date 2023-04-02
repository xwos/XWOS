use std::env;
use std::fs;
use std::path::Path;

fn main() {
    let out_dir = env::var_os("OUT_DIR").unwrap();
    let dest_path = Path::new(&out_dir).join("cfg.rs");
    let xwos_dir = env::var_os("XWOS_PATH").unwrap();
    let wkspc_dir = env::var_os("XWOS_WKSPC_DIR").unwrap();
    let src_path = Path::new(&xwos_dir).join(&wkspc_dir).join("XWOS.cfg.rs");

    fs::copy(&src_path, &dest_path).unwrap();

    println!("cargo:rerun-if-changed=build.rs");

    // Rerun build.rs always.
    let xwrust_buildtime = format!("{:?}", std::time::SystemTime::now());
    std::env::set_var("XWRUST_BUILDTIME", xwrust_buildtime);
    println!("cargo:rerun-if-env-changed=XWRUST_BUILDTIME");
}
