//
// Created by wastl on 09.12.22.
//

#include <absl/strings/str_join.h>
#include <cctype>
#include <iostream>
#include <cpprest/uri.h>                        // URI library
#include <cpprest/http_msg.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <glog/logging.h>
#include <gumbo-query/Document.h>
#include <gumbo-query/Node.h>

#include "hafnertec_client.h"

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

namespace hafnertec {

    double HafnertecData::getTempBrennkammer() const {
        return temp_brennkammer;
    }

    void HafnertecData::setTempBrennkammer(double tempBrennkammer) {
        temp_brennkammer = tempBrennkammer;
    }

    double HafnertecData::getTempRuecklauf() const {
        return temp_ruecklauf;
    }

    void hafnertec::HafnertecData::setTempRuecklauf(double tempRuecklauf) {
        temp_ruecklauf = tempRuecklauf;
    }

    double HafnertecData::getTempVorlauf() const {
        return temp_vorlauf;
    }

    void HafnertecData::setTempVorlauf(double tempVorlauf) {
        temp_vorlauf = tempVorlauf;
    }

    double HafnertecData::getDurchlauf() const {
        return durchlauf;
    }

    void HafnertecData::setDurchlauf(double durchlauf) {
        HafnertecData::durchlauf = durchlauf;
    }

    double HafnertecData::getVentilator() const {
        return ventilator;
    }

    void HafnertecData::setVentilator(double ventilator) {
        HafnertecData::ventilator = ventilator;
    }

    double HafnertecData::getAnteilHeizung() const {
        return anteil_heizung;
    }

    void HafnertecData::setAnteilHeizung(double anteilHeizung) {
        anteil_heizung = anteilHeizung;
    }

    double parse_numeric(absl::string_view s) {
        int start, end;

        // find start of number
        for (start = 0; start < s.size(); start++) {
            if (std::isdigit(s[start]) && s[start] != '-' && s[start] != '.' && s[start] != '+') {
                break;
            }
        }

        // find end of number
        for (end = s.size() - 1; end > start; end--) {
            if (std::isdigit(s[end]) && s[end] != '-' && s[end] != '.' && s[end] != '+') {
                break;
            }
        }

        if (start == end) {
            LOG(ERROR) << "Could not find number in '" << s << "'";
            return 0;
        }

        std::string n = std::string(s.substr(start, end));

        return std::atof(n.c_str());
    }

    void query(const std::string &uri, std::string &user, std::string password,
               const std::function<void(const HafnertecData &)> &handler) {
        // Create http_client to send the request.
        web::credentials credentials(user, password);
        http_client_config config;
        config.set_credentials(credentials);

        http_client client(U(uri), config);

        // request data

        // Build request URI and start the request.
        uri_builder builder(U("/schematic_files/9.cgi"));
        try {
            return client.request(methods::GET, builder.to_string()).then(
                    [=](http_response response) {
                        if (response.status_code() == status_codes::OK) {

                            std::string html = absl::StrCat("<html>", response.extract_string().get(), "</html>");
                            CDocument doc;
                            doc.parse(html.c_str());

                            CSelection c = doc.find("html div");

                            std::cout << "Brennkammer: " << parse_numeric(c.nodeAt(1).text()) << std::endl;
                            hafnertec::HafnertecData data;
                            data.setTempBrennkammer(parse_numeric(c.nodeAt(1).text()));
                            data.setAnteilHeizung(parse_numeric(c.nodeAt(2).text()));
                            data.setTempVorlauf(parse_numeric(c.nodeAt(3).text()));
                            data.setTempRuecklauf(parse_numeric(c.nodeAt(4).text()));
                            data.setDurchlauf(parse_numeric(c.nodeAt(5).text()));
                            data.setVentilator(parse_numeric(c.nodeAt(9).text()));

                            handler(data);
                        } else {
                            LOG(ERROR) << "Hafnertec query failed: " << response.reason_phrase();
                        }
                    }).get();
        } catch (const std::exception &e) {
            LOG(ERROR) << "Error while retrieving Hafnertec data: " << e.what();
        }
    }


}  // namespace hafnertec