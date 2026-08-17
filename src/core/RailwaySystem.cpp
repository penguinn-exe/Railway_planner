#include "../../include/core/RailwaySystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

RailwaySystem::RailwaySystem() : nextReviewId(1), pnrCounter(8400123456ULL) {}

std::string RailwaySystem::generatePNR() {
    return std::to_string(pnrCounter++);
}

void RailwaySystem::addStation(const Station& station) {
    stationTable.put(station.code, station);
    autoCompleter.insert(station.code);
    autoCompleter.insert(station.name);
    autoCompleter.insert(station.city);
}

void RailwaySystem::addTrain(const Train& train) {
    trainTree.insert(train.trainNumber, train);
    autoCompleter.insert(std::to_string(train.trainNumber));
    autoCompleter.insert(train.trainName);
    buildNetworkGraph();
}

void RailwaySystem::addReview(const Review& r) {
    Review rev = r;
    if (rev.reviewId <= 0) {
        rev.reviewId = nextReviewId++;
    } else {
        nextReviewId = std::max(nextReviewId, rev.reviewId + 1);
    }
    reviewList.push_back(rev);

    // Index words in review comment and tags into Trie
    std::stringstream ss(rev.comment);
    std::string word;
    while (ss >> word) {
        // Strip punctuation
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) {
            return !std::isalnum(c);
        }), word.end());

        if (word.length() >= 3) {
            autoCompleter.insert(word);
        }
    }
    for (const auto& tag : rev.tags) {
        autoCompleter.insert(tag);
    }
}

void RailwaySystem::buildNetworkGraph() {
    networkGraph.clear();
    auto allTrains = trainTree.inOrder();

    for (const auto& pair : allTrains) {
        const Train& train = pair.second;
        for (size_t i = 0; i < train.stops.size(); ++i) {
            for (size_t j = i + 1; j < train.stops.size(); ++j) {
                const auto& s1 = train.stops[i];
                const auto& s2 = train.stops[j];

                RouteEdge edge;
                edge.srcCode = s1.stationCode;
                edge.destCode = s2.stationCode;
                edge.trainNumber = train.trainNumber;
                edge.trainName = train.trainName;
                edge.distanceKm = s2.distanceKm - s1.distanceKm;
                edge.travelTimeMins = s2.arrivalTimeMins - s1.departureTimeMins;
                if (edge.travelTimeMins < 0) edge.travelTimeMins += 24 * 60;
                edge.fareINR = s2.fareINR - s1.fareINR;
                edge.depTimeMins = s1.departureTimeMins;
                edge.arrTimeMins = s2.arrivalTimeMins;

                networkGraph.addEdge(edge);
            }
        }
    }
}

Station* RailwaySystem::getStation(const std::string& code) {
    return stationTable.get(code);
}

Train* RailwaySystem::getTrain(int trainNumber) {
    return trainTree.find(trainNumber);
}

std::vector<Station> RailwaySystem::getAllStations() const {
    std::vector<Station> result;
    auto entries = stationTable.entries();
    for (const auto& e : entries) {
        result.push_back(e.second);
    }
    return result;
}

std::vector<Train> RailwaySystem::getAllTrains() const {
    std::vector<Train> result;
    auto pairs = trainTree.inOrder();
    for (const auto& p : pairs) {
        result.push_back(p.second);
    }
    return result;
}

RouteItinerary RailwaySystem::findRoute(const std::string& srcCode, const std::string& destCode, WeightCriterion criterion) {
    return networkGraph.findShortestPath(srcCode, destCode, criterion);
}

std::vector<RouteItinerary> RailwaySystem::findConnectingRoutes(const std::string& srcCode, const std::string& destCode) {
    return networkGraph.findConnectingRoutes(srcCode, destCode);
}

std::vector<Train> RailwaySystem::findDirectTrains(const std::string& srcCode, const std::string& destCode) {
    std::vector<Train> directTrains;
    auto allTrains = getAllTrains();
    for (const auto& t : allTrains) {
        if (t.passesThrough(srcCode, destCode)) {
            directTrains.push_back(t);
        }
    }
    return directTrains;
}

std::vector<std::string> RailwaySystem::getAutocompleteSuggestions(const std::string& prefix, size_t limit) const {
    return autoCompleter.getSuggestions(prefix, limit);
}

std::vector<Review> RailwaySystem::searchReviewsKeywordKMP(const std::string& keyword) const {
    std::vector<Review> matches;
    for (const auto& rev : reviewList) {
        if (KMP::contains(rev.comment, keyword) || KMP::contains(rev.author, keyword) || KMP::contains(rev.targetCode, keyword)) {
            matches.push_back(rev);
        } else {
            // Check tags
            for (const auto& tag : rev.tags) {
                if (KMP::contains(tag, keyword)) {
                    matches.push_back(rev);
                    break;
                }
            }
        }
    }
    return matches;
}

std::vector<Review> RailwaySystem::getReviewsSorted(const std::string& sortBy) {
    std::vector<Review> copy = reviewList;

    if (sortBy == "RATING_DESC") {
        Quicksort::sort(copy, [](const Review& a, const Review& b) {
            return a.rating > b.rating;
        });
    } else if (sortBy == "RATING_ASC") {
        Quicksort::sort(copy, [](const Review& a, const Review& b) {
            return a.rating < b.rating;
        });
    } else if (sortBy == "ID_DESC") {
        Quicksort::sort(copy, [](const Review& a, const Review& b) {
            return a.reviewId > b.reviewId;
        });
    } else if (sortBy == "ID_ASC") {
        Quicksort::sort(copy, [](const Review& a, const Review& b) {
            return a.reviewId < b.reviewId;
        });
    }

    return copy;
}

Ticket RailwaySystem::bookTicket(int trainNumber, const std::string& srcCode, const std::string& destCode,
                                const std::string& passengerName, int age, const std::string& seatClass, const std::string& date) {
    Train* train = getTrain(trainNumber);
    if (!train) {
        throw std::invalid_argument("Invalid train number: " + std::to_string(trainNumber));
    }
    if (!train->passesThrough(srcCode, destCode)) {
        throw std::invalid_argument("Train does not operate between specified stations.");
    }

    double fare = train->calculateFare(srcCode, destCode, seatClass);
    std::string pnr = generatePNR();
    std::string status = "CNF";

    // Update seat count
    if (train->availableSeats[seatClass] > 0) {
        train->availableSeats[seatClass]--;
    } else {
        status = "WL/1";
    }

    Ticket ticket(pnr, trainNumber, train->trainName, srcCode, destCode, passengerName, age, seatClass, fare, status, date);
    ticketTable.put(pnr, ticket);
    return ticket;
}

Ticket* RailwaySystem::lookupPNR(const std::string& pnr) {
    return ticketTable.get(pnr);
}

void RailwaySystem::printDSAStatistics() const {
    std::cout << "\n=======================================================\n";
    std::cout << "          DATA STRUCTURE PERFORMANCE METRICS           \n";
    std::cout << "=======================================================\n";
    std::cout << " [1] AVL Tree (Trains Index):\n";
    std::cout << "     - Total Node Count: " << trainTree.size() << "\n";
    std::cout << "     - Tree Height: " << trainTree.height() << "\n";
    std::cout << "     - Search Complexity: O(log N)\n\n";

    std::cout << " [2] Hash Table (Stations & PNR Index):\n";
    std::cout << "     - Stations Loaded: " << stationTable.size() << "\n";
    std::cout << "     - Station Bucket Count: " << stationTable.bucketCount() << "\n";
    std::cout << "     - Station Load Factor: " << stationTable.currentLoadFactor() << "\n";
    std::cout << "     - Tickets Booked: " << ticketTable.size() << "\n";
    std::cout << "     - Lookup Complexity: Average O(1)\n\n";

    std::cout << " [3] Trie (Auto-Complete Index):\n";
    std::cout << "     - Total Indexed Words: " << autoCompleter.size() << "\n";
    std::cout << "     - Prefix Search Complexity: O(L) where L is string length\n\n";

    std::cout << " [4] Review Management Engine:\n";
    std::cout << "     - Total Reviews: " << reviewList.size() << "\n";
    std::cout << "     - String Match Algorithm: KMP Pattern Match O(N + M)\n";
    std::cout << "     - Sorting Algorithm: Quicksort O(N log N) Average\n";
    std::cout << "=======================================================\n\n";
}

void RailwaySystem::loadSampleData() {
    // 1. Stations
    addStation(Station(1, "CSMT", "Mumbai CSMT", "Mumbai", "Maharashtra", 18));
    addStation(Station(2, "NDLS", "New Delhi", "New Delhi", "Delhi", 16));
    addStation(Station(3, "HWH", "Howrah Junction", "Kolkata", "West Bengal", 23));
    addStation(Station(4, "SBC", "KSR Bengaluru", "Bengaluru", "Karnataka", 10));
    addStation(Station(5, "MAS", "Chennai Central", "Chennai", "Tamil Nadu", 12));
    addStation(Station(6, "ADI", "Ahmedabad Junction", "Ahmedabad", "Gujarat", 12));
    addStation(Station(7, "BRC", "Vadodara Junction", "Vadodara", "Gujarat", 7));
    addStation(Station(8, "BPL", "Bhopal Junction", "Bhopal", "Madhya Pradesh", 6));
    addStation(Station(9, "PUNE", "Pune Junction", "Pune", "Maharashtra", 6));
    addStation(Station(10, "PNBE", "Patna Junction", "Patna", "Bihar", 10));

    // 2. Trains
    // Train 12951: Mumbai Rajdhani (CSMT -> BRC -> ADI -> BPL -> NDLS)
    Train t1(12951, "Mumbai Rajdhani", "Rajdhani", "CSMT", "NDLS");
    t1.stops.push_back(TrainStop("CSMT", "Mumbai CSMT", 0, 1010, 0, 0, 1));        // 16:50
    t1.stops.push_back(TrainStop("BRC", "Vadodara Junction", 1220, 1230, 392, 650, 1)); // 20:30
    t1.stops.push_back(TrainStop("ADI", "Ahmedabad Junction", 1310, 1320, 492, 800, 1));
    t1.stops.push_back(TrainStop("BPL", "Bhopal Junction", 210, 220, 835, 1400, 2));   // 03:30
    t1.stops.push_back(TrainStop("NDLS", "New Delhi", 510, 510, 1384, 2200, 2));       // 08:30
    addTrain(t1);

    // Train 12002: Bhopal Shatabdi (NDLS -> BPL)
    Train t2(12002, "Bhopal Shatabdi", "Shatabdi", "NDLS", "BPL");
    t2.stops.push_back(TrainStop("NDLS", "New Delhi", 0, 360, 0, 0, 1));         // 06:00
    t2.stops.push_back(TrainStop("BPL", "Bhopal Junction", 840, 840, 705, 1150, 1)); // 14:00
    addTrain(t2);

    // Train 12260: Duronto Express (NDLS -> HWH)
    Train t3(12260, "Howrah Duronto", "Duronto", "NDLS", "HWH");
    t3.stops.push_back(TrainStop("NDLS", "New Delhi", 0, 750, 0, 0, 1));       // 12:30
    t3.stops.push_back(TrainStop("PNBE", "Patna Junction", 1380, 1390, 998, 1600, 1));
    t3.stops.push_back(TrainStop("HWH", "Howrah Junction", 360, 360, 1530, 2400, 2)); // 06:00
    addTrain(t3);

    // Train 12626: Kerala Express (NDLS -> BPL -> SBC -> MAS)
    Train t4(12626, "Kerala Express", "Superfast", "NDLS", "MAS");
    t4.stops.push_back(TrainStop("NDLS", "New Delhi", 0, 1210, 0, 0, 1));        // 20:10
    t4.stops.push_back(TrainStop("BPL", "Bhopal Junction", 300, 310, 705, 950, 2));
    t4.stops.push_back(TrainStop("SBC", "KSR Bengaluru", 1200, 1215, 2180, 2100, 2));
    t4.stops.push_back(TrainStop("MAS", "Chennai Central", 1430, 1430, 2530, 2500, 2));
    addTrain(t4);

    // Train 11077: Jhelum Express (CSMT -> PUNE -> BPL -> NDLS)
    Train t5(11077, "Jhelum Express", "Express", "CSMT", "NDLS");
    t5.stops.push_back(TrainStop("CSMT", "Mumbai CSMT", 0, 1040, 0, 0, 1));     // 17:20
    t5.stops.push_back(TrainStop("PUNE", "Pune Junction", 1250, 1260, 192, 250, 1));
    t5.stops.push_back(TrainStop("BPL", "Bhopal Junction", 450, 460, 890, 1100, 2));
    t5.stops.push_back(TrainStop("NDLS", "New Delhi", 990, 990, 1590, 1850, 2));
    addTrain(t5);

    // 3. Reviews
    addReview(Review(1, "TRAIN", "12951", "Aarav Sharma", 5, "2026-07-15", "Exceptional cleanliness and punctual arrival. The catering food quality was superb!", {"clean", "punctual", "food"}));
    addReview(Review(2, "TRAIN", "12951", "Neha Verma", 4, "2026-07-20", "Very comfortable seats in 3A. Washrooms were surprisingly clean throughout the journey.", {"clean", "comfortable", "3A"}));
    addReview(Review(3, "TRAIN", "12002", "Rohan Mehta", 3, "2026-08-01", "Train delayed by 45 minutes near Agra. Food was average.", {"delayed", "food"}));
    addReview(Review(4, "STATION", "CSMT", "Priya Nair", 5, "2026-08-05", "Heritage architectural marvel! Excellent modern waiting lounges and fast Wi-Fi.", {"wifi", "lounge", "heritage"}));
    addReview(Review(5, "STATION", "NDLS", "Vikram Singh", 2, "2026-08-10", "Crowded platforms and delayed auto announcements on platform 16.", {"crowded", "delayed"}));
    addReview(Review(6, "TRAIN", "12260", "Ananya Das", 5, "2026-08-12", "Superb speed and zero delays! Highly recommend Howrah Duronto for fast travel.", {"speed", "punctual", "fast"}));
}

bool RailwaySystem::loadStationsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string idStr, code, name, city, state, platStr;

        std::getline(ss, idStr, ',');
        std::getline(ss, code, ',');
        std::getline(ss, name, ',');
        std::getline(ss, city, ',');
        std::getline(ss, state, ',');
        std::getline(ss, platStr, ',');

        if (!code.empty()) {
            int id = idStr.empty() ? 0 : std::stoi(idStr);
            int plat = platStr.empty() ? 4 : std::stoi(platStr);
            addStation(Station(id, code, name, city, state, plat));
        }
    }
    return true;
}

bool RailwaySystem::loadTrainsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string numStr, name, type, src, dest, stopsStr;

        std::getline(ss, numStr, ',');
        std::getline(ss, name, ',');
        std::getline(ss, type, ',');
        std::getline(ss, src, ',');
        std::getline(ss, dest, ',');

        if (!numStr.empty()) {
            int num = std::stoi(numStr);
            Train train(num, name, type, src, dest);

            // Parse stops if present in semicolon separated format
            // e.g. "CSMT:0:1010:0:0;BRC:1220:1230:392:650"
            std::string stopToken;
            while (std::getline(ss, stopToken, ';')) {
                if (stopToken.empty()) continue;
                std::stringstream stopSS(stopToken);
                std::string stCode, stName, arrStr, depStr, distStr, fareStr;
                std::getline(stopSS, stCode, ':');
                std::getline(stopSS, stName, ':');
                std::getline(stopSS, arrStr, ':');
                std::getline(stopSS, depStr, ':');
                std::getline(stopSS, distStr, ':');
                std::getline(stopSS, fareStr, ':');

                if (!stCode.empty()) {
                    int arr = arrStr.empty() ? 0 : std::stoi(arrStr);
                    int dep = depStr.empty() ? 0 : std::stoi(depStr);
                    double dist = distStr.empty() ? 0.0 : std::stod(distStr);
                    double fare = fareStr.empty() ? 0.0 : std::stod(fareStr);
                    train.stops.push_back(TrainStop(stCode, stName, arr, dep, dist, fare));
                }
            }
            addTrain(train);
        }
    }
    return true;
}

bool RailwaySystem::loadReviewsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string idStr, type, code, author, ratingStr, date, comment;

        std::getline(ss, idStr, ',');
        std::getline(ss, type, ',');
        std::getline(ss, code, ',');
        std::getline(ss, author, ',');
        std::getline(ss, ratingStr, ',');
        std::getline(ss, date, ',');
        std::getline(ss, comment, ',');

        if (!type.empty()) {
            int id = idStr.empty() ? 0 : std::stoi(idStr);
            int rating = ratingStr.empty() ? 5 : std::stoi(ratingStr);
            addReview(Review(id, type, code, author, rating, date, comment));
        }
    }
    return true;
}
