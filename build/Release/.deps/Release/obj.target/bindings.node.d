cmd_Release/obj.target/bindings.node := g++ -shared -pthread -rdynamic  -Wl,-soname=bindings.node -o Release/obj.target/bindings.node -Wl,--start-group Release/obj.target/bindings/bindings.o -Wl,--end-group /opt/drum-control/libeXaDrums/.libs/libexadrums.a -lsystemd -ltinyxml2 -lasound -lstdc++fs -latomic -lpthread
