#pragma once
#include <cmath>
#include <algorithm>

struct Vec2 {
    float x=0,y=0;
    Vec2()=default;
    Vec2(float x,float y):x(x),y(y){}
    Vec2 operator+(const Vec2&rhs)const{return{x+rhs.x,y+rhs.y};}
    Vec2 operator-(const Vec2&rhs)const{return{x-rhs.x,y-rhs.y};}
    Vec2 operator*(float v)const{return{x*v,y*v};}
    Vec2 operator/(float v)const{return v!=0?Vec2{x/v,y/v}:Vec2{0,0};}
    void operator+=(const Vec2&rhs){x+=rhs.x;y+=rhs.y;}
    void operator-=(const Vec2&rhs){x-=rhs.x;y-=rhs.y;}
    void operator*=(float v){x*=v;y*=v;}
    void operator/=(float v){if(v!=0){x/=v;y/=v;}else{x=y=0;}}
    bool operator==(const Vec2&rhs)const{return x==rhs.x&&y==rhs.y;}
    bool operator!=(const Vec2&rhs)const{return!(*this==rhs);}
    float length()const{return std::sqrt(x*x+y*y);}
    float lengthSquared()const{return x*x+y*y;}
    float dist(const Vec2&rhs)const{return (*this-rhs).length();}
    float distSquared(const Vec2&rhs)const{return (*this-rhs).lengthSquared();}
    Vec2 normalize()const{float l=length();return l>0?(*this)/l:Vec2(0,0);}
    void normalizeInPlace(){float l=length();if(l>0){x/=l;y/=l;}else{x=y=0;}}
    Vec2 rotate(float r)const{float cs=std::cos(r),sn=std::sin(r);return{x*cs-y*sn,x*sn+y*cs};}
    void rotateInPlace(float r){float cs=std::cos(r),sn=std::sin(r);float nx=x*cs-y*sn,ny=x*sn+y*cs;x=nx;y=ny;}
    float dot(const Vec2&rhs)const{return x*rhs.x+y*rhs.y;}
    float cross(const Vec2&rhs)const{return x*rhs.y-y*rhs.x;}
    float angle()const{return std::atan2(y,x);}
    float angleDegrees()const{return angle()*180.0f/3.14159265f;}
    static Vec2 lerp(const Vec2&a,const Vec2&b,float t){return a+(b-a)*t;}
    static Vec2 clamp(const Vec2&v,float minX,float maxX,float minY,float maxY){return{std::fmax(minX,std::fmin(maxX,v.x)),std::fmax(minY,std::fmin(maxY,v.y))};}
    static Vec2 project(const Vec2&v,const Vec2&onto){float d=onto.lengthSquared();return d>0?onto*(v.dot(onto)/d):Vec2(0,0);}
    Vec2 perp()const{return{-y,x};}
    Vec2 reflect(const Vec2&n)const{return *this - n*(2.0f*dot(n));}
    float distToSegment(const Vec2&a,const Vec2&b)const{Vec2 ab=b-a;float t=std::clamp(((*this-a).dot(ab))/ab.lengthSquared(),0.0f,1.0f);return(*this-(a+ab*t)).length();}
    Vec2 rotateAround(const Vec2&p,float r)const{return p+(*this-p).rotate(r);}
};

