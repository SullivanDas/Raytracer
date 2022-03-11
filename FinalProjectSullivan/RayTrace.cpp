#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "image_lib.h"
#include "sceneObject.h"
#include "Light.h"
#include <string.h>
#include <vector>
#include "parseFile.h"
#include "rayHelper.h"
#include "vec3.h"




int main(int argc, char** argv) {
	//if (argc < 2) { return 0; }

	float d = imgHeight / 2 / tanf(halfAngleVFOV * (M_PI / 180.0f));

	float halfWidth = imgWidth / 2;
	float halfHeight = imgHeight / 2;

	float imgW = imgWidth;
	float imgH = imgHeight;


	std::string imgName = "dice.png";
	std::string texName = "CrateTexture.jpg";
	//std::string sceneFileName = argv[1];
	

	Image output = Image(imgWidth, imgHeight);
	
	
	Image black = Image(1, 1, Color(0, 0, 0));
	Image white = Image(1, 1, Color(1, 1, 1));
	//Image diffuseFile = Image("..\\CrateTexture.jpg");

	Image tex2 = Image("..\\Cali_Grass-2.jpg");

	//triangle t = triangle(vec3(0, 0, 2), vec3(0, 1, 2), vec3(1, 0, 2), material(&tex));
	sphere s[] = { sphere(vec3(-1, 1, -1), 0.01), sphere(vec3(1,1,1), 0.01), sphere(vec3(1,1,-1), 0.01) };
	sphere inside = sphere(vec3(-3, -4, 5), 1);
	triangle t = triangle(vertex(vec3(-1, 1, -1),vec3(0,0,0),vec3(0,0,0)), vertex(vec3(1, 1, 1), vec3(0, 0, 0), vec3(0, 0, 0)), vertex(vec3(1, 1, -1), vec3(0, 0, 0), vec3(0, 0, 0)), material(&tex));
	triangle floor = triangle(vertex(vec3(-10, -1, -10), vec3(0, 0, 0), vec3(0, 0, 0)), vertex(vec3(10, -1, 10), vec3(0, 0, 0), vec3(1, 1, 0)), vertex(vec3(-10, -1, 10), vec3(0, 0, 0), vec3(0, 1, 0)), material(&tex2));
	triangle floor2 = triangle(vertex(vec3(10, -1, -10), vec3(0, 0, 0), vec3(1, 0, 0)), vertex(vec3(10, -1, 10), vec3(0, 0, 0), vec3(1, 1, 0)), vertex(vec3(-10, -1, -10), vec3(0, 0, 0), vec3(0, 0, 0)), material(&tex2));
	std::vector<triangle> floorT;
	floorT.push_back(floor);
	floorT.push_back(floor2);
	
	//parseFile::parseOBJ("..\\CrateTest.obj");
	parseFile::parseOBJ("..\\Die.obj");
	//models.push_back(boundingSphere(vec3(-10, -1, -10), vec3(10, -1, 10), floorT));
	pointLight l = pointLight(vec3(3, -7, 3));
	lights.push_back(l);

	/*
	std::vector<triangle> empty;
	node *root = NULL;
	root = insert(root, vec3(0,0,0), vec3(imgWidth, imgHeight,0), empty);
	root = insert(root, vec3(0, 0, 0), vec3(imgWidth/2, imgHeight, 0), empty);
	root = insert(root, vec3(0, 0, 0), vec3(imgWidth / 2, imgHeight / 2, 0), getTrianglesInBounds(vec3(0, 0, 0), vec3(imgWidth / 2, imgHeight / 2, 0), triangles));
	root = insert(root, vec3(0, imgHeight / 2, 0), vec3(imgWidth / 2, imgHeight, 0), getTrianglesInBounds(vec3(0, imgHeight / 2, 0), vec3(imgWidth / 2, imgHeight, 0), triangles));
	root = insert(root, vec3(imgWidth / 2, 0, 0), vec3(imgWidth, imgHeight, 0), empty);
	root = insert(root, vec3(imgWidth / 2, 0, 0), vec3(imgWidth, imgHeight/2, 0), getTrianglesInBounds(vec3(imgWidth / 2, 0, 0), vec3(imgWidth, imgHeight / 2, 0), triangles));
	root = insert(root, vec3(imgWidth / 2, imgHeight/2, 0), vec3(imgWidth, imgHeight, 0), getTrianglesInBounds(vec3(imgWidth / 2, imgHeight / 2, 0), vec3(imgWidth, imgHeight, 0), triangles));
	
	*/


	for (int i = 0; i < imgWidth; i++) {
		for (int j = 0; j < imgHeight; j++) {
			vec3 rayDir = constructRay(eye, i, j, imgW, imgH, d).normalized();
			raycastHit hit;
			Color c = Color(0, 0, 0);

			for (int k = 0; k < models.size(); k++) {
				c = TracePath(rayDir, eye, 0);
				output.setPixel(i, j, c);
			}
			/*

			}
			*/
			/*

						if (inside.isRayIntersecting(eye, rayDir, hit)) {
				c = Color(1, 1, 1);
			}
						if (inside.isRayIntersecting(eye, rayDir, hit)) {
				c = Color(1, 1, 1);
			}


			if (inside.isRayIntersecting(eye, rayDir, hit)) {
				c = Color(1, 1, 0);
			}

			*/


			
			//Color color = EvaluateRayTree(rayDir, eye, 0);
			//output.setPixel(i, j, color);
			output.setPixel(i, j, c);
		}
	}
	/*
		for (int i = 0; i < triangles.size(); i++) {
		printf("Triangle %d\n\n", i);
		for (int j = 0; j < 3; j++) {
			printf("V %f,%f,%f, T %f,%f, N %f, %f\n", triangles[i].vertices[j].pos.x, triangles[i].vertices[j].pos.y, triangles[i].vertices[j].pos.z, triangles[i].vertices[j].uvCoord.x, triangles[i].vertices[j].uvCoord.y, triangles[i].vertices[j].normal.x, triangles[i].vertices[j].normal.y, triangles[i].vertices[j].normal.z);
		}
	}
	*/

	printf("Name: %s\n", imgName.c_str());
	output.write(imgName.c_str());
	
	return 0;
}

