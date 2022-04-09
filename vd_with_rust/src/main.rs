mod bindings;

use std::{ffi::CString, os::raw::c_char};

fn main() {
    let args = std::env::args().skip(1).collect();
    if let Some(s) = duration(args) {
        println!("{s}");
    }
}

fn duration(paths: Vec<String>) -> Option<String> {
    let cstr_paths: Vec<_> = paths.iter()
        .map(|path| CString::new(path.as_str()).unwrap())
        .collect();

    let mut p_paths: Vec<_> = cstr_paths.iter()
        .map(|path| path.as_ptr() as *mut c_char)
        .collect();

    p_paths.push(std::ptr::null_mut::<c_char>());

    unsafe {
        let seconds = bindings::duration_sum_in_file_hierarchy(p_paths.as_mut_ptr());
        
        if seconds > 0 {
            let h = seconds / 3600;
            let m = seconds / 60 % 60;
            let s = seconds % 60;

            return Some(format!("{h:02}:{m:02}:{s:02}"));
        }
    }
    None
}
