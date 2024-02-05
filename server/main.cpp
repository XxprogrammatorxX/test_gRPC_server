#include "server.h"

#include "connection.h"
#include "DeviceSpec.h"

#include "remote_target.pb.h"
#include "remote_target.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <chrono>

int main(int argc, char* argv[])
{
	std::string addr;
	if (argc == 2)
		addr = argv[1];
	else
		addr = "0.0.0.0:50051";

	grpc::ServerBuilder builder;
	builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

	RemoteTargetService service;
	builder.RegisterService(&service);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	server->Wait();
	
	return 0;
}