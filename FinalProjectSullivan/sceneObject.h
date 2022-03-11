#pragma once
#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H
#include <vector>
#include <math.h>
#include "vec3.h"

struct raycastHit; 

Image black = Image(1, 1, Color(0, 0, 0));
Image white = Image(1, 1, Color(1, 1, 1));
Image blue = Image(1, 1, Color(0.5, 0.5, 1));

float lerp(float a, float b, float f)
{
	return (a * (1.0 - f)) + (b * f);
}

struct material {
	Image *ambient, *diffuse, *specular, *transmissive, *normal;
	float phongPower;

	/*
	
	*/
	material() : ambient(&black), diffuse(&white), specular(&black), transmissive(&black), normal(&blue), phongPower(0) {}
	
	//material(const char* ai, const char* di, const char* si, const char* ti, float p) : ambient(Image(ai)), diffuse(Image(di)), specular(Image(si)), transmissive(Image(ti)), phongPower(p) {}

	material(Image *di) : ambient(&black), diffuse(di), specular(&white), transmissive(&black), normal(&blue), phongPower(0) {
		
	}

	material(Image *di, Image *ni) : ambient(&black), diffuse(di), specular(&white), transmissive(&black), normal(ni), phongPower(0) {

	}

};

struct sceneObject {
	material mat;
	virtual bool isRayIntersecting(vec3 origin, vec3 ray, raycastHit& hit) { return true; }
	sceneObject() : mat(material()) {}
	
};



struct raycastHit {
	vec3 normal;
	vec3 hitPoint;
	Color hitColor;
	Color specularColor;
	sceneObject hitObject;
	bool hit;
	raycastHit() : normal(vec3(0, 0, 0)), hitPoint(vec3(0, 0, 0)), hitObject(sceneObject()), hitColor(Color()), specularColor(Color()), hit(false) {}
	raycastHit(vec3 n, vec3 h, sceneObject o, bool hit) : normal(n), hitPoint(h), hitObject(o), hit(hit), specularColor(Color()) {}
};

struct vertex {
	vec3 pos;
	vec3 normal;
	vec3 uvCoord;

	vertex() : pos(vec3(0,0,0)), normal(vec3(0,0,0)), uvCoord(vec3(0,0,0)) {}
	vertex(vec3 p, vec3 n, vec3 u) : pos(p), normal(n), uvCoord(u) {}
};


struct triangle : sceneObject{ 
	vertex vertices[3];

	triangle(vertex a, vertex b, vertex c) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
	}

	triangle(vertex a, vertex b, vertex c, material m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		mat = m;
	}
	bool isRayIntersecting(vec3 origin, vec3 ray, raycastHit& hit) override {
		vec3 BA = vertices[1].pos - vertices[0].pos;
		vec3 CB = vertices[2].pos - vertices[1].pos;
		vec3 AC = vertices[0].pos - vertices[2].pos;

		vec3 n = cross(CB, BA).normalized();

		
		float denom = dot(n, ray);
		if (denom < 0.000005f && denom >= 0) {
			return false;
		}
		
		float num = dot(vertices[1].pos - origin, n);
		float t = num / denom;
		vec3 point = ray * t + origin;

		/*
				float u = dot(n, cross(BA, point - vertices[0].pos));
		float v = dot(n, cross(CB, point - vertices[1].pos));
		float w = dot(n, cross(AC, point - vertices[2].pos));
		*/
		float triangleArea = (cross(BA, AC)).length() / 2;
		
		float u = (cross(AC, point - vertices[0].pos)).length() / 2 / triangleArea;
		float v = (cross(CB, point - vertices[1].pos)).length() / 2 / triangleArea;
		float w = (cross(BA, point - vertices[0].pos)).length() / 2 / triangleArea;
		
		hit.normal = n;
		hit.hitPoint = point;
		hit.hit = u * v > 0 && u > 0 && v > 0 && u + v < 1 && v + w < 1 && u + w < 1;
		if (hit.hit) {
			/*
			float adjustedU = lerp(vertices[0].uvCoord.x,vertices[2].uvCoord.x, u);
			float adjustedV = lerp(vertices[1].uvCoord.y, vertices[2].uvCoord.y, v);
			float adjustedW = lerp(vertices[0].uvCoord.y, vertices[1].uvCoord.y, w);
			*/
			vec3 uv = vertices[0].uvCoord * v + vertices[1].uvCoord * u + vertices[2].uvCoord * w;

			//printf("U:%f V: %f W: %f \n", u, v, w);
			hit.hitColor = mat.diffuse->getPixel(clamp(int(abs(uv.x) * mat.diffuse->width),0,mat.diffuse->width-1), clamp(int(abs(uv.y) * mat.diffuse->height),0,mat.diffuse->height - 1));
			Color normalColor = mat.normal->getPixel(clamp(int(abs(uv.x) * mat.normal->width), 0, mat.normal->width - 1), clamp(int(abs(uv.y) * mat.normal->height), 0, mat.normal->height - 1));
			vec3 convertedNormal = vec3(normalColor.r / 0.5 - 1, normalColor.g / 0.5 - 1, normalColor.b);
			hit.normal = n * convertedNormal.z + ray * convertedNormal.x + cross(ray, n) * convertedNormal.y;

		}
		

		//return u * v > 0 && u > 0 && v > 0 && u + v < 1 && v + w < 1 && u + w < 1;
		return u * v > 0 && u > 0 && v > 0 && u + v < 1 && v + w < 1 && u + w < 1;
	}
};
/*
struct boundingBox : sceneObject {
	vec3 min;
	vec3 max;
	std::vector<triangle> tris;

	boundingBox(vec3 mi, vec3 ma, std::vector<triangle> t)
	{
		min = mi;
		max = ma;
		tris = t;
	}

	bool isRayIntersecting(vec3 origin, vec3 ray, raycastHit& hitInfo) override {
		bool hit = false;
		float tminX = (min.x - origin.x) / ray.x;
		float tmaxX = (max.x - origin.x) / ray.x;

		float tminY = (min.y - origin.y) / ray.y;
		float tmaxY = (max.y - origin.y) / ray.y;

		if ((tminX > tmaxY || tminY > tmaxX)) { return false; }

		float tminZ = (min.z - origin.z) / ray.z;
		float tmaxZ = (max.z - origin.z) / ray.z;

		if ((tminX > tmaxZ) || (tminZ > tmaxX)) { return false; }
		return true;
		float minHitDistance = INFINITY;
		raycastHit minHit = raycastHit();

		for (int k = 0; k < tris.size(); k++) {
			bool newHit = tris[k].isRayIntersecting(origin, ray, hitInfo);
			if (newHit)
			{
				hit  = true;
			}

			hitInfo.hitObject = tris[k];
			if (newHit && (hitInfo.hitPoint - origin).length() < minHitDistance) {
				minHitDistance = (hitInfo.hitPoint - origin).length();
				minHit = hitInfo;
			}
		}

		hitInfo = minHit;
		return hit;
	}
};
*/

struct boundingSphere : sceneObject{
	vec3 center;
	float radius;

	std::vector<triangle> tris;


	boundingSphere() {
		center = vec3(0, 0, 0);
		radius = 1;
	}

	boundingSphere(vec3 min, vec3 max, std::vector<triangle> t) {
		center = (min + max) / 2;
		radius = (max - center).length();
		tris = t;
	}

	bool isRayIntersecting(vec3 origin, vec3 ray, raycastHit& hit) override {
		float a = dot(ray, ray);
		vec3 toStart = (origin - center);
		float b = 2 * dot(ray, toStart);
		float c = dot(toStart, toStart) - radius * radius;
		float discr = b * b - 4 * a*c;
		float minHitDistance = INFINITY;

		//solve quadratic
		if (discr < 0) {
			//imaginary number
			return false;
		}
		else {

			float t0 = (-b + sqrt(discr)) / (2 * a);
			float t1 = (-b - sqrt(discr)) / (2 * a);

			if (t0 < 0 || t1 < 0) {

				bool didHit = false;
				
				raycastHit minHit = raycastHit();

				for (int k = 0; k < tris.size(); k++) {
					bool newHit = tris[k].isRayIntersecting(origin, ray, hit);
					if (newHit)
					{
						didHit = true;
					}

					hit.hitObject = tris[k];
					if (newHit && (hit.hitPoint - origin).length() < minHitDistance) {
						minHitDistance = (hit.hitPoint - origin).length();
						minHit = hit;
					}
				}
				
				hit = minHit;
				return didHit;

			}

		}
		return false;
	}


};

struct sphere : sceneObject{
	vec3 center;
	float radius;
	
	sphere(vec3 c, float r) : center(c), radius(r) {}

	bool isRayIntersecting(vec3 origin, vec3 ray, raycastHit& hit) override{
		float a = dot(ray, ray);
		vec3 toStart = (origin - center);
		float b = 2 * dot(ray, toStart);
		float c = dot(toStart, toStart) - radius * radius;
		float discr = b * b - 4 * a*c;

		//solve quadratic
		if (discr < 0) {
			//imaginary number
			return false;
		}
		else {

			float t0 = (-b + sqrt(discr)) / (2 * a);
			float t1 = (-b - sqrt(discr)) / (2 * a);

			if (t0 < 0 || t1 < 0) {
				if (t1 < 0) {
					hit.hitPoint = ray * t0 + origin;
				}
				else if (t0 < 0) {
					hit.hitPoint = ray * t1 + origin;
				}
				else {
					hit.hitPoint = (ray * t0 + origin).length() < (ray * t1 + origin).length() ? (ray * t0 + origin) : (ray * t1 + origin);
				}
				hit.normal = (hit.hitPoint - center).normalized();
				return true;
			}

		}
		return false;
	}
	
	
};



#endif