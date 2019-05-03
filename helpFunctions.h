#ifndef __HELP_FUNC__
#define __HELP_FUNC__

#include <vector>
#include <string>
#include <algorithm>

#include <cstring>

using namespace std;

bool fileExists(string path) {
	FILE*	handle	= fopen(path.c_str(), "r");
	if(handle == nullptr) {
		return false;
	}
	fclose(handle);
	return true;
}

string tostring(int number) {
	stringstream ss;
	ss	<< number;
	return ss.str();
}
string tostring(float number) {
	stringstream ss;
	ss	<< number;
	return ss.str();
}
float str2float(string str) {
#if __unix__
	return stof(str);
#else
	return atof(str.c_str());
#endif
}

bool startsWith(const string& haystack, const string& needle) {
	return	needle.length() <= haystack.length() 
	&&		equal(needle.begin(), needle.end(), haystack.begin());
}
bool endsWith(const string& haystack, const string& needle) {
	return	needle.length() <= haystack.length() 
	&&		equal(needle.rbegin(), needle.rend(), haystack.rbegin());
}

string tolower(string in) {
	string ret	= in;
	transform(in.begin(), in.end(), ret.begin(), 
		[](unsigned char c) -> unsigned char {
			return tolower(c);
		}
	);
	return ret;
}

void processingBar(unsigned int value, unsigned int max) {
	if(value == 0) {
		cout	<< "Processing[";
		for(unsigned int p = 0; p < max; ++p) {
			cout	<< '_';
		}
		cout	<< ']' << flush;
		return;
	}
	cout	<< "\rProcessing[";
	for(unsigned int p = 0; p < value; ++p) {
		cout	<< '|';
	}
	cout << flush;
}

template<class T> void wipeVector(vector<T*>& list) {
	for(T* element : list) {
		delete	element;
	}
	list.clear();
}

#endif