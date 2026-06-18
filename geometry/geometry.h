#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <utility>
#include <algorithm>

static constexpr double kAccuracy = 1e-9;

class Line;

struct Point{
    double x = 0;
    double y = 0;

    Point() = default;

    Point(double px, double py): x(px), y(py){}

    double length() const{
        return std::sqrt(length2());
    }

    double length2() const{
        return x * x + y * y;
    }

    double crossProduct(const Point& another) const{
        return x * another.y - another.x * y;
    }

    Point normalize() const{
        double len = length();
        return Point(x / len, y / len);
    }

    void rotate(const Point& center, double angle){
        double dx = x - center.x;
        double dy = y - center.y;

        double rx = dx * std::cos(angle) - dy * std::sin(angle);
        double ry = dx * std::sin(angle) + dy * std::cos(angle);

        x = center.x + rx;
        y = center.y + ry;
    }

    void reflect(const Point& center){
        x = 2 * center.x - x;
        y = 2 * center.y - y;
    }

    void reflect(const Line& axis);

    void scale(const Point& center, double coefficient){
        x = center.x + (x - center.x) * coefficient;
        y = center.y + (y - center.y) * coefficient;
    }

    Point perpendicular() const{
        return Point(-y, x);
    }
};

bool operator==(const Point& lhs, const Point& rhs){
    return std::fabs(lhs.x - rhs.x) < kAccuracy && std::fabs(lhs.y - rhs.y) < kAccuracy;
}

bool operator!=(const Point& lhs, const Point& rhs){
    return !(lhs == rhs);
}

Point operator+(const Point& lhs, const Point& rhs){
    return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point operator-(const Point& lhs, const Point& rhs){
    return Point(lhs.x - rhs.x, lhs.y - rhs.y);
}

Point operator*(const Point& p, double k){
    return Point(p.x * k, p.y * k);
}

Point operator/(const Point& p, double k){
    return Point(p.x / k, p.y / k);
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
        cC_ = -cA_ * p1.x - cB_ * p1.y;
    }

    Line(const Point& point, double k): cA_(k), cB_(-1), cC_(point.y - k * point.x){}

    Line(double k, double b): cA_(k), cB_(-1), cC_(b){}

    bool operator==(const Line& another) const{
        return std::fabs(cA_ * another.cB_ - cB_ * another.cA_) < kAccuracy
            && std::fabs(cA_ * another.cC_ - cC_ * another.cA_) < kAccuracy
            && std::fabs(cC_ * another.cB_ - cB_ * another.cC_) < kAccuracy;
    }

    bool operator!=(const Line& another) const{
        return !(*this == another);
    }

    Point getReflectedPoint(const Point& point) const{
        double factor = 2.0 * (cA_ * point.x + cB_ * point.y + cC_) / (cA_ * cA_ + cB_ * cB_);
        return Point(point.x - factor * cA_, point.y - factor * cB_);
    }

    Point intersection(const Line& another) const{
        double det = cA_ * another.cB_ - another.cA_ * cB_;
        double px = (cB_ * another.cC_ - another.cB_ * cC_) / det;
        double py = (another.cA_ * cC_ - cA_ * another.cC_) / det;
        return Point(px, py);
    }
};

void Point::reflect(const Line& axis){
    Point reflected = axis.getReflectedPoint(*this);
    x = reflected.x;
    y = reflected.y;
}


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
protected:
    std::vector<Point> vertices_;

    static std::vector<double> profile(const std::vector<Point>& v){
        size_t n = v.size();
        std::vector<double> result(2 * n);
        for(size_t i = 0; i < n; ++i){
            Point prev = v[(i + n - 1) % n] - v[i];
            Point next = v[(i + 1) % n] - v[i];
            result[2 * i] = std::atan2(std::fabs(prev.crossProduct(next)), prev.x * next.x + prev.y * next.y);
            result[2 * i + 1] = next.length();
        }
        return result;
    }

    static bool matchProfile(const std::vector<double>& a, const std::vector<double>& b, bool similar){
        size_t n = a.size();
        if(b.size() != n) return false;
        for(size_t shift = 0; shift < n; shift += 2){
            double k = -1;
            bool ok = true;
            for(size_t i = 0; i < n; ++i){
                double av = a[i];
                double bv = b[(i + shift) % n];
                if(i % 2 == 0){
                    if(std::fabs(av - bv) > kAccuracy){ ok = false; break; }
                } else if(similar){
                    if(k < 0) k = av / bv;
                    if(std::fabs(av / bv - k) > kAccuracy){ ok = false; break; }
                } else if(std::fabs(av - bv) > kAccuracy){
                    ok = false;
                    break;
                }
            }
            if(ok) return true;
        }
        return false;
    }

    bool sameShape(const Shape& another, bool similar) const{
        const Polygon* ptr = dynamic_cast<const Polygon*>(&another);
        if(!ptr || verticesCount() != ptr->verticesCount()) return false;
        std::vector<Point> mine = vertices_;
        std::vector<Point> other = ptr->vertices_;
        if(matchProfile(profile(mine), profile(other), similar)) return true;
        std::reverse(other.begin(), other.end());
        return matchProfile(profile(mine), profile(other), similar);
    }

public:
    Polygon(const std::vector<Point>& points): vertices_(points){}

    template <typename ...Points>
    Polygon(const Points& ...points): vertices_({points...}){}

    size_t verticesCount() const{
        return vertices_.size();
    }

    std::vector<Point> getVertices() const{
        return vertices_;
    }

    bool isConvex() const{
        bool positive = false;
        bool negative = false;
        size_t n = vertices_.size();
        for(size_t i = 0; i < n; ++i){
            double cross = (vertices_[i] - vertices_[(i + 1) % n]).crossProduct(vertices_[(i + 1) % n] - vertices_[(i + 2) % n]);
            if(cross > 0) positive = true;
            else negative = true;
        }
        return !(positive && negative);
    }

    double perimeter() const override{
        double result = 0;
        size_t n = vertices_.size();
        for(size_t i = 0; i < n; ++i){
            result += (vertices_[i] - vertices_[(i + 1) % n]).length();
        }
        return result;
    }

    double area() const override{
        double result = 0;
        size_t n = vertices_.size();
        for(size_t i = 0; i < n; ++i){
            result += (vertices_[i] - vertices_[0]).crossProduct(vertices_[(i + 1) % n] - vertices_[0]);
        }
        return std::fabs(result) / 2;
    }

    bool operator==(const Shape& another) const override{
        const Polygon* ptr = dynamic_cast<const Polygon*>(&another);
        if(!ptr || verticesCount() != ptr->verticesCount()) return false;
        size_t n = verticesCount();
        for(int dir = 0; dir < 2; ++dir){
            for(size_t shift = 0; shift < n; ++shift){
                bool equal = true;
                for(size_t i = 0; i < n; ++i){
                    size_t j = dir == 0 ? (i + shift) % n : (shift + n - i) % n;
                    if(vertices_[i] != ptr->vertices_[j]){ equal = false; break; }
                }
                if(equal) return true;
            }
        }
        return false;
    }

    bool isCongruentTo(const Shape& another) const override{
        return sameShape(another, false);
    }

    bool isSimilarTo(const Shape& another) const override{
        return sameShape(another, true);
    }

    bool containsPoint(const Point& point) const override{
        size_t n = vertices_.size();

        auto onSegment = [&](const Point& a, const Point& b, const Point& q){
            double cross = Point{q.x - a.x, q.y - a.y}.crossProduct(Point{b.x - a.x, b.y - a.y});
            if(std::fabs(cross) > kAccuracy) return false;

            double dot = (q.x - a.x) * (b.x - a.x) + (q.y - a.y) * (b.y - a.y);
            if(dot < -kAccuracy) return false;
            double len2 = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
            return dot <= len2 + kAccuracy;
        };

        bool inside = false;

        for(size_t i = 0, j = n - 1; i < n; j = i++){
            const Point& a = vertices_[j];
            const Point& b = vertices_[i];

            if(onSegment(a, b, point)) return true;

            if((a.y > point.y) != (b.y > point.y)){
                double xIntersect = a.x + (b.x - a.x) * (point.y - a.y) / (b.y - a.y);
                if(xIntersect > point.x - kAccuracy){
                    inside = !inside;
                }
            }
        }

        return inside;
    }

    void rotate(const Point& point, double angle) override{
        for(Point& vertex : vertices_) vertex.rotate(point, angle);
    }

    void reflect(const Point& center) override{
        for(Point& vertex : vertices_) vertex.reflect(center);
    }

    void reflect(const Line& axis) override{
        for(Point& vertex : vertices_) vertex.reflect(axis);
    }

    void scale(const Point& center, double coefficient) override{
        for(Point& vertex : vertices_) vertex.scale(center, coefficient);
    }
};


class Ellipse: public Shape{
protected:
    Point focus1_;
    Point focus2_;
    double diameter_;

    double majorAxis() const{
        return diameter_ / 2;
    }

    double minorAxis() const{
        return std::sqrt(diameter_ * diameter_ - (focus1_ - focus2_).length2()) / 2;
    }

public:
    Ellipse(const Point& focus1, const Point& focus2, double diameter): focus1_(focus1), focus2_(focus2), diameter_(diameter){}

    std::pair<Point, Point> focuses() const{
        return std::make_pair(focus1_, focus2_);
    }

    Point center() const{
        return (focus1_ + focus2_) / 2;
    }

    double eccentricity() const{
        return std::sqrt(1 - minorAxis() * minorAxis() / (majorAxis() * majorAxis()));
    }

    std::pair<Line, Line> directrices() const{
        Point mid = center();
        double dist = majorAxis() / eccentricity();
        Point direction = (focus2_ - focus1_).normalize();
        Point perp = direction.perpendicular();

        Point first = mid + direction * dist;
        Point second = mid - direction * dist;
        return std::make_pair(Line(first, first + perp), Line(second, second + perp));
    }

    double perimeter() const override{
        double a = majorAxis();
        double b = minorAxis();
        return M_PI * (3 * (a + b) - std::sqrt((3 * a + b) * (a + 3 * b)));
    }

    double area() const override{
        return M_PI * majorAxis() * minorAxis();
    }

    bool operator==(const Shape& another) const override{
        const Ellipse* ptr = dynamic_cast<const Ellipse*>(&another);
        if(!ptr) return false;
        return ((focus1_ == ptr->focus1_ && focus2_ == ptr->focus2_)
            || (focus1_ == ptr->focus2_ && focus2_ == ptr->focus1_))
            && std::fabs(diameter_ - ptr->diameter_) < kAccuracy;
    }

    bool isCongruentTo(const Shape& another) const override{
        const Ellipse* ptr = dynamic_cast<const Ellipse*>(&another);
        if(!ptr) return false;
        return std::fabs(diameter_ - ptr->diameter_) < kAccuracy
            && std::fabs((focus1_ - focus2_).length2() - (ptr->focus1_ - ptr->focus2_).length2()) < kAccuracy;
    }

    bool isSimilarTo(const Shape& another) const override{
        const Ellipse* ptr = dynamic_cast<const Ellipse*>(&another);
        if(!ptr) return false;
        return std::fabs(eccentricity() - ptr->eccentricity()) < kAccuracy;
    }

    bool containsPoint(const Point& point) const override{
        return (focus1_ - point).length() + (focus2_ - point).length() < diameter_ + kAccuracy;
    }

    void rotate(const Point& point, double angle) override{
        focus1_.rotate(point, angle);
        focus2_.rotate(point, angle);
    }

    void reflect(const Point& center) override{
        focus1_.reflect(center);
        focus2_.reflect(center);
    }

    void reflect(const Line& axis) override{
        focus1_.reflect(axis);
        focus2_.reflect(axis);
    }

    void scale(const Point& center, double coefficient) override{
        focus1_.scale(center, coefficient);
        focus2_.scale(center, coefficient);
        diameter_ *= coefficient;
    }
};


class Circle final: public Ellipse{
public:
    Circle(const Point& center, double radius): Ellipse(center, center, 2 * radius){}

    double radius() const{
        return diameter_ / 2;
    }

    double perimeter() const override{
        return 2 * M_PI * radius();
    }

    double area() const override{
        return M_PI * radius() * radius();
    }
};


class Rectangle: public Polygon{
public:
    using Polygon::Polygon;

    Rectangle(const Point& p1, const Point& p2, double ratio): Polygon(std::vector<Point>(4)){
        Point mid = (p1 + p2) / 2;
        double angle = 2 * std::atan(ratio);

        Point v1 = p1;
        Point v3 = p2;
        v1.rotate(mid, angle);
        v3.rotate(mid, angle);

        vertices_[0] = p1;
        vertices_[1] = v1;
        vertices_[2] = p2;
        vertices_[3] = v3;
    }

    Point center() const{
        std::pair<Line, Line> d = diagonals();
        return d.first.intersection(d.second);
    }

    std::pair<Line, Line> diagonals() const{
        return std::make_pair(Line(vertices_[0], vertices_[2]), Line(vertices_[1], vertices_[3]));
    }

    double perimeter() const override{
        return 2 * ((vertices_[0] - vertices_[1]).length() + (vertices_[1] - vertices_[2]).length());
    }

    double area() const override{
        return (vertices_[0] - vertices_[1]).length() * (vertices_[1] - vertices_[2]).length();
    }
};


class Square final: public Rectangle{
public:
    using Rectangle::Rectangle;

    Square(const Point& p1, const Point& p2): Rectangle(p1, p2, 1.0){}

    Circle circumscribedCircle() const{
        Point mid = (vertices_[0] + vertices_[2]) / 2;
        return Circle(mid, (vertices_[0] - mid).length());
    }

    Circle inscribedCircle() const{
        Point mid = (vertices_[0] + vertices_[2]) / 2;
        return Circle(mid, (vertices_[0] - vertices_[1]).length() / 2);
    }
};


class Triangle final: public Polygon{
public:
    using Polygon::Polygon;

    Circle circumscribedCircle() const{
        Point mid1 = (vertices_[0] + vertices_[1]) / 2;
        Point mid2 = (vertices_[1] + vertices_[2]) / 2;

        Point perp1 = (vertices_[0] - vertices_[1]).perpendicular();
        Point perp2 = (vertices_[1] - vertices_[2]).perpendicular();

        Point mid = Line(mid1, mid1 + perp1).intersection(Line(mid2, mid2 + perp2));
        return Circle(mid, (mid - vertices_[0]).length());
    }

    Circle inscribedCircle() const{
        double a = (vertices_[1] - vertices_[2]).length();
        double b = (vertices_[0] - vertices_[2]).length();
        double c = (vertices_[0] - vertices_[1]).length();
        Point incenter = (vertices_[0] * a + vertices_[1] * b + vertices_[2] * c) / (a + b + c);
        return Circle(incenter, 2 * area() / perimeter());
    }

    Point centroid() const{
        return (vertices_[0] + vertices_[1] + vertices_[2]) / 3;
    }

    Point orthocenter() const{
        Point perp_bc = (vertices_[2] - vertices_[1]).perpendicular();
        Line altitude_a(vertices_[0], vertices_[0] + perp_bc);

        Point perp_ac = (vertices_[2] - vertices_[0]).perpendicular();
        Line altitude_b(vertices_[1], vertices_[1] + perp_ac);

        return altitude_a.intersection(altitude_b);
    }

    Circle ninePointsCircle() const{
        Circle circle = circumscribedCircle();
        Point mid = (orthocenter() + circle.center()) / 2;
        return Circle(mid, circle.radius() / 2);
    }

    Line EulerLine() const{
        return Line(orthocenter(), circumscribedCircle().center());
    }

    double area() const override{
        double a = (vertices_[0] - vertices_[1]).length();
        double b = (vertices_[1] - vertices_[2]).length();
        double c = (vertices_[2] - vertices_[0]).length();
        double p = (a + b + c) / 2;
        return std::sqrt(p * (p - a) * (p - b) * (p - c));
    }
};
