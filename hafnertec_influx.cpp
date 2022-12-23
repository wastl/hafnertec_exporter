//
// Created by wastl on 09.12.22.
//

#include "hafnertec_influx.h"

#include <chrono>
#include <influxdb.hpp>
#include <glog/logging.h>

using namespace std::chrono;

int hafnertec::write_influx(const std::string &server, int port, const std::string &db, const hafnertec::HafnertecData &data) {
    long long ts = time_point_cast<nanoseconds>(system_clock::now()).time_since_epoch().count();

    int result;
    std::string response;

    influxdb_cpp::server_info si(server, port, db);
    result = influxdb_cpp::builder()
            .meas("heizleistung")
            .field("brennkammer", data.getTempBrennkammer())
            .field("vorlauf", data.getTempVorlauf())
            .field("ruecklauf", data.getTempRuecklauf())
            .field("ventilator", data.getVentilator())
            .field("durchlauf", data.getDurchlauf())
            .field("anteil_heizung", data.getAnteilHeizung())
            .timestamp(ts)

            .post_http(si, &response);

    if (result != 0) {
        LOG(ERROR) << "Error while writing to InfluxDB: " << response;
        return result;
    }

    return 0;
}