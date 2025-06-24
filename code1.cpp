#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>
#include <limits>

using namespace std;

struct Building {
    double x, y, weight;
};

struct Point {
    double x, y;
    Point(double a = 0, double b = 0) : x(a), y(b) {}
};

class ZonePlanner {
private:
    vector<Building> buildings;
    int totalCount, minRequired;
    double bestCost;
    vector<Point> bestPolygon;

    vector<Point> formRectangle(const vector<int>& indices) {
        if (indices.empty()) return {};

        double minX = buildings[indices[0]].x, maxX = minX;
        double minY = buildings[indices[0]].y, maxY = minY;

        for (int idx : indices) {
            const auto& b = buildings[idx];
            minX = min(minX, b.x);
            maxX = max(maxX, b.x);
            minY = min(minY, b.y);
            maxY = max(maxY, b.y);
        }

        return {
            Point(minX, minY), Point(maxX, minY),
            Point(maxX, maxY), Point(minX, maxY)
        };
    }

    void evaluate(const vector<int>& selected) {
        if ((int)selected.size() < minRequired) return;

        double minX = buildings[selected[0]].x, maxX = minX;
        double minY = buildings[selected[0]].y, maxY = minY;
        double weightSum = 0.0;

        for (int idx : selected) {
            const auto& b = buildings[idx];
            minX = min(minX, b.x);
            maxX = max(maxX, b.x);
            minY = min(minY, b.y);
            maxY = max(maxY, b.y);
            weightSum += b.weight;
        }

        double perimeter = 2 * ((maxX - minX) + (maxY - minY));
        double totalCost = perimeter + weightSum;

        if (totalCost < bestCost) {
            bestCost = totalCost;
            bestPolygon = {
                Point(minX, minY), Point(maxX, minY),
                Point(maxX, maxY), Point(minX, maxY)
            };
        }
    }

    // Hybrid sliding window with wide sweeping strategy
    void scanExpandedWindows() {
        vector<int> xSorted(totalCount);
        iota(xSorted.begin(), xSorted.end(), 0);

        sort(xSorted.begin(), xSorted.end(), [&](int a, int b) {
            return buildings[a].x < buildings[b].x;
        });

        int step = 30;          // skip value for performance
        int maxWidth = 4000;    // x-range for big windows
        int maxKBoost = 1000;   // allow bigger rectangles than k

        for (int i = 0; i < totalCount; i += step) {
            for (int j = i + minRequired; j < totalCount &&
                 (buildings[xSorted[j]].x - buildings[xSorted[i]].x <= maxWidth); j += step) {
                
                vector<int> xWindow(xSorted.begin() + i, xSorted.begin() + j + 1);

                sort(xWindow.begin(), xWindow.end(), [&](int a, int b) {
                    return buildings[a].y < buildings[b].y;
                });

                for (int m = 0; m + minRequired <= (int)xWindow.size(); ++m) {
                    int maxHeight = min((int)xWindow.size(), m + minRequired + maxKBoost);
                    for (int n = m + minRequired; n <= maxHeight; n += 100) {
                        vector<int> subset(xWindow.begin() + m, xWindow.begin() + n);
                        evaluate(subset);
                    }
                }
            }
        }
    }

public:
    ZonePlanner(vector<Building>& data, int k)
        : buildings(data), totalCount((int)data.size()), minRequired(k),
          bestCost(numeric_limits<double>::max()) {}

    pair<double, vector<Point>> computeOptimal() {
        scanExpandedWindows();
        return {bestCost, bestPolygon};
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    ifstream input("input09.txt");
    if (!input) {
        cerr << "Failed to open input file\n";
        return 1;
    }

    int n, k;
    input >> n >> k;
    vector<Building> buildings(n);
    for (int i = 0; i < n; ++i) {
        input >> buildings[i].x >> buildings[i].y >> buildings[i].weight;
    }
    input.close();

    ZonePlanner solver(buildings, k);
    auto [optCost, rect] = solver.computeOptimal();

    cout << fixed << setprecision(6) << optCost << "\n";
    for (size_t i = 0; i < rect.size(); ++i) {
        size_t next = (i + 1) % rect.size();
        cout << fixed << setprecision(6)
             << rect[i].x << " " << rect[i].y << " "
             << rect[next].x << " " << rect[next].y << "\n";
    }

    return 0;
}
