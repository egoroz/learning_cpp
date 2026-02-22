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

    inline double length() const{
        return std::sqrt(length2());
    }

    inline double length2() const{
        return x * x + y * y;
    }

    inline double crossProduct(const Point& another) const{
        return x * another.y - another.x * y;
    }

    void rotate(const Point& point, double angle){
        double nx = x - point.x;
        double ny = y - point.y;

        nx = nx * (std::cos(angle) - std::sin(angle));
        ny = ny * (std::sin(angle) + std::cos(angle)); 

        x = point.x + nx;
        y = point.y + ny;
    }

    void reflect(const Point& center){
        double nx = x - center.x;
        double ny = y - center.y;

        x = center.x - nx;
        y = center.y - ny;
    }

    void reflect(const Line& axis){
        Point reflected = axis.getReflectedPoint(*this);
        x = reflected.x;
        y = reflected.y;
    }

    void scale(const Point& center, double coefficient){
        double nx = (x - center.x) * coefficient;
        double ny = (y - center.y) * coefficient;

        x = center.x + nx;
        y = center.y + ny;
    }

    Point perpendicular() const{
        Point p = *this;
        std::swap(p.x, p.y);
        p.x = -p.x;
        return p;
    }
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

    Point getReflectedPoint(Point point) const{
        Point reflected;
        double factor = 2.0 * (cA_ * point.x + cB_ * point.y + cC_) / (cA_ * cA_ + cB_ * cB_);

        reflected.x = point.x - factor * cA_;
        reflected.y = point.y - factor * cB_;
        return reflected;
    }

    Point intersection(const Line& another){
        return Point((cC_ * another.cB_ - cB_ * another.cC_) / (cB_ * another.cA_ - cA_ - another.cB_)
                    , (cA_ * another.cC_ - cC_ * another.cA_) / (cB_ * another.cA_ - cA_ - another.cB_));
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
protected:
    std::vector<Point> vertices_;

public:
    Polygon(const std::vector<Point>& points): vertices_(points){}

    template <typename ...Points>
    Polygon(const Points& ...points): vertices_({...points}){}

    size_t vecticesCount() const{
        return vertices_.size();
    }

    std::vector<Point> getVertices() const{
        return vertices_;
    }  
    
    bool isConvex() const{
        bool positive_orientation = false, negative_orientation = false;
        size_t v_size = vertices_.size();
        for(int i = 0; i < v_size; ++i){
            if((vertices_[i] - vertices_[(i + 1) % v_size]).crossProduct(vertices_[(i + 1) % v_size] - vertices_[(i + 2) % v_size]) > 0){
                positive_orientation = true;
            } else{
                negative_orientation = true;
            }
        }
        return !(positive_orientation && negative_orientation);
    }

    double perimeter() const override{
        double perimeter = 0;
        size_t v_size = vertices_.size();
        for(int i = 0; i < v_size; ++i){
            perimeter += (vertices_[i] - vertices_[(i + 1) % v_size]).length();
        }
        return perimeter;
    }

    double area() const override{
        double area = 0;
        size_t v_size = vertices_.size();
        for(int i = 0; i < v_size; ++i){
            area += (vertices_[0] - vertices_[i]).crossProduct(vertices_[0] - vertices_[(i + 1) % v_size]);
        }
        return area;
    } 

    bool operator==(const Shape& another) const override{
        const Polygon* another_ptr = dynamic_cast<const Polygon*> (&another);
        if(!another_ptr) return false;
        if(vecticesCount() != another_ptr->verticesCount()) return false;
        bool is_equal = true;
        int shift;
        for(int i = 0; i < another.verticesCount(); ++i){
            if(vertices_[0] == another_ptr->vertices_[i]){
                shift = i;
            }
        }
        for(int i = 0; i < another_ptr->verticesCount(); ++i){
            is_equal = is_equal && (vertices_[i] == another_ptr->vertices_[(i + shift) % another_ptr->verticesCount()]);
        }
        return is_equal;
    }

    double* getSimilarCoefficient(const Polygon& another){
        //TODO. Пройти в 2 стороны
        return nullptr;
    }

    bool isCongruentTo(const Shape& another) const override{
        const Polygon* another_ptr = dynamic_cast<const Polygon*> (&another);
        if(!another_ptr) return false;
        //TODO
    }

    bool isSimilarTo(const Shape& another) const override{
        const Polygon* another_ptr = dynamic_cast<const Polygon*> (&another);
        if(!another_ptr) return false;
        //TODO
    }

    bool containsPoint(const Point& point) const override{
        size_t n = vertices_.size();

        auto onSegment = [&](const Point& a, const Point& b, const Point& q) {
            double cross = Point{q.x - a.x, q.y - a.y}.crossProduct(Point{b.x - a.x, b.y - a.y});
            if (std::fabs(cross) > kAccuracy) return false;

            double dot = (q.x - a.x) * (b.x - a.x) + (q.y - a.y) * (b.y - a.y);
            if (dot < -kAccuracy) return false;
            double len2 = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
            return dot <= len2 + kAccuracy;
        };

        bool inside = false;

        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Point& a = vertices_[j];
            const Point& b = vertices_[i];

            if (onSegment(a, b, point)) return true;

            if ((a.y > point.y) != (b.y > point.y)) {
                double xIntersect = a.x + (b.x - a.x) * (point.y - a.y) / (b.y - a.y);
                if (xIntersect > point.x - kAccuracy) {
                    inside = !inside;
                }
            }
        }

        return inside;
    }

    void rotate(const Point& point, double angle) override{
        for(int i = 0; i < vertices_.size(); ++i){
            vertices_[i].rotate(point, angle);
        }
    }

    void reflect(const Point& center) override{
        for(int i = 0; i < vertices_.size(); ++i){
            vertices_[i].reflect(center);
        }
    }

    void reflect(const Line& axis) override{
        for(int i = 0; i < vertices_.size(); ++i){
            vertices_[i].reflect(axis);
        }
    }

    void scale(const Point& center, double coefficient) override{
        for(int i = 0; i < vertices_.size(); ++i){
            vertices_[i].scale(center, coefficient);
        }
    }

};



class Ellipse: public Shape{
protected:
    Point focus1_;
    Point focus2_;
    double diameter_;

public:
    Ellipse(const Point& focus1, const Point& focus2, double diameter): focus1_(focus1), focus2_(focus2), diameter_(diameter){}

    std::pair<Point, Point> focuses() const{
        return std::make_pair(focus1_, focus2_);
    }

    std::pair<Line, Line> directrices() const{
        Point center_pt = center();
        double dist = majorAxis() / eccentricity();
        Point direction = (focus2_ - focus1_) / (focus2_ - focus1_).length();

        Point perp_direction = direction;
        std::swap(perp_direction.x, perp_direction.y);
        perp_direction.x = -perp_direction.x;

        Point directrice_pt = center_pt + direction * dist;
        Line d1(directrice_pt, directrice_pt + perp_direction);
        directrice_pt = center_pt - direction * dist;
        Line d2(directrice_pt, directrice_pt + perp_direction);

        return std::make_pair(d1, d2);
    }

    double eccentricity() const{
        return std::sqrt(1 - std::pow(minorAxis(), 2) / std::pow(majorAxis(), 2))
    }

    Point center() const{
        Point center_pt = (focus1_ + focus2_) / 2;
        return center_pt;
    }

    double perimeter() const override{
        return 2 * M_PI * std::sqrt((std::pow(majorAxis(), 2) + std::pow(minorAxis())) / 2, 2);
    }

    double area() const override{
        return M_PI * majorAxis() * minorAxis();
    }  

    bool operator==(const Shape& another) const override{
        const Ellipse* another_ptr = dynamic_cast<const Ellipse*> (&another);
        if(!another_ptr){
            return false;
        }

        return ((focus1_ == another_ptr->focus1_ && focus2_ == another_ptr->focus2_)
        || (focus1_ == another_ptr->focus2_ && focus2_ == another_ptr->focus1_)) 
        && (diameter_ - another_ptr->diameter_) < kAccuracy;
    }

    bool isCongruentTo(const Shape& another) const override{
        const Ellipse* another_ptr = dynamic_cast<const Ellipse*> (&another);
        if(!another_ptr){
            return false;
        }
        return eccentricity() == another_ptr->eccentricity() && (focus1_ - focus2_).lenght2() == (another_ptr->focus1_ - another_ptr->focus2_).lenght2();
    }

    bool isSimilarTo(const Shape& another) const override{
        const Ellipse* another_ptr = dynamic_cast<const Ellipce*> (&another);
        if(!another_ptr){
            return false;
        }
        return eccentricity() - another->eccentricity() < kAccuracy;
    }


    bool containsPoint(const Point& point) const override{
        return ((focus1_ - point).length() + (focus2_ - point).length()) < diameter_;
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

protected:
    inline double majorAxis(){
        return diameter_ / 2;
    }

    inline double minorAxis(){
        return std::sqrt(diameter_ * diameter_ - (focus1_ - focus2_).length2()) / 2;
    }

};


class Circle final: public Ellipse{
public:
    Circle(const Point& center, double radius): focus1_(center), focus2_(center), diameter_(2 * radius){}

    inline double radius() const{
        return diameter_ / 2;
    }
    
    double perimeter() const override{
        return 2* M_PI radius();
    }

    double area() const override{
        return M_PI * std::pow(radius(), 2);
    }

    bool operator==(const Shape& another) const override{
        const Circle* another_ptr = dynamic_cast<const Circle*> (&another);
        if(!another_ptr){
            return false;
        }
        return radius() - another_ptr->radius() < kAccuracy 
        && (center() == another_ptr->center());
    }

    bool isCongruentTo(const Shape& another) const override{
        const Circle* another_ptr = dynamic_cast<const Circle*> (&another);
        if(!another_ptr){
            return false;
        }
        return radius() - another_ptr->radius() < kAccuracy;
    }

    bool isSimilarTo(const Shape& another) const override{
        const Circle* another_ptr = dynamic_cast<const Circle*> (&another);
        if(!another_ptr){
            return false;
        }
        return true;
    }

    bool containsPoint(const Point& point) const override{
        return (point - focus1_).length() < radius();
    }


};



class Rectangle: public Polygon{
public:
    using Polygon::Polygon;

    Rectangle(const Point& p1, const Point& p2, double ratio): vertices_(std::vector<Point>(4)){
        vertices_[0] = p1;
        vertices_[2] = p2;

        Point direction = p2 - p1;
        Point perp_direction = direction.perpendicular();

        double little_edge = std::sqrt(direction.length2() / (ratio * ratio + 1));
        double big_edge = ratio * little_edge;

        vertices_[1] = (direction.rotate(p1, std::atan(big_edge / little_edge))).normalize() * little_edge;
        vertices_[3] = (direction.rotate(p1, -std::atan(little_edge / big_edge))).normalize() * big_edge;      
    }

    Point center() const{
        std::pair<Line, Line> diagonals = diagonals();
        Point center = diagonals.first.intersection(diagonals.second);
        return center;
    }
    std::pair<Line, Line> diagonals() const{
        Line d1(vertices_[0], vertices_[2]);
        Line d2(vertices_[1], vertices_[3]);
        return std::make_pair(d1, d2);
    }

    double perimeter() const override{
        return 2 * ((vertices_[0] - vertices_[1]).length() + (vertices_[1] - vertices_[2]).length());
    }

    double area() const override{
        return std::sqrt((vertices_[0] - vertices_[1]).length2() * (vertices_[1] - vertices_[2]).length2());
    }

    bool operator==(const Shape& another) const override{
        const Rectangle* another_ptr = dynamic_cast<const Rectangle*> (&another);
        //TODO
    }

    bool isCongruentTo(const Shape& another) const override{

    }

    bool isSimilarTo(const Shape& another) const override{

    }
};

class Square final: public Rectangle{
public:
    using Rectangle::Rectangle;

    Square(const Point& p1, const Point& p2): vertices_(std::vector<Point>(4)){
        vertices_[0] = p1;
        vertices_[2] = p2;

        Point direction = p2 - p1;
        Point perp_direction = direction.perpendicular();

        vertices_[1] = center() + perp_direction / 2;
        vertices_[3] = center() - perp_direction / 2;
    }

    Circle circumscribedCircle() const{
        Point center = (vertices_[0] + vertices_[2]) /2;
        return Circle((center, (vertices_[0] - center).length()));
    }
    Circle inscribedCircle() const{
        Point center = (vertices_[0] + vertices_[2]) /2;
        return Circle(center, (vertices_[0] - vertices_[1]).length() / 2);
    }

    double perimeter() const override{
        return 4 * (vertices_[0] - vertices_[1]).length();
    }

    double area() const override{
        return (vertices_[0] - vertices_[1]).length2();
    }

    bool operator==(const Shape& another) const override{

    }

    bool isCongruentTo(const Shape& another) const override{

    }

    bool isSimilarTo(const Shape& another) const override{

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

        Point mid = Line(mid1, mid1 + perp1).intersection(mid2, mid2 + perp2);
        return Circle(mid, (mid - vertices_[0]).length());
    }

    Circle inscribedCircle() const{
        return Circle(orthocenter(), 2 * area() / perimeter());
    }

    Point centroid() const{
        Point mid1 = (vertices_[0] + vertices_[1]) / 2;
        Point mid2 = (vertices_[1] + vertices_[2]) / 2;
        return Line(mid1, vertices_[2]).intersection(Line(mid2, vertices_[0]));
    }

    Point orthocenter() const{
        Point bc = vertices_[2] - vertices_[1];
        Point perp_bc = bc.perpendicular();
        Line altitude_a(vertices_[0], vertices_[0] + perp_bc);

        Point ac = vertices_[2] - vertices_[0];
        Point perp_ac = ac.perpendicular();
        Line altitude_b(vertices_[1], vertices_[1] + perp_ac);

        return altitude_a.intersection(altitude_b);
    }

    Line EulerLine() const{
        return Line(ninePointsCircle().center(), centroid());
    }

    Circle ninePointsCircle() const{
        Circle circle = circumscribedCircle();
        Point mid = (orthocenter() + circle.center()) / 2.0;
        return Circle(mid, circle.radius() / 2.0);
    }


    double area() const override{
        double a = (vertices_[0] - vertices_[1]).length();
        double b = (vertices_[1] - vertices_[2]).length();
        double c = (vertices_[2] - vertices_[0]).length();
        double p = 0.5 * (a + b + c);
        return std::sqrt(p * (p - a) * (p - b) * (p - c));
    }

    bool operator==(const Shape& another) const override{

    }

    bool isCongruentTo(const Shape& another) const override{

    }

    bool isSimilarTo(const Shape& another) const override{

    }
};