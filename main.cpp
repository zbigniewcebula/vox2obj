#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>

#include <cctype>
#include <cstring>

#include <dirent.h>
#include <errno.h>

#include "VOX.h"
#include "VOXSplit.h"
#include "Model.h"

#include "helpFunctions.h"
#include "ParamsManager.h"


using namespace std;

int main(int argc, char** argv) {
	//Checking args
	if(argc <= 1) {
		cerr	<< "No parameters given! Aborting..." << endl;
		return 1;
	}

	ParamsManager	paramManager;
	paramManager.addParam("-h", "--help", "Shows help", "");
	paramManager.addParamSeparator();
	paramManager.addParam("-i", "--in", "Sets input VOX file", "INPUT_VOX");
	paramManager.addParam("-o", "--out", "Sets output OBJ file (overrites existing file! disabled -i and -o flag)", "OUTPUT_OBJ");
	//TODO
	paramManager.addParam("-id", "--in-dir", "Sets input directory for recursively find VOX files (use with -od flag)", "INPUT_VOX_DIR");
	//TODO
	paramManager.addParam(
		"-od", "--out-dir", "Sets output directory for recursively found VOX files (use with -id flag)", "OUTPUT_VOX_DIR"
	);
	paramManager.addParamSeparator();
	paramManager.addParam("-m", "--mtl", "Sets output MTL file (used in OBJs)", "INPUT_MTL");
	paramManager.addParam("-cm", "--create-mtl", "Creates default MTL (overrites existing file!)", "OUTPUT_MTL");
	paramManager.addParam("-t", "--texture", "Uses texture (use with -cm flag)", "INTPUT_TEXTURE");
	paramManager.addParam("-s", "--split", "Takes separated parts of VOX model and splits them into separated OBJs", "");
	paramManager.addParam("-sc", "--scale", "Takes separated parts of VOX model and splits them into separated OBJs", "");


	if(paramManager.process(argc, argv) == false)
		return 1;
	//--

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

	VOXModel	vox;
	string		inputPath	= paramManager.getValueOf("-i");
	string		outputPath	= paramManager.getValueOf("-o");
	if(inputPath != "") {
		if(not vox.Load(inputPath)) {
			cerr	<< "Cannot load VOX file: '" << inputPath << "'... Aborting!" << endl;
			return 1;
		}
	} else {
		cerr	<< "No input file given! Aborting!" << endl;
		return 1;
	}
	if(outputPath != "") {
		if(fileExists(outputPath)) {
			cerr	<< "Output file exists! Overriting!" << endl;
		}
	} else {
		cerr	<< "No output file path given! Using '" << inputPath << ".obj'!" << endl;
		outputPath = inputPath + ".obj";
	}

	string		mtlPath	= paramManager.getValueOf("-m");
	if(paramManager.exists("-s") != paramManager.badParameter()) {
		vector<VOXModel*> models = SplitVOX(vox);
		int	partNum = 0;

		size_t	lastOBJ = outputPath.find_last_of(".obj");
		if(lastOBJ != string::npos) {
			outputPath.replace(lastOBJ - 3, 4, "_");
		}

		for(VOXModel* mdl : models) {
			Model model;
			model.LoadVOX(*mdl);
			model.SaveOBJ(
				outputPath + tostring(partNum++) + ".obj",
				mtlPath == ""? "material.mtl": mtlPath
			);
			delete mdl;
		}
		models.clear();
	} else {
		Model model;
		model.LoadVOX(vox);
		model.SaveOBJ(outputPath, mtlPath == ""? "material.mtl": mtlPath);
	}

	return 0;
}