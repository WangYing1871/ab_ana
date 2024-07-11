#ifndef TrackFind_h
#define TrackFind_h
// this package contains the variables and functions for tack finding, it is called in Hitsmap.h and ConstructTree.h

#include <iostream>
#include <vector>
#include <cmath>
#include <set>

struct Point {
    int x;
    float y;
    Point() : x(0), y(0) {}
    Point(int _x, float _y) : x(_x), y(_y) {}

    // operator redefined
    bool operator<(const Point& other) const {
        return y < other.y;
    }
};

// define the calculation method of distance
// since for the output data, the X/Y dimenssion is represented by the strip id, it should times 1.33 to convert to mm
// also, the z dimension is represented by sampling point, 1 for 40 ns, considering the drift velocity of about 3.5cm/us, it should times 40*1e-3*3.5*10 to convert to mm
float calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow((p1.x - p2.x)*1.33, 2) + pow((p1.y - p2.y)*40*1e-3*3.5*10, 2));
}



// connect the points according to the neareast to the last point
std::vector<Point> findNearestNeighbors(const std::vector<Point>& points, float& tracklength) {
    std::vector<Point> result;
    std::set<Point> visited;
    
    // find starting point, with max Z
    Point startingPoint = *std::max_element(points.begin(), points.end());

    // std::cout << "Start position: (" << startingPoint.x << ", " << startingPoint.y << ")" << std::endl;

    //define maximum loop time in order to avoid the case that the visited points is always smaller than the total points (maybe due to 2 points overlap)
    int n_loop = 0;
    int maximum = points.size();

    while (visited.size() < points.size()) {
        result.push_back(startingPoint);
        visited.insert(startingPoint);

        // find nearest remaining point to this point
        float minDistance = std::numeric_limits<float>::max();
        Point nextPoint;

        for (const Point& neighbor : points) {
            if (visited.find(neighbor) == visited.end()) {
                float distance = calculateDistance(startingPoint, neighbor);
                if (distance < minDistance) {
                    minDistance = distance;
                    nextPoint = neighbor;
                }
            }
        }

        startingPoint = nextPoint;

        n_loop++;
        if(n_loop>=maximum || (startingPoint.x==0 && startingPoint.y==0)) break;
        tracklength += minDistance;
    }

    return result;
}

#endif