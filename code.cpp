#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>

using namespace std;

struct Building
{
    double x, y, weight;
    int id;
};

struct Point
{
    double x, y;
    Point(double a = 0, double b = 0) : x(a), y(b) {}
};

class ZonePlanner
{
private:
    vector<Building> buildings;
    int totalCount, minRequired;
    double bestCost;
    vector<Point> bestPolygon;

    vector<Point> formRectangle(const vector<int> &indices)
    {
        if (indices.empty())
            return {};

        double minX = buildings[indices[0]].x;
        double maxX = buildings[indices[0]].x;
        double minY = buildings[indices[0]].y;
        double maxY = buildings[indices[0]].y;

        for (size_t i = 1; i < indices.size(); ++i)
        {
            int idx = indices[i];
            minX = min(minX, buildings[idx].x);
            maxX = max(maxX, buildings[idx].x);
            minY = min(minY, buildings[idx].y);
            maxY = max(maxY, buildings[idx].y);
        }

        return {Point(minX, minY), Point(maxX, minY), Point(maxX, maxY), Point(minX, maxY)};
    }

    double computePerimeter(const vector<int> &included)
    {
        if (included.empty())
            return 0;

        double minX = buildings[included[0]].x;
        double maxX = minX;
        double minY = buildings[included[0]].y;
        double maxY = minY;

        for (size_t i = 1; i < included.size(); ++i)
        {
            int idx = included[i];
            minX = min(minX, buildings[idx].x);
            maxX = max(maxX, buildings[idx].x);
            minY = min(minY, buildings[idx].y);
            maxY = max(maxY, buildings[idx].y);
        }

        return 2 * ((maxX - minX) + (maxY - minY));
    }

    void evaluate(const vector<int> &selected)
    {
        if ((int)selected.size() < minRequired)
            return;

        double borderLength = computePerimeter(selected);
        double totalWeight = 0.0;

        for (int idx : selected)
            totalWeight += buildings[idx].weight;

        double currentCost = borderLength + totalWeight;

        if (currentCost < bestCost)
        {
            bestCost = currentCost;
            bestPolygon = formRectangle(selected);
        }
    }

    vector<int> initialGreedy()
    {
        vector<int> chosen;
        vector<int> order(totalCount);
        iota(order.begin(), order.end(), 0);

        sort(order.begin(), order.end(), [&](int a, int b)
             { return buildings[a].weight < buildings[b].weight; });

        for (int i = 0; i < min(minRequired, totalCount); ++i)
            chosen.push_back(order[i]);

        for (int i = minRequired; i < totalCount && i < minRequired + 10; ++i)
        {
            int addIdx = order[i];
            if (buildings[addIdx].weight < 0)
            {
                vector<int> extended = chosen;
                extended.push_back(addIdx);

                double costOld = computePerimeter(chosen);
                for (int id : chosen)
                    costOld += buildings[id].weight;

                double costNew = computePerimeter(extended);
                for (int id : extended)
                    costNew += buildings[id].weight;

                if (costNew < costOld)
                    chosen = extended;
            }
        }

        return chosen;
    }

    vector<int> includeNegatives()
    {
        vector<int> negList;
        for (int i = 0; i < totalCount; ++i)
            if (buildings[i].weight < 0)
                negList.push_back(i);

        if ((int)negList.size() >= minRequired)
            return negList;

        vector<int> positives;
        for (int i = 0; i < totalCount; ++i)
            if (buildings[i].weight >= 0)
                positives.push_back(i);

        sort(positives.begin(), positives.end(), [&](int a, int b)
             { return buildings[a].weight < buildings[b].weight; });

        size_t p = 0;
        while (negList.size() < (size_t)minRequired && p < positives.size())
            negList.push_back(positives[p++]);

        return negList;
    }

public:
    ZonePlanner(vector<Building> &data, int need)
        : buildings(data), totalCount((int)data.size()), minRequired(need), bestCost(1e18) {}

    pair<double, vector<Point>> computeOptimal()
    {
        vector<int> try1 = includeNegatives();
        evaluate(try1);

        vector<int> try2 = initialGreedy();
        evaluate(try2);

        vector<int> all(totalCount);
        iota(all.begin(), all.end(), 0);
        sort(all.begin(), all.end(), [&](int a, int b)
             { return buildings[a].weight < buildings[b].weight; });

        vector<int> try3(all.begin(), all.begin() + minRequired);
        evaluate(try3);

        return {bestCost, bestPolygon};
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, k;
cin >> n >> k;
vector<Building> inputBuildings(n);
for (int i = 0; i < n; ++i)
{
    cin >> inputBuildings[i].x >> inputBuildings[i].y >> inputBuildings[i].weight;
}


    ZonePlanner solver(inputBuildings, k);
    auto [optCost, rect] = solver.computeOptimal();


    cout << fixed << setprecision(6) << optCost << "\n";
    for (size_t i = 0; i < rect.size(); ++i)
    {
        size_t next = (i + 1) % rect.size();
        cout << fixed << setprecision(6)
             << rect[i].x << " " << rect[i].y << " "
             << rect[next].x << " " << rect[next].y << "\n";
    }

    return 0;
}
