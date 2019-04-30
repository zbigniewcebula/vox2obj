#ifndef __VOX_SPLIT__
#define __VOX_SPLIT__

#include <vector>
#include <cstring>

#include <iomanip>

#include "VOX.h"

void reqursiveSearch(VOXModel& vox, VOXModel& map, int x, int y, int z, int ID) {
	if(x >= 0 and y >= 0 and z >= 0
	and x < vox.SizeX() and y < vox.SizeY() and z < vox.SizeZ()
	) {
		if(map.VoxelColorID(x, y, z) == 0 and vox.VoxelColorID(x, y, z) != 0) {
			map.SetVoxel(x, y, z, ID);

			if(vox.VoxelColorID(x + 1, y, z) != 0) {
				reqursiveSearch(vox, map, x + 1, y, z, ID);
			}
			if(vox.VoxelColorID(x - 1, y, z) != 0) {
				reqursiveSearch(vox, map, x - 1, y, z, ID);
			}
			if(vox.VoxelColorID(x, y + 1, z) != 0) {
				reqursiveSearch(vox, map, x, y + 1, z, ID);
			}
			if(vox.VoxelColorID(x, y - 1, z) != 0) {
				reqursiveSearch(vox, map, x, y - 1, z, ID);
			}
			if(vox.VoxelColorID(x, y, z + 1) != 0) {
				reqursiveSearch(vox, map, x, y, z + 1, ID);
			}
			if(vox.VoxelColorID(x, y, z - 1) != 0) {
				reqursiveSearch(vox, map, x, y, z - 1, ID);
			}
		}
	}
}

vector<VOXModel*> SplitVOX(VOXModel& vox) {
	vector<VOXModel*>	list;

	VOXModel	map;
	map.SetSize(vox.SizeX(), vox.SizeY(), vox.SizeZ());

	for(int z = 0; z < vox.SizeZ(); ++z)
		for(int y = 0; y < vox.SizeY(); ++y)
			for(int x = 0; x < vox.SizeX(); ++x)
				if(vox.VoxelColorID(x, y, z) != 0 and map.VoxelColorID(x, y, z) == 0) {
					reqursiveSearch(vox, map, x, y, z, list.size() + 1);

					//New part
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
			//--
		//--
	//--

	for(int z = 0; z < vox.SizeZ(); ++z)
		for(int y = 0; y < vox.SizeY(); ++y)
			for(int x = 0; x < vox.SizeX(); ++x) {
				//Detect part
				int partID = map.VoxelColorID(x, y, z);
				if(partID > 0) {
					//Copy voxels
					int	clrID = vox.VoxelColorID(x, y, z);
					list[partID - 1]->SetVoxel(x, y, z, clrID);
				}
			}
		//--
	//--

	return list;
}


#endif