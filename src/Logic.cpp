#include "../include/Logic.h"
#include "../include/Material.h"
#include "algorithm"
#include "../include/Texture.h"
int MAX_DEPTH = 5;
bool USE_RUSSIAN_ROULETTE = true;
void validate_scene(const std::vector<Object*>& objs) {
    for (size_t i = 0; i < objs.size(); i++) {
        if (objs[i] == nullptr) {
            std::cerr << "Error: Object at index " << i << " is nullptr!" << std::endl;
            exit(1);
        }
        if (objs[i]->mat == nullptr) {
            std::cerr << "Error: Object at index " << i << " has nullptr material!" << std::endl;
            exit(1);
        }
    }
    std::cout << "Scene validation passed: all objects have valid materials." << std::endl;
}
ImageTexture::ImageTexture(const char* filename){
    std::ifstream file(filename,std::ios::binary);
    if(!file){
        std::cerr<<"Error: Cannot open image: "<<filename<<std::endl;
        data=NULL;
        width=0;
        height=0;
        return ;
    }
    //checheout whether it's PPM(P6)
    std::string header;
    file>>header;
    if(header !="P6"){
        std::cerr<<"Error: Not a PPM file!"<<std::endl;
        data =NULL;
        width=0;
        height=0;
        return ;
    }
    //get information
    file>>width>>height;
    int max_val;
    file>>max_val;
    file.ignore();
    //distribute the RAM
    data =new unsigned char[3*width*height];
    file.read((char*)data,3*width*height);
}
Vec3 ImageTexture::value(double u,double v,const Vec3 &p)const{
	if(data==NULL) return Vec3(0,0,0);
	u=u-floor(u);
	v=v-floor(v);

	int i=static_cast<int>(u*width);
    int j=static_cast<int>((1-v)*height-0.001);

    if(i<0) i=0;
    if(i>=width) i=width-1;
    if(j<0) j=0;
    if(j>=height) j=height-1;

    const unsigned char *pixel=data+3*(j*width+i);
    double r=static_cast<double>(pixel[0])/255.0;
    double g=static_cast<double>(pixel[1])/255.0;
    double b=static_cast<double>(pixel[2])/255.0;
    return Vec3(r,g,b);

}
Vec3 random_vector(){
    double r1=(double)std::rand()/RAND_MAX;
    double r2=(double)std::rand()/RAND_MAX;
    double theta=2.0*std::acos(-1.0)*r1;
    double phi =std::acos(2.0*r2-1.0);
    return Vec3(std::sin(phi)*std::cos(theta),std::sin(phi)*std::cos(theta),std::cos(phi));
}
bool MetalMaterial::scatter(
    const Ray &rayIn,
    const Vec3 &normal,
    const Vec3 hitpoint,
    double u,
    double v,
    Ray &scatter,
    Vec3 &attenuation,
    Object* obj
) const {
	Vec3 reflect_dir=(rayIn.direction-normal*2.0*(rayIn.direction.dot(normal))).normalize();
	if(roughness>0){
		Vec3 random_offset=random_vector()*roughness;
        reflect_dir=(reflect_dir+random_offset).normalize();	
	}
	if(reflect_dir.dot(normal)<=0){
		return false;
	}
	scatter=Ray(hitpoint+normal*0.001,reflect_dir);
	
	attenuation=texture->value(u,v,hitpoint);
	return true;
}
struct ObjectComparator {
    int axis;
    ObjectComparator(int a) : axis(a) {}
    bool operator()(Object* a, Object* b) const {
        return a->getbounding().min_bound[axis] < b->getbounding().min_bound[axis];
    }
};
bool LambertianMaterial::scatter(
    const Ray &rayIn,
    const Vec3 &normal,
    const Vec3 hitpoint,
    double u,
    double v,
    Ray &scatter,
    Vec3 &attenuation,
    Object* obj
) const {
	Vec3 random_dir=random_hemisphere_direction(normal).normalize();
	scatter=Ray(hitpoint+random_dir*0.001,random_dir);
	
	attenuation=texture->value(u,v,hitpoint)*albedo;
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
bool DielectricMaterial::scatter (const Ray &rayIn,const Vec3 &normal,const Vec3 hitpoint,double u,double v,Ray &scatter,Vec3 &attenuation,Object* obj) const{
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

bool bvh_intersect(BVHNode *node,double t_min,double t_max,const Ray &ray,hit_record& rec){
	if(!node) return false;
	if(!node->box.intersect(ray)) return false;
	if(node->obj){
		return node->obj->is_hit(ray,t_min,t_max,rec);
	}

	bool hit_left = false, hit_right = false;
	hit_record left_rec, right_rec;
	
	if(node->left) hit_left = bvh_intersect(node->left, t_min, t_max, ray, left_rec);
	if(node->right) hit_right = bvh_intersect(node->right, t_min, t_max, ray, right_rec);
	
	if(hit_left && hit_right){
		rec = (left_rec.t < right_rec.t) ? left_rec : right_rec;
		return true;
	}
	if(hit_left){
		rec = left_rec;
		return true;
	}
	if(hit_right){
		rec = right_rec;
		return true;
	}
	return false;
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
	hit_record rec;
	
	

	if(bvh_intersect(bvh_root,0.000001,1e10,ray,rec)){
		Object *obj=rec.obj;
		Material* mat=rec.mat; 
		
		if(!obj || !mat){
			std::cerr << "Error: obj or mat is null!" << std::endl;
			return Vec3(0, 0, 0);
		}
		
		Vec3 p=rec.p;
		Vec3 normal=rec.normal;

		//ensure normal dir
		if(normal.dot(ray.direction)>0) normal=-normal;

		Ray scatter;
		Vec3 attenuation;
		if(!mat->scatter(ray,normal,p,rec.u,rec.v,scatter,attenuation,obj)){
			return Vec3(0, 0, 0);
		}
		if(depth>=MAX_DEPTH&&USE_RUSSIAN_ROULETTE){
			double p=std::max(attenuation.x,std::max(attenuation.y,attenuation.z));
			p=std::max(0.1,std::min(1.0,p));
			if((double)std::rand()/RAND_MAX>p){
				return Vec3(0.0,0.0,0.0);
			}
			return attenuation*compute_color(depth+1,scatter,bvh_root)/p;
		}
		return attenuation*compute_color(depth+1,scatter,bvh_root);

	} else {
		return Vec3(135 / 255.0, 206 / 255.0, 235 / 255.0);
	}
	
	return Vec3(0, 0, 0); 
}
