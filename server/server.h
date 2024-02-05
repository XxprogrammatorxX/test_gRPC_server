#pragma once

#include "remote_target.pb.h"
#include "remote_target.grpc.pb.h"

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>

#include <shared_mutex>
#include <chrono>

#include "connection.h"


class RemoteTargetService final : public remote_target::RemoteTarget::Service {
	std::unordered_map <connectionID, std::shared_ptr<Connection>> connections;
	ConnectionIDGenerator connection_gen;

	std::chrono::steady_clock::duration confirmation_interval;

	mutable std::shared_mutex mutex;

	void async_confirmation_waiter(std::shared_ptr<Connection> connection_ptr);

public:
	// Request a connection
	virtual ::grpc::Status RequestConnection(::grpc::ServerContext* context, const ::remote_target::EmptyReq* request, ::remote_target::Ping* response);
	// Confirmes the connection
	virtual ::grpc::Status ConfirmConnection(::grpc::ServerContext* context, const ::remote_target::Pong* request, ::remote_target::ConnectionConfirmed* response);
	// Terminates the connection
	virtual ::grpc::Status Disconnect(::grpc::ServerContext* context, const ::remote_target::ConnectionID* request, ::remote_target::ConnectionTerminated* response);
	// Available only after confirmed connection
	virtual ::grpc::Status GetDeviceSpec(::grpc::ServerContext* context, const ::remote_target::ConnectionID* request, ::remote_target::DeviceSpec* response);
};

