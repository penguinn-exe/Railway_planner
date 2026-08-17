#ifndef RAILWAY_SYSTEM_HPP
#define RAILWAY_SYSTEM_HPP

#include <string>
#include <vector>
#include <memory>
#include <random>
#include <sstream>
#include <fstream>
#include <iostream>

#include "../ds/AVLTree.hpp"
#include "../ds/HashTable.hpp"
#include "../ds/Trie.hpp"
#include "../ds/MinHeap.hpp"
#include "../algo/KMP.hpp"
#include "../algo/Quicksort.hpp"
#include "../models/Station.hpp"
#include "../models/Train.hpp"
#include "../models/Review.hpp"
#include "../models/Ticket.hpp"
#include "Graph.hpp"

class RailwaySystem {
private:
    AVLTree<int, Train> trainTree;
    HashTable<std::string, Station> stationTable;
    HashTable<std::string, Ticket> ticketTable;
    Trie autoCompleter;
    Graph networkGraph;
    std::vector<Review> reviewList;
    int nextReviewId;
    uint64_t pnrCounter;

    void buildNetworkGraph();
    void indexReviewsInTrie();
    std::string generatePNR();

public:
    RailwaySystem();
    ~RailwaySystem() = default;

    // Data Loading & Management
    void addStation(const Station& station);
    void addTrain(const Train& train);
    void addReview(const Review& review);

    bool loadStationsFromCSV(const std::string& filename);
    bool loadTrainsFromCSV(const std::string& filename);
    bool loadReviewsFromCSV(const std::string& filename);
    void loadSampleData();

    // Query Methods
    Station* getStation(const std::string& code);
    Train* getTrain(int trainNumber);
    std::vector<Station> getAllStations() const;
    std::vector<Train> getAllTrains() const;

    // Route Finding (Graph Algorithms)
    RouteItinerary findRoute(const std::string& srcCode, const std::string& destCode, WeightCriterion criterion = WeightCriterion::DISTANCE);
    std::vector<RouteItinerary> findConnectingRoutes(const std::string& srcCode, const std::string& destCode);
    std::vector<Train> findDirectTrains(const std::string& srcCode, const std::string& destCode);

    // Review Management (Trie, KMP, Quicksort)
    std::vector<std::string> getAutocompleteSuggestions(const std::string& prefix, size_t limit = 10) const;
    std::vector<Review> searchReviewsKeywordKMP(const std::string& keyword) const;
    std::vector<Review> getReviewsSorted(const std::string& sortBy = "RATING_DESC");

    // Ticketing & PNR
    Ticket bookTicket(int trainNumber, const std::string& srcCode, const std::string& destCode,
                       const std::string& passengerName, int age, const std::string& seatClass, const std::string& date);
    Ticket* lookupPNR(const std::string& pnr);

    // Inspector & Benchmarks
    void printDSAStatistics() const;
};

#endif // RAILWAY_SYSTEM_HPP
