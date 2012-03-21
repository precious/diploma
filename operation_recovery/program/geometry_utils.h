#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include "types.h"
#include <algorithm>
#include <iostream>
#include <cmath>

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

using namespace std;

template <typename T>
class Checker;

struct ThreePoints;
struct Line;
struct Point;
struct Plane;
struct Sphere;
struct Vector;
struct Triangle;
struct Particle;
struct Object3D;

class GeometryUtils {
public:
    static bool isPointInsideTriangle(ThreePoints&,Point&);

    DEPRECATED(static Point getPlaneAndLineIntersection(ThreePoints&,Line));
    static Point getPlaneAndLineIntersection2(ThreePoints&,Line);

    static bool doesLineIntersectTriangle(ThreePoints&,Line&);

    static Point getRandomPointFromSphere(Sphere);

    static Point getRandomPointOnSphere(Sphere);

    static Vector getRandomOrthogonalVector(Vector);

    static Point getPointOnPlaneProjection(ThreePoints&,Point);

    static real getDistanceBetweenPoints(Point,Point);

    static real getDistanceBetweenPointAndPlane(ThreePoints&,Point);

    static Particle getFastestParticle(vector<Particle>&,Point,function<bool (Particle)>);

    static bool isPointInsideParallelepiped(Point,Point,Point);

    //static bool isPointInsideObject(Point,Object3D&);

    static bool doesParticlesTrajectoryIntersectObject(Particle,Object3D&);

    static bool doesLineIntersectParallelepiped(Line,Point,Point);
};

#endif // GEOMETRY_UTILS_H

