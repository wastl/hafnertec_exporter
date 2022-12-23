//
// Created by wastl on 09.12.22.
//
#include <functional>
#include <string>

#ifndef HAFNERTEC_EXPORTER_HAFNERTEC_CLIENT_H
#define HAFNERTEC_EXPORTER_HAFNERTEC_CLIENT_H
namespace hafnertec {

    class HafnertecData {
    private:
        double temp_brennkammer;
        double temp_ruecklauf;
        double temp_vorlauf;
        double durchlauf;
        double ventilator;
        double anteil_heizung;
    public:
        double getAnteilHeizung() const;

        void setAnteilHeizung(double anteilHeizung);

    public:

        double getTempBrennkammer() const;

        void setTempBrennkammer(double tempBrennkammer);

        double getTempRuecklauf() const;

        void setTempRuecklauf(double tempRuecklauf);

        double getTempVorlauf() const;

        void setTempVorlauf(double tempVorlauf);

        double getDurchlauf() const;

        void setDurchlauf(double durchlauf);

        double getVentilator() const;

        void setVentilator(double ventilator);
    };


    void query(const std::string& uri, std::string& user, std::string password, const std::function<void(const HafnertecData&)>& handler);
}



#endif //HAFNERTEC_EXPORTER_HAFNERTEC_CLIENT_H
