{
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "bindings.cpp" ],      
      "include_dirs": [ "/home/bigbn/Projects/libeXaDrums"],
      "libraries": [ "-lexadrums", "-L/home/bigbn/Projects/libeXaDrums/.libs" ],
      "cflags_cc": ['-fexceptions', '-std=c++17', '-c', '-fpic']
    }
  ]
}
