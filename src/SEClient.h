#pragma once

#include "management.grpc.pb.h"

#include <string>

#include <arrow/flight/client.h>
#include <grpcpp/grpcpp.h>

namespace gilt {

class SEClient final : public arrow::flight::FlightClient {
public:
    grpc::Status createTable(const rpc::management::Table &request);
    grpc::Status getTable(const rpc::management::TableName &request, rpc::management::Table *reply);
    grpc::Status getTableRowCount(const rpc::management::TableName &request, rpc::management::TableRowCount *reply);
    grpc::Status getTableKeys(const rpc::management::TableName &request, rpc::management::TableKeys *reply);
    grpc::Status listTables(rpc::management::Tables *reply);
    grpc::Status dropTable(const rpc::management::TableName &request);
    grpc::Status dropTables(const rpc::management::TableNames &request);

    static void connect(int port, std::unique_ptr<SEClient> *instance);
private:
    std::unique_ptr<rpc::management::TableManager::Stub> stub;
};
using SEClientPtr = std::unique_ptr<SEClient>;

namespace rpc::client {
SEClientPtr create(int port);
} // namespace rpc::client

} // namespace gilt
