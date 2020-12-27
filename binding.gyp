{
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "bindings.cpp" ],      
      "include_dirs": [ "libeXaDrums"],
      "libraries": [ "-lexadrums", "-LlibeXaDrums/.libs" ],
      "cflags_cc": ['-fexceptions', '-std=c++17', '-c', '-fpic']
    }
  ]
}
