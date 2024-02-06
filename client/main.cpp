#include "remote_target.pb.h"
#include "remote_target.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

namespace remote_target_test
{

int test_with_timeout(std::shared_ptr<::remote_target::RemoteTarget::Stub> stub, std::ostream& out, std::chrono::seconds timeout)
{
	uint32_t id = 0;
	{
		::remote_target::EmptyReq connection_req;
		::remote_target::Ping ping_response;

		grpc::ClientContext context;
		grpc::Status status = stub->RequestConnection(&context, connection_req, &ping_response);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		id = ping_response.connectionid();
		out << "Connection requested with id : " << id << std::endl;
	}
	std::this_thread::sleep_for(timeout);
	{
		::remote_target::Pong pong_request;
		pong_request.set_connectionid(id);
		::remote_target::ConnectionConfirmed confirmed_response;

		grpc::ClientContext context;
		grpc::Status status = stub->ConfirmConnection(&context, pong_request, &confirmed_response);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		if (confirmed_response.success())
			out << "Connection confirmed with id : " << id << std::endl;
		else
		{
			out << "Connection not confirmed." << std::endl;
			return -1;
		}
	}
	{
		::remote_target::ConnectionID devspec_request;
		devspec_request.set_connectionid(id);
		::remote_target::DeviceSpec devspec;
	
		grpc::ClientContext context;
		grpc::Status status = stub->GetDeviceSpec(&context, devspec_request, &devspec);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		out << "name : " << devspec.name() << std::endl;
		out << "OS_version : " << devspec.os_version() << std::endl;
		out << "serial_number : " << devspec.serial_number() << std::endl;
		out << "comment :  " << devspec.comment() << std::endl;
	}
	{
		::remote_target::ConnectionID disconnect_request;
		disconnect_request.set_connectionid(id);
		::remote_target::ConnectionTerminated respone;

		grpc::ClientContext context;
		grpc::Status status = stub->Disconnect(&context, disconnect_request, &respone);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		out << "Connection disconnected with id : " << id << std::endl;
	}
	return 0;
}

int test_without_confirmation(std::shared_ptr<::remote_target::RemoteTarget::Stub> stub, std::ostream& out)
{
	uint32_t id = 0;
	{
		::remote_target::EmptyReq connection_req;
		::remote_target::Ping ping_response;

		grpc::ClientContext context;
		grpc::Status status = stub->RequestConnection(&context, connection_req, &ping_response);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		id = ping_response.connectionid();
		out << "Connection requested with id : " << id << std::endl;
	}
	{
		::remote_target::ConnectionID devspec_request;
		devspec_request.set_connectionid(id);
		::remote_target::DeviceSpec devspec;
	
		grpc::ClientContext context;
		grpc::Status status = stub->GetDeviceSpec(&context, devspec_request, &devspec);

		if (status.error_code() != grpc::StatusCode::OK)
		{
			out << "The connection to the server is down. ID : " << id << std::endl;
			return -1;
		}

		out << "name : " << devspec.name() << std::endl;
		out << "OS_version : " << devspec.os_version() << std::endl;
		out << "serial_number : " << devspec.serial_number() << std::endl;
		out << "comment :  " << devspec.comment() << std::endl;

		if (!devspec.name().empty() || !devspec.os_version().empty() ||
			!devspec.serial_number().empty() || !devspec.comment().empty())
			return -1;
	}
	return 0;
}

}


int main(int argc, char* argv[])
{
	std::string addr;
	if (argc == 2)
		addr = argv[1];
	else
		addr = "0.0.0.0:50051";

	auto channel = grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
	std::shared_ptr<::remote_target::RemoteTarget::Stub> stub = ::remote_target::RemoteTarget::NewStub(channel);
	
	size_t testN = 0;
	auto print_test_result = [& testN](int result)
	{
		std::cout << "Test #" << testN << ":" << ((result == 0) ? " succeed." : " failed.") << std::endl;
		testN++;
	};

	std::ofstream out ("./log", std::ios::out);

	print_test_result(remote_target_test::test_with_timeout(stub, out, std::chrono::seconds(0)));
	print_test_result(remote_target_test::test_with_timeout(stub, out, std::chrono::seconds(5)));
	print_test_result(remote_target_test::test_with_timeout(stub, out, std::chrono::seconds(10)));
	print_test_result(remote_target_test::test_without_confirmation(stub, out));

	std::cout << "See additional info in ./log" << std::endl;

	return 0;
}
