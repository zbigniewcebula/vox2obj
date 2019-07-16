#ifndef __MODEL__
#define __MODEL__

#include <vector>
#include <fstream>
#include <cstring>
#include <cmath>

#include "VOX.h"

class Voxel {
	public:
		//position + palette color ID
		//vec4 vox;

		//pos + vertex ID
			//8 for OBJ
			//12 for MarchingCubes
		vec4i vertex[12];

		//vertexID, vertexID, vertexID, trisID
		vec4i triangle[12];

		//Normal OBJ
			//up, back, right
			//down, front, left
		bool neighbors[6];

		int colorListIndex = -1;

		Voxel() {
			memset(neighbors, 0, 8 * sizeof(bool));
		}
};

class Model {
	private:
		static constexpr double halfTexturePixelSize	= 0.001953125;
		static constexpr double texturePixelSize		= 0.00390625;


		vector<Voxel*>	voxel;
		vector<int>		colorList;

		float			scale	= 0.03125f;

		vec4i			size;

		bool			marchingCube	= false;

		const int edgeTable[256] = {
			0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
			0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
			0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
			0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
			0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
			0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
			0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
			0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
			0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
			0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
			0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
			0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
			0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
			0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
			0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
			0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
			0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
			0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
			0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
			0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
			0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
			0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
			0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
			0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
			0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
			0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
			0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
			0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
			0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
			0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
			0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
			0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
		};
		const int triangulation[256][16] = {
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
			{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
			{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
			{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
			{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
			{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
			{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
			{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
			{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
			{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
			{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
			{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
			{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
			{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
			{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
			{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
			{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
			{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
			{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
			{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
			{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
			{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
			{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
			{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
			{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
			{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
			{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
			{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
			{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
			{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
			{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
			{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
			{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
			{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
			{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
			{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
			{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
			{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
			{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
			{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
			{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
			{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
			{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
			{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
			{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
			{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
			{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
			{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
			{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
			{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
			{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
			{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
			{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
			{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
			{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
			{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
			{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
			{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
			{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
			{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
			{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
			{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
			{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
			{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
			{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
			{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
			{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
			{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
			{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
			{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
			{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
			{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
			{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
			{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
			{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
			{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
			{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
			{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
			{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
			{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
			{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
			{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
			{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
			{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
			{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
			{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
			{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
			{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
			{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
			{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
			{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
			{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
			{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
			{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
			{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
			{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
			{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
			{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
			{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
			{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
			{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
			{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
		};

		const vec4i normals[6] = {
			{0, 0, 1},	//up
			{0, 1, 0},	//back
			{1, 0, 0},	//right
			{0, 0, -1},	//down
			{0, -1, 0},	//front
			{-1, 0, 0}	//left
		};
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

		void LoadVOXMarchingCubes(VOXModel& vox) {
			int vID = 1;
			int tID = 0;

			Clear();

			vec4i neighDirs[26] = {
				{-1, 1, -1},//01
				{1, 1, -1},//02
				{1, -1, -1},//04
				{-1, -1, -1},//08
				
				{-1, 1, 1},//10
				{1, 1, 1},//20
				{1, -1, 1},//40
				{-1, -1, 1},//80
				
				{-1, 0, 0},//99
				{0, -1, 0},//CC
				{0, 0, -1},//0F
				{0, 0, 1},//F0
				{0, 1, 0},//33
				{1, 0, 0},//66
				{-1, -1, 0},//88
				{-1, 0, -1},//09
				{-1, 0, 1},//90
				{-1, 1, 0},//11
				{0, -1, -1},//0C
				{0, -1, 1},//D0
				{0, 1, -1},//03
				{0, 1, 1},//30
				{1, -1, 0},//44
				{1, 0, -1},//06
				{1, 0, 1},//60
				{1, 1, 0}//22
			};

			unsigned char neighBits[26] = {
				0b00000001,//01
				0b00000010,//02
				0b00000100,//04
				0b00001000,//08

				0b00010000,//10
				0b00100000,//20
				0b01000000,//40
				0b10000000,//80
				
				0b10011001,//99
				0b11001100,//CC
				0b00001111,//0F
				0b11110000,//F0
				0b00110011,//33
				0b01100110,//66
				0b10001000,//88
				0b00001001,//09
				0b10010000,//90
				0b00010001,//11
				0b00001100,//0C
				0b11010000,//D0
				0b00000011,//03
				0b00110000,//30
				0b01000100,//44
				0b00000110,//06
				0b01100000,//60
				0b00100010//22
			};

			size.Set(vox.SizeX() * 2, vox.SizeY() * 2, vox.SizeZ() * 2, 0);

			colorList.push_back(0);

			vec4i*		edges[12];
			edges[0]	= new vec4i(1, 2, 0);
			edges[1]	= new vec4i(2, 1, 0);
			edges[2]	= new vec4i(1, 0, 0);
			edges[3]	= new vec4i(0, 1, 0);

			edges[4]	= new vec4i(1, 2, 2);
			edges[5]	= new vec4i(2, 1, 2);
			edges[6]	= new vec4i(1, 0, 2);
			edges[7]	= new vec4i(0, 1, 2);

			edges[8]	= new vec4i(0, 2, 1);
			edges[9]	= new vec4i(2, 2, 1);
			edges[10]	= new vec4i(2, 0, 1);
			edges[11]	= new vec4i(0, 0, 1);

			for(float z = -1; z < vox.SizeZ(); ++z) {
				for(float y = -1; y < vox.SizeY(); ++y) {
					for(float x = -1; x < vox.SizeX(); ++x) {
						unsigned char bits = 0;
						int ID = 0;

						for(int i = 0; i < 8; ++i) {
							bool res = vox.VoxelColorID(
								round(x + 0.5 + neighDirs[i].x * 0.5),
								round(y + 0.5 + neighDirs[i].y * 0.5),
								round(z + 0.5 + neighDirs[i].z * 0.5)
								//x + neighDirs[i].x,
								//y + neighDirs[i].y,
								//z + neighDirs[i].z
							) == 0;

							if(x == 0 && y == 0 && z == 0) {
							cout	<< "================\n"
									<<	round(x + 0.5 + neighDirs[i].x * 0.5) << endl
									<<	round(y + 0.5 + neighDirs[i].y * 0.5) << endl
									<<	round(z + 0.5 + neighDirs[i].z * 0.5) << endl
									<< "==================";
								}

							if(res) {
								bits |= neighBits[i];
							}
						}
						bits = ~bits;

						if(bits != 0x0 && bits != 0xFF) {
							Voxel*	V = new Voxel();

							int X, Y, Z;

							ID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x: x + 1,
								y >= vox.SizeY() / 2? y: y + 1,
								z//z >= vox.SizeZ() * 0.5? z: z + 1
							);

							int prevID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x - 1: x + 2,
								y >= vox.SizeY() / 2? y: y + 1,
								z//z >= vox.SizeZ() * 0.5? z: z + 1
							);
							int nextID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x + 1: x,
								y >= vox.SizeY() / 2? y: y + 1,
								z//z >= vox.SizeZ() * 0.5? z: z + 1
							);

							int backID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x: x + 1,
								y >= vox.SizeY() / 2? y - 1: y + 2,
								z//z >= vox.SizeZ() * 0.5? z: z + 1
							);
							int frontID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x: x + 1,
								y >= vox.SizeY() / 2? y + 1: y,
								z//z >= vox.SizeZ() * 0.5? z: z + 1
							);

							int downID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x: x + 1,
								y >= vox.SizeY() / 2? y: y + 1,
								z - 1
							);
							int upID = vox.VoxelColorID(
								x >= vox.SizeX() / 2? x: x + 1,
								y >= vox.SizeY() / 2? y: y + 1,
								z + 1
							);

							if(ID == 0) {
								//Try 1
								ID = vox.VoxelColorID(
									x >= vox.SizeX() / 2? x + 2: x - 1,
									y,//y >= vox.SizeY() / 2? y - 1: y,
									z//z >= vox.SizeZ() * 0.5? z: z + 1
								);

								//Try 5
								if(ID == 0) {
									for(int i = 8; i < 14; ++i) {
										int newID = vox.VoxelColorID(
											round(x + 0.5 + neighDirs[i].x * 0.5),
											round(y + 0.5 + neighDirs[i].y * 0.5),
											round(z + 0.5 + neighDirs[i].z * 0.5)
										);
										if(newID != 0) {
											ID = newID;
											break;
										}
									}
								}

								//Try 2
								if(ID == 0) {
									if(prevID == 0) {
										ID = nextID;
									} else {//if (nextID == 0) {
										ID = prevID;
									}
								}

								//Try 3
								if(ID == 0) {
									if(backID == 0) {
										ID = frontID;
									} else {//if (nextID == 0) {
										ID = backID;
									}
								}

								//Try 4
								if(ID == 0) {
									if(upID == 0) {
										ID = downID;
									} else {//if (nextID == 0) {
										ID = upID;
									}
								}

								//Try 6
								if(ID == 0) {
									for(int i = 0; i < 8; ++i) {
										int newID = vox.VoxelColorID(
											round(x + 0.5 + neighDirs[i].x * 0.5),
											round(y + 0.5 + neighDirs[i].y * 0.5),
											round(z + 0.5 + neighDirs[i].z * 0.5)
										);
										if(newID != 0) {
											ID = newID;
											break;
										}
									}
								}
							}
							//	ID = 215;

							auto it = find(colorList.begin(), colorList.end(), ID);
							if(it == colorList.end()) {
								colorList.push_back(ID);
								V->colorListIndex = colorList.size() - 1;
							} else {
								V->colorListIndex = it - colorList.begin();
							}
							V->colorListIndex += 1;

							for(int i = 0; i < 12; ++i) {
								V->vertex[i].Set(0, 0, 0, -1);
								V->triangle[i].Set(0, 0, 0, -1);
							}

							for(int i = 0; i < 5; ++i) {
								if(triangulation[bits][(i * 3) + 0] == -1) {
									break;
								}

								X = x * 2;
								Y = y * 2;
								Z = z * 2;

								int vIDX = vID;
								V->vertex[(i * 3) + 0].Set(
									edges[triangulation[bits][(i * 3) + 0]]->x + X,
									edges[triangulation[bits][(i * 3) + 0]]->z + Z,
									edges[triangulation[bits][(i * 3) + 0]]->y + Y,
									vID++
								);

								V->vertex[(i * 3) + 2].Set(
									edges[triangulation[bits][(i * 3) + 1]]->x + X,
									edges[triangulation[bits][(i * 3) + 1]]->z + Z,
									edges[triangulation[bits][(i * 3) + 1]]->y + Y,
									vID++
								);

								V->vertex[(i * 3) + 1].Set(
									edges[triangulation[bits][(i * 3) + 2]]->x + X,
									edges[triangulation[bits][(i * 3) + 2]]->z + Z,
									edges[triangulation[bits][(i * 3) + 2]]->y + Y,
									vID++
								);

								V->triangle[i].Set(vIDX, vIDX + 1, vIDX + 2, tID++);
							}

							voxel.push_back(V);
						}
					}
				}
			}

			for(int i = 0; i < 12; ++i) {
				delete edges[i];
			}
			marchingCube = true;
		}

		void SaveOBJ(string outPath, string mtlFile) {
			string vn = "221166554433";
	
			ofstream hFile(outPath, ios::trunc | ios::out);

			hFile
				<< "#Voxel count: " << voxel.size() << '\n'
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

			if(marchingCube) {
				scale *= 0.5;
			}

			for(int id : colorList) {
				hFile 
					<< "vt "
					<< (id * texturePixelSize - halfTexturePixelSize)
					<< " 0.5"
				<< endl;
			}
			hFile << endl;

			float offsetX = -size.x * 0.5f;
			float offsetY = 0;
			float offsetZ = -size.z * 0.25f;

			for(size_t i = 0; i < voxel.size(); ++i) {
				if(voxel[i] != nullptr) {
					for(int j = 0; j < 12; ++j) {
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

					hFile << "\n\n";
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