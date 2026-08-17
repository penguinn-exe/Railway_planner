#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <chrono>
#include "../include/core/RailwaySystem.hpp"

// ANSI Color Codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define MAGENTA "\033[35m"
#define BLUE    "\033[34m"

void printHeader() {
    std::cout << CYAN << BOLD;
    std::cout << "========================================================================\n";
    std::cout << "     IIT BOMBAY DSA LAB - RAILWAY PLANNER & ENQUIRY PORTAL              \n";
    std::cout << "  (AVL Tree | Hash Table | MinHeap | Trie | Dijkstra | KMP | Quicksort) \n";
    std::cout << "========================================================================\n" << RESET;
}

void printMenu() {
    std::cout << BOLD << "\nMAIN PORTAL MENU:\n" << RESET;
    std::cout << GREEN << " [1] " << RESET << "Route Planner & Train Search (Dijkstra Shortest Path)\n";
    std::cout << GREEN << " [2] " << RESET << "Station & Train Schedule Enquiry (AVL & Hash Table Lookup)\n";
    std::cout << GREEN << " [3] " << RESET << "Review Portal (Trie Auto-Complete, KMP Search, Quicksort Ranking)\n";
    std::cout << GREEN << " [4] " << RESET << "Ticket Booking & PNR Status Inquiry\n";
    std::cout << GREEN << " [5] " << RESET << "DSA Performance & Benchmark Inspector\n";
    std::cout << GREEN << " [6] " << RESET << "Exit Application\n";
    std::cout << BOLD << "Select option (1-6): " << RESET;
}

void handleRouteSearch(RailwaySystem& system) {
    std::cout << BOLD << "\n--- [1] ROUTE PLANNER & TRAIN SEARCH --- \n" << RESET;
    std::string src, dest;
    std::cout << "Enter Source Station Code (e.g. CSMT, NDLS, HWH, SBC): ";
    std::cin >> src;
    std::cout << "Enter Destination Station Code (e.g. NDLS, BPL, MAS): ";
    std::cin >> dest;

    for (auto &c : src) c = toupper(c);
    for (auto &c : dest) c = toupper(c);

    if (!system.getStation(src) || !system.getStation(dest)) {
        std::cout << RED << "Error: Invalid station code(s) provided!\n" << RESET;
        return;
    }

    std::cout << "\nChoose Route Optimization Metric:\n";
    std::cout << " 1. Shortest Distance (Km)\n";
    std::cout << " 2. Fastest Duration (Time)\n";
    std::cout << " 3. Lowest Fare (INR)\n";
    std::cout << "Option (1-3): ";
    int opt;
    std::cin >> opt;

    WeightCriterion criterion = WeightCriterion::DISTANCE;
    if (opt == 2) criterion = WeightCriterion::TIME;
    else if (opt == 3) criterion = WeightCriterion::FARE;

    std::cout << CYAN << "\nComputing optimal path using Dijkstra's algorithm with MinHeap...\n" << RESET;
    RouteItinerary route = system.findRoute(src, dest, criterion);

    if (route.segments.empty()) {
        std::cout << YELLOW << "No direct or single-train route found between " << src << " and " << dest << ".\n" << RESET;
    } else {
        std::cout << GREEN << BOLD << "\n✓ OPTIMAL ITINERARY FOUND:\n" << RESET;
        std::cout << "--------------------------------------------------------\n";
        std::cout << " Total Distance : " << route.totalDistanceKm << " km\n";
        std::cout << " Total Fare     : ₹" << route.totalFareINR << "\n";
        std::cout << " Transfers      : " << route.transfers << "\n";
        std::cout << "--------------------------------------------------------\n";
        std::cout << BOLD << " Journey Segments:\n" << RESET;
        for (size_t i = 0; i < route.segments.size(); ++i) {
            const auto& seg = route.segments[i];
            std::cout << "  Leg " << (i + 1) << ": Train #" << seg.trainNumber << " (" << seg.trainName << ")\n";
            std::cout << "         Board : " << seg.boardStationCode << " at " << TrainStop::formatTime(seg.depTimeMins) << "\n";
            std::cout << "         Alight: " << seg.alightStationCode << " at " << TrainStop::formatTime(seg.arrTimeMins) << "\n";
            std::cout << "         Dist  : " << seg.distanceKm << " km | Fare: ₹" << seg.fareINR << "\n";
        }
        std::cout << "--------------------------------------------------------\n";
    }

    // Also check connecting routes
    std::vector<RouteItinerary> connecting = system.findConnectingRoutes(src, dest);
    if (!connecting.empty()) {
        std::cout << MAGENTA << BOLD << "\n⚡ ALTERNATIVE CONNECTING ROUTES (1-Transfer):\n" << RESET;
        for (size_t idx = 0; idx < connecting.size(); ++idx) {
            const auto& conn = connecting[idx];
            std::cout << " Option " << (idx + 1) << ": Transfer at " << conn.segments[0].alightStationCode
                      << " | Total Dist: " << conn.totalDistanceKm << " km | Total Fare: ₹" << conn.totalFareINR << "\n";
            std::cout << "    Leg 1: Train #" << conn.segments[0].trainNumber << " (" << conn.segments[0].trainName
                      << ") " << conn.segments[0].boardStationCode << " -> " << conn.segments[0].alightStationCode << "\n";
            std::cout << "    Leg 2: Train #" << conn.segments[1].trainNumber << " (" << conn.segments[1].trainName
                      << ") " << conn.segments[1].boardStationCode << " -> " << conn.segments[1].alightStationCode << "\n";
        }
    }
}

void handleEnquiry(RailwaySystem& system) {
    std::cout << BOLD << "\n--- [2] STATION & TRAIN SCHEDULE ENQUIRY --- \n" << RESET;
    std::cout << " 1. Search Station by Code (Hash Table O(1))\n";
    std::cout << " 2. Search Train Schedule by Number (AVL Tree O(log N))\n";
    std::cout << " 3. List All Stations\n";
    std::cout << " 4. List All Trains\n";
    std::cout << "Option (1-4): ";
    int subOpt;
    std::cin >> subOpt;

    if (subOpt == 1) {
        std::string code;
        std::cout << "Enter Station Code: ";
        std::cin >> code;
        for (auto &c : code) c = toupper(c);

        Station* st = system.getStation(code);
        if (st) {
            std::cout << GREEN << "\n✓ Station Details (O(1) Hash Table Lookup):\n" << RESET;
            std::cout << " Code      : " << st->code << "\n";
            std::cout << " Name      : " << st->name << "\n";
            std::cout << " City/State: " << st->city << ", " << st->state << "\n";
            std::cout << " Platforms : " << st->platforms << "\n";
        } else {
            std::cout << RED << "Station not found.\n" << RESET;
        }
    } else if (subOpt == 2) {
        int tnum;
        std::cout << "Enter Train Number: ";
        std::cin >> tnum;

        Train* train = system.getTrain(tnum);
        if (train) {
            std::cout << GREEN << "\n✓ Train Details (O(log N) AVL Tree Lookup):\n" << RESET;
            std::cout << " Train #" << train->trainNumber << " - " << train->trainName << " (" << train->type << ")\n";
            std::cout << " Route   : " << train->sourceCode << " -> " << train->destCode << "\n";
            std::cout << " Schedule:\n";
            std::cout << "  " << std::left << std::setw(8) << "Code" << std::setw(22) << "Station Name"
                      << std::setw(8) << "Arr" << std::setw(8) << "Dep" << std::setw(10) << "Dist(km)" << "Fare(₹)\n";
            std::cout << "  ---------------------------------------------------------------\n";
            for (const auto& stop : train->stops) {
                std::cout << "  " << std::left << std::setw(8) << stop.stationCode
                          << std::setw(22) << stop.stationName
                          << std::setw(8) << TrainStop::formatTime(stop.arrivalTimeMins)
                          << std::setw(8) << TrainStop::formatTime(stop.departureTimeMins)
                          << std::setw(10) << stop.distanceKm
                          << stop.fareINR << "\n";
            }
        } else {
            std::cout << RED << "Train not found.\n" << RESET;
        }
    } else if (subOpt == 3) {
        auto stations = system.getAllStations();
        std::cout << CYAN << "\nAll Registered Stations (" << stations.size() << " total):\n" << RESET;
        for (const auto& st : stations) {
            std::cout << " [" << st.code << "] " << st.name << " (" << st.city << ", " << st.state << ")\n";
        }
    } else if (subOpt == 4) {
        auto trains = system.getAllTrains();
        std::cout << CYAN << "\nAll Registered Trains (" << trains.size() << " total - In-Order AVL Traversal):\n" << RESET;
        for (const auto& tr : trains) {
            std::cout << " #" << tr.trainNumber << " - " << tr.trainName << " [" << tr.sourceCode << " -> " << tr.destCode << "]\n";
        }
    }
}

void handleReviewPortal(RailwaySystem& system) {
    std::cout << BOLD << "\n--- [3] REVIEW PORTAL & KEYWORD SEARCH --- \n" << RESET;
    std::cout << " 1. Add New User Review\n";
    std::cout << " 2. Auto-Complete Keyword / Tag Suggestion (Trie O(L))\n";
    std::cout << " 3. KMP Keyword Pattern Search in Reviews (KMP O(N+M))\n";
    std::cout << " 4. View Reviews Sorted by Rating / ID (Quicksort O(N log N))\n";
    std::cout << "Option (1-4): ";
    int subOpt;
    std::cin >> subOpt;

    if (subOpt == 1) {
        std::string type, code, author, comment;
        int rating;
        std::cout << "Target Type (TRAIN/STATION): ";
        std::cin >> type;
        std::cout << "Target Code (e.g. 12951 or CSMT): ";
        std::cin >> code;
        std::cin.ignore();
        std::cout << "Author Name: ";
        std::getline(std::cin, author);
        std::cout << "Rating (1-5): ";
        std::cin >> rating;
        std::cin.ignore();
        std::cout << "Comment: ";
        std::getline(std::cin, comment);

        system.addReview(Review(0, type, code, author, rating, "2026-08-17", comment));
        std::cout << GREEN << "✓ Review successfully added and indexed in Trie!\n" << RESET;
    } else if (subOpt == 2) {
        std::string prefix;
        std::cout << "Type prefix for auto-complete: ";
        std::cin >> prefix;

        auto suggestions = system.getAutocompleteSuggestions(prefix, 8);
        std::cout << MAGENTA << "\n✓ Auto-Complete Suggestions for '" << prefix << "':\n" << RESET;
        if (suggestions.empty()) {
            std::cout << " (No suggestions found matching prefix)\n";
        } else {
            for (size_t i = 0; i < suggestions.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << suggestions[i] << "\n";
            }
        }
    } else if (subOpt == 3) {
        std::string kw;
        std::cout << "Enter search keyword (uses KMP pattern matching algorithm): ";
        std::cin >> kw;

        auto start = std::chrono::high_resolution_clock::now();
        auto matches = system.searchReviewsKeywordKMP(kw);
        auto end = std::chrono::high_resolution_clock::now();
        double elapsedUs = std::chrono::duration<double, std::micro>(end - start).count();

        std::cout << GREEN << "\n✓ Found " << matches.size() << " review(s) matching '" << kw
                  << "' (KMP Search execution time: " << elapsedUs << " µs):\n" << RESET;
        for (const auto& r : matches) {
            std::cout << "  [Review #" << r.reviewId << "] " << r.targetType << " " << r.targetCode
                      << " | Rating: " << r.rating << "/5 | Author: " << r.author << "\n";
            std::cout << "  Comment: \"" << r.comment << "\"\n";
            std::cout << "  ---------------------------------------------------------\n";
        }
    } else if (subOpt == 4) {
        std::cout << "Sort By:\n 1. Highest Rating (Desc)\n 2. Lowest Rating (Asc)\n 3. Latest Review (ID Desc)\nOption: ";
        int sortChoice;
        std::cin >> sortChoice;

        std::string sortKey = "RATING_DESC";
        if (sortChoice == 2) sortKey = "RATING_ASC";
        else if (sortChoice == 3) sortKey = "ID_DESC";

        auto sortedReviews = system.getReviewsSorted(sortKey);
        std::cout << CYAN << "\nSorted Reviews (Quicksort Algorithm):\n" << RESET;
        for (const auto& r : sortedReviews) {
            std::cout << "  [#" << r.reviewId << "] " << r.targetCode << " | " << r.rating << "★ | "
                      << r.author << " (" << r.date << "): \"" << r.comment << "\"\n";
        }
    }
}

void handleTicketing(RailwaySystem& system) {
    std::cout << BOLD << "\n--- [4] TICKET BOOKING & PNR ENQUIRY --- \n" << RESET;
    std::cout << " 1. Book Train Ticket\n";
    std::cout << " 2. PNR Status Lookup (Hash Table O(1))\n";
    std::cout << "Option (1-2): ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        int trainNum, age;
        std::string src, dest, passenger, seatClass, date;
        std::cout << "Train Number: ";
        std::cin >> trainNum;
        std::cout << "Boarding Station Code: ";
        std::cin >> src;
        std::cout << "Destination Station Code: ";
        std::cin >> dest;
        std::cin.ignore();
        std::cout << "Passenger Name: ";
        std::getline(std::cin, passenger);
        std::cout << "Passenger Age: ";
        std::cin >> age;
        std::cout << "Class (SL / 3A / 2A / 1A): ";
        std::cin >> seatClass;
        std::cout << "Journey Date (YYYY-MM-DD): ";
        std::cin >> date;

        try {
            Ticket t = system.bookTicket(trainNum, src, dest, passenger, age, seatClass, date);
            std::cout << GREEN << BOLD << "\n✓ BOOKING CONFIRMED!\n" << RESET;
            std::cout << " PNR Number    : " << t.pnr << "\n";
            std::cout << " Train         : #" << t.trainNumber << " (" << t.trainName << ")\n";
            std::cout << " Passenger     : " << t.passengerName << " (Age: " << t.age << ")\n";
            std::cout << " Route         : " << t.sourceCode << " -> " << t.destCode << "\n";
            std::cout << " Class / Status: " << t.seatClass << " / " << t.status << "\n";
            std::cout << " Total Fare    : ₹" << t.fare << "\n";
        } catch (const std::exception& e) {
            std::cout << RED << "Booking Failed: " << e.what() << "\n" << RESET;
        }
    } else if (choice == 2) {
        std::string pnr;
        std::cout << "Enter 10-digit PNR Number: ";
        std::cin >> pnr;

        Ticket* t = system.lookupPNR(pnr);
        if (t) {
            std::cout << GREEN << "\n✓ PNR Record Found (O(1) Hash Table Lookup):\n" << RESET;
            std::cout << " PNR           : " << t->pnr << "\n";
            std::cout << " Train         : #" << t->trainNumber << " (" << t->trainName << ")\n";
            std::cout << " Passenger     : " << t->passengerName << " (Age: " << t->age << ")\n";
            std::cout << " Route         : " << t->sourceCode << " -> " << t->destCode << "\n";
            std::cout << " Date          : " << t->journeyDate << "\n";
            std::cout << " Class / Status: " << t->seatClass << " / " << t->status << "\n";
            std::cout << " Fare          : ₹" << t->fare << "\n";
        } else {
            std::cout << RED << "PNR not found in system.\n" << RESET;
        }
    }
}

void handleBenchmarks(RailwaySystem& system) {
    std::cout << BOLD << "\n--- [5] DSA PERFORMANCE & BENCHMARK INSPECTOR --- \n" << RESET;
    system.printDSAStatistics();

    std::cout << CYAN << "Running Benchmark Tests on Custom Data Structures...\n" << RESET;

    // Benchmark 1: AVL Tree vs Sequential Search
    std::cout << "\n [Test 1] AVL Tree Search vs Linear Search (10,000 insertions):\n";
    AVLTree<int, int> testAVL;
    std::vector<int> keys;
    for (int i = 1; i <= 10000; ++i) {
        testAVL.insert(i, i * 10);
        keys.push_back(i);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int k = 1; k <= 1000; ++k) {
        volatile int* val = testAVL.find(k * 10);
        (void)val;
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    double avlUs = std::chrono::duration<double, std::micro>(t2 - t1).count();
    std::cout << "  -> AVL Tree 1000 lookups time: " << avlUs << " µs (Height: " << testAVL.height() << ")\n";

    // Benchmark 2: KMP vs Naive Substring Search
    std::cout << "\n [Test 2] KMP Algorithm vs Naive Search on 50,000 char text:\n";
    std::string bigText;
    for (int i = 0; i < 5000; ++i) bigText += "the quick brown fox jumps over the lazy dog ";
    std::string pattern = "lazy dog";

    t1 = std::chrono::high_resolution_clock::now();
    auto matchesKMP = KMP::search(bigText, pattern);
    t2 = std::chrono::high_resolution_clock::now();
    double kmpUs = std::chrono::duration<double, std::micro>(t2 - t1).count();

    t1 = std::chrono::high_resolution_clock::now();
    size_t naivePos = 0;
    int naiveMatches = 0;
    while ((naivePos = bigText.find(pattern, naivePos)) != std::string::npos) {
        naiveMatches++;
        naivePos += pattern.length();
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    double naiveUs = std::chrono::duration<double, std::micro>(t3 - t1).count();

    std::cout << "  -> KMP Search Time  : " << kmpUs << " µs (Found " << matchesKMP.size() << " occurrences)\n";
    std::cout << "  -> Naive Search Time: " << naiveUs << " µs (Found " << naiveMatches << " occurrences)\n";

    // Benchmark 3: Custom Quicksort on 10,000 elements
    std::cout << "\n [Test 3] Custom Quicksort (Median-of-three) on 10,000 elements:\n";
    std::vector<int> randNums(10000);
    for (int i = 0; i < 10000; ++i) randNums[i] = rand() % 100000;

    t1 = std::chrono::high_resolution_clock::now();
    Quicksort::sort(randNums);
    t2 = std::chrono::high_resolution_clock::now();
    double qsUs = std::chrono::duration<double, std::micro>(t2 - t1).count();
    std::cout << "  -> Quicksort 10,000 elements time: " << qsUs << " µs (Sorted: "
              << (std::is_sorted(randNums.begin(), randNums.end()) ? "YES" : "NO") << ")\n";
}

int main() {
    RailwaySystem system;
    system.loadSampleData();

    printHeader();

    int choice = 0;
    while (true) {
        printMenu();
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == 1) handleRouteSearch(system);
        else if (choice == 2) handleEnquiry(system);
        else if (choice == 3) handleReviewPortal(system);
        else if (choice == 4) handleTicketing(system);
        else if (choice == 5) handleBenchmarks(system);
        else if (choice == 6) {
            std::cout << GREEN << "\nThank you for using the IIT Bombay Railway Portal!\n" << RESET;
            break;
        } else {
            std::cout << RED << "Invalid option! Please enter a number between 1 and 6.\n" << RESET;
        }
    }

    return 0;
}
