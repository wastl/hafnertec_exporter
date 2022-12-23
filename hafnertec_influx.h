//
// Created by wastl on 09.12.22.
//

#ifndef HAFNERTEC_EXPORTER_HAFNERTEC_INFLUX_H
#define HAFNERTEC_EXPORTER_HAFNERTEC_INFLUX_H

#include "hafnertec_client.h"

#include <string>

namespace hafnertec {

    int write_influx(const std::string& server, int port, const std::string& db, const HafnertecData& data);
}



#endif //HAFNERTEC_EXPORTER_HAFNERTEC_INFLUX_H
