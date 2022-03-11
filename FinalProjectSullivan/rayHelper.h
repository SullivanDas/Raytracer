#pragma once
#ifndef RAYHELPER_H
#define RAYHELPER_H

#include "vec3.h"
#include <vector>
#include "Light.h"
#include "parseFile.h"


int maxDepth = 0;
std::vector<pointLight> lights;


sphere inside = sphere(vec3(0, 0, 0), 1.73);
Color ambientLight = Color(0.1, 0.1, 0.1);

float max(float a, float b) {
	return a > b ? a : b;
}

bool isWithinBounds(vec3 pos, vec3 u, vec3 l){
	return pos.x > u.x && pos.x <= l.x && pos.y > u.y && pos.y <= l.y;
}

std::vector<triangle> getTrianglesInBounds(vec3 u, vec3 l, std::vector<triangle> t) {
	std::vector<triangle> hitTriangles;
	for (int i = 0; i < t.size(); i++) {
		for (int j = 0; j < 3; j++) {
			if (isWithinBounds(t[i].vertices[j].pos, u, l)){
				hitTriangles.push_back(t[i]);
			}
		}
	}
	return hitTriangles;
}

struct node {
	node* left;
	node* right;
	vec3 ulBound;
	vec3 lrBound;
	std::vector<triangle> tris;

	node() {}
	node(vec3 u, vec3 l, std::vector<triangle> t) {
		ulBound = u;
		lrBound = l;
		tris = t;
	}
};


bool isLeftBound(vec3 newUpperBound, vec3 upperBound, vec3 newLowerBound, vec3 lowerBound) {
	return newUpperBound.x <= upperBound.x && newUpperBound.y <= upperBound.y && newLowerBound.x <= lowerBound.x && newLowerBound.y <= lowerBound.y;
}
node* newNode(vec3 upper, vec3 lower, std::vector<triangle> t) {
	node* temp = new node(upper,lower, t);
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}

node* insert(node* root, vec3 upper, vec3 lower, std::vector<triangle> t) {
		
	if (root == NULL) return newNode(upper,lower, t);

	if (isLeftBound(upper, root->ulBound, lower, root->lrBound)) {
		root->left = insert(root->left, upper, lower, t);
	}
	else {
		root->right = insert(root->right, upper, lower, t);
	}
	return root;
}

void deleteTree(node* root) {
	if (root == NULL) { return; }
	deleteTree(root->left);
	deleteTree(root->right);
	free(root);
}




vec3 constructRay(vec3 eye, int i, int j, float w, float h, float d) {

	float u = (w / 2 - i);
	float v = (h / 2 - j);

	vec3 p = eye - d * forward + u * right + v * up;

	return (p - eye).normalized();

}
Color ApplyLighting(vec3 ray, vec3 currentOrigin, raycastHit hit, int currentDepth);

Color TracePath(vec3 ray, vec3 currentOrigin, int currentDepth) {

	
	raycastHit hitInfo;
	raycastHit minHit;
	float minHitDistance = INFINITY;
	if (currentDepth > maxDepth) {
		return Color(0, 0, 0);
	}

	bool h = false;
	for (int m = 0; m < models.size(); m++) {

		bool newHit = models[m].isRayIntersecting(currentOrigin, ray, hitInfo);
		
		if (newHit)
		{
			if (m = 1) {

			}
			h = true;
		}

		if (newHit && (hitInfo.hitPoint - currentOrigin).length() < minHitDistance) {
			minHitDistance = (hitInfo.hitPoint - currentOrigin).length();
			minHit = hitInfo;
		}
	}

	if (h) {
		return ApplyLighting(ray, currentOrigin, minHit, currentDepth);
	}
	else {
		return Color(0, 0, 0);
	}

}

Color diffuseContribution(vec3 shadow, raycastHit hit) {

	vec3 sn = shadow.normalized();
	float lambCalc = max(0, dot(hit.normal, shadow.normalized()));
	if (lambCalc == 0) {
		int i = 1;
	}
	float r = lambCalc * hit.hitColor.r;
	float g = lambCalc * hit.hitColor.g;
	float b = lambCalc * hit.hitColor.b;

	return Color(r, g, b);
}

vec3 bisector(vec3 a, vec3 b) {
	return dot(b, b) * a + dot(a, a) * b;
}

Color specularContribution(vec3 camDir, vec3 light, raycastHit hit, pointLight pL) {
	vec3 h = bisector(camDir, light);
	float specCalc = pow(max(0, dot(hit.normal, h.normalized())), 0.5);

	float r = specCalc * hit.hitObject.mat.ambient->getPixel(0,0).r;
	float g = specCalc * hit.hitObject.mat.ambient->getPixel(0, 0).g;
	float b = specCalc * hit.hitObject.mat.ambient->getPixel(0, 0).b;

	return Color(r, g, b);
}

Color ambientContribution(raycastHit hit) {
	float r = ambientLight.r * hit.hitColor.r;
	float g = ambientLight.g * hit.hitColor.g;
	float b = ambientLight.b * hit.hitColor.b;

	return Color(r, g, b);
}


Color ApplyLighting(vec3 ray, vec3 currentOrigin, raycastHit hit, int currentDepth) {
	Color color = Color(0, 0, 0);
	vec3 hitPoint;
	
	hitPoint = hit.hitPoint;

	vec3 toEye = -1 * ray;
	vec3 reflection = (2 * (dot(toEye, hit.normal) * hit.normal) - toEye).normalized();


	for (int l = 0; l < lights.size(); l++) {
		vec3 shadowRay = lights[l].getShadowRay(hitPoint);
		bool isInShadow = false;
		for (int k = 0; k < models.size(); k++) {

			raycastHit shadowHit;
			if (models[k].isRayIntersecting(hitPoint, shadowRay, shadowHit) ) {
				//isInShadow = true;
			}
		}
		if (!isInShadow) {
			color = color + diffuseContribution(shadowRay, hit);

			color = color + specularContribution(toEye, shadowRay, hit, lights[l]);
		}
	}
	


	Color newColor = TracePath(reflection, hitPoint, currentDepth + 1);
	color = color + Color(newColor.r * hit.specularColor.r, newColor.g * hit.specularColor.g, newColor.b * hit.specularColor.b);
	color = color + ambientContribution(hit);
	return color;
}

#endif