#pragma once
class Vec2{
    public:
    double x,y;
    Vec2(double a,double b):x(a),y(b){}
    Vec2 operator+(const Vec2 &b)const{
        return Vec2(b.x+x,b.y+y);
    }
};