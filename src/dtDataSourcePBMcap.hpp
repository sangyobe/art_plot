// This file is part of dtCore, a C++ library for robotics software
// development.
//
// This library is commercial and cannot be redistributed, and/or modified
// WITHOUT ANY ALLOWANCE OR PERMISSION OF Hyundai Motor Company.

#ifndef __DT_DAQ__DATASSOURCEPBMCAP_H__
#define __DT_DAQ__DATASSOURCEPBMCAP_H__

#include <dtCore/src/dtDAQ/dtDataSourcePB.hpp>
#include <dtCore/src/dtUtils/dtFileHelper.hpp>
#include <sys/time.h>

#include <chrono>
#include <fstream>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

#define MCAP_COMPRESSION_NO_LZ4
#define MCAP_COMPRESSION_NO_ZSTD
#define MCAP_IMPLEMENTATION
#include <mcap/mcap.hpp>

#define USE_LOGGINGTIME_AS_TIMESTAMP

namespace dt
{
namespace DAQ
{

template <typename T>
class DataSourcePBMcap : public DataSourcePB<T>
{
public:
    DataSourcePBMcap(const std::string &topic_name, const std::string &schema_name, const std::string &file_name)
        : _topic_name(topic_name), _schema_name(schema_name), _file_name(file_name)
    {
        const auto res = _reader.open(file_name);
        if (!res.ok())
        {
            std::cerr << "Failed to open " << file_name << " for reading: " << res.message
                      << std::endl;
        }
        else
        {
            auto messageView = _reader.readMessages();
            mcap::Timestamp logTime_min{mcap::MaxTime}, logTime_max{0};

            for (auto itr = messageView.begin(); itr != messageView.end(); itr++)
            {
                if (itr->schema->encoding != "protobuf" || itr->schema->name != _schema_name)
                {
                    continue;
                }

                T message;
                if (!message.ParseFromArray(static_cast<const void *>(itr->message.data), itr->message.dataSize))
                {
                    std::cerr << "could not parse " << _schema_name << std::endl;
                    break;
                }

#ifdef USE_LOGGINGTIME_AS_TIMESTAMP
                // use message's logTime
                mcap::Timestamp logTime = itr->message.logTime;
                message.mutable_header()->mutable_time_stamp()->set_seconds((long)(logTime / 1000000000));
                message.mutable_header()->mutable_time_stamp()->set_nanos((long)(logTime % 1000000000));
#else
                // use message's timestamp
                mcap::Timestamp logTime = (uint64_t)(message.header().time_stamp().seconds() * 1000000000) + (uint64_t)message.header().time_stamp().nanos();
#endif

                if (logTime_min > logTime) logTime_min = logTime;
                if (logTime_max < logTime) logTime_max = logTime;

                _msgs.push_back(message);
            }

            _msg_count = _msgs.size();
            _msg_index = 0;
            _logTime_min = logTime_min;
            _logTime_max = logTime_max;

            PrintDataInfo();

            // close the reader
            _reader.close();
        }
    }

    ~DataSourcePBMcap()
    {
        // close the reader
        _reader.close();
    }

    bool UpdateData(void *context) override // context --> uint64_t nsec
    {
        bool updated = false;

        // play data until ctx->curT_nsec
        if (_msg_index < _msg_count)
        {
            this->_msg = _msgs[_msg_index];
            _msg_index++;
            updated = true;
        }

        return updated;
    }

    void OnParseErrorMcap(const mcap::Status &status)
    {
        std::cerr << "Errors on parsing mcap file, " << _file_name << ": " << status.message
                  << std::endl;

        _reader.close();
    }

    uint32_t msg_size() const { return _msg_count; }
    mcap::Timestamp logTime_min() const { return _logTime_min; }
    mcap::Timestamp logTime_max() const { return _logTime_max; }

    void PrintDataInfo()
    {
        std::cout << "> file name   : " << _file_name << std::endl;
        std::cout << "> topic name  : " << _topic_name << std::endl;
        std::cout << "> schema name : " << _schema_name << std::endl;
        std::cout << "> message count: " << _msg_count << " " << std::endl;
        std::cout << "> logTime(min): " << (uint64_t)(_logTime_min / 1000000000) << "(s) " << _logTime_min % 1000000000 << "(ns) " << std::endl;
        std::cout << "         (max): " << (uint64_t)(_logTime_max / 1000000000) << "(s) " << _logTime_max % 1000000000 << "(ns) " << std::endl;
        std::cout << "" << std::endl;
    }

protected:
    std::string _topic_name;
    std::string _schema_name;
    std::string _file_name;
    std::atomic_bool _messageAvailable{false};
    std::vector<T> _msgs;
    uint32_t _msg_count{0};
    mcap::Timestamp _logTime_min{mcap::MaxTime};
    mcap::Timestamp _logTime_max{0};
    uint32_t _msg_index{0};

    // mcap reader
    mcap::McapReader _reader;
};

} // namespace DAQ
} // namespace dt

#endif // __DT_DAQ__DATASSOURCEPBMCAP_H__