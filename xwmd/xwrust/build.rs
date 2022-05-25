use std::env;
use std::fs;
use std::path::Path;

fn main() {
    let out_dir = env::var_os("OUT_DIR").unwrap();
    let dest_path = Path::new(&out_dir).join("cfg.rs");

    let xwmmcfg_alignment = env::var("XWMMCFG_ALIGNMENT").unwrap_or("8".into());
    let mut strbuf = format!("pub const XWMMCFG_ALIGNMENT: usize = {};\n",
                             xwmmcfg_alignment);
    match env::var("CPUCFG_L1_CACHELINE_SIZE") {
        Ok(cpucfg_l1_cacheline_size) => {
            strbuf += format!("pub const CPUCFG_L1_CACHELINE_SIZE: usize = {};\n",
                               cpucfg_l1_cacheline_size).as_str();
        },
        Err(_) => {},
    };

    fs::write(&dest_path, strbuf).unwrap();

    println!("cargo:rerun-if-changed=build.rs");

    // Rerun build.rs always.
    let xwrust_buildtime = format!("{:?}", std::time::SystemTime::now());
    std::env::set_var("XWRUST_BUILDTIME", xwrust_buildtime);
    println!("cargo:rerun-if-env-changed=XWRUST_BUILDTIME");
}
