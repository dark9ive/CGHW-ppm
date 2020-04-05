#include<iostream>
#include<fstream>
#include<math.h>
#include<vector>
#include<float.h>
#include<string.h>

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
		float length(){
			return pow(this->dot(*this), 0.5);
		}
		float v[3];
};

class sphere{
	public:
		vec3 posi;
		float r;
		int material;
		sphere(vec3 pos, float R, int materi){
			this->posi = pos;
			this->r = R;
			this->material = materi;
		}
};

vec3 drawpixel(float R, float G, float B, vec3 camposi, vec3 direction, vector<sphere>& spherevec, vector<vec3>& lightsources, int level);

vec3 skyinit(vec3 direction){
	direction.mult(1/direction.length());
	float t = 0.5*(direction.v[1]+1.0);
	vec3 returnval(1, 1, 1);
	returnval.mult(1.0-t);
	returnval.add(t*0.5, t*0.7, t*1.0);
	
	return returnval;
}

bool hitsphere(vec3 camposi, vec3 dire, vec3 sphereposi, float r){
	float num = 0;
	float noi = 0;
	for(int a = 0; a < 3; a++){
		num += (camposi.v[a]-sphereposi.v[a])*dire.v[a];
		noi += pow(dire.v[a], 2);
	}
	float t = num/noi*-1;
	if(t <= 0){
		return 0;
	}
	vec3 verticalposi(camposi.v[0]+dire.v[0]*t, camposi.v[1]+dire.v[1]*t, camposi.v[2]+dire.v[2]*t);
	vec3 sphere2vertical(0, 0, 0);
	sphere2vertical.subt(sphereposi);
	sphere2vertical.add(verticalposi);
	float distance = pow(sphere2vertical.dot(sphere2vertical), 0.5);
	if(distance <= r){
		return 1;
	}
	else{
		return 0;
	}
}

vec3 seesphere(vec3 camposi, vec3 dire, int index, vector<vec3>& lightsources, vector<sphere>& spheres, int level, float R, float G, float B){
	float num = 0;
	float noi = 0;
	for(int a = 0; a < 3; a++){
		num += (camposi.v[a]-spheres[index].posi.v[a])*dire.v[a];
		noi += pow(dire.v[a], 2);
	}
	float t = num/noi*-1;
	if(t <= 0){
		return vec3(FLT_MAX, 0, 0);
	}
	vec3 verticalposi(camposi.v[0]+dire.v[0]*t, camposi.v[1]+dire.v[1]*t, camposi.v[2]+dire.v[2]*t);
	vec3 sphere2vertical(0, 0, 0);
	sphere2vertical.subt(spheres[index].posi);
	sphere2vertical.add(verticalposi);
	float distance = pow(sphere2vertical.dot(sphere2vertical), 0.5);
	if(distance <= spheres[index].r){
		float unit2surface = pow(pow(spheres[index].r, 2) - pow(distance, 2), 0.5)/pow(dire.dot(dire), 0.5);
		vec3 surfaceposi(0, 0, 0);
		vec3 direbuf(dire);
		direbuf.mult(unit2surface);
		surfaceposi.add(verticalposi);
		surfaceposi.subt(direbuf);
		if(spheres[index].material == 0){
			float totalreturnval = 0.001;
			for(int a = 0; a < lightsources.size(); a++){
				float returnval = 0;
				vec3 surface2center(surfaceposi);
				surface2center.subt(spheres[index].posi);
				vec3 light2surface(lightsources[a]);
				light2surface.subt(surfaceposi);
				returnval = light2surface.dot(surface2center);
				if(returnval < 0){
					continue;
				}
				else{
					bool hided = 0;
					for(int b = 0; b < spheres.size(); b++){
						if(b != index){
							if(hitsphere(surfaceposi, light2surface, spheres[b].posi, spheres[b].r)){
								hided = 1;
								break;
							}
						}
					}
					if(hided == 0){
						returnval /= pow(light2surface.dot(light2surface), 0.5);
						returnval /= pow(surface2center.dot(surface2center), 0.5);
						totalreturnval = max(totalreturnval, returnval);
					}
				}
			}
			surfaceposi.subt(camposi);
			return vec3(surfaceposi.length(), totalreturnval, 0);
		}
		else if(spheres[index].material == 1){
			vec3 surface2center(surfaceposi);
			surface2center.subt(spheres[index].posi);
			float coslength = dire.dot(surface2center);
			coslength /= surface2center.length();
			float t = coslength / surface2center.length();
			vec3 buf(surface2center);
			buf.mult(2*t);
			dire.subt(buf);
			vec3 color = drawpixel(R, G, B, surfaceposi, dire, spheres, lightsources, level+1);
			float returnval;
			int returnbuf = 0;
			for(int a = 0; a < 3; a++){
				int buf = color.v[a]*255;
				//cout << buf << " " << 8*(2-a) << endl;
				returnbuf += (buf << (8*(2-a)));
			}
			memcpy(&returnval, &returnbuf, 4);
			surfaceposi.subt(camposi);
			//return vec3(FLT_MAX, 0, 0);
			return vec3(surfaceposi.length(), returnval, 0);
		}
	}
	else{
		return vec3(FLT_MAX, 0, 0);
	}
}

vec3 hitfloor(vec3 camposi, vec3 direction, vector<vec3> lightsources, vector<sphere> spheres){
	vec3 floor2cam(0, -4, 0);
	if(direction.dot(floor2cam) <= 0){
		return vec3(FLT_MAX, 0, 0);
	}
	else{
		float yunit = -4/direction.v[1];
		float distance = pow(direction.dot(direction), 0.5)*yunit;
		vec3 floorposi(direction);
		floorposi.mult(yunit);
		if(floorposi.v[0] > 20 || floorposi.v[0] < -20 || floorposi.v[2] > 70 || floorposi.v[2] < -70){
			return vec3(FLT_MAX, 0, 0);
		}
		float totalreturnval = 0.001;
		for(int a = 0; a < lightsources.size(); a++){
			vec3 light2floor(lightsources[a]);
			light2floor.subt(floorposi);

			vec3 floor(0, 4, 0);
			float returnval = light2floor.dot(floor);
			returnval /= pow(light2floor.dot(light2floor), 0.5);
			returnval /= pow(floor.dot(floor), 0.5);
			//returnval = pow(returnval, 2);			//	deepen the lights on floor(optional)
			totalreturnval = max(returnval, totalreturnval);

			for(int b = 0; b < spheres.size(); b++){
				if(hitsphere(floorposi, light2floor, spheres[b].posi, spheres[b].r)){
					return vec3(distance, 0.001, 0);
				}
			}
		}
		return vec3(distance, totalreturnval, 0);
	}
}

vector<sphere> spherevecinit(){
	vector<sphere> spherevec;
	//spherevec.push_back(sphere(vec3(0.25, 0, -2), 0.5, 0));
	spherevec.push_back(sphere(vec3(-0.5, -1, -3), 0.2, 0));
	spherevec.push_back(sphere(vec3(-0.25, -3, -5), 0.8, 0));
	spherevec.push_back(sphere(vec3(1, -2, -6), 0.5, 0));
	spherevec.push_back(sphere(vec3(2, -2, -7), 0.5, 0));
	spherevec.push_back(sphere(vec3(-3, -1, -8), 0.5, 0));
	//spherevec.push_back(sphere(vec3(-3, 1.5, -5), 0.8, 1));
	spherevec.push_back(sphere(vec3(-2, 0, -4), 0.8, 1));
	spherevec.push_back(sphere(vec3(2, 0, -4), 0.8, 1));
	return spherevec;
}

vector<vec3> lightsourcesinit(){
	vector<vec3> lightsources;
	lightsources.push_back(vec3(-2, 3, 0));
	//lightsources.push_back(vec3(0, 0, 0));
	return lightsources;
}

vec3 drawpixel(float R, float G, float B, vec3 camposi, vec3 direction, vector<sphere>& spherevec, vector<vec3>& lightsources, int level){
	vec3 color = skyinit(direction);
	/*
	if(level != 0){
		cout << skyinit(direction).v[0] << " " << skyinit(direction).v[1] << " " <<skyinit(direction).v[2] <<endl;
	}
	*/
	float nearestdistance = FLT_MAX;
	
	vec3 floor = hitfloor(camposi, direction, lightsources, spherevec);
	if(floor.v[0] < FLT_MAX){
		nearestdistance = floor.v[0];
		color = vec3(0.35+0.65*floor.v[1], 0.35+0.65*floor.v[1], 0.35+0.65*floor.v[1]);
	}
	
	for(int a = 0; a < spherevec.size(); a++){
		vec3 ans = seesphere(camposi, direction, a, lightsources, spherevec, level, R, G, B);
		if(ans.v[0] < nearestdistance){
			nearestdistance = ans.v[0];
			if(spherevec[a].material == 0){
				color = vec3(R+(1-R)*ans.v[1], G+(1-G)*ans.v[1], B+(1-B)*ans.v[1]);
			}
			else if(spherevec[a].material == 1){
				int R_, G_, B_;
				memcpy(&R_, &ans.v[1], 4);
				memcpy(&G_, &ans.v[1], 4);
				memcpy(&B_, &ans.v[1], 4);
				R_ = (R_ & 0x00FF0000) >> 16;
				G_ = (G_ & 0x0000FF00) >> 8;
				B_ = (B_ & 0x000000FF);
				color = vec3(float(R_)/256, float(G_)/256, float(B_)/256);
			}
		}
	}
	return color;
}

int main(){
	int width = 1000;
	int height = 500;

	fstream File;
	File.open("ray.ppm", ios::out);

	File << "P3\n" << width << " " << height << "\n255\n";
	float R, G, B;
	cout << "Please insert the RGB color of the sphere(ex: 255 0 255): ";
	cin >> R >> G >> B;
	R /= 256;
	G /= 256;
	B /= 256;

	clock_t time = clock();
	vector<vec3> lightsources = lightsourcesinit();
	vector<sphere> spherevec = spherevecinit();
	
	for(int x = height-1; x >= 0; x--){
		for(int y = 0; y < width; y++){	
			vec3 camposi(0, 0, 0);
			vec3 direction(4.0*((float(y)+0.5)/float(width))-2.0, 2.0*((float(x)+0.5)/float(height))-1.0, -1);
			vec3 color = drawpixel(R, G, B, camposi, direction, spherevec, lightsources, 0);
			File << int(color.v[0] * 256) << " " << int(color.v[1] * 256) << " " << int(color.v[2] * 256) << "\n";
		}
	}

	time = clock() - time;
	cout << "Done." << endl;
	cout << "Time used: " << float(time)/CLOCKS_PER_SEC << "(s)" << endl;
	cout << "Pixels calculated per sec:" << float(width*height)/float(time)*CLOCKS_PER_SEC << "(pixels/s)" << endl;
	return 0;
}
