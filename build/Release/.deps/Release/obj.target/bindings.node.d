cmd_Release/obj.target/bindings.node := g++ -shared -pthread -rdynamic -m64  -Wl,-soname=bindings.node -o Release/obj.target/bindings.node -Wl,--start-group Release/obj.target/bindings/bindings.o -Wl,--end-group -lexadrums -L/home/bigbn/Projects/libeXaDrums/.libs
