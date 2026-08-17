#ifndef TRAIN_HPP
#define TRAIN_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <sstream>

struct TrainStop {
    std::string stationCode;
    std::string stationName;
    int arrivalTimeMins;   // Minutes from 00:00 on Day 1
    int departureTimeMins; // Minutes from 00:00 on Day 1
    double distanceKm;
    double fareINR;
    int dayNumber;

    TrainStop() : arrivalTimeMins(0), departureTimeMins(0), distanceKm(0.0), fareINR(0.0), dayNumber(1) {}

    TrainStop(std::string code, std::string name, int arrMins, int depMins, double dist, double fare, int day = 1)
        : stationCode(std::move(code)), stationName(std::move(name)), arrivalTimeMins(arrMins),
          departureTimeMins(depMins), distanceKm(dist), fareINR(fare), dayNumber(day) {}

    static std::string formatTime(int mins) {
        int hours = (mins / 60) % 24;
        int m = mins % 60;
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setfill('0') << std::setw(2) << m;
        return oss.str();
    }
};

struct Train {
    int trainNumber;
    std::string trainName;
    std::string type;
    std::string sourceCode;
    std::string destCode;
    std::vector<TrainStop> stops;
    std::unordered_map<std::string, int> availableSeats; // e.g. "SL": 50, "3A": 20, "2A": 10, "1A": 5

    Train() : trainNumber(0) {}

    Train(int number, std::string name, std::string type_, std::string src, std::string dest)
        : trainNumber(number), trainName(std::move(name)), type(std::move(type_)),
          sourceCode(std::move(src)), destCode(std::move(dest)) {
        availableSeats["SL"] = 100;
        availableSeats["3A"] = 60;
        availableSeats["2A"] = 30;
        availableSeats["1A"] = 12;
    }

    int getStopIndex(const std::string& stationCode) const {
        for (size_t i = 0; i < stops.size(); ++i) {
            if (stops[i].stationCode == stationCode) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool passesThrough(const std::string& srcCode, const std::string& destCode) const {
        int srcIdx = getStopIndex(srcCode);
        int destIdx = getStopIndex(destCode);
        return (srcIdx != -1 && destIdx != -1 && srcIdx < destIdx);
    }

    double calculateDistance(const std::string& srcCode, const std::string& destCode) const {
        int srcIdx = getStopIndex(srcCode);
        int destIdx = getStopIndex(destCode);
        if (srcIdx != -1 && destIdx != -1 && srcIdx < destIdx) {
            return stops[destIdx].distanceKm - stops[srcIdx].distanceKm;
        }
        return 0.0;
    }

    double calculateFare(const std::string& srcCode, const std::string& destCode, const std::string& seatClass = "SL") const {
        int srcIdx = getStopIndex(srcCode);
        int destIdx = getStopIndex(destCode);
        if (srcIdx != -1 && destIdx != -1 && srcIdx < destIdx) {
            double baseFare = stops[destIdx].fareINR - stops[srcIdx].fareINR;
            if (seatClass == "3A") baseFare *= 1.8;
            else if (seatClass == "2A") baseFare *= 2.5;
            else if (seatClass == "1A") baseFare *= 4.0;
            return baseFare;
        }
        return 0.0;
    }

    int calculateDurationMins(const std::string& srcCode, const std::string& destCode) const {
        int srcIdx = getStopIndex(srcCode);
        int destIdx = getStopIndex(destCode);
        if (srcIdx != -1 && destIdx != -1 && srcIdx < destIdx) {
            return stops[destIdx].arrivalTimeMins - stops[srcIdx].departureTimeMins;
        }
        return 0;
    }
};

#endif // TRAIN_HPP
