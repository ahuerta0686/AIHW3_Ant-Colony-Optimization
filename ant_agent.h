#include <list>
#include <set>
#include <iostream>

class ant_agent {
private:
    list<string> tour_history;
    set<string> visited_locations;
    double distance_travelled;
    graph<string, int>* location_graph;
    
public:
    ant_agent(graph<string, int>& g, string start) {
        tour_history = list<string>();
        visited_locations = set<string>();
        tour_history.push_back(start);
        visited_locations.insert(start);
        distance_travelled = 0;
        location_graph = &g;
    }

	void reset_ant() {
		tour_history.clear();
		visited_locations.clear();
		distance_travelled = 0;
	}

    // Check if an ant agent has visited a location already
    bool has_visited(string location) {
        for (set<string>::iterator it = visited_locations.begin(); it != visited_locations.end(); ++it) {
            if (*it == location)
                return true;
        }
        return false;
    }

    // Mark a location as visited
    void visit_location(string location) {
        if (!has_visited(location)) {
            distance_travelled += location_graph->get_edge_cost(get_location(), location);
            tour_history.push_back(location);
            visited_locations.insert(location);
        }
    }

    // Step back a location in case the agent got stuck
    void step_backwards() {
        string removed = tour_history.back();
        tour_history.pop_back();
        distance_travelled -= location_graph->get_edge_cost(get_location(), removed);
    }

    // Get the current location of the ant agent
    string get_location() {
        return tour_history.back();
    }

    double get_distance_travelled() {
        return distance_travelled;
    }

    list<string> get_tour_history() {
        return tour_history;
    }
    
    void print_tour_history() {
        for (list<string>::iterator it = tour_history.begin(); it != tour_history.end(); ++it) {
            cout << *it << " ";
        }
		cout << endl;
    }


};
