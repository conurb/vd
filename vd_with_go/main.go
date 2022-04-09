package main

// #cgo pkg-config: libavutil libavformat
// #cgo CFLAGS: -Wall
// #include <stdlib.h>
// #include "vd.h"
import "C"

import (
	"fmt"
	"os"
	"unsafe"
)

func main() {
	fmt.Println(duration(os.Args))
}

func duration(args []string) string {
	if len(args) < 2 {
		return ""
	}

	path := make([]*C.char, len(args)-1)

	for i, arg := range args[1:] {
		path[i] = C.CString(arg)
	}

	c_path_ptr := (**C.char)(unsafe.Pointer(&path[0]))
	seconds := int64(C.duration_sum_in_file_hierarchy(c_path_ptr))

	for i := range path {
		C.free(unsafe.Pointer(path[i]))
	}

	h := seconds / 3600
	m := seconds / 60 % 60
	s := seconds % 60

	return fmt.Sprintf("%02d:%02d:%02d", h, m, s)
}
