#include "SEClient.h"

#include <arrow/util/logging.h>
#include <fmt/format.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using namespace arrow::flight;

namespace gilt {
using namespace rpc::management;

SEClientPtr rpc::client::create(int port) {
    SEClientPtr instance;
    instance->connect(port, &instance);
    return instance;
}

void SEClient::connect(int port, std::unique_ptr<SEClient> *instance) {
    Location loc;
    ARROW_CHECK_OK(Location::ForGrpcTcp("localhost", port, &loc));
    ARROW_CHECK_OK(FlightClient::Connect(loc, reinterpret_cast<std::unique_ptr<FlightClient> *>(instance)));
    std::string targetStr = fmt::format("localhost:{}", port);
    (*instance)->stub = TableManager::NewStub(grpc::CreateChannel(targetStr, grpc::InsecureChannelCredentials()));

    assert((*instance)->stub != nullptr);
}

Status SEClient::createTable(const Table &request) {
    // Container for the data we expect from the server.
    google::protobuf::Empty reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->CreateTable(&context, request, &reply);

    return status;
}

Status SEClient::getTable(const TableName &request, Table *reply) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->GetTable(&context, request, reply);

    return status;
}

grpc::Status SEClient::getTableRowCount(const TableName &request, rpc::management::TableRowCount *reply) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->GetTableRowCount(&context, request, reply);

    return status;
}

grpc::Status SEClient::getTableKeys(const TableName &request, rpc::management::TableKeys *reply) {
    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->GetTableKeys(&context, request, reply);

    return status;
}

grpc::Status SEClient::listTables(rpc::management::Tables *reply) {
    google::protobuf::Empty request;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->ListTables(&context, request, reply);

    return status;
}

grpc::Status SEClient::dropTable(const TableName &request) {
    // Container for the data we expect from the server.
    google::protobuf::Empty reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->DropTable(&context, request, &reply);

    return status;
}

grpc::Status SEClient::dropTables(const TableNames &request) {
    // Container for the data we expect from the server.
    google::protobuf::Empty reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub->DropTables(&context, request, &reply);

    return status;
}
} // namespace gilt
