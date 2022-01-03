// All functionality that forms the correct requests to DataIO
#pragma once

#include "Serialization.h"

#include <cstddef>
#include <magic_enum.hpp>

#include <arrow/buffer.h>
#include <arrow/flight/types.h>

#include <sstream>

namespace gilt {
using namespace magic_enum;

enum class ETLCommand {
    SESSION_START,
    SESSION_END,
    CHECKPOINT,
    COMMIT,
    REPARTITION
};

inline std::string cmdToString(ETLCommand c) {
    return static_cast<std::string>(enum_name(c));
}

inline arrow::flight::Action startSession(const std::string &sessionId) {
    return {.type = cmdToString(ETLCommand::SESSION_START),
            .body = arrow::Buffer::FromString(sessionId)};
}

inline arrow::flight::Action endSession(const std::string &sessionId) {
    return {.type = cmdToString(ETLCommand::SESSION_END),
            .body = arrow::Buffer::FromString(sessionId)};
}

inline arrow::flight::Action doCheckpoint(const std::string &sessionId, const SessionProgress &progress) {
    return {.type = cmdToString(ETLCommand::CHECKPOINT),
            .body = arrow::Buffer::FromString(serializeSession(sessionId, progress))};
}

inline arrow::flight::Action doCommit(const std::string &sessionId, const SessionProgress &progress) {
    return {.type = cmdToString(ETLCommand::COMMIT),
            .body = arrow::Buffer::FromString(serializeSession(sessionId, progress))};
}

inline arrow::flight::Action doRepartition(const std::string &tableId) {
    return {.type = cmdToString(ETLCommand::REPARTITION),
            .body = arrow::Buffer::FromString(tableId)};
}

inline arrow::flight::FlightDescriptor insertDescriptor(const std::string &sessionId, const std::string &tableId) {
    return arrow::flight::FlightDescriptor::Path({{sessionId}, {tableId}});
}

enum class ScanCommand {
    METADATA,
    DIRECT
};

inline arrow::flight::Ticket scanTicket(const ScanCommand &command, const std::string &table, const std::vector<std::string> &columns,
                                        size_t batchSize = static_cast<size_t>(1024 * 1024)) {
    return {serializeScanCommand(static_cast<std::string>(enum_name(command)), table, columns, batchSize)};
}
} // namespace gilt
