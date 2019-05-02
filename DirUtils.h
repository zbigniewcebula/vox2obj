#ifndef __DIR_UTILS__
#define __DIR_UTILS__

#include <vector>
#include <string>

#include <dirent.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef __unix__
	#define createDir(PTH)	mkdir(PTH, 0755)
#else
	#include "windows.h"
	#define createDir(PTH)	CreateDirectory(PTH, NULL)
#endif

using namespace std;

bool dirExists(string path) {
	struct stat info;
	if(stat(path.c_str(), &info) != 0)
		return false;
	return bool(info.st_mode & S_IFDIR);
}

inline bool isDir(string path) {
	//Lazy wrap for code's beauty sake...
	return dirExists(path);
}

void getDirectoriesList(vector<string>& searchIn) {
	vector<string> found;

	for(string path : searchIn) {
		if(isDir(path)) {
			DIR*	dir	= opendir(path.c_str());
			dirent*	dp	= readdir(dir);
			do {
				string name	= dp->d_name;
				if(name != "." and name != "..")
					if(isDir(path + "/" + name)) {
						found.push_back(path + "/" + name);
					}
				//--
				dp	= readdir(dir);
			} while(dp not_eq NULL);
			closedir(dir);
		}
	}
	if(found.size() == 0)
		return;

	getDirectoriesList(found);
	searchIn.insert(searchIn.end(), found.begin(), found.end());
}

void createDirectoryTree(string where, vector<string>& dirsList) {
	if(!isDir(where)) {
		createDir(where.c_str());
	}
	for(string path : dirsList) {
		createDir((where + "/" + path).c_str());
	}
}

#endif