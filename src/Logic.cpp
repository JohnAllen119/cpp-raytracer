#include "../include/Logic.h"
#include "../include/Material.h"
#include "algorithm"
Vec3 random_vector(){
    double r1=(double)std::rand()/RAND_MAX;
    double r2=(double)std::rand()/RAND_MAX;
    double theta=2.0*std::acos(-1.0)*r1;
    double phi =std::acos(2.0*r2-1.0);
    return Vec3(std::sin(phi)*std::cos(theta),std::sin(phi)*std::cos(theta),std::cos(phi));
}
bool MetalMaterial::scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const{
	Vec3 reflect_dir=(rayIn.direction-normal*2.0*(rayIn.direction.dot(normal))).normalize();
	if(roughness>0){
		Vec3 random_offset=random_vector()*roughness;
        reflect_dir=(reflect_dir+random_offset).normalize();	
	}
	if(reflect_dir.dot(normal)<=0){
		return false;
	}
	scatter=Ray(hitpoint+normal*0.001,reflect_dir);
	attenuation=color;
	return true;
}
struct ObjectComparator {
    int axis;
    ObjectComparator(int a) : axis(a) {}
    bool operator()(Object* a, Object* b) const {
        return a->getbounding().min_bound[axis] < b->getbounding().min_bound[axis];
    }
};
bool LambertianMaterial:: scatter(const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation)const {
	Vec3 random_dir=random_hemisphere_direction(normal).normalize();
	scatter=Ray(hitpoint+random_dir*0.001,random_dir);
	attenuation=color*albedo;
	return true;
}
Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    
    double cos_theta = uv.dot(n);
    if (cos_theta > 1.0) cos_theta = 1.0;
    if (cos_theta < -1.0) cos_theta = -1.0;
  
    Vec3 r_in_perp = uv - n * cos_theta;
    Vec3 r_out_perp = r_in_perp * etai_over_etat;
    
    double r_out_perp_x_sq = r_out_perp.x * r_out_perp.x;
    double r_out_perp_y_sq = r_out_perp.y * r_out_perp.y;
    double r_out_perp_z_sq = r_out_perp.z * r_out_perp.z;
    double r_out_perp_len_sq = r_out_perp_x_sq + r_out_perp_y_sq + r_out_perp_z_sq;

    double sin_theta_out_sq = r_out_perp_len_sq;
    double cos_theta_out = std::sqrt(std::abs(1.0 - sin_theta_out_sq));

    Vec3 r_out_parallel = -n * cos_theta_out;
    
    return r_out_perp + r_out_parallel;
}

double reflectance(double cosine,double ref_idx){
    double r0=(1-ref_idx)/(1+ref_idx);
    r0=r0*r0;
    return r0+(1-r0)*pow((1-cosine),5);
}
bool DielectricMaterial::scatter (const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,Ray &scatter,Vec3 &attenuation) const{
	attenuation=Vec3(1.0,1.0,1.0);
	double etai_over_tat;
	Vec3 outward_normal;
	if(rayIn.direction.dot(normal)>0){
		outward_normal=-normal;
		etai_over_tat=ir;
	}else{
		outward_normal=normal;
		etai_over_tat=1.0/ir;
	}
	Vec3 unit_direction=rayIn.direction.normalize();
	double cos_theta= std::min(outward_normal.dot(-unit_direction),1.0);
	double sin_theta=std::sqrt(1.0-cos_theta*cos_theta);
	Vec3 scattered_dir;
	bool cannot_refract=etai_over_tat*sin_theta>1.0;
	if(cannot_refract||reflectance(cos_theta,etai_over_tat)>(double) std::rand()/RAND_MAX){
		scattered_dir=unit_direction-outward_normal*2*unit_direction.dot(outward_normal);
		scattered_dir=scattered_dir.normalize();
	}else{
		scattered_dir=refract(unit_direction,outward_normal,etai_over_tat);
	}
		scatter=Ray(hitpoint+outward_normal*0.001,scattered_dir);
		return true;
	}
BVHNode *build_bvh(std::vector<Object*>&objects,int start,int end,int depth){
	if(start==end){
		BVHNode *node=new BVHNode();
		node->obj=objects[start];
		node->box=objects[start]->getbounding();
		return node;
	} 
	if(start+1==end){
		BVHNode *node=new BVHNode();
		node->left=new BVHNode();
		node->left->obj=objects[start];
		node->left->box=objects[start]->getbounding();
		node->right=new BVHNode();
		node->right->obj=objects[end];
		node->right->box=objects[end]->getbounding();
		node->box=AABB::merge(node->left->box, node->right->box);
		return node;
	}

	int axis = depth % 3;
	std::sort(objects.begin() + start, objects.begin() + end + 1, ObjectComparator(axis));
	
	int mid = (start + end) / 2;
	BVHNode *node = new BVHNode();
	node->left = build_bvh(objects, start, mid, depth + 1);
	node->right = build_bvh(objects, mid + 1, end, depth + 1);
	node->box = AABB::merge(node->left->box, node->right->box);
	return node;
}


Object *bvh_hit_object=NULL;

double bvh_intersect(BVHNode *node,const Ray &ray){
	if(!node) return -1;
	if(!node->box.intersect(ray)) return -1;
	if(node->obj){
		double t=node->obj->is_hit(ray);
		if(t>0){
			bvh_hit_object=node->obj;
			return t;
		}
		return -1;
	}
	double t_left=-1;
	double t_right=-1;
	Object *obj_left=NULL;
	Object *obj_right=NULL;
	
	if(node->left){
		Object *prev_hit=bvh_hit_object;
		t_left=bvh_intersect(node->left,ray);
		obj_left=bvh_hit_object;
		bvh_hit_object=prev_hit;
	}
	if(node->right){
		Object *prev_hit=bvh_hit_object;
		t_right=bvh_intersect(node->right,ray);
		obj_right=bvh_hit_object;
		bvh_hit_object=prev_hit;
	}
	if(t_right>0&&t_left>0){
		if(t_left<t_right){
			bvh_hit_object=obj_left;
			return t_left;
		}else{
			bvh_hit_object=obj_right;
			return t_right;
		}
	}
	if(t_left>0){
		bvh_hit_object=obj_left;
		return t_left;
	}
	if(t_right>0){
		bvh_hit_object=obj_right;
		return t_right;
	}
	return -1;
}	

Vec3 random_hemisphere_direction(const Vec3 &normal) {
	double r1=(double)std::rand()/RAND_MAX;
	double r2=(double)std::rand()/RAND_MAX;
	double theta=std::acos(std::sqrt(1-r1));
	double phi=2.0*std::acos(-1.0)*r2;
	Vec3 local_dir(std::sin(theta)*std::cos(phi),std::sin(theta)*std::sin(phi),std::cos(theta));
	Vec3 up = std::fabs(normal.y) < 0.999 ? Vec3(0,1.0,0) : Vec3(1.0,0,0);
	Vec3 tangent = up.cross(normal).normalize();
	Vec3 biotangent = tangent.cross(normal).normalize();
	return tangent*local_dir.x + biotangent*local_dir.y + normal*local_dir.z;
}

Vec3 compute_color(int depth,const Ray &ray,BVHNode *bvh_root){
	if(depth>=5){
		return Vec3(0,0,0);
	}
	//bvh
	bvh_hit_object=NULL;
	double obj_t=bvh_intersect(bvh_root,ray);
	Object *hitObj=bvh_hit_object;

	
	if(obj_t > 0&&hitObj){
		Object *sphere=hitObj;
		Material* mat=sphere->mat; 
		Vec3 p=ray.at(obj_t);
		Vec3 normal=sphere->getnormal(p).normalize(); 

		//ensure normal dir
		if(normal.dot(ray.direction)>0) normal=-normal;

		Ray scatter;
		Vec3 attenuation;
		if(mat->scatter(ray,normal,p,scatter,attenuation)){
			return attenuation * compute_color(depth+1, scatter, bvh_root);
		}
		
		return Vec3(0, 0, 0);

	} else {
		return Vec3(135 / 255.0, 206 / 255.0, 235 / 255.0);
	}
	
	return Vec3(0, 0, 0); 
}
