#ifndef __VOX_SPLIT__
#define __VOX_SPLIT__

#include <vector>
#include <cstring>

#include "VOX.h"

int& access3DMap(VOXModel& vox, int* partsMap, int x, int y, int z) {
	return partsMap[z * vox.SizeY() + y * vox.SizeX() + x];
}

void reqursiveSearch(VOXModel& vox, int* partsMap, int x, int y, int z, int ID) {
	if(access3DMap(vox, partsMap, x, y, z) not_eq 0
	or vox.VoxelColorID(x, y, z) == 0
	)
		return;

	access3DMap(vox, partsMap, x, y, z) = ID;

	reqursiveSearch(vox, partsMap, x + 1, y, z, ID);
	reqursiveSearch(vox, partsMap, x + 1, y, z, ID);
	reqursiveSearch(vox, partsMap, x, y + 1, z, ID);
	reqursiveSearch(vox, partsMap, x, y + 1, z, ID);
	reqursiveSearch(vox, partsMap, x, y, z + 1, ID);
	reqursiveSearch(vox, partsMap, x, y, z + 1, ID);
}

vector<VOXModel*> SplitVOX(VOXModel& vox) {
	int		uniqueParts = 1;

	size_t	mapSize = vox.SizeX() * vox.SizeY() * vox.SizeZ();
	int*	map		= new int[mapSize];
	memset(map, 0, mapSize * sizeof(int));

	for(int z = 0; z < vox.SizeZ(); ++z)
		for(int y = 0; y < vox.SizeY(); ++y)
			for(int x = 0; x < vox.SizeX(); ++x)
				if(vox.VoxelColorID(x, y, z) != 0)
					reqursiveSearch(vox, map, x, y, z, uniqueParts++);
			//--
		//--
	//--

	uniqueParts -= 1;

	vector<VOXModel*>	list;

	for(int i = 0; i < uniqueParts; ++i) {
		for(int z = 0; z < vox.SizeZ(); ++z)
			for(int y = 0; y < vox.SizeY(); ++y)
				for(int x = 0; x < vox.SizeX(); ++x)
					if(access3DMap(vox, map, x, y, z) == (i + 1)) {
						//Subpart exists?
						if(list.size() < size_t(i + 1)) {
							VOXModel* temp	= new VOXModel();
							//New space
							temp->SetSize(vox.SizeX(), vox.SizeY(), vox.SizeZ());

							//Copy orginal palette
							for(int i = 0; i < 256; ++i) {
								temp->SetPaletteColor(i, vox.PaletteColor(i));
							}

							//Add to subpart list
							list.push_back(temp);
						}
						//Copy voxels
						int	clrID = vox.VoxelColorID(x, y, z);
						list[i]->SetVoxel(x, y, z, clrID);
					}
				//--
			//--
		//--
	}


	delete[]	map;
	return list;
}


#endif