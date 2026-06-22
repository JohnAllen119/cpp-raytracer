#ifndef TEXTURE_H
#define TEXTURE_H
#include "Vec3.h"
class Texture{
public:
    virtual Vec3 value(double u,double v,const Vec3 &p) const=0;
    virtual ~Texture(){}
};
class SolidColorTexture: public Texture{
public:
    Vec3 color;
    SolidColorTexture(const Vec3& c):color(c){}
    Vec3 value(double u,double v,const Vec3 &p)const override{
        return color;
    }
};

class  CheckerTexture: public Texture{
public:
    Vec3 color1;
    Vec3 color2;
    double scale;

    CheckerTexture(const Vec3 &c1,const Vec3 &c2,double s=1.0):color1(c1),color2(c2),scale(s){}
    Vec3 value(double u,double v,const Vec3 &p)const override{
        double sines = sin(scale * u) * sin(scale * v);
        if(sines > 0){
            return color1;
        }else{
            return color2;
        }
    }
};
class ImageTexture: public Texture{
public:
    unsigned char* data;
    int width;
    int height;
    ImageTexture():data(NULL), width(0),height(0){}
    ImageTexture(const char* filename);
    ~ImageTexture(){delete[]data;}
    Vec3 value(double u,double v,const Vec3 &p)const override;
};
#endif