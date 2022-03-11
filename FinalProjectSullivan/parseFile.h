#pragma once
#ifndef PARSEFILE_H
#define PARSEFILE_H
#include "vec3.h"
#include <fstream>
#include <iostream>

int imgWidth = 800, imgHeight = 600;
float halfAngleVFOV = 35;
vec3 eye = vec3(2, -2, 6);
vec3 forward = vec3(0, 0, -1).normalized();
vec3 up = vec3(0, 1, 0).normalized();
vec3 right = cross(up,forward).normalized();
Color background = Color(0, 0, 0);

Image tex = Image("..\\de.jpg");
Image normTex = Image("..\\NormalMap.png");

std::vector<triangle> triangles;
std::vector<boundingSphere> models;

class parseFile {

public:
	static void parseOBJ(std::string fileName) {
		std::fstream fs;
		fs.open(fileName);

		if (!fs.is_open()) {
			printf("Could not open File");
			return;
		}

		std::string line;
		std::string lineStart;
		int vertCount = 0;
		int normCount = 0;
		int uvCount = 0;
		int triCount = 0;

		while (fs >> lineStart) {
			if (lineStart == "v") {
				vertCount += 1;
				std::getline(fs, line);
			}
			else if (lineStart == "vn") {
				normCount += 1;
				std::getline(fs, line);
			}
			else if (lineStart == "vt") {
				uvCount += 1;
				std::getline(fs, line);
			}
			else if (lineStart == "f") {
				triCount += 1;
				std::getline(fs, line);
			}
		}

		fs.clear();
		fs.seekg(0);
		vec3* verticies = new vec3[vertCount];
		vec3* normals = new vec3[normCount];
		vec3* uvs = new vec3[uvCount];

		int curVert = 0;
		int curNorm = 0;
		int curUV = 0;

		while (fs >> lineStart) {
			if (lineStart.at(0) == '#') {
				std::getline(fs, line);
			}
			else if (lineStart == "v") {
				float x, y, z;
				fs >> x;
				fs >> y;
				fs >> z;
				verticies[curVert] = vec3(x, y, z);
				curVert++;
			}
			else if (lineStart == "vt") {
				float u, v;
				fs >> u;
				fs >> v;
				uvs[curUV] = vec3(u, v, 0);
				curUV++;
			}
			else if (lineStart == "vn") {
				float x, y, z;
				fs >> x;
				fs >> y;
				fs >> z;
				normals[curNorm] = vec3(x, y, z);
				curNorm++;
			}
		}

		fs.clear();
		fs.seekg(0);
		float minX = INFINITY;
		float minY = INFINITY;
		float minZ = INFINITY;

		float maxX = -INFINITY;
		float maxY = -INFINITY;
		float maxZ = -INFINITY;

		while (fs >> lineStart) {
			if (lineStart == "f") {
				std::string a;
				std::string b;
				std::string c;

				int ap, an, at;
				int bp, bn, bt;
				int cp, cn, ct;

				fs >> a;
				fs >> b;
				fs >> c;

				ap = std::stoi(a.substr(0, a.find("/")));
				a.erase(0, a.find("/") + 1);
				at = std::stoi(a.substr(0, a.find("/")));
				a.erase(0, a.find("/") + 1);
				an = std::stoi(a.substr(0, a.find("/")));
				a.erase(0, a.find("/") + 1);

				bp = std::stoi(b.substr(0, b.find("/")));
				b.erase(0, b.find("/") + 1);
				bt = std::stoi(b.substr(0, b.find("/")));
				b.erase(0, b.find("/") + 1);
				bn = std::stoi(b.substr(0, b.find("/")));
				b.erase(0, b.find("/") + 1);

				cp = std::stoi(c.substr(0, c.find("/")));
				c.erase(0, c.find("/") + 1);
				ct = std::stoi(c.substr(0, c.find("/")));
				c.erase(0, c.find("/") + 1);
				cn = std::stoi(c.substr(0, c.find("/")));
				c.erase(0, c.find("/") + 1);

				vertex va = vertex(verticies[ap-1], normals[an - 1], uvs[at - 1]);
				vertex vb = vertex(verticies[bp - 1], normals[bn - 1], uvs[bt - 1]);
				vertex vc = vertex(verticies[cp - 1], normals[cn - 1], uvs[ct - 1]);

				if (va.pos.x < minX) { minX = va.pos.x; }
				if (va.pos.y < minY) { minY = va.pos.y; }
				if (va.pos.z < minZ) { minZ = va.pos.z; }

				if (vb.pos.x < minX) { minX = vb.pos.x; }
				if (vb.pos.y < minY) { minY = vb.pos.y; }
				if (vb.pos.z < minZ) { minZ = vb.pos.z; }

				if (vc.pos.x < minX) { minX = vc.pos.x; }
				if (vc.pos.y < minY) { minY = vc.pos.y; }
				if (vc.pos.z < minZ) { minZ = vc.pos.z; }



				if (va.pos.x > maxX) { maxX = va.pos.x; }
				if (va.pos.y > maxY) { maxY = va.pos.y; }
				if (va.pos.z > maxZ) { maxZ = va.pos.z; }

				if (vb.pos.x > maxX) { maxX = vb.pos.x; }
				if (vb.pos.y > maxY) { maxY = vb.pos.y; }
				if (vb.pos.z > maxZ) { maxZ = vb.pos.z; }

				if (vc.pos.x > maxX) { maxX = vc.pos.x; }
				if (vc.pos.y > maxY) { maxY = vc.pos.y; }
				if (vc.pos.z > maxZ) { maxZ = vc.pos.z; }

				triangle t = triangle(va,vb,vc, material(&tex, &normTex));
				triangles.push_back(t);

				
			}
		}
		
		boundingSphere b = boundingSphere(vec3(minX,minY,minZ), vec3(maxX,maxY,maxZ), triangles);
		models.push_back(b);
		fs.close();
		free(verticies);
		free(normals);
		free(uvs);
	}



};



#endif
