//! XWOS RUST：宏
//! ========
//!

#[allow(unused_macros)]
macro_rules! xwos_struct {
    (
        $(#[$attr:meta])*
        pub struct $name:ident {
            $($field:tt)*
        }
    ) => (
        #[repr(C)]
        #[cfg_attr(target_pointer_width = "32", repr(align(8)))]
        #[cfg_attr(target_pointer_width = "64", repr(align(16)))]
        $(#[$attr])*
        pub struct $name {
            $($field)*
        }
    );

    (
        $(#[$attr:meta])*
        pub(crate) struct $name:ident {
            $($field:tt)*
        }
    ) => (
        #[repr(C)]
        #[cfg_attr(target_pointer_width = "32", repr(align(8)))]
        #[cfg_attr(target_pointer_width = "64", repr(align(16)))]
        $(#[$attr])*
        pub(crate) struct $name {
            $($field)*
        }
    );

    (
        $(#[$attr:meta])*
        struct $name:ident {
            $($field:tt)*
        }
    ) => (
        #[repr(C)]
        #[cfg_attr(target_pointer_width = "32", repr(align(8)))]
        #[cfg_attr(target_pointer_width = "64", repr(align(16)))]
        $(#[$attr])*
        struct $name {
            $($field)*
        }
    );
}
