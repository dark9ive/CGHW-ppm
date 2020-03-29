#include<iostream>
#include<fstream>
#include<math.h>
#include<vector>
#include<float.h>
//#include "vec3.h"

using namespace std;

class vec3{
	public:
		vec3(){}
		vec3(float v0, float v1, float v2){
			v[0] = v0;
			v[1] = v1;
			v[2] = v2;
		}
		void add(vec3& b){
			v[0] += b.v[0];
			v[1] += b.v[1];
			v[2] += b.v[2];
		}
		void add(float a, float b, float c){
			v[0] += a;
			v[1] += b;
			v[2] += c;
		}
		void subt(vec3& b){
			v[0] -= b.v[0];
			v[1] -= b.v[1];
			v[2] -= b.v[2];
		}
		void subt(float a, float b, float c){
			v[0] -= a;
			v[1] -= b;
			v[2] -= c;
		}
		void mult(vec3& b){
			v[0] *= b.v[0];
			v[1] *= b.v[1];
			v[2] *= b.v[2];
		}
		void mult(float a){
			v[0] *= a;
			v[1] *= a;
			v[2] *= a;
		}
		void mult(float a, float b, float c){
			v[0] *= a;
			v[1] *= b;
			v[2] *= c;
		}
		float dot(vec3& b){
			return v[0]*b.v[0] + v[1]*b.v[1] + v[2]*b.v[2];
		}
		float v[3];
};

vec3 skyinit(vec3 direction){
	float t = 0.5*(direction.v[1]+1.0);
	vec3 returnval(1, 1, 1);
	returnval.mult(1.0-t, 1.0-t, 1.0-t);
	returnval.add(t*0.5, t*0.7, t*1.0);
	return returnval;
}

vec3 hitsphere(vec3 camposi, vec3 dire, vec3 sphereposi, float r, vector<vec3> lightsources){
	float num = 0;
	float noi = 0;
	for(int a = 0; a < 3; a++){
		num += (camposi.v[a]-sphereposi.v[a])*dire.v[a];
		noi += pow(dire.v[a], 2);
	}
	float t = num/noi*-1;
	vec3 verticalposi(camposi.v[0]+dire.v[0]*t, camposi.v[1]+dire.v[1]*t, camposi.v[2]+dire.v[2]*t);
	vec3 sphere2vertical(0, 0, 0);
	sphere2vertical.subt(sphereposi);
	sphere2vertical.add(verticalposi);
	float distance = pow(sphere2vertical.dot(sphere2vertical), 0.5);
	if(distance <= r){
		float unit2surface = pow(pow(r, 2) - pow(distance, 2), 0.5)/pow(dire.dot(dire), 0.5);
		vec3 surfaceposi(0, 0, 0);
		vec3 direbuf(dire);
		direbuf.mult(unit2surface);
		surfaceposi.add(verticalposi);
		surfaceposi.subt(direbuf);
		//cout << surfaceposi.v[0] << " " << surfaceposi.v[1] << " " << surfaceposi.v[2] << endl;
		float totalreturnval = 0.001;
		for(int a = 0; a < lightsources.size(); a++){
			float returnval = 0;
			vec3 surface2center(surfaceposi);
			surface2center.subt(sphereposi);
			vec3 light2surface(lightsources[a]);
			light2surface.subt(surfaceposi);
			returnval = light2surface.dot(surface2center);
			if(returnval < 0){
				continue;
			}
			else{
				returnval /= pow(light2surface.dot(light2surface), 0.5);
				returnval /= pow(surface2center.dot(surface2center), 0.5);
				totalreturnval = max(totalreturnval, returnval);
			}
		}
		surfaceposi.subt(camposi);
		return vec3(pow(surfaceposi.dot(surfaceposi), 0.5), totalreturnval, 0);
	}
	else{
		return vec3(FLT_MAX, 0, 0);
	}
}

vec3 hitfloor(vec3 camposi, vec3 direction, vector<vec3> lightsources){
	vec3 floor2cam(0, -5, 0);
	if(direction.dot(floor2cam) <= 0){
		return vec3(FLT_MAX, 0, 0);
	}
	else{
		float yunit = -5/direction.v[1];
		float distance = pow(direction.dot(direction), 0.5)*yunit;
		vec3 floorposi(direction);
		floorposi.mult(yunit);
		float totalreturnval = 0.001;
		for(int a = 0; a < lightsources.size(); a++){
			vec3 light2floor(lightsources[a]);
			light2floor.subt(floorposi);
			vec3 floor(0, 5, 0);
			float returnval = light2floor.dot(floor);
			returnval /= pow(light2floor.dot(light2floor), 0.5);
			returnval /= pow(floor.dot(floor), 0.5);
			totalreturnval = max(returnval, totalreturnval);
		}
		return vec3(distance, totalreturnval, 0);
	}
}

vec3 drawpixel(int x, int y, int width, int height, float R, float G, float B){
	vec3 camposi(0, 0, 0);
	vec3 direction(4.0*((float(y)+0.5)/float(width))-2.0, 2.0*((float(x)+0.5)/float(height))-1.0, -1);
	vec3 color = skyinit(direction);

	vector<vec3> lightsources;
	lightsources.push_back(vec3(-5, 5, 5));
	//lightsources.push_back(vec3(0, 0, 0));

	vector<pair<vec3, float>> sphereposi;
	sphereposi.push_back(pair<vec3, float>(vec3(0.25, 0, -1), 0.2));
	sphereposi.push_back(pair<vec3, float>(vec3(-0.25, 0, -2), 0.5));
	
	float nearestdistance = FLT_MAX;
	vec3 floor = hitfloor(camposi, direction, lightsources);
	if(floor.v[0] < FLT_MAX){
		nearestdistance = floor.v[0];
		color = vec3(0.5+0.5*floor.v[1], 0.5+0.5*floor.v[1], 0.5+0.5*floor.v[1]);
	}
	for(int a = 0; a < sphereposi.size(); a++){
		vec3 ans = hitsphere(camposi, direction, sphereposi[a].first, sphereposi[a].second, lightsources);
		if(ans.v[1] > 0.0){
			if(ans.v[0] < nearestdistance){
				nearestdistance = ans.v[0];
				color = vec3(R+(1-R)*ans.v[1], G+(1-G)*ans.v[1], B+(1-B)*ans.v[1]);
			}
		}
	}
	return color;

}

int main(){
	int width = 1000;
	int height = 500;

	fstream file;
	file.open("ray.ppm", ios::out);

	file << "P3\n" << width << " " << height << "\n255\n";
	float R, G, B;
	cout << "Please insert the RGB color of the sphere(0~255): ";
	cin >> R >> G >> B;
	R /= 256;
	G /= 256;
	B /= 256;

	for(int a = height-1; a >= 0; a--){
		for(int b = 0; b < width; b++){
			vec3 color = drawpixel(a, b, width, height, R, G, B);
			file << int(color.v[0] * 255) << " " << int(color.v[1] * 255) << " " << int(color.v[2] * 255) << "\n";
		}
	}
	cout << "Done." << endl;
	return 0;
}
