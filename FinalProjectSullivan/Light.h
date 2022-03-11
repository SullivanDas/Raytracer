#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"
#include <math.h>

struct light {
	float intensity;
	virtual float getIntensity(float distance) { return intensity; }
	virtual vec3 getShadowRay(vec3 hitPoint) { return vec3(0, 0, 0); }
};

struct pointLight : light{
	vec3 pos;
	float getIntensity(float distance) override {
		return intensity * (1 / pow(distance, 2));
	}
	vec3 getShadowRay(vec3 hitPoint) override {
		return hitPoint-pos;
	}
	pointLight(vec3 p) {
		pos = p;
	}
};

struct directionalLight : light {
	vec3 dir;
	vec3 getShadowRay(vec3 hitPoint) override {
		return dir * -1;
	}
};

struct spotLight : light {

};

#endif
