//////////////////////////////////////////////////////////////
// Name:        Alejandro Huerta                            //
// Subject:     Artificial Intelligence                     //
// Date:        04/26/2015                                  //
// Description: graph_maker.h - takes input file and makes  //
//                  a graph                                 //
//////////////////////////////////////////////////////////////
#include<fstream>
#include<map>
#include<algorithm>
#include<vector>
#include<sstream>
#include<iostream>
#include<cctype>
#include<string>
#include<iterator>
#include<cstdlib>
using namespace std;

#include "graph.h"
//#include "road.h"

#define DEBUG 0

class graph_maker {
private:
    string point_to_point_path;
    string straight_line_distance_path;
    
    ifstream paths_input;
    ifstream distance_input;

    bool graph_created;
    bool table_created;

    graph<string, int> paths_graph;
    map<string, int> distance_table;

//    // Based on a vector of tokens generate the proper road
//    road extract_road(vector<string> t) {
//        double risk_level = atof(t[t.size() - 1].c_str());
//        double road_quality = atof(t[t.size() - 2].c_str());
//        int distance = atoi(t[t.size() - 3].c_str());
//        
//        return road(distance, road_quality, risk_level);
//    }

    // Based on a vector of tokens generate the proper road length
    int extract_road_length(vector<string> t) {
        int distance = atoi(t[t.size() - 1].c_str());
        
        return distance;
    }

    // Using vector of tokens return the name of the road
    string extract_road_name(vector<string> t) {
        string destination = "";
        for (int i = 0; i < t.size() - 1; i++) {
            if (i + 1 != t.size() - 1)
                destination += t[i] + " ";
            else
                destination += t[i];
        }

        return destination;
    }

    // Using vector of tokens return the name and distance
    // for a straight line distance record in a pair
    pair<string, int> extract_sld_record(vector<string> t) {
        string destination = "";
        int distance = atoi(t[t.size() - 1].c_str());
        for (int i = 0; i < t.size() - 1; i++) {
            if (i + 1 != t.size() - 1)
                destination += t[i] + " ";
            else
                destination += t[i];
        }
        return pair<string, int>(destination, distance);
    }

public:
    // Sets paths for input files
    graph_maker(string ptp_path, string sld_path) {
        point_to_point_path = ptp_path;
        straight_line_distance_path = sld_path;
        
        graph_created = false;
        table_created = false;
    }

    // Create a graph representation based on point to point information
    bool create_graph() {
        if (!graph_created) {
            paths_input.open(point_to_point_path.c_str());
            int num_lines = 0;
            string parent_location = "";
            
            // Read in first line
            string line;
            getline(paths_input, line);
            num_lines++;
            while (!paths_input.eof()) {
                // Check if line is a parent location, i.e. in the input
                // the location doesn't have path info but has paths to
                // other locations following it
                if (!isspace(line[0])) {
                    parent_location = line;
                    if (!isalpha(parent_location[parent_location.length() - 1]))
                        parent_location = parent_location.substr(0, parent_location.length() - 1);

                    getline(paths_input, line);
                    num_lines++;
                    if (DEBUG)
                        cout << parent_location << endl;

                    // Path information from a parent location is noted
                    // by whitespace in front
                    while (isspace(line[0]) && !paths_input.eof()) {

                        // Tokenize path information
                        istringstream iss(line);
                        vector<string> tokens;
                        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
                        
                        //road rd = extract_road(tokens);
                        int rd_length = extract_road_length(tokens);
                        string rd_name = extract_road_name(tokens);
                        if (DEBUG) {
                            cout << rd_name << endl;
                            cout << rd_length << endl;
                        }
                        
                        paths_graph.add_edge(parent_location, rd_name, rd_length);

                        getline(paths_input, line);
                        num_lines++;
                    }
                    if (DEBUG)
                        cout << "=======" << endl;
                    parent_location = "";
                }
            }
            if (DEBUG) {
                cout << num_lines << endl;
                paths_graph.print();
            }
            graph_created = true; 
            return true;
        }
        else {
            return false;
        }
    }
    
    // Return graph of locations and roads if it has been created
    graph<string, int> get_graph() {
        if (graph_created)
            return paths_graph;
        cout << "Error, call create_graph() first" << endl;
        return graph<string, int>();
    }

    bool create_table() {
        if (!table_created) {
            distance_input.open(straight_line_distance_path.c_str());
            int num_lines = 0;
            
            // Read in first line
            string line;
            getline(distance_input, line);
            num_lines++;
            while (!distance_input.eof()) {
                // Tokenize path information
                istringstream iss(line);
                vector<string> tokens;
                copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));
                
                pair<string, int> record = extract_sld_record(tokens);
                distance_table.insert(record);
                getline(distance_input, line);
                num_lines++;
            }
            if (DEBUG)
                cout << num_lines << endl;
            table_created = true;
            return true;
        }
        else {
            return false;
        }
    }

    map<string, int> get_table() {
        if (table_created)
            return distance_table;
        cout << "Error, call create_table() first" << endl;
        return map<string, int>();
    }
};
