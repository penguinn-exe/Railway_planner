#ifndef STATION_HPP
#define STATION_HPP

#include <string>
#include <iostream>

struct Station {
    int id;
    std::string code;
    std::string name;
    std::string city;
    std::string state;
    int platforms;

    Station() : id(0), platforms(1) {}

    Station(int id_, std::string code_, std::string name_, std::string city_, std::string state_, int platforms_ = 4)
        : id(id_), code(std::move(code_)), name(std::move(name_)), city(std::move(city_)), state(std::move(state_)), platforms(platforms_) {}
};

#endif // STATION_HPP
