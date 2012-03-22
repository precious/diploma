#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <vector>

typedef float real;

#include "constants.h"
#include "geometry_utils.h"
#include "time_utils.h"

using namespace std;

struct Point;
struct Plane;
struct Vector;
struct OrientedPlane;
struct Object3D;
typedef OrientedPlane PlaneType;

extern Point POINT_OF_ORIGIN; // (0,0,0)

// system of coordinates orientation
#define ORIENT_RIGHT_HANDED 1
#define ORIENT_LEFT_HANDED 0

// order - clockwise or counterclockwise
#define ORDER_CW 1
#define ORDER_CCW 0

// Particle types
#define PTYPE_ELECTRON 1
#define PTYPE_ION 0

static bool ORIENTATION = ORIENT_RIGHT_HANDED;
void setOrientation(bool);
bool getOrientation();

class GeometryUtils;

class InvalidPrimitiveException: public runtime_error {
public:
    InvalidPrimitiveException(string *msg): runtime_error(*msg) {}
    InvalidPrimitiveException(string &msg): runtime_error(msg) {}
};



struct Point {
    real x;
    real y;
    real z;

    Point(): x(0),y(0),z(0) {}
    Point(real _x, real _y, real _z): x(_x),y(_y),z(_z) {}
    Point(const Point &p) {x = p.x;y = p.y;z = p.z;}

    Point operator+(Vector v);
    Point operator-(Vector v);
    friend ostream& operator<<(ostream &os, const Point &p) {
        os << '(' << p.x << ',' << p.y << ',' << p.z << ')';
        return os;
    }
    bool operator==(Point a) const {
        return (x == a.x && y == a.y && z == a.z);
    }
    bool operator!=(Point b) const {
        return !(*this == b);
    }
    Point rotateAroundZ(double cos,double sin) {
        return Point(x*cos - y*sin,x*sin + y*cos,z);
    }
    Point rotateAroundY(double cos,double sin) {
        return Point(x*cos + z*sin,y,-x*sin + z*cos);
    }
};

struct Vector: public Point {
    Vector(): Point() {}
    Vector(Point p): Point(p) {}
    Vector(real _x, real _y, real _z): Point(_x,_y,_z) {}
    Vector(Point b,Point a): Point(a.x - b.x,a.y - b.y,a.z - b.z) {}

    real operator*(Vector right) {
        return x*right.x + y*right.y + z*right.z;
    }
    Vector operator*(double k) {
        return Vector(k*x,k*y,k*z);
    }
    Vector operator/(double k) {
        return Vector(x/k,y/k,z/k);
    }
    Vector operator+(Vector v) {
        return Vector(x + v.x,y + v.y, z + v.z);
    }
    Vector operator-(Vector v) {
        return Vector(x - v.x,y - v.y, z - v.z);
    }
    real length() {
        return sqrt(x*x + y*y + z*z);
    }
    real cos(Vector right) {
        return ((*this)*right)/(this->length()*right.length());
    }
    Vector vectorProduct(Vector left) {
        return Vector(y*left.z - z*left.y, -x*left.z +
                      z*left.x, x*left.y - y*left.x);
    }
    Vector normalize() {
        double len = length();
        return Vector(x/len,y/len,z/len);
    }
};

template <unsigned int T>
struct Locus { // collection of points
    Point set[T];
    friend ostream& operator<<(ostream &os, const Locus &l) {
        for(unsigned int i = 0;i < T - 1;i++)
            os << l.set[i] << ", ";
        os << l.set[T - 1];
        return os;
    }
};

struct Line: public Locus<2> {
    Vector directionVector;
    Line(Point _a,Point _b): a(set[0]), b(set[1]) {
        set[0] = _a; set[1] = _b;
        directionVector = Vector(_a,_b);
    }
    Line(Point _a,Vector v): a(set[0]), b(set[1]) {
        directionVector = v;
        set[0] = _a;
        set[1] = _a + v;
    }
    Point& a;
    Point& b;
    Point getPointByCoef(real coef) {
        return Point(set[0].x - coef*directionVector.x,
                     set[0].y - coef*directionVector.y,
                     set[0].z - coef*directionVector.z);
    }
};

struct ThreePoints: public Locus<3> {
    ThreePoints(): a(set[0]), b(set[1]), c(set[2]) {}
    ThreePoints(const ThreePoints &tP): a(set[0]), b(set[1]), c(set[2]) {
        set[0] = tP.set[0]; set[1] = tP.set[1]; set[2] = tP.set[2];
    }
    ThreePoints(Point _a,Point _b,Point _c): a(set[0]), b(set[1]), c(set[2]) {
        set[0] = _a; set[1] = _b; set[2] = _c;
    }
    ThreePoints& operator=(const ThreePoints& right) {
        return *(new ThreePoints(right));
    }

    Point& a;
    Point& b;
    Point& c;
    virtual Vector getNormal() {
        return Vector(a,b).vectorProduct(Vector(a,c) );
    }
};

struct Triangle: public ThreePoints {
    Triangle(Point _a,Point _b,Point _c): ThreePoints(_a,_b,_c) {}
    Triangle(ThreePoints &tP): ThreePoints(tP) {}
};

struct Plane: public ThreePoints {
    Plane(): ThreePoints() {}
    Plane(Point _a,Point _b,Point _c): ThreePoints(_a,_b,_c) {}
    Plane(ThreePoints &tP): ThreePoints(tP) {}
    Plane(Point, Vector);
    bool doesPointBelongPlane(Point p) {
        /// TODO possible error because of mashine precision
        return Vector(a,p)*getNormal() == 0;
    }
};

struct OrientedPlane: public Plane {
    Vector normal;
    OrientedPlane(): Plane(), normal() {}
    OrientedPlane(Point _a,Point _b,Point _c, bool _pointsOrder = ORDER_CCW):
        Plane(_a,_b,_c), pointsOrder(_pointsOrder) {
        initNormal();
    }
    OrientedPlane(ThreePoints &tP,  bool _pointsOrder = ORDER_CCW):
        Plane(tP), pointsOrder(_pointsOrder) {
        initNormal();
    }
    OrientedPlane(Plane p, Vector v): Plane(p), normal(v) {}
    OrientedPlane(Point p, Vector v): Plane(p,v), normal(v) {}
    OrientedPlane(const OrientedPlane &op): Plane((ThreePoints&)op),
        normal(op.normal) {}
    Vector getNormal() {
        return normal;
    }

private:
    bool pointsOrder;
    void initNormal() {
        Vector ab(a,b);
        Vector ac(a,c);
        // get cross product
        normal = ab.vectorProduct(ac);
        if (pointsOrder == ORDER_CW) {
            normal = normal*(-1);
        }
    }
};

struct Particle: public Point {
public:
    int ttl;
    Vector step;
    real weight;
    Particle operator+(Vector v);
    Particle(): Point(), step(), weight(0), ttl(-1) {}
    Particle(Point p, Vector s, real w): Point(p), step(s), weight(w), ttl(-1) {}
};

struct Sphere {
    Point center;
    real radius;
    Sphere(): center(), radius(0) {}
    Sphere(Point _p, real _r): center(_p), radius(_r) {}
    Sphere(const Sphere &_s): center(_s.center), radius(_s.radius) {}
};

// i'm using GenerativeSphere, not this class >>
struct GenerativeSurface: public OrientedPlane {
public:
    GaussianDistributionGenerator *electronVelocityGenerator;
    GaussianDistributionGenerator *ionVelocityGenerator;
    Point center;
    real width, height;
    Vector i, j;
    Vector objectDirection;
    GenerativeSurface(OrientedPlane oP, Point _center,
                      real _width, real _height, Vector _objectDirection):
            OrientedPlane(oP,oP.normal), center(_center),
            width(_width), height(_height), objectDirection(_objectDirection) {
        i = Vector(_center,(_center != a)? a: b).normalize();
        j = normal.vectorProduct(i).normalize();

        electronVelocityGenerator = getGaussianDistributionGenerator(ELECTRON_VELOCITY,ELECTRON_VELOCITY/3.0);
        ionVelocityGenerator = getGaussianDistributionGenerator(ION_VELOCITY,ION_VELOCITY/3.0);
    }
    Point generatePoint();
    Particle generateParticle(int);
};

struct GenerativeSphere: public Sphere {
    GaussianDistributionGenerator *electronVelocityGenerator;
    GaussianDistributionGenerator *ionVelocityGenerator;
    Vector objectDirection;

    GenerativeSphere(Point _p, real _r,Vector _objectDirection):
            Sphere(_p, _r), objectDirection(_objectDirection) {
        init();
    }
    GenerativeSphere(const Sphere &_s,Vector _objectDirection):
            Sphere(_s), objectDirection(_objectDirection) {
        init();
    }

    void init() {
        // expectation and standart deviation are calculated due the 3-sigma rule
        // max and min possible velocities are 2*ELECTRON_VELOCITY and 0 respectively
        electronVelocityGenerator = getGaussianDistributionGenerator(ELECTRON_VELOCITY,ELECTRON_VELOCITY/3.0);
        ionVelocityGenerator = getGaussianDistributionGenerator(ION_VELOCITY,ION_VELOCITY/3.0);
    }

    Particle generateParticle(int);
};

struct Object3D {
    Vector front;
    Point maxCoords, minCoords;
    Point nearestPoint, furthermostPoint; // relatively to front of the object
    vector<PlaneType> *polygons;
    velocity speed;

    Object3D(int polygonsNumber, Vector _front = Vector(100,0,0)): front(_front) {
        polygons = new vector<PlaneType>(polygonsNumber);
        init();
    }
    Object3D(vector<PlaneType> *_polygons, Vector _front = Vector(100,0,0)):
            front(_front), polygons(_polygons) {
        init();
    }

    void init() {
        speed = ORBITAL_VELOCITY;
        nearestPoint = furthermostPoint = maxCoords = minCoords = polygons->at(0).set[0];
        for(vector<PlaneType>::iterator it = polygons->begin();it != polygons->end();it++)
            for(int i = 0;i < 3;i++) {
                if (Vector(nearestPoint,(*it).set[i]).cos(front) > 0)
                    nearestPoint = (*it).set[i];
                if (Vector(furthermostPoint,(*it).set[i]).cos(front) < 0)
                    furthermostPoint = (*it).set[i];

                if (maxCoords.x < (*it).set[i].x) maxCoords.x = (*it).set[i].x;
                if (maxCoords.y < (*it).set[i].y) maxCoords.y = (*it).set[i].y;
                if (maxCoords.z < (*it).set[i].z) maxCoords.z = (*it).set[i].z;
                if (minCoords.x > (*it).set[i].x) minCoords.x = (*it).set[i].x;
                if (minCoords.y > (*it).set[i].y) minCoords.y = (*it).set[i].y;
                if (minCoords.z > (*it).set[i].z) minCoords.z = (*it).set[i].z;
            }
    }
    PlaneType& operator[](int i) {
        return polygons->at(i);
    }
    Point center() {
        return Point((maxCoords.x + minCoords.x)/2,
                     (maxCoords.y + minCoords.y)/2,
                     (maxCoords.z + minCoords.z)/2);
    }
};

#endif // TYPES_H

