#include "remote_target.pb.h"
#include "remote_target.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <chrono>

#include "connection.h"
#include "DeviceSpec.h"

class RemoteTargetService final : public remote_target::RemoteTarget::Service {
	std::unordered_map <connectionID, std::shared_ptr<Connection>> connections;
	ConnectionIDGenerator connection_gen;

	std::chrono::steady_clock::duration confirmation_interval;

	mutable std::shared_mutex mutex;

	void async_confirmation_waiter(std::shared_ptr<Connection> connection_ptr)
	{
		std::this_thread::sleep_for(std::chrono::seconds(10));

		std::unique_lock lock(mutex);
		auto it = connections.find(connection_ptr->id);
		if (it != connections.end())
			if(!connection_ptr->confirmed)
				connections.erase(connection_ptr->id);

		std::cout << "Connection " << connection_ptr->id << " dropped by a timeout." << std::endl;
	}

	public:

		// Request a connection
		virtual ::grpc::Status RequestConnection(::grpc::ServerContext* context, const ::remote_target::EmptyReq* request, ::remote_target::Ping* response)
		{
			std::unique_lock lock(mutex);

			auto tod = std::chrono::steady_clock::now();
			auto id = connection_gen.get_new_ID();
			auto connection_ptr = std::make_shared<Connection>(id, false, tod);

			connections.emplace(id, connection_ptr);
			std::thread(&RemoteTargetService::async_confirmation_waiter, this, connection_ptr).detach();

			response->set_connectionid(id);
			std::cout << "Connection " << id << " requested." << std::endl;
			return grpc::Status::OK;
		}
		// Confirmes the connection
		virtual ::grpc::Status ConfirmConnection(::grpc::ServerContext* context, const ::remote_target::Pong* request, ::remote_target::ConnectionConfirmed* response)
		{
			connectionID id = request->connectionid();
			
			response->set_connectionid(id);
			response->set_success(false);

			{
				std::shared_lock lock(mutex);
				auto it = connections.find(id);
				if (it != connections.end())
				{
					it->second->confirmed = true;
					response->set_success(true);
					std::cout << "Connection " << id << " confirmed." << std::endl;
				}
				else
					std::cout << "Connection " << id << " not confirmed." << std::endl;
			}

			return grpc::Status::OK;
		}
		// Terminates the connection
		virtual ::grpc::Status Disconnect(::grpc::ServerContext* context, const ::remote_target::ConnectionID* request, ::remote_target::ConnectionTerminated* response)
		{
			connectionID id = request->connectionid();
			{
				std::unique_lock lock(mutex);
				connections.erase(id);
				std::cout << "Connection " << id << " dropped by a request." << std::endl;
			}

			using namespace std::chrono;
			auto tod = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();
			response->set_tod(tod);

			return grpc::Status::OK;
		}
		// Available only after confirmed connection
		virtual ::grpc::Status GetDeviceSpec(::grpc::ServerContext* context, const ::remote_target::ConnectionID* request, ::remote_target::DeviceSpec* response)
		{
			connectionID id = request->connectionid();

			std::shared_lock lock(mutex);
			DeviceSpec spec;
			auto it = connections.find(id);
			if (it != connections.end() && it->second->confirmed)
			{
				spec = get_device_spec();
				std::cout << "DeviceSpec been shared with connection " << id << "." << std::endl;
			}
			else
				std::cout << "Empty DeviceSpec been shared with connection " << id << "." << std::endl;

			response->set_name(spec.name);
			response->set_os_version(spec.OS_version);
			response->set_serial_number(spec.serial_number);

			return grpc::Status::OK;		
		}

};

int main(int argc, char* argv[])
{
	grpc::ServerBuilder builder;
	builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

	RemoteTargetService service;
	builder.RegisterService(&service);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	server->Wait();
	
	return 0;
}