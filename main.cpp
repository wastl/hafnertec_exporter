#include <iostream>
#include <thread>
#include <chrono>
#include <yaml-cpp/yaml.h>
#include <glog/logging.h>

#include "hafnertec_client.h"
#include "hafnertec_influx.h"

using namespace std::chrono;

class config {
public:
    config() {}

    // Hafnertec configuration
    std::string hafnertec_server;
    std::string hafnertec_user = "";
    std::string hafnertec_password = "";

    // Influx configuration
    std::string influx_server = "127.0.0.1";
    int influx_port = 8086;
    std::string influx_db = "hafnertec";
    std::string influx_user = "";
    std::string influx_password = "";

    // Tool configuration
    int poll_interval = 1;
};

#define CONFIG(FIELD, CFG, TYPE) \
    if(yaml[CFG]) {             \
       c-> FIELD = yaml[CFG].as<TYPE>(); \
    }

config* load_config(const std::string& file) {
    YAML::Node yaml = YAML::LoadFile(file);

    config* c = new config();
    if (!yaml["hafnertec_server"]) {
        LOG(ERROR) << "Hafnertec Server nicht konfiguriert";
        return c;
    }
    c->hafnertec_server = yaml["hafnertec_server"].as<std::string>();

    CONFIG(hafnertec_user, "hafnertec_user", std::string);
    CONFIG(hafnertec_password, "hafnertec_password", std::string);
    CONFIG(influx_server, "influx_server", std::string);
    CONFIG(influx_db, "influx_db", std::string);
    CONFIG(influx_port, "influx_port", int);
    CONFIG(influx_user, "influx_user", std::string);
    CONFIG(influx_password, "influx_password", std::string);
    CONFIG(poll_interval, "poll_interval", int);

    return c;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: senec_exporter <path-to-config.yml>" << std::endl;
        return 1;
    }

    google::InitGoogleLogging(argv[0]);

    config* cfg = load_config(argv[1]);
    LOG(INFO) << "Hafnertec Server: " << cfg->hafnertec_server;
    LOG(INFO) << "Influx Server: " << cfg->influx_server;
    LOG(INFO) << "Poll Interval: " << cfg->poll_interval;

    auto next = system_clock::now();
    while (true) {
        next += seconds(cfg->poll_interval);
        std::this_thread::sleep_until(next);

        hafnertec::query(cfg->hafnertec_server, cfg->hafnertec_user, cfg->hafnertec_password, [=](const hafnertec::HafnertecData& data) {
            LOG(INFO) << "Hafnertec Daten empfangen (Brennkammer: " << data.getTempBrennkammer()
                      << ", Vorlauf: " << data.getTempVorlauf()
                      << ", Rücklauf: " << data.getTempRuecklauf() <<")";

           hafnertec::write_influx(cfg->influx_server, cfg->influx_port, cfg->influx_db, data);
        });
    }

    return 0;
}