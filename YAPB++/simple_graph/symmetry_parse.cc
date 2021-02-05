#define YAPB_NO_GAP

#include "simple_graph.h"
#include "gason/gason.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>

using namespace std;

// Add own implementation for ease of use
template<typename T>
std::string
tostring(const T& t)
{
  std::ostringstream streamOut("");
  streamOut << t;
  return streamOut.str();
}

// gason method printerror at bottom of file
void printJSONParseError(JsonParseStatus status, char *endptr, char *source,
                         size_t size);
void printSymmetries(set<vector<pair<string, string>>> &permutations);
// constants
const string JSON_VARIABLES_TAG = "nodes_to_swap";
;
const string JSON_SYMMETRIC_TAG = "symmetricChildren";
const string primePrefix = "%";
const string varPrefix = "#";

map<string, set<int>> vars;
// mapping from variables to the nodes containing that variable

map<string, set<int>> colours;
// mapping from colour name to set of nodes of that colour

map<int, string> originalPrimeColours; // mapping from node value to the
                                       // original prime colour

set<map<string, string>> permutations;

// function that returns passed in string with added prefix
string escapeVar(string s) { return varPrefix + s; }

string unescapeVar(string s) {
  while(s.find(varPrefix) == 0)
  {
      s.erase(0, varPrefix.size());
  }
  return s;
}
// adds a prefix to distinguish this as prime variable
string makePrimeVar(string s) { return primePrefix + s; }

// function that converts a primitive json value to a string representation
string jsonPrimitiveToString(JsonValue &o) {
  switch (o.getTag()) {
  case JSON_TAG_STRING:
    return o.toString();
  case JSON_TAG_NUMBER:
    return tostring(o.toNumber());
  case JSON_TAG_BOOL:
    return (o.toBool()) ? "true" : "false";
  default:
    return "not primitive";
  }
}

// function to parse json array into variables map (at this time values of map
// left empty - only keys are set)
map<string, set<int>> parseVariableList(JsonValue &o) {
  map<string, set<int>> vars;
  if (!o.toNode()) // empty array
    return vars;

  // otherwise add all array contents to set
  for (auto i : o) {
    string value = escapeVar(
        jsonPrimitiveToString(i->value)); // convert to string and add prefix

    vars[value]; // adds key to map with empty value
  }
  return vars;
}

// looks through a json object for the symmetric boolean flag returning its
// value (true or false)
// returns false if flag not present
bool hasSymmetricFlag(JsonValue &o) {
  for (auto i : o) {
    if (i->key == JSON_SYMMETRIC_TAG) // checks for symmetric flag
      return i->value.toBool();
  }
  return false;
}

/*creates a new node, addes the edge between it and the specified parent and
stores the mapping from the node value (colour) to its position (vector index)
in the graph.
any nodes to be added that ar not part of the provided json should call this
function with an extra bool set to true to make sure that it is not escaped. */
int addNode(vec1<vec1<UncolouredEdge>> &graph, int parentNode, string value,
            bool escapeValue = true) {
  if (escapeValue) {
    value = escapeVar(value);
  }
  // make new node
  graph.push_back(vec1<UncolouredEdge>());
  int newNode = graph.size();

  // make connection
  graph[parentNode].push_back(newNode);

  // save colour mapping
  if (vars.count(value)) { // this is a variable
    // all variables must be same colour so use name of first variable in var
    // set

    string tempColourName = vars.begin()->first;
    colours[tempColourName].insert(newNode);
    // also save mapping to indevidual variable
    vars[value].insert(newNode);

  } else { // is not variable so just save mapping
    colours[value].insert(newNode);
  }
  return newNode;
}

void jsonToGraph(JsonValue &o, vec1<vec1<UncolouredEdge>> &graph, int currentNode,
                 bool symmetryAllowed = false) {
  switch (o.getTag()) {
  case JSON_TAG_OBJECT: {
    // identify whether contained json arrays are symmetric children
    symmetryAllowed = hasSymmetricFlag(o);
    // make node representing object
    int objectNode = addNode(graph, currentNode, "object", false);
    for (auto i : o) {
      if (i->key != JSON_SYMMETRIC_TAG && i->key != JSON_VARIABLES_TAG) {
        int newNode = addNode(graph, objectNode, i->key);
        jsonToGraph(i->value, graph, newNode, symmetryAllowed);
      }
    }
    break;
  }
  case JSON_TAG_ARRAY: {
    // make node representing array
    int arrayNode = addNode(graph, currentNode, "array", false);

    int index = 0;
    for (auto i : o) {
      if (!symmetryAllowed) { // connect children via asymmetric nodes  to
                              // prevent symmetry in graph

        string middleNodeName = "child" + tostring(index++);

        // create middle node then connect original child to graph via middle
        // node
        int middleNode = addNode(graph, arrayNode, middleNodeName, false);
        jsonToGraph(i->value, graph, middleNode);
      } else { // symmetry allowed so just connect node to graph
        jsonToGraph(i->value, graph, arrayNode);
      }
    }
    break;
  }
  default:
    addNode(graph, currentNode, jsonPrimitiveToString(o));
    break;
  }
}

void addPrimeVariables(vec1<vec1<UncolouredEdge>> &graph, map<string, set<int>> &vars) {
  string tempColourName = makePrimeVar(vars.begin()->first);
  for (pair<string, set<int>> var : vars) {
    graph.push_back(vec1<UncolouredEdge>());
    int newNode = graph.size();
    originalPrimeColours[newNode] = var.first;
    for (int nodeValue : var.second) {
      graph[nodeValue].push_back(newNode);
      // add a prefix to the variable to distinguish as prime variable
      colours[tempColourName].insert(newNode);
    }
  }
}

// reads in the specified file adding in a \0 character at the end
// returned buffer must be returned when no longer used
char *readFile(string fileName, int *length) {
  ifstream file(fileName);
  file.seekg(0, ios::end);
  int fileLength = file.tellg();
  file.seekg(0, ios::beg);
  char *buffer = new char[fileLength + 1];
  file.read(buffer, fileLength);
  buffer[fileLength] = '\0';
  file.close();
  *length = fileLength + 1;
  return buffer;
}

bool parseToJson(char *buffer, int bufferLength, JsonValue &value,
                 JsonAllocator &allocator) {
  char *endptr;
  JsonParseStatus status = jsonParse(buffer, &endptr, &value, allocator);
  if (status != JSON_PARSE_OK) {
    printJSONParseError(status, endptr, buffer, bufferLength);
    return false;
  } else
    return true;
}

void jsonToGraph(JsonValue o, vec1<vec1<UncolouredEdge>> &graph) {
  // get variable list
  for (auto i : o) {
    if (i->key == JSON_VARIABLES_TAG) {
      vars = parseVariableList(i->value);
      break;
    }
  }

  jsonToGraph(o, graph, 1, false);

  addPrimeVariables(graph, vars);
}

void solveJsonGraph(const std::string& filename, SearchOptions so, GraphConfig gc) {
  /*
  if (argc < 2) {
    cout << "usage: " << argv[0] << " filePath [-q]\n-q = quiet mode\n";
    exit(EXIT_FAILURE);
  }
  string quietFlag("-q");
  ostream *out = &cout;
  bool quietMode = false;
  for (int i = 2; i < argc; i++) {
    if (argv[i] == quietFlag) {
      out = new ofstream("");
      quietFlag = true;
    }
  }
  */
  
  //*out << "Parsing JSON\n";

  // readfile, parse to json and convert to graph
  int bufferLength;
  char *buffer = readFile(filename, &bufferLength);
  JsonValue o;
  JsonAllocator allocator;

  if (!parseToJson(buffer, bufferLength, o, allocator))
    exit(EXIT_FAILURE);
  //*out << "converting to graph...\n";
  vec1<vec1<UncolouredEdge>> edges;
  jsonToGraph(o, edges);

  //*out << "graph build.\nLooking for symmetries...\n";

  // build graph object for symmetry detection
  ParsedGraph graph(edges.size());
  graph.edges = edges;

  // load colour groups
  for (pair<string, set<int>> colour : colours) {
    graph.parts.push_back(colour.second);
  }

  vec1<Permutation> solutions = SolveGraph(graph, so, gc, GraphDirected_yes);
  set<int> primeNodeValues = colours[makePrimeVar(vars.begin()->first)];

  // build set of permutations to insure no duplicates
  set<vector<pair<string, string>>> permutations;
  for (auto const &sol : solutions) {
    vector<pair<string, string>> permutation;
    for (int i : primeNodeValues) {
      int newValue = sol[i];
      if (newValue != i) {
        permutation.push_back(
            pair<string, string>(unescapeVar(originalPrimeColours[i]),
                                 unescapeVar(originalPrimeColours[newValue])));
      }
    }
    if (permutation.size() > 0)
      permutations.insert(move(permutation));
  }
  printSymmetries(permutations);
  delete[] buffer;
  //if (quietMode) delete out;
}

// prints symmetries out
void printSymmetries(set<vector<pair<string, string>>> &permutations) {
  cout << "[";
  bool firstJSONObject = true;
  for (vector<pair<string, string>> permutation : permutations) {
    if (!firstJSONObject)
      cout << ",\n";
    else {
      firstJSONObject = false;
      cout << "\n";
    }
    bool firstJSONMember = true;
    cout << "{\n";
    for (pair<string, string> mapping : permutation) {
      if (!firstJSONMember)
        cout << ",\n";
      else
        firstJSONMember = false;
      cout << "\"" << mapping.first << "\":\"" << mapping.second << "\"";
    }
    cout << "\n}";
  }
  if (!firstJSONObject)
    cout << "\n";
  cout << "]\n";
}

// gason code to print error
void printJSONParseError(JsonParseStatus status, char *endptr, char *source,
                         size_t size) {
  static const char *status2string[] = {
      "JSON_PARSE_OK",               "JSON_PARSE_BAD_NUMBER",
      "JSON_PARSE_BAD_STRING",       "JSON_PARSE_BAD_IDENTIFIER",
      "JSON_PARSE_STACK_OVERFLOW",   "JSON_PARSE_STACK_UNDERFLOW",
      "JSON_PARSE_MISMATCH_BRACKET", "JSON_PARSE_UNEXPECTED_CHARACTER",
      "JSON_PARSE_UNQUOTED_KEY",     "JSON_PARSE_BREAKING_BAD"};

  char *s = endptr;
  while (s != source && *s != '\n')
    --s;
  if (s != endptr && s != source)
    ++s;

  int lineno = 0;
  for (char *it = s; it != source; --it) {
    if (*it == '\n') {
      ++lineno;
    }
  }

  int column = (int)(endptr - s);

  fprintf(stderr, "%d:%d: error %s\n", lineno + 1, column + 1,
          status2string[status]);

  while (s != source + size && *s != '\n') {
    int c = *s++;
    switch (c) {
    case '\b':
      fprintf(stderr, "\\b");
      column += 1;
      break;
    case '\f':
      fprintf(stderr, "\\f");
      column += 1;
      break;
    case '\n':
      fprintf(stderr, "\\n");
      column += 1;
      break;
    case '\r':
      fprintf(stderr, "\\r");
      column += 1;
      break;
    case '\0':
      fprintf(stderr, "\"");
      break;
    default:
      fputc(c, stderr);
    }
  }

  fprintf(stderr, "\n%*s\n", column + 1, "^");
}
