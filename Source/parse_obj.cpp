#include <iostream>
#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "raytracer.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::ivec4;


//string line; //stores a line of the input
//vector<vec3> vertices; //store the triangle points
//vector<ivec4> facets; //store the face vertex indices
//std::vector<Triangle> triangles; //store the resulting triangles
std::vector<std::string> split(std::string strToSplit, char delimeter);

void load_triangles(std::vector<Triangle>& triangles) {
	string line; //stores
	vector<vec3> vertices;
	vector<ivec4> facets;
	std::ifstream modelFile;
	modelFile.open("Source/cubo.obj", std::ifstream::in);

	if (!modelFile || modelFile.fail()) {
    cout << "Unable to open file" << endl;
    exit(1);   // call system to stop
	}

	// read file line by line create a vec3 for 3D coordinates
	while( std::getline(modelFile, line) ){
		if(line[0] == 'v' && line[1] == ' '){
			stringstream ss(line);
			string item;
			vector<string> splittedStrings;
			int i = 0;
			while(std::getline(ss, item, ' ')){
				if(i != 0){ 
					splittedStrings.push_back(string(item));
				}
				i++;
			}
		    vec3 vec(stof(splittedStrings[1]), stof(splittedStrings[2]), stof(splittedStrings[3]));
			vertices.push_back(vec);
		}
		if(line[0] == 'f'){
			stringstream ss(line);
			string item;
			vector<string> splittedStrings;
			int i = 0;
			while(std::getline(ss, item, ' ')){  //1st char in each item = one of facets vertex index.
				if(i != 0) splittedStrings.push_back(string(item));
				i++;
			}
			// split up this thing below with " " and get the first in each
			//splittedStrings[0] == '1/1/1 2/2/2 3/3/3 4/4/4' 
			ivec4 facet;
		    for(size_t i = 0; i < splittedStrings.size(); i++){
		    	vector<string> words = split(splittedStrings[i], ' '); // 1 word = "10/12/11"

		    	for(size_t j = 0; j < words.size(); j++) { 
		    		vector<string> items = split(words[j], '/');  //1 item = 10
		    		if(i != 4)facet[i] = atoi(items[0].c_str());
		    	}
			}
			facets.push_back(facet);
		}
	}
	vec3 green(  0.15f, 0.75f, 0.15f );
	for(size_t i = 0; i < facets.size(); i++){
		vec3 v0 = vertices[facets[i][0]];
		vec3 v1 = vertices[facets[i][1]];
		vec3 v2 = vertices[facets[i][2]];
		vec3 v3 = vertices[facets[i][3]];
		triangles.push_back(Triangle( v0, v1, v2, green ));
		triangles.push_back(Triangle( v0, v2, v3, green ));
	}
	modelFile.close();
}


std::vector<std::string> split(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}
