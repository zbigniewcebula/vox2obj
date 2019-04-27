#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

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
};

int main(int argc, char** argv) {
	//Checking args
	if(argc <= 1) {
		cerr	<< "No parameters given! Aborting..." << endl;
		return 1;
	}

	VOXModel vox(argv[1]);
	int vID = 0;
	int tID = 0;

	int size = vox.SizeZ() * vox.SizeY() * vox.SizeX();
	//Cube** map = new Cube*[size];
	vector<Cube*> map;

	vec4 dirs[6] = {
		{0, 0, 1},
		{0, 1, 0},
		{1, 0, 0},
		{0, 0, -1},
		{0, -1, 0},
		{-1, 0, }
	};

	vox.ForEachVoxel([&](int x, int y, int z, int ID) {
		Cube*	cube = nullptr;
		if(ID != 0) {
			cube = new Cube();
			cube->vox.Set(x, y, z, ID);

			for(int i  0; i < 6; ++i) {
				if(vox.VoxelColorID(
					x + dirs[i].x, y + dirs[i].y, z + dirs[i].z
				) != 0) {
					neigh[i] = true;
				}	
			}
			for(int i  0; i < 8; ++i)
				cube->v[i].w = -1;

			int _1 = x + 1;
			int _2 = y + 1;
			//Dół
			auto A = cube->v[0].Set(x, 	y, 	z);
			auto B = cube->v[1].Set(_1, 	y, 	z);
			auto C = cube->v[2].Set(_1, 	_2,	z);
			auto D = cube->v[3].Set(x, 	_2,	z);
			//Góra
			++z;
			auto E = cube->v[4].Set(x, 	y, 	z);
			auto F = cube->v[5].Set(_1, 	y, 	z);
			auto G = cube->v[6].Set(_1, 	_2, z);
			auto H = cube->v[7].Set(x, 	_2, z);

			if(neigh[0]) {
				cube->v[0].Set(x, 	y, 	z, vID); vID++;
				cube->v[1].Set(x, 	y, 	z, vID); vID++;
				cube->v[3].Set(x, 	y, 	z, vID); vID++;
				cube->v[4].Set(x, 	y, 	z, vID); vID++;
			}

			/*

			cube->t[0].Set(A.w, B.w, F.w, tID++);
			cube->t[1].Set(B.w, C.w, G.w, tID++);	
			cube->t[2].Set(C.w, H.w, D.w, tID++);
			cube->t[3].Set(D.w, A.w, H.w, tID++);
			
			cube->t[4].Set(A.w, F.w, E.w, tID++);
			cube->t[5].Set(B.w, G.w, F.w, tID++);
			cube->t[6].Set(C.w, G.w, H.w, tID++);
			cube->t[7].Set(A.w, H.w, E.w, tID++);

			cube->t[8].Set(A.w, C.w, B.w, tID++);
			cube->t[9].Set(A.w, D.w, C.w, tID++);
			cube->t[10].Set(E.w, F.w, G.w, tID++);
			cube->t[11].Set(E.w, G.w, H.w, tID++);
			*/
			map.push_back(cube);
		}
		//map[z * vox.SizeY() + y * vox.SizeX() + x] = cube;
	}, [&](int y, int z) {
	//	cout << endl;
	}, [&](int z) {
	//	cout << endl;
	});
	
	string vn = "423142315566";
	
	cout	<< "#" << map.size() << endl;

	cout	<< "g TEST\n"
			<< "mtllib TEST.mtl\n"
			<< "usemtl palette\n"
			<< "\n"
			<< "vn -1 0 0\n"
			<< "vn 1 0 0\n"
			<< "vn 0 0 1\n"
			<< "vn 0 0 -1\n"
			<< "vn 0 -1 0\n"
			<< "vn 0 1 0\n"
			<< "vt 0.5 0.5\n"
	<< endl;

	for(size_t i = 0; i < map.size(); ++i) {
		for(int j = 0; j < 8; ++j) {
			if(map[i]->v[j].w != -1) {
				cout	<< "v "
						<< int(map[i]->v[j].x) << ' '
						<< int(map[i]->v[j].y) << ' '
						<< int(map[i]->v[j].z)// << " #"
						//<< int(map[i]->v[j].w)
				<< endl;
			}
		}
	}
	cout << "\n\n";

	for(size_t i = 0; i < map.size(); ++i) {
		if(map[i] != nullptr)
			for(int j = 0; j < 12; ++j) {
				cout	<< "f "
						<< int(map[i]->t[j].x + 1) << '/'
						<< "1/" << int(vn[j] - '0') << " "

						<< int(map[i]->t[j].y + 1) << '/'
						<< "1/" << int(vn[j] - '0') << " "

						<< int(map[i]->t[j].z + 1) << '/'
						<< "1/" << int(vn[j] - '0')
				<< endl;
			}
	}
	cout << endl;

	for(int i = 0; i < map.size(); ++i) {
		if(map[i] != nullptr)
			delete	map[i];
	}
	return 0;
}