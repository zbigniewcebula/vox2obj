#ifndef __MODEL__
#define __MODEL__

#include <vector>
#include <fstream>

#include "VOX.h"

class Voxel {
	public:
		//position + palette color ID
		//vec4 vox;

		//pos + vertex ID
		vec4i vertex[8];

		//vertexID, vertexID, vertexID, trisID
		vec4i triangle[12];

		Voxel() {}

		bool neighbors[6] = {
			//up, back, right
			false, false, false,
			//down, front, left
			false, false, false
		};

		int colorListIndex = -1;
};

class Model {
	private:
		static constexpr double halfTexturePixelSize	= 0.001953125;
		static constexpr double texturePixelSize		= 0.00390625;


		vector<Voxel*>	voxel;
		vector<int>		colorList;

		float			scale	= 0.03125f;

		vec4i			size;
	public:
		string			name	= "Model";

		Model() {}
		~Model() {
			Clear();
		}

		void Clear() {
			if(voxel.size() > 0) {
				for(Voxel* v : voxel) {
					if(v != nullptr) {
						delete v;
					}
				}
				voxel.clear();
			}
			colorList.clear();

			size.Set(0, 0, 0, 0);
		}

		vec4f Center() {
			vec4f ret(0, 0, 0, 0);
			if(voxel.size() > 0) {
				for(Voxel* v : voxel) {
					for(int i = 0; i < 8; ++i) {
						ret += v->vertex[i];
					}
				}
			}

			return ret * (1.0f / (voxel.size() * 8.0f)) + vec4<float>(
				size.x * 0.5f, 0, -size.z * 0.5f, 0
			);
		}

		void LoadVOX(VOXModel& vox) {
			int vID = 0;
			int tID = 0;

			Clear();

			vec4i normals[6] = {
				{0, 0, 1},	//up
				{0, 1, 0},	//back
				{1, 0, 0},	//right
				{0, 0, -1},	//down
				{0, -1, 0},	//front
				{-1, 0, 0}	//left
			};

			size.Set(vox.SizeX(), vox.SizeY(), vox.SizeZ(), 0);

			for(int z = 0; z < vox.SizeZ(); ++z)
				for(int y = 0; y < vox.SizeY(); ++y)
					for(int x = 0; x < vox.SizeX(); ++x) {
						int ID = vox.VoxelColorID(x, y, z);
						if(ID != 0) {
							Voxel*	v = new Voxel();

							auto it = find(colorList.begin(), colorList.end(), ID);
							if(it == colorList.end()) {
								colorList.push_back(ID);
								v->colorListIndex = colorList.size() - 1;
							} else {
								v->colorListIndex = it - colorList.begin();
							}
							v->colorListIndex += 1;

							for(int i = 0; i < 6; ++i) {
								v->neighbors[i] = vox.VoxelColorID(
									x + normals[i].x, y + normals[i].y, z + normals[i].z
								) != 0;
							}

							int _1 = x + 1;
							int _2 = y + 1;
							//Dół
							auto& A = v->vertex[0].Set(-x, z, y);
							auto& B = v->vertex[1].Set(-_1, z, y);
							auto& C = v->vertex[2].Set(-_1, z, _2);
							auto& D = v->vertex[3].Set(-x, z, _2);
							//Góra
							++z;
							auto& E = v->vertex[4].Set(-x,	z, y);
							auto& F = v->vertex[5].Set(-_1, 	z, y);
							auto& G = v->vertex[6].Set(-_1, z, _2);
							auto& H = v->vertex[7].Set(-x, z, _2);
							--z;

							for(int i = 0; i < 8; ++i)
								v->vertex[i].a = -1;
							for(int i = 0; i < 12; ++i)
								v->triangle[i].a = -1;
							
							if(!v->neighbors[0]) {	//up
								H.a = 1;
								G.a = 1;
								E.a = 1;
								F.a = 1;
							}
							if(!v->neighbors[1]) {	//back
								H.a = 1;
								G.a = 1;
								C.a = 1;
								D.a = 1;
							}
							if(!v->neighbors[2]) {	//right
								B.a = 1;
								G.a = 1;
								C.a = 1;
								F.a = 1;
							}
							if(!v->neighbors[4]) {	//front
								B.a = 1;
								A.a = 1;
								E.a = 1;
								F.a = 1;
							}
							if(!v->neighbors[3]) {	//bottom
								B.a = 1;
								A.a = 1;
								C.a = 1;
								D.a = 1;
							}
							if(!v->neighbors[5]) {	//left
								D.a = 1;
								A.a = 1;
								H.a = 1;
								E.a = 1;
							}

							for(int i = 0; i < 8; ++i) {
								if(v->vertex[i].a == 1)
									v->vertex[i].a += vID++;
							}

							if(!v->neighbors[0]) {	//up
								v->triangle[0].Set(H.a, E.a, F.a, tID++);
								v->triangle[1].Set(H.a, F.a, G.a, tID++);
							}
							if(!v->neighbors[1]) {	//back
								v->triangle[2].Set(C.a, H.a, G.a, tID++);
								v->triangle[3].Set(C.a, D.a, H.a, tID++);
							}
							if(!v->neighbors[2]) {	//right
								v->triangle[4].Set(B.a, G.a, F.a, tID++);
								v->triangle[5].Set(B.a, C.a, G.a, tID++);
							}
							if(!v->neighbors[3]) {	//bottom
								v->triangle[6].Set(D.a, C.a, B.a, tID++);
								v->triangle[7].Set(D.a, B.a, A.a, tID++);
							}
							if(!v->neighbors[4]) {	//front
								v->triangle[8].Set(A.a, B.a, E.a, tID++);
								v->triangle[9].Set(B.a, F.a, E.a, tID++);
							}
							if(!v->neighbors[5]) {	//left
								v->triangle[10].Set(D.a, A.a, E.a, tID++);
								v->triangle[11].Set(D.a, E.a, H.a, tID++);
							}

							voxel.push_back(v);
						}
					}
				//--
			//--
		}

		void SaveOBJ(string outPath, string mtlFile) {
			string vn = "221166554433";
	
			ofstream hFile(outPath, ios::trunc | ios::out);

			hFile
				<< "#" << voxel.size() << '\n'
				<< "g " << (name == ""? "Model": name) << '\n'
				<< "mtllib " << mtlFile << "\n"
				<< "usemtl palette\n"
				<< "\n"
				<< "vn 0 0 1\n"
				<< "vn 0 1 0\n"
				<< "vn 1 0 0\n"
				<< "vn 0 0 -1\n"
				<< "vn 0 -1 0\n"
				<< "vn -1 0 0\n"
			<< endl;

			for(int id : colorList) {
				hFile 
					<< "vt "
					<< (id * texturePixelSize - halfTexturePixelSize)
					<< " 0.5"
				<< endl;
			}
			hFile << endl;

			float offsetX = size.x * 0.5f;
			float offsetY = 0;
			float offsetZ = -size.z * 0.5f;

			for(size_t i = 0; i < voxel.size(); ++i) {
				for(int j = 0; j < 8; ++j) {
					if(voxel[i]->vertex[j].a > -1) {
						hFile	<< "v "
								<< ((voxel[i]->vertex[j].x + offsetX) * scale)
								<< ' '
								<< ((voxel[i]->vertex[j].y + offsetY) * scale)
								<< ' '
								<< ((voxel[i]->vertex[j].z + offsetZ) * scale)
						<< endl;
					}
				}
			}
			hFile << "\n\n";

			for(size_t i = 0; i < voxel.size(); ++i) {
				if(voxel[i] != nullptr)
					for(int j = 0; j < 12; ++j) {
						if(voxel[i]->triangle[j].a > -1)
						hFile	<< "f "
								<< int(voxel[i]->triangle[j].x) << '/'
								<< voxel[i]->colorListIndex << "/"
								<< int(vn[j] - '0') << " "

								<< int(voxel[i]->triangle[j].y) << '/'
								<< voxel[i]->colorListIndex << "/"
								<< int(vn[j] - '0') << " "

								<< int(voxel[i]->triangle[j].z) << '/'
								<< voxel[i]->colorListIndex << "/"
								<< int(vn[j] - '0')
						<< endl;
					}
			}
			hFile << endl;
			hFile.flush();
			hFile.close();
		}

		void SetScale(float newScale) {
			scale	= newScale;
		}
};

#endif