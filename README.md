# gRPC server and client

### Build:
``` 
$ mkdir build && cd build && cmake .. && make -j8
```

### Run:
Server and client by default runs on 0.0.0.0:50051, but you can specify address in command line parameters:
```
$ ./server/remote_target_server 0.0.0.0:50051
$ ./client/remote_target_client 0.0.0.0:50051
```
Client consist of general tests of connection and reports additional info to the file "./log".

