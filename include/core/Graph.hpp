#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <iostream>
#include <algorithm>
#include "../ds/MinHeap.hpp"
#include "../models/Train.hpp"
#include "../models/Station.hpp"

struct RouteEdge {
    std::string srcCode;
    std::string destCode;
    int trainNumber;
    std::string trainName;
    double distanceKm;
    int travelTimeMins;
    double fareINR;
    int depTimeMins;
    int arrTimeMins;
};

struct RouteSegment {
    int trainNumber;
    std::string trainName;
    std::string boardStationCode;
    std::string alightStationCode;
    int depTimeMins;
    int arrTimeMins;
    double distanceKm;
    double fareINR;
};

struct RouteItinerary {
    double totalDistanceKm;
    int totalDurationMins;
    double totalFareINR;
    int transfers;
    std::vector<RouteSegment> segments;

    RouteItinerary() : totalDistanceKm(0.0), totalDurationMins(0), totalFareINR(0.0), transfers(0) {}
};

enum class WeightCriterion {
    DISTANCE,
    TIME,
    FARE
};

class Graph {
private:
    std::unordered_map<std::string, std::vector<RouteEdge>> adjList;

public:
    Graph() = default;

    void addEdge(const RouteEdge& edge) {
        adjList[edge.srcCode].push_back(edge);
    }

    void clear() {
        adjList.clear();
    }

    bool hasStation(const std::string& stationCode) const {
        return adjList.find(stationCode) != adjList.end();
    }

    std::vector<RouteEdge> getDirectEdges(const std::string& srcCode, const std::string& destCode) const {
        std::vector<RouteEdge> directEdges;
        auto it = adjList.find(srcCode);
        if (it != adjList.end()) {
            for (const auto& edge : it->second) {
                if (edge.destCode == destCode) {
                    directEdges.push_back(edge);
                }
            }
        }
        return directEdges;
    }

    // Dijkstra's Shortest Path using MinHeap Priority Queue
    RouteItinerary findShortestPath(const std::string& srcCode, const std::string& destCode, WeightCriterion criterion = WeightCriterion::DISTANCE) const {
        struct NodeState {
            std::string stationCode;
            double cost;

            bool operator<(const NodeState& other) const {
                return cost < other.cost;
            }
        };

        const double INF = std::numeric_limits<double>::infinity();
        std::unordered_map<std::string, double> dist;
        std::unordered_map<std::string, RouteEdge> parentEdge;

        for (const auto& pair : adjList) {
            dist[pair.first] = INF;
            for (const auto& edge : pair.second) {
                dist[edge.destCode] = INF;
            }
        }

        dist[srcCode] = 0.0;

        MinHeap<NodeState> pq;
        pq.push({srcCode, 0.0});

        while (!pq.empty()) {
            NodeState current = pq.pop();
            std::string u = current.stationCode;
            double currentCost = current.cost;

            if (currentCost > dist[u]) continue;
            if (u == destCode) break;

            auto it = adjList.find(u);
            if (it == adjList.end()) continue;

            for (const auto& edge : it->second) {
                double weight = 0.0;
                if (criterion == WeightCriterion::DISTANCE) weight = edge.distanceKm;
                else if (criterion == WeightCriterion::TIME) weight = static_cast<double>(edge.travelTimeMins);
                else if (criterion == WeightCriterion::FARE) weight = edge.fareINR;

                if (dist[u] + weight < dist[edge.destCode]) {
                    dist[edge.destCode] = dist[u] + weight;
                    parentEdge[edge.destCode] = edge;
                    pq.push({edge.destCode, dist[edge.destCode]});
                }
            }
        }

        RouteItinerary itinerary;
        if (dist[destCode] == INF) {
            return itinerary; // No route found
        }

        // Reconstruct path
        std::string curr = destCode;
        std::vector<RouteEdge> pathEdges;
        while (curr != srcCode) {
            auto it = parentEdge.find(curr);
            if (it == parentEdge.end()) break;
            pathEdges.push_back(it->second);
            curr = it->second.srcCode;
        }

        std::reverse(pathEdges.begin(), pathEdges.end());

        // Consolidate segments if same train
        for (const auto& edge : pathEdges) {
            if (!itinerary.segments.empty() && itinerary.segments.back().trainNumber == edge.trainNumber) {
                // Extend previous segment
                auto& lastSeg = itinerary.segments.back();
                lastSeg.alightStationCode = edge.destCode;
                lastSeg.arrTimeMins = edge.arrTimeMins;
                lastSeg.distanceKm += edge.distanceKm;
                lastSeg.fareINR += edge.fareINR;
            } else {
                RouteSegment seg;
                seg.trainNumber = edge.trainNumber;
                seg.trainName = edge.trainName;
                seg.boardStationCode = edge.srcCode;
                seg.alightStationCode = edge.destCode;
                seg.depTimeMins = edge.depTimeMins;
                seg.arrTimeMins = edge.arrTimeMins;
                seg.distanceKm = edge.distanceKm;
                seg.fareINR = edge.fareINR;
                itinerary.segments.push_back(seg);
            }

            itinerary.totalDistanceKm += edge.distanceKm;
            itinerary.totalFareINR += edge.fareINR;
        }

        if (!itinerary.segments.empty()) {
            itinerary.transfers = static_cast<int>(itinerary.segments.size()) - 1;
            itinerary.totalDurationMins = itinerary.segments.back().arrTimeMins - itinerary.segments.front().depTimeMins;
            if (itinerary.totalDurationMins < 0) itinerary.totalDurationMins += 24 * 60; // next day wrap
        }

        return itinerary;
    }

    // Find 1-transfer connecting train itineraries
    std::vector<RouteItinerary> findConnectingRoutes(const std::string& srcCode, const std::string& destCode) const {
        std::vector<RouteItinerary> connectingRoutes;

        auto srcIt = adjList.find(srcCode);
        if (srcIt == adjList.end()) return connectingRoutes;

        for (const auto& leg1 : srcIt->second) {
            std::string transferStation = leg1.destCode;
            if (transferStation == destCode) continue; // Skip direct edges

            auto transIt = adjList.find(transferStation);
            if (transIt == adjList.end()) continue;

            for (const auto& leg2 : transIt->second) {
                if (leg2.destCode == destCode && leg1.trainNumber != leg2.trainNumber) {
                    // Valid 1-transfer route
                    // Buffer time of at least 30 minutes at transfer station
                    int layoverMins = leg2.depTimeMins - leg1.arrTimeMins;
                    if (layoverMins < 0) layoverMins += 24 * 60;

                    if (layoverMins >= 30 && layoverMins <= 720) { // between 30 mins and 12 hours
                        RouteItinerary itinerary;
                        itinerary.transfers = 1;
                        itinerary.totalDistanceKm = leg1.distanceKm + leg2.distanceKm;
                        itinerary.totalFareINR = leg1.fareINR + leg2.fareINR;
                        itinerary.totalDurationMins = (leg2.arrTimeMins - leg1.depTimeMins);
                        if (itinerary.totalDurationMins < 0) itinerary.totalDurationMins += 24 * 60;

                        RouteSegment seg1{leg1.trainNumber, leg1.trainName, leg1.srcCode, leg1.destCode, leg1.depTimeMins, leg1.arrTimeMins, leg1.distanceKm, leg1.fareINR};
                        RouteSegment seg2{leg2.trainNumber, leg2.trainName, leg2.srcCode, leg2.destCode, leg2.depTimeMins, leg2.arrTimeMins, leg2.distanceKm, leg2.fareINR};
                        itinerary.segments.push_back(seg1);
                        itinerary.segments.push_back(seg2);

                        connectingRoutes.push_back(itinerary);
                    }
                }
            }
        }

        return connectingRoutes;
    }
};

#endif // GRAPH_HPP
