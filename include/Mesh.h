#pragma once
#include <vector>
#include "Triangle.h"
#include "Object.h"
#include <algorithm>
#include <limits>
#include "OBJLoader.h"

class Mesh:public Object{
private:
    std::vector<Triangle> triangles;
public:
    Mesh()=default;
    Mesh(const OBJData& data);
    Mesh(const OBJData& data, Material* m);
    Mesh(const std::vector<Triangle> &triangles,Material *m):triangles(triangles),Object(m){}
    bool is_hit(const Ray &ray,double t_min,double t_max,hit_record &rec)const override{
        double t=std::numeric_limits<double>::infinity();
        
        
        for(int i=0;i<triangles.size();i++){
            hit_record tmp_rec;
            if( !triangles[i].is_hit(ray,t_min,t_max,tmp_rec))continue;
            
            if(t>tmp_rec.t){
                t=tmp_rec.t;
                rec=tmp_rec;
            }
        }
        return t!=std::numeric_limits<double>::infinity();

    }
    Vec3 getnormal(const Vec3 &point) const override;
    virtual AABB getbounding() const override;
    virtual void getuv(double &u,double &v,const Vec3 &hitPoint)const override;
};