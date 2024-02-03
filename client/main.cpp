#include "remote_target.pb.h"
#include "remote_target.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include <iostream>


int main(int argc, char* argv[])
{

	// Call
	auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
	std::unique_ptr<::remote_target::RemoteTarget::Stub> stub = ::remote_target::RemoteTarget::NewStub(channel);
	
	uint32_t id = 0;

	std::cout << "1" << std::endl;
	{
		// Setup request
		::remote_target::EmptyReq connection_req;
		::remote_target::Ping ping_response;

		grpc::ClientContext context;
		grpc::Status status = stub->RequestConnection(&context, connection_req, &ping_response);

		id = ping_response.connectionid();
		std::cout << "2" << std::endl;
	}
	{
		// Setup request
		::remote_target::Pong pong_request;
		pong_request.set_connectionid(id);
		::remote_target::ConnectionConfirmed confirmed_response;

		grpc::ClientContext context;
		grpc::Status status = stub->ConfirmConnection(&context, pong_request, &confirmed_response);

		std::cout << "3" << std::endl;
	}
	{
		// Setup request
		::remote_target::ConnectionID devspec_request;
		devspec_request.set_connectionid(id);
		::remote_target::DeviceSpec devspec;
	
		grpc::ClientContext context;
		grpc::Status status = stub->GetDeviceSpec(&context, devspec_request, &devspec);
	
		std::cout << "4" << std::endl;

	
		// Output result
		std::cout << "name : " << devspec.name() << std::endl;
		std::cout << "OS_version : " << devspec.os_version() << std::endl;
		std::cout << "serial_number : " << devspec.serial_number() << std::endl;
		std::cout << "comment :  " << devspec.comment() << std::endl;
	}
	{
		// Setup request
		::remote_target::ConnectionID disconnect_request;
		disconnect_request.set_connectionid(id);
		::remote_target::ConnectionTerminated respone;

		grpc::ClientContext context;
		grpc::Status status = stub->Disconnect(&context, disconnect_request, &respone);

		std::cout << "5" << std::endl;
	}
	return 0;
}