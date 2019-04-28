#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <map>

#include <cctype>
#include <cstdio>
#include <cstring>

#include <dirent.h>
#include <errno.h>

#include "VOX.h"

#include "helpFunctions.h"

using namespace std;

struct Cube {
	//pos + palette color
	vec4 vox;

	//pos + vertex ID
	vec4 v[8];

	//vertexID, vertexID, vertexID, trisID
	vec4 t[12];

	Cube() {

	}

	bool neigh[6] = {
		false, false, false, false, false, false
	};

	int clr = -1;
};

int main(int argc, char** argv) {
	//Checking args
	if(argc <= 1) {
		cerr	<< "No parameters given! Aborting..." << endl;
		return 1;
	}

	VOXModel vox;
	vox.Load(argv[1]);
	int vID = 0;
	int tID = 0;

	//size_t size = vox.SizeZ() * vox.SizeY() * vox.SizeX();
	//Cube** map = new Cube*[size];
	vector<Cube*> map;

	vec4 dirs[6] = {
		{0, 0, 1},
		{0, 1, 0},
		{1, 0, 0},
		{0, 0, -1},
		{0, -1, 0},
		{-1, 0, 0}
	};

	vector<int>	color;

	//vox.ForEachVoxel([&](int x, int y, int z, int ID) {
	for(int z = 0; z < vox.SizeZ(); ++z)
		for(int y = 0; y < vox.SizeY(); ++y)
			for(int x = 0; x < vox.SizeX(); ++x) {
				int ID = vox.VoxelColorID(x, y, z);
				if(ID != 0) {
					Cube*	cube = new Cube();
					cube->vox.Set(x, y, z, ID);

					auto it = find(color.begin(), color.end(), ID);
					if(it == color.end()) {
						color.push_back(ID);
						cube->clr = color.size() - 1;
					} else {
						cube->clr = it - color.begin();
					}
					cube->clr += 1;

					for(int i = 0; i < 6; ++i) {
						cube->neigh[i] = vox.VoxelColorID(
							x + dirs[i].x, y + dirs[i].y, z + dirs[i].z
						) != 0;
					}

					int _1 = x + 1;
					int _2 = y + 1;
					//Dół
					auto& A = cube->v[0].Set(-x, z, y);
					auto& B = cube->v[1].Set(-_1, z, y);
					auto& C = cube->v[2].Set(-_1, z, _2);
					auto& D = cube->v[3].Set(-x, z, _2);
					//Góra
					++z;
					auto& E = cube->v[4].Set(-x,	z, y);
					auto& F = cube->v[5].Set(-_1, 	z, y);
					auto& G = cube->v[6].Set(-_1, z, _2);
					auto& H = cube->v[7].Set(-x, z, _2);
					--z;

					for(int i = 0; i < 8; ++i)
						cube->v[i].a = -1;

					if(!cube->neigh[0]) {	//up
						H.a = 1;
						G.a = 1;
						E.a = 1;
						F.a = 1;
					}
					if(!cube->neigh[1]) {	//back
						H.a = 1;
						G.a = 1;
						C.a = 1;
						D.a = 1;
					}
					if(!cube->neigh[2]) {	//right
						B.a = 1;
						G.a = 1;
						C.a = 1;
						F.a = 1;
					}
					if(!cube->neigh[4]) {	//front
						B.a = 1;
						A.a = 1;
						E.a = 1;
						F.a = 1;
					}
					if(!cube->neigh[3]) {	//bottom
						B.a = 1;
						A.a = 1;
						C.a = 1;
						D.a = 1;
					}
					if(!cube->neigh[5]) {	//left
						D.a = 1;
						A.a = 1;
						H.a = 1;
						E.a = 1;
					}

					for(int i = 0; i < 8; ++i) {
						if(cube->v[i].a == 1)
							cube->v[i].a += vID++;
					}
					for(int i = 0; i < 12; ++i) {
						cube->t[i].a = -1;
					}

					if(!cube->neigh[0]) {	//up
						cube->t[0].Set(H.a, E.a, F.a, tID++);
						cube->t[1].Set(H.a, F.a, G.a, tID++);
					}
					if(!cube->neigh[1]) {	//back
						cube->t[2].Set(C.a, H.a, G.a, tID++);
						cube->t[3].Set(C.a, D.a, H.a, tID++);
					}
					if(!cube->neigh[2]) {	//right
						cube->t[4].Set(B.a, G.a, F.a, tID++);
						cube->t[5].Set(B.a, C.a, G.a, tID++);
					}
					if(!cube->neigh[3]) {	//bottom
						cube->t[6].Set(D.a, C.a, B.a, tID++);
						cube->t[7].Set(D.a, B.a, A.a, tID++);
					}
					if(!cube->neigh[4]) {	//front
						cube->t[8].Set(A.a, B.a, E.a, tID++);
						cube->t[9].Set(B.a, F.a, E.a, tID++);
					}
					if(!cube->neigh[5]) {	//left
						cube->t[10].Set(D.a, A.a, E.a, tID++);
						cube->t[11].Set(D.a, E.a, H.a, tID++);
					}

					map.push_back(cube);
				}
			}
		//--
	//--
	
	string vn = "221166554433";
	
	ofstream hFile("./output.obj", ios::trunc | ios::out);


	hFile	<< "#" << map.size() << endl;

	hFile	<< "g TEST\n"
			<< "mtllib TEST.mtl\n"
			<< "usemtl palette\n"
			<< "\n"
			<< "vn 0 0 1\n"
			<< "vn 0 1 0\n"
			<< "vn 1 0 0\n"
			<< "vn 0 0 -1\n"
			<< "vn 0 -1 0\n"
			<< "vn -1 0 0\n"
	<< endl;

	float halfPx = (1.0f / 256.0f) / 2.0f;
	for(size_t i = 0; i < color.size(); ++i) {
		hFile 	<< "vt "
				<< (color[i] * (2.0f * halfPx) - halfPx)
				<< " 0.5\n";
	}
	hFile << endl;

	int offsetX = 0;//-vox.SizeX() / 2;
	int offsetY = 0;//-vox.SizeY() / 2;

	for(size_t i = 0; i < map.size(); ++i) {
		for(int j = 0; j < 8; ++j) {
			if(map[i]->v[j].a > -1) {
				hFile	<< "v "
						<< ((map[i]->v[j].x + offsetX)  / 32.0f)
						<< ' ' << ((map[i]->v[j].y + offsetY)  / 32.0f)
						<< ' ' << (map[i]->v[j].z / 32.0f)
						//<< "\n#" << int(map[i]->v[j].w)
				<< endl;
			}
		}
	}
	hFile << "\n\n";

	for(size_t i = 0; i < map.size(); ++i) {
		if(map[i] != nullptr)
			for(int j = 0; j < 12; ++j) {
				if(map[i]->t[j].a > -1)
				hFile	<< "f "
						<< int(map[i]->t[j].x) << '/'
						<< map[i]->clr << "/"
						<< int(vn[j] - '0') << " "

						<< int(map[i]->t[j].y) << '/'
						<< map[i]->clr << "/"
						<< int(vn[j] - '0') << " "

						<< int(map[i]->t[j].z) << '/'
						<< map[i]->clr << "/"
						<< int(vn[j] - '0')
				<< endl;
			}
	}
	hFile << endl;
	hFile.flush();
	hFile.close();

	for(size_t i = 0; i < map.size(); ++i) {
		if(map[i] != nullptr)
			delete	map[i];
	}
	return 0;
}