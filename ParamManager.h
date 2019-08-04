#ifndef __PARAMS_MANAGER__
#define __PARAMS_MANAGER__

#include <vector>
#include <iostream>
#include <algorithm>

#include "helpFunctions.h"

using namespace std;

class ParamManager {
	public:
		class Param {
			public:
				string small;
				string big;
				string description;
				string valueDescription;

				string value	= "";

				Param() {}
				Param(string s, string b, string d, string vd)
					:	small(s), big(b), description(d), valueDescription(vd)
				{}
				Param(const Param& org) {
					small				= org.small;
					big					= org.big;
					description			= org.description;
					valueDescription	= org.valueDescription;
					value				= org.value;
				}

				void print() {
					cout	<< right << setw(7) << small << (small.empty()? "": ", ") << left << setw(14) << big
							<< setw(20) << valueDescription
							<< '\t' << setw(40) << description << '\n';
				}

				bool operator==(const Param& rhs) {
					return rhs.small == small and rhs.big == big and description == rhs.description;
				}
				bool operator==(const string& rhs) {
					return (small == rhs) or (big == rhs);
				}
		};
	private:
		vector<Param>	params;

	public:
		ParamManager() {
			addParam("-h", "--help", "Shows help", "");
		}

		void addParam(string small, string big, string description, string valueDescription) {
			if(find(params.begin(), params.end(), small) == params.end()
			and find(params.begin(), params.end(), big) == params.end()
			) {
				params.emplace_back(small, big, description, valueDescription);
			}
		}
		void addParamSeparator() {
			params.emplace_back("", "", "", "");
		}

		bool exists(string flag) {
			return find_if(params.begin(), params.end(), [&](Param& p) -> bool {
				return p == flag;
			}) != params.end();
		}

		bool hasValue(string flag) {
			return getValueOf(flag) != "";
		}

		string getValueOf(string flag) {
			auto	it = find_if(params.begin(), params.end(), [&](Param& p) -> bool {
				return p == flag;
			});
			if(it != params.end()) {
				return (*it).value;
			}
			return "";
		}

		void printList() {
			for(Param p : params) {
				p.print();
			}
			cout	<< flush;
		}

		void printHelp() {
			cout	<< "Usage: vox2obj [OPTION [VALUE]]\n\n"
					<< "Converts VOX file(s) into OBJ format file(s)\n\n"
					<< "Available options:\n";
			printList();
			cout	<< "\nFor more visit: https://github.com/zbigniewcebula/vox2obj\n"
					<< endl;
		}

		bool process(int argc, char** argv) {
			string		tempStr;
			bool		paramOverload	= false;
			auto		lastParam		= params.end();
			for(int i = 1; i < argc; ++i) {
				tempStr	= argv[i];
				if(startsWith(tempStr, "-")) {
					lastParam	= find_if(params.begin(), params.end(), [&](Param& p) -> bool {
						return p == tempStr;
					});
					if(lastParam == params.end()) {
						cerr	<< "Unknown parameter \"" << tempStr << "\"! Aborting..." << endl;
						return false;
					} else if((*lastParam) == "-h") {
						printHelp();
						return false;
					} else if((*lastParam) == "-s" || (*lastParam) == "-ms"
						|| (*lastParam) == "-fl" || (*lastParam) == "-mc"
						 || (*lastParam) == "-t"
					) {
						(*lastParam).value	= "1";
					} else if((*lastParam).value != "") {
						cerr	<< "Parameter \"" << tempStr << "\" used multiple times! Aborting..." << endl;
						return false;
					}
				} else {
					if(lastParam != params.end()) {
						(*lastParam).value	= tempStr;
					} else {
						paramOverload	= true;
					}
					lastParam	= params.end();
				}
			}
			if(paramOverload) {
				cout	<< "WARNING! Too much parameters, ignoring not used..." << endl;
			}
			return true;
		}
};

#endif