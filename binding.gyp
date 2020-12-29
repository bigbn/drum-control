{
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "bindings.cpp" ],      
      "include_dirs": [ "libeXaDrums"],
      "libraries": [ 
	"/opt/drum-control/libeXaDrums/.libs/libexadrums.a" , 
	"-lsystemd", 
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
