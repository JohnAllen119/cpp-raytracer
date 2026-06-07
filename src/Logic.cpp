#include "../include/Logic.h"

struct ObjectComparator {
    int axis;
    ObjectComparator(int a) : axis(a) {}
    bool operator()(Object* a, Object* b) const {
        return a->getbounding().min[axis] < b->getbounding().min[axis];
    }
};

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

Vec3 compute_refract_ray(double n1,double n2,const Vec3 &incident_dir,const Vec3 &normal){
	double cos1=-incident_dir.dot(normal);
	double sin1=std::sqrt(1-cos1*cos1);
	double sin2=n1/n2*sin1;
	double cos2=std::sqrt(1-sin2*sin2);
	if(sin2>1.0){
		return Vec3(0,0,0);
	}
	Vec3 T= incident_dir * (n1/n2) + normal * ((n1/n2) * cos1 - cos2);
    return T;
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

double ao_compute(const Vec3 &p,const Vec3 &normal,BVHNode *bvh_root,int samples,double max_dist){
	double occlusion=0.0;
	for(int i=0;i<samples;i++){
		Vec3 dir=random_hemisphere_direction(normal);
		Ray ao_ray(p+dir*0.001,dir);
		double t=bvh_intersect(bvh_root,ao_ray);
		if(t>0&&t<max_dist){
			occlusion+=1.0;
		}
	}
	return occlusion/samples;
}

Vec3 compute_color(int depth,const Ray &ray,std::vector<Object*> &object,std::vector<Light> &lights,BVHNode *bvh_root){
	bvh_hit_object=NULL;
	double obj_t=bvh_intersect(bvh_root,ray);
	Object *hitObj=bvh_hit_object;
	Vec3 ambient_color=Vec3(0.15,0.15,0.15);
	Vec3 color=ambient_color;
	
	if(obj_t > 0){
		Object *sphere=hitObj;
		Material* mat=sphere->mat; 
		Vec3 p=ray.at(obj_t);
		Vec3 normal=sphere->getnormal(p); 

		double ao=ao_compute(p,normal,bvh_root,8,2.0);
		double ao_factor=1.0-ao*0.8;
		color = color * ao_factor;

		for(int l=0;l<lights.size();l++){
			Light light=lights[l];
			Vec3 light_dir=(light.direction-p).normalize();

			double diffuse=std::max(0.0,normal.dot(light_dir));
			double dot=normal.dot(light_dir);
			
			Vec3 spe_dir=((normal*dot*2)-light_dir).normalize();
			Vec3 view_dir=(ray.origin-p).normalize();
			double specular = std::pow(std::max(0.0, spe_dir.dot(view_dir)),mat->specular);
			Vec3 specular_color=light.color*specular*0.5;
			Vec3 diffuse_color=mat->color*mat->albedo*diffuse*light.intensity;
			
			Ray shadow_ray(p+normal*0.01,light_dir);
			bool inshadow=false;
			for(int k=0;k<object.size();k++){
				if(object[k]==hitObj){
					continue;
				}
				double shadow_t=object[k]->is_hit(shadow_ray);
				if(shadow_t > 0){
					inshadow=true;
					break;
				}
			} 
			
			if(!inshadow){
				color = color + diffuse_color + specular_color;
			} else {
				color = color + sphere->mat->color * 0.1;
			}
		}

		if(depth<5&&mat->reflectivity>0){
			Vec3 reflect_dir = ray.direction - normal * (2.0 * normal.dot(ray.direction));
			Ray reflect_ray(Vec3(p+normal*0.001),reflect_dir);
			Vec3 reflect_color=compute_color(depth+1,reflect_ray,object,lights,bvh_root);
			color = color * (1 - mat->reflectivity) + reflect_color * mat->reflectivity;				
		}
		
		if(depth<5&&mat->transparency){
			Vec3 incident_dir=ray.direction.normalize();
			bool is_enter=incident_dir.dot(normal)<0;
			double n1=is_enter?1.0:mat->refractivity;
			double n2=is_enter?mat->refractivity:1.0;
			Vec3 N=is_enter? normal :-normal;
			Vec3 refract_dir=compute_refract_ray(n1,n2,incident_dir,N);
			if(!(refract_dir.x==0&&refract_dir.y==0&&refract_dir.z==0)){
				Ray refract_ray(p-N*0.001,refract_dir);
				Vec3 refract_color=compute_color(depth+1,refract_ray,object,lights,bvh_root);
				color = color * (1 - mat->transparency) + refract_color * mat->transparency;
			}
		} 
	} else {
		return Vec3(0.0, 0.0, 1.2);
	}

	return color; 
}
