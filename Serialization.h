#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <arrow/buffer.h>

namespace gilt {
using DatasourceProgress = std::unordered_map<std::string, std::string>;
using SessionProgress = std::unordered_map<std::string, DatasourceProgress>;

/// TODO: Consider serialization lib.

inline std::string serializeProgress(const SessionProgress &progress) {
    std::stringstream ss;
    ss << progress.size();
    for (const auto &[datasourceName, datasourceProgress] : progress) {
        ss << " " << datasourceName << " " << datasourceProgress.size();
        for (const auto &[key, val] : datasourceProgress) {
            ss << " " << key << " " << val;
        }
    }
    return ss.str();
}

inline std::shared_ptr<arrow::Buffer> serializeProgressToBuffer(const SessionProgress &progress) {
    return arrow::Buffer::FromString(serializeProgress(progress));
}

inline std::string serializeSession(const std::string &sessionId, const SessionProgress &progress) {
    std::stringstream ss;
    ss << sessionId << " " << serializeProgress(progress);
    return ss.str();
}

inline SessionProgress deserializeProgress(std::stringstream &ss) {
    SessionProgress progress;

    size_t progressSize = 0;
    ss >> progressSize;

    for (size_t i = 0; i < progressSize; i++) {
        std::string datasourceName;
        DatasourceProgress datasourceProgress;
        size_t datasourceProgressSize = 0;
        ss >> datasourceName >> datasourceProgressSize;
        for (size_t j = 0; j < datasourceProgressSize; j++) {
            std::string key, val;
            ss >> key >> val;
            datasourceProgress[key] = val;
        }
        progress[datasourceName] = std::move(datasourceProgress);
    }

    return progress;
}

inline SessionProgress deserializeProgress(const std::shared_ptr<arrow::Buffer> &buffer) {
    std::stringstream ss(buffer->ToString());
    return deserializeProgress(ss);
}

inline std::tuple<std::string, SessionProgress> deserializeSessionAndProgress(const std::string &from) {
    std::stringstream ss(from);

    std::string sessionId;
    ss >> sessionId;
    auto progress = deserializeProgress(ss);

    return std::make_tuple(std::move(sessionId), std::move(progress));
}

inline std::string serializeScanCommand(const std::string &commandStr, const std::string &table, const std::vector<std::string> &columns,
                                        size_t batchSize) {
    std::stringstream ss;
    ss << commandStr << " " << table << " " << columns.size();
    for (const auto &column : columns) {
        ss << " " << column;
    }
    ss << " " << batchSize;
    return ss.str();
}

inline std::tuple<std::string, std::string, std::vector<std::string>, size_t> deserializeScanCommand(const std::string &from) {
    std::stringstream ss(from);
    std::string commandStr;
    std::string table;
    std::vector<std::string> columnNames;
    size_t batchSize = 0;
    ss >> commandStr >> table;
    size_t columnsSize = 0;
    ss >> columnsSize;
    columnNames.reserve(columnsSize);
    for (size_t i = 0; i < columnsSize; i++) {
        std::string column;
        ss >> column;
        columnNames.push_back(column);
    }
    ss >> batchSize;
    return {std::move(commandStr), std::move(table), std::move(columnNames), std::move(batchSize)};
}
} // namespace gilt
