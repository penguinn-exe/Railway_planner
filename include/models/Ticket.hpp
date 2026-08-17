#ifndef TICKET_HPP
#define TICKET_HPP

#include <string>

struct Ticket {
    std::string pnr;
    int trainNumber;
    std::string trainName;
    std::string sourceCode;
    std::string destCode;
    std::string passengerName;
    int age;
    std::string seatClass;
    double fare;
    std::string status; // "CNF", "RAC", "WL"
    std::string journeyDate;

    Ticket() : trainNumber(0), age(0), fare(0.0) {}

    Ticket(std::string pnr_, int trainNum, std::string trainName_, std::string src, std::string dest,
           std::string passenger, int age_, std::string cls, double fare_, std::string status_, std::string date_)
        : pnr(std::move(pnr_)), trainNumber(trainNum), trainName(std::move(trainName_)), sourceCode(std::move(src)),
          destCode(std::move(dest)), passengerName(std::move(passenger)), age(age_), seatClass(std::move(cls)),
          fare(fare_), status(std::move(status_)), journeyDate(std::move(date_)) {}
};

#endif // TICKET_HPP
