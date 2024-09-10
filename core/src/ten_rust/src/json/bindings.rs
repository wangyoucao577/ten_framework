//
// This file is part of the TEN Framework project.
// See https://github.com/TEN-framework/ten_framework/LICENSE for license
// information.
//
use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::ptr;

use crate::json;

#[no_mangle]
pub extern "C" fn ten_remove_json_comments(
    json_with_comments: *const c_char,
) -> *mut c_char {
    if json_with_comments.is_null() {
        return ptr::null_mut();
    }

    // Convert C string to rust string.
    let c_str = unsafe { CStr::from_ptr(json_with_comments) };
    let rust_str = match c_str.to_str() {
        Ok(s) => s,
        Err(_) => return ptr::null_mut(),
    };

    let json_result = match json::ten_remove_json_comments(rust_str) {
        Ok(json) => json,
        Err(_) => return ptr::null_mut(),
    };

    // Convert Rust string to C string.
    let c_string = CString::new(json_result).unwrap();

    c_string.into_raw()
}