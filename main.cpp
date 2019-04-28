#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
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

	vox.ForEachVoxel([&](int x, int y, int z, int ID) {
		Cube*	cube = nullptr;
		if(ID != 0) {
			cube = new Cube();
			cube->vox.Set(x, y, z, ID);

			for(int i = 0; i < 6; ++i) {
				cube->neigh[i] = vox.VoxelColorID(
					x + dirs[i].x, y + dirs[i].y, z + dirs[i].z
				) != 0;
			}

			int _1 = x + 1;
			int _2 = y + 1;
			//Dół
			auto& A = cube->v[0].Set(x, y, 	z);
			auto& B = cube->v[1].Set(_1, y, z);
			auto& C = cube->v[2].Set(_1, _2, z);
			auto& D = cube->v[3].Set(x, _2,	z);
			//Góra
			++z;
			auto& E = cube->v[4].Set(x,	y, 	z);
			auto& F = cube->v[5].Set(_1, y, z);
			auto& G = cube->v[6].Set(_1, _2, z);
			auto& H = cube->v[7].Set(x, _2, z);

			for(int i = 0; i < 8; ++i)
				cube->v[i].w = -1;

			//if(!cube->neigh[0]) {	//up
				H.w = 1;
				G.w = 1;
				E.w = 1;
				F.w = 1;
			//}
			//if(!cube->neigh[1]) {	//back
				H.w = 1;
				G.w = 1;
				C.w = 1;
				D.w = 1;
			//}
			//if(!cube->neigh[2]) {	//right
				B.w = 1;
				G.w = 1;
				C.w = 1;
				F.w = 1;
			//}
			//if(!cube->neigh[3]) {	//front
				B.w = 1;
				A.w = 1;
				E.w = 1;
				F.w = 1;
			//}
			//if(!cube->neigh[4]) {	//bottom
				B.w = 1;
				A.w = 1;
				C.w = 1;
				D.w = 1;
			//}
			//if(!cube->neigh[5]) {	//left
				D.w = 1;
				A.w = 1;
				H.w = 1;
				E.w = 1;
			//}

			for(int i = 0; i < 8; ++i) {
				if(cube->v[i].w == 1)
					cube->v[i].w += vID++;
			}
			for(int i = 0; i < 12; ++i) {
				cube->t[i].w = -1;
			}

			if(!cube->neigh[0]) {	//up
				cube->t[0].Set(H.w, E.w, F.w, tID++);
				cube->t[1].Set(H.w, F.w, G.w, tID++);
			}
			if(!cube->neigh[1]) {	//back
				cube->t[2].Set(C.w, H.w, G.w, tID++);
				cube->t[3].Set(C.w, D.w, H.w, tID++);
			}
			if(!cube->neigh[2]) {	//right
				cube->t[4].Set(B.w, G.w, F.w, tID++);
				cube->t[5].Set(B.w, C.w, G.w, tID++);
			}
			if(!cube->neigh[3]) {	//front
				cube->t[6].Set(A.w, B.w, E.w, tID++);
				cube->t[7].Set(B.w, F.w, E.w, tID++);
			}
			if(!cube->neigh[4]) {	//bottom
				cube->t[8].Set(D.w, C.w, B.w, tID++);
				cube->t[9].Set(D.w, B.w, A.w, tID++);
			}
			if(!cube->neigh[5]) {	//left
				cube->t[10].Set(D.w, A.w, E.w, tID++);
				cube->t[11].Set(D.w, E.w, H.w, tID++);
			}

			map.push_back(cube);
			cube = nullptr;
		}
	}, [&](int y, int z) {
	//	cout << endl;
	}, [&](int z) {
	//	cout << endl;
	});
	
	/*
	vox.ForEachVoxel([&](int x, int y, int z, int ID) {
		cout << int(ID) << " ";
	}, [&](int y, int z) {
		cout << endl;
	}, [&](int z) {
		cout << endl;
	});
	*/

	string vn = "112233445566";
	
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
			<< "vt 0.5 0.5\n"
	<< endl;

	for(size_t i = 0; i < map.size(); ++i) {
		for(int j = 0; j < 8; ++j) {
			if(map[i]->v[j].w > -1) {
				hFile	<< "v "
						<< int(map[i]->v[j].x) << ' '
						<< int(map[i]->v[j].y) << ' '
						<< int(map[i]->v[j].z) <<
						"\n#" << int(map[i]->v[j].w)
				<< endl;
			}
		}
	}
	hFile << "\n\n";

	for(size_t i = 0; i < map.size(); ++i) {
		if(map[i] != nullptr)
			for(int j = 0; j < 12; ++j) {
				if(map[i]->t[j].w > -1)
				hFile	<< "f "
						<< int(map[i]->t[j].x) << '/'
						<< "1/"
						<< int(vn[j] - '0') << " "

						<< int(map[i]->t[j].y) << '/'
						<< "1/"
						<< int(vn[j] - '0') << " "

						<< int(map[i]->t[j].z) << '/'
						<< "1/"
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