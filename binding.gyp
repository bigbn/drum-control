{
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "bindings.cpp" ],
      "libraries": [ 
        "-lexadrums",
      	"-ltinyxml2", 
	      "-lasound", 
	      "-lstdc++fs",
	      "-latomic",
	      "-lpthread"
      ],
      "cflags_cc": ['-fexceptions', '-std=c++17', '-c', '-fpic']
    }
  ]
}
