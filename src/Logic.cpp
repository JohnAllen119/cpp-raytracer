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

		//produce random_dir
		Vec3 random_dir=random_hemisphere_direction(normal);
		//produce random scattering dir
		Ray scattering_ray(p+normal*0.001,random_dir);

		Vec3 next_color=compute_color(depth+1,scattering_ray,bvh_root);

		return mat->color * next_color * mat->albedo  ;

	} else {
		return  Vec3(135 / 255.0, 206 / 255.0, 235 / 255.0);
  }
	

	return {}; 
}
