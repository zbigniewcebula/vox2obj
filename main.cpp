#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>

#include <cctype>
#include <cstring>

#include "VOX.h"
#include "VOXSplit.h"
#include "Model.h"

#include "helpFunctions.h"

#include "ParamManager.h"
#include "DirUtils.h"

using namespace std;

int singleVOX2OBJ(
	string inPath, string outPath, string mtlPath,
	bool split = false, float scale = 0.03125f,
	bool markSides = false, bool marchingCubes = false,
	const char* modelName = nullptr
);

int main(int argc, char** argv) {
	//Checking args
	if(argc <= 1) {
		cerr	<< "No parameters given! Aborting..." << endl;
		return 1;
	}

	ParamManager	paramManager;
	paramManager.addParamSeparator();
	paramManager.addParam("-i", "--in", "Sets input VOX file", "INPUT_VOX");
	paramManager.addParam("-o", "--out", "Sets output OBJ file (overrites existing file!)", "OUTPUT_OBJ");
	paramManager.addParam("-id", "--in-dir", "Sets input directory for recursively find VOX files (use with -od flag, relative dir)", "INPUT_VOX_DIR");
	paramManager.addParam(
		"-od", "--out-dir", "Sets output directory for recursively found VOX files (use with -id flag, relative dir), copying -id dirs structure", "OUTPUT_VOX_DIR"
	);
	paramManager.addParamSeparator();
	paramManager.addParam("-m", "--mtl", "Sets output MTL file (used in OBJs)", "INPUT_MTL");
	paramManager.addParam("-cm", "--create-mtl", "Creates default MTL (overrites existing file!)", "OUTPUT_MTL");
	paramManager.addParam("-t", "--texture", "Uses texture (use with -cm flag)", "INTPUT_TEXTURE");
	paramManager.addParam("-s", "--split", "Takes separated parts of VOX model and splits them into separated OBJs", "");
	paramManager.addParam("-as", "--auto-split", "Uses -s flag on every file with given sufinx in VOX file name (overrites -s flag)", "SUFIX");
	paramManager.addParam("-sc", "--scale", "Changes scale of output OBJ, default: 0.03125", "SCALE");
	paramManager.addParam("-ms", "--mark-sides", "Adds side indicator L or R to OBJ file name after spliting (use with -s or -as flags)", "");
	paramManager.addParam("-mc", "--marching-cubes", "Uses Marching Cubes algorithm to generate output OBJ", "");

	paramManager.addParam("-if", "--ignore-filter", "Ignores conversion of file if name/path contains given phrase (use with -id and -od)", "IGNORE_PHRASE");

	paramManager.addParam("-fl", "--fail-log", "Creates log for failed VOX conversions (use with -id and -od)", "OUTPUT_LOG");

	if(paramManager.process(argc, argv) == false)
		return 1;
	//--

	//MTL Creation
	string mtlCreatePath = paramManager.getValueOf("-cm");
	if(mtlCreatePath != "") {
		if(fileExists(mtlCreatePath)) {
			cerr	<< "MTL creation file exists! Overriting!" << endl;
		}

		string texturePath = paramManager.getValueOf("-t");
		if(texturePath == "")	texturePath = "texture.png";

		fstream hMTL(mtlCreatePath, ios::trunc | ios::out);
		hMTL
			<< "newmtl " << mtlCreatePath << endl
			<< "illum 1" << endl
			<< "Ka 0.000 0.000 0.000" << endl
			<< "Kd 1.000 1.000 1.000" << endl
			<< "Ks 0.000 0.000 0.000" << endl
			<< "map_Kd " << texturePath
		<< endl;
		hMTL.flush();
		hMTL.close();
	}

	//Convertion
	bool multipleFiles = (paramManager.hasValue("-id") and paramManager.hasValue("-od"));
	if(multipleFiles) {
		if(isDir(paramManager.getValueOf("-id"))) {
			vector<string> dirs;
			vector<string> voxFiles;

			string	outDir			= paramManager.getValueOf("-od");
			string	ignorePhrase	= paramManager.getValueOf("-if");
			bool	ignoreFilter	= paramManager.hasValue("-if");

			//Find directories structure
			dirs.push_back(paramManager.getValueOf("-id"));
			getDirectoriesList(dirs);
			//Copy structure
			createDirectoryTree(outDir, dirs);

			/////////////////////////////////

			ofstream hLog;
			if(paramManager.hasValue("-fl"))
				hLog.open(paramManager.getValueOf("-fl"), ios::trunc | ios::out);

			float	scale			= paramManager.hasValue("-sc")? str2float(paramManager.getValueOf("-sc")): 0.03125f;

			string	mtlFile 		= paramManager.getValueOf("-m");
			bool	doSplit			= paramManager.hasValue("-s");
			string	autoSplitVal	= paramManager.getValueOf("-as");
			int		fileIdx	= 1;
			for(string path : dirs) {
				if(isDir(path)) {
					DIR*	dir	= opendir(path.c_str());
					dirent*	dp	= readdir(dir);
					do {
						string name	= dp->d_name;
						if(ignoreFilter and name.find(ignorePhrase) != string::npos) {
							dp	= readdir(dir);
							cout	<< "[IGNORED] " << outDir << '/' << path << '/' << name << endl;
							continue;
						}

						if(name != "." and name != "..") {
							if(endsWith(tolower(name), ".vox")) {
								if(not autoSplitVal.empty()) {
									string ending	= name.substr(0, name.length() - 4);
									doSplit			= endsWith(ending, autoSplitVal);
								}

								string out		= outDir + '/' + path + '/' + name;
								out.replace(out.find_last_of(".vox") - 3, 4, ".obj");

								cout	<< "[" << (fileIdx++) << "] " << out << " => " << flush;
								int result = singleVOX2OBJ(
									path + "/" + name, out,
									mtlFile, doSplit, scale,
									paramManager.hasValue("-ms"), paramManager.hasValue("-mc"),
									name.substr(0, name.length() - 4).c_str()
								);
								if(result > 0) {
									cout << "Done! (" << result << " parts)" << endl;
								} else {
									cout << "Fail!!!!!!!!!!" << endl;
									if(paramManager.hasValue("-fl") and hLog.good())
										hLog << "Cannot convert file: " << (path + "/" + name) << endl;
								}
							}
						}
						dp	= readdir(dir);
					} while(dp not_eq NULL);
					closedir(dir);
				}
			}
			if(paramManager.hasValue("-fl") and hLog.good())
				hLog.close();
		}
	} else {
		if(paramManager.hasValue("-id") or paramManager.hasValue("-od")) {
			cerr << "Flags -id and -od are usefull only when used together! Aborting...";
			return 1;
		} else {
			string	name			= paramManager.getValueOf("-i");
			bool	doSplit			= paramManager.hasValue("-s");
			string	autoSplitVal	= paramManager.getValueOf("-as");
			if(not autoSplitVal.empty()) {
				string ending	= tolower(name).substr(0, name.length() - 4);
				doSplit			= endsWith(ending, autoSplitVal);
			}

			if(singleVOX2OBJ(
				name, paramManager.getValueOf("-o"),
				paramManager.getValueOf("-m"), doSplit,
				paramManager.hasValue("-sc")? str2float(paramManager.getValueOf("-sc")): 0.03125f,
				paramManager.hasValue("-ms"), paramManager.hasValue("-mc"),
				name.substr(0, name.length() - 4).c_str()
			) == 0) {
				return 1;
			}
		}
	}

	return 0;
}

int singleVOX2OBJ(string inPath, string outPath, string mtlPath, bool split, float scale, bool markSides, bool marchingCubes, const char* modelName) {
	VOXModel	vox;
	Model 		model;
	if(modelName != nullptr) {
		model.name = modelName;
	}
	model.SetScale(scale);
	
	if(inPath.empty()) {
		cerr	<< "No input file given! Aborting!" << endl;
		return 0;
	} else if(not vox.Load(inPath)) {
		cerr	<< "Cannot load VOX file: '" << inPath << "'... Aborting!" << endl;
		return 0;
	}

	if(outPath != "") {
		if(fileExists(outPath)) {
			cerr	<< "Output file exists! Overriting!" << endl;
		}
	} else {
		cerr	<< "No output file path given! Using '" << inPath << ".obj'!" << endl;
		outPath = inPath + ".obj";
	}

	if(mtlPath.empty())
		mtlPath	= "material.mtl";

	if(split) {
		vector<VOXModel*> models = SplitVOX(vox);
		
		if(models.size() > 1) {
			int	partNum = 0;

			size_t	lastOBJ = outPath.find_last_of(".obj");
			if(lastOBJ != string::npos) {
				outPath.replace(lastOBJ - 3, 4, "_");
			}

			string sideAddition;
			for(VOXModel* mdl : models) {
				if(marchingCubes) {
					model.LoadVOXMarchingCubes(*mdl);
				} else {
					model.LoadVOX(*mdl);
				}
				if(markSides) {
					vec4f center = model.Center();
					if(center.x < 0) {
						sideAddition = "_L";
					} else if(center.x > 0) {
						sideAddition = "_R";
					} else {
						sideAddition = "_C";
					}
					cout << sideAddition << ", ";
				}
				model.SaveOBJ(
					outPath + tostring(partNum++) + sideAddition + ".obj",
					mtlPath
				);
				delete mdl;
				model.Clear();
			}
			partNum	= models.size();
			models.clear();
			return partNum;
		}
	}
	if(marchingCubes) {
		model.LoadVOXMarchingCubes(vox);	
	} else {
		model.LoadVOX(vox);
	}
	
	model.SaveOBJ(outPath, mtlPath);
	return 1;
}