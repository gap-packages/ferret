#include "simple_graph.h"
#include "gason/gason.h"
#include<fstream>
#include<iostream>
#include <string>
#include<sstream>
#include <map>
#include <vector>
#include<set>
using namespace std;
//constants
const string JSON_VARIABLES_TAG = "nodes_to_swap";;
const string JSON_SYMMETRIC_TAG = "symmetricChildren";
const string primePrefix = "%";
const string varPrefix = "#";


map<string, set<int>>  vars; //mapping from variables to the nodes containing that variable

map<string, set<int>> colours; //mapping from colour name to set of nodes of that colour





//function that returns passed in string with added prefix
string escapeVar(string s) {
	return varPrefix + s;
} 

//adds a prefix to distinguish this as prime variable
string makePrimeVar(string s) {
	return primePrefix + s;
}


//function that converts a primitive json value to a string representation 
	string jsonPrimitiveToString(JsonValue& o) {
switch (o.getTag()) {
	case JSON_TAG_STRING:
	return o.toString();
		case JSON_TAG_NUMBER:
		return to_string(o.toNumber());
				case JSON_TAG_BOOL:
				return (o.toBool())? "true" : "false";
				default:
return "not primitive";				
}
	}






//function to parse json array into variables map (at this time values of map left empty - only keys are set)
map<string, set<int>> parseVariableList(JsonValue& o) {
	map<string, set<int>> vars;
	if (!o.toNode())  //empty array 
		return vars;
	
	//otherwise add all array contents to set
	for (auto i: o) {
		string value = escapeVar(jsonPrimitiveToString(i->value));  //convert to string and add prefix 
		
		vars[value];//adds key to map with empty value
	}
	return vars;
	}


	//looks through a json object for the symmetric boolean flag returning its value (true or false)
	//returns false if flag not present 
	bool hasSymmetricFlag(JsonValue& o) {
		for (auto i: o) {
			if (i->key == JSON_SYMMETRIC_TAG) //checks for symmetric flag 
				return i->value.toBool();
		}
		return false;
	}

	/*creates a new node, addes the edge between it and the specified parent and stores the mapping from the node value (colour) to its position (vector index) in the graph.
	any nodes to be added that ar not part of the provided json should call this function with an extra bool set to true to make sure that it is not escaped. */
	int addNode(vec1<vec1<int>>& graph, int parentNode, string value, bool escapeValue = true) {
		if (escapeValue)  {
			value = escapeVar(value);
		}
		//make new node
		graph.push_back(vec1<int>());
		int newNode = graph.size();

		//make connection
		graph[parentNode].push_back(newNode);
		
		//save colour mapping
		if (vars.count(value)) { //this is a variable
			//all variables must be same colour so use name of first variable in var set 
			
			string tempColourName = vars.begin()->first;
			colours[tempColourName].insert(newNode);
			//also save mapping to indevidual variable 
			vars[value].insert(newNode);
			
		} else { //is not variable so just save mapping 
		colours[value].insert(newNode);
	}
		return newNode;
	}

void jsonToGraph(JsonValue& o, vec1<vec1<int>>& graph, int currentNode, bool symmetryAllowed = false) {
switch (o.getTag()) {
	case JSON_TAG_OBJECT: 
	//identify whether contained json arrays are symmetric children
	symmetryAllowed = hasSymmetricFlag(o);
	for (auto i: o) {
		if (i->key != JSON_SYMMETRIC_TAG && i->key != JSON_VARIABLES_TAG) {
		int newNode = addNode(graph, currentNode, i->key);
		jsonToGraph(i->value, graph, newNode, symmetryAllowed);
	}
	}
	break;

	
	case JSON_TAG_ARRAY: {
	int index = 0;
	for (auto i: o) {
			if (!symmetryAllowed) {				//connect children via asymmetric nodes  to prevent symmetry in graph
				
				string middleNodeName = "child" + to_string(index++);
						
						//create middle node then connect original child to graph via middle node
				int middleNode = addNode(graph, currentNode, middleNodeName , false);
				jsonToGraph(i->value, graph, middleNode);
			} else { //symmetry allowed so just connect node to graph
				jsonToGraph(i->value, graph, currentNode);
			}
	}
	break;
}
	default: 
	 addNode(graph, currentNode, jsonPrimitiveToString(o));
	break;
}
}


void addPrimeVariables(vec1<vec1<int>>& graph, map<string,  set<int>>& vars) {
				string tempColourName = makePrimeVar(vars.begin()->first);
	for (pair<string, set<int>> var: vars) {
		graph.push_back(vec1<int>());
		int newNode = graph.size();
		for (int nodeValue: var.second) {
			graph[nodeValue].push_back(newNode);
 //add a prefix to the variable to distinguish as prime variable 
			colours[tempColourName].insert(newNode);
		}
	}
}
//reads in the specified file adding in a \0 character at the end
//returned buffer must be deleted when no longer used
	char*  readFile(string fileName) {
		ifstream file(fileName);
		file.seekg(0, ios::end);
		int fileLength = file.tellg();
		file.seekg(0, ios::beg);
		char* buffer = new char[fileLength + 1];
		file.read(buffer, fileLength);
		buffer[fileLength] = '\0';
		file.close();
		return buffer;
	}
		
	
	
	
	JsonValue parseToJson(char* buffer) {
		char *endptr;
		JsonValue value;
		JsonAllocator allocator;
		JsonParseStatus status = jsonParse(buffer, &endptr, &value, allocator);
		if (status != JSON_PARSE_OK) {
			cerr <<  "error at " << endptr;
			exit(EXIT_FAILURE);
		} else
			return value;
		return value;
	}
	

	
void jsonToGraph(JsonValue o, vec1<vec1<int>>& graph) {
	//get variable list
	for (auto i: o) {
		if (i->key == JSON_VARIABLES_TAG) {
			vars = parseVariableList(i->value);
			break;
		}
	}
	
	jsonToGraph(o, graph, 1, false);
	addPrimeVariables(graph, vars);
}	
	
	int main(int argc, char** argv) {
		if (argc < 2) {
			cout <<  "usage: " << argv[0] << " filePath\n";
			exit(EXIT_FAILURE);
		}
//readfile, parse to json and convert to graph		
		char* buffer = readFile(argv[1]);
		JsonValue o = parseToJson(buffer);
		vec1<vec1<int>> edges;
		jsonToGraph(o, edges);
		
//build graph object for symmetry detection 
		Graph graph(edges.size());
		graph.edges = edges;
		
		//load colour groups
		for (pair<string, set<int>> colour: colours) {
			graph.parts.push_back(colour.second);
		}
		
		vec1<Permutation> solutions = SolveGraph(graph);

	    std::cout << "[";
	    for(auto const& sol : solutions)
	    {
	            std::cout << sol.cycle() << ",\n";
	    }
	    std::cout << "()]\n";
		
}