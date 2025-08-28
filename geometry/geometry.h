#pragma once
#include <iostream>
#include <vector>
#include <cmath>

static constexpr double kAccuracy = 1e-9;

struct Point{
    double x = 0;
    double y = 0;


    Point() = default;

    Point(double x, double y): x(x), y(y){}
};

bool operator==(const Point& lhs, const Point& rhs){
    return std::fabs(lhs.x - rhs.x) < kAccuracy && std::fabs(lhs.y - rhs.y) < kAccuracy;
}


class Line{
private:
    double cA_;
    double cB_;
    double cC_;

public:
    Line(const Point& p1, const Point& p2){
        cA_ = p2.y - p1.y;
        cB_ = p1.x - p2.x;
        cC_ = - cA_ * p1.x - cB_ * p1.y;
    }

    Line(const Point& point, double k): cA_(1), cB_(-k), cC_(k * point.y - point.x){}

    Line(double k, double b): cA_(1), cB_(-k), cC_(k * b){}

    bool operator==(const Line& another) const{
        return fabs(cA_ * another.cB_- cB_ * another.cA_) < kAccuracy
        && fabs(cA_ * another.cC_- cC_ * another.cA_) < kAccuracy
        && fabs(cC_ * another.cB_- cB_ * another.cC_) < kAccuracy;
    }
};


class Shape{
public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;

    virtual bool operator==(const Shape& another) const = 0;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& point, double angle) = 0;
    virtual void reflect(const Point& center) = 0;
    virtual void reflect(const Line& axis) = 0;
    virtual void scale(const Point& center, double coefficient) = 0;

    virtual ~Shape() = default;
};



class Polygon: public Shape{
private:
    std::vector<Point> vertices_;

public:
    Polygon(const std::vector<Point>& points);

    template <typename ...Points>
    Polygon(const Points& ...points): vertices_({...points}){}


    size_t vecticesCount() const;
    std::vector<Point> getVertices() const;  
    bool isConvex() const;  //Выпуклый?

};



class Ellipse: public Shape{
protected:
    Point focus1_;
    Point focus2_;
    double diameter_;

public:
    Ellipse(const Point& focus1, const Point& focus2, double diameter)

    std::pair<Point, Point> focuses() const;
    std::pair<Line, Line> directrices() const;
    double eccentricity() const;
    Point center() const;



};


class Circle final: public Ellipse{
public:
    Circle(const Point& center, double radius);
    double radius() const;

};



class Rectangle: public Polygon{
public:
    using Polygon::Polygon;

    Rectangle(const Point& p1, const Point& p2, double ratio);

    Point center() const;
    std::pair<Line, Line> diagonals() const;

};

class Square final: public Rectangle{
public:
    using Rectangle::Rectangle;

    Square(const Point& p1, const Point& p2);

    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
};


class Triangle final: public Polygon{
public:
    using Polygon::Polygon;

    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
    Point centroid() const;
    Point orthocenter() const;
    Line EulerLine() const;
    Circle ninePointsCircle() const;
};