#include <iostream>
#include <math.h>
#include <map>
#include <ctime>
#include <vector>
#include <functional>
#include <algorithm>

#include "graph_maker.h"
#include "ant_agent.h"

using namespace std;

string pick_edge(list<graph<string, int>::edge> edges, double alpha, double beta, ant_agent a);
string random_distribution_select(map<double, string, greater<double> > distributions);
double random_double(double min, double max);
double calculate_pheromone(double Q, ant_agent a);
void evaporate_pheromone(graph<string, int> g, double rho);
void deposit_pheromone(graph<string, int> g, list<string> path, double pheromone, double rho);
bool test_covergence(vector<ant_agent> vec_ants);

int main() {
    srand(time(NULL));
    graph_maker generator("./input_files/point_to_point.ants", "");
    generator.create_graph();
    
    graph<string, int> romania_graph;
    romania_graph = generator.get_graph();

    double pheromone_weight      =   2;
    double heuristic_weight      =   1;
    double pheromone_persistence = 0.8;
    double pheromone_units       = 100;

    int population_size          =  10;
    int maximum_cycles           =  50;

    string start_location =     "Arad";
    string end_location =  "Bucharest";


    cout << "=======================" << endl;
    cout << " Travelling in Romania " << endl;
    cout << "=======================" << endl;
    cout << " - Enter α: ";
    cin >> pheromone_weight;

    cout << " - Enter β: ";
    cin >> heuristic_weight;

    cout << " - Enter ρ: ";
    cin >> pheromone_persistence;

    cout << " - Enter Q: ";
    cin >> pheromone_units;

    cout << " - Enter population size: ";
    cin >> population_size;
    
    cout << " - Enter maximum cycles: ";
    cin >> maximum_cycles;
    
    cout << " - Enter start location: ";
    cin >> start_location;

    cout << " - Enter end location: ";
    cin >> end_location;



    for (int i = 0; i < maximum_cycles; i++) {
		vector<ant_agent> ants(10, ant_agent(romania_graph, start_location));
        for (int j = 0; j < population_size; j++) {
            while (ants[j].get_location() != end_location) {
                list<graph<string, int>::edge> near_edges = romania_graph.find_vertex(ants[j].get_location())->neighbors;
                string next_location = pick_edge(near_edges, pheromone_weight, heuristic_weight, ants[j]);
                if (next_location == "")
                    ants[j].step_backwards();
				else
					ants[j].visit_location(next_location);
            }
        }
        evaporate_pheromone(romania_graph, pheromone_persistence);
        for (int j = 0; j < population_size; j++) {
            double pheromone_amount = calculate_pheromone(pheromone_units, ants[j]);            
            deposit_pheromone(romania_graph, ants[j].get_tour_history(), pheromone_amount, pheromone_persistence);
        }

        cout << "Tour " << i << " paths:" << endl;
        for (int j = 0; j < population_size; j++) {
            ants[j].print_tour_history();
            cout << "===========================================================" << endl;
        }
        cout << endl << endl;
//        if (test_covergence(ants))
//            break;
    }


    return 0;
}

// Ant movement formula: Choose an edge based on the pheromone and heuristic information
//      and the weighting factors alpha and beta
string pick_edge(list<graph<string, int>::edge> edges, double alpha, double beta, ant_agent a) {
    int dead_end_check = edges.size();
    double denominator = 0;
    for (list<graph<string, int>::edge>::iterator it = edges.begin(); it != edges.end(); ++it) {
        graph<string, int>::edge e = *it;
        if (!a.has_visited(e.end->data)) {
			double left = pow(e.pheromone, alpha);
			double right = pow(e.cost, beta);
			denominator += left * right;
        }
        else {
            dead_end_check--;
        }
    }

	if (dead_end_check == 0)
		return "";
    
/*
    if (DEBUG)
        cout << "Denominator: " << denominator << endl;
*/
    map<double, string, greater<double> > edge_probabilities;
    for (list<graph<string, int>::edge>::iterator it = edges.begin(); it != edges.end(); ++it) {
        graph<string, int>::edge e = *it;
        if (!a.has_visited(e.end->data)) {
            double numerator = pow(e.pheromone, alpha) * pow((double)e.cost, beta);
/*
            if (DEBUG)
                cout << "Numerator: " << numerator << " = " << pow(e.pheromone, alpha) << " * " << pow(e.cost, beta) << endl;
*/
            edge_probabilities.insert(pair<double, string>(numerator / denominator, e.end->data));
        }
    }
/*
    for (map<double, string>::iterator it = edge_probabilities.begin(); it != edge_probabilities.end(); ++it) {
        pair<double, string> entry = *it;
        cout << entry.first << ":" << entry.second << endl;
    }
*/
    return random_distribution_select(edge_probabilities);
}

// Select a random string based on associated distributions
string random_distribution_select(map<double, string, greater<double> > distributions) {
    double probability_total = 1;
    for (map<double, string>::iterator it = distributions.begin(); it != distributions.end(); ++it) {
        pair<double, string> entry = *it;
        if (random_double(0, probability_total) < entry.first) {
            return entry.second;
        }
        else {
            probability_total -= entry.first;
        }
    }
}

// Pick a random double from min to max
double random_double(double min, double max) {
    double r = (double)rand() / RAND_MAX;
    return min + r * (max - min);
}

// Calculate pheromone to deposit on each edge
double calculate_pheromone(double Q, ant_agent a) {
    return Q / a.get_distance_travelled();
}

// Go through entire graph and evaporate the pheromone on edges
void evaporate_pheromone(graph<string, int> g, double rho) {
    for (list<graph<string, int>::vertex*>::iterator it = g.vertices.begin(); it != g.vertices.end(); ++it) {
        graph<string, int>::vertex* v = *it;
        for (list<graph<string, int>::edge>::iterator it2 = v->neighbors.begin(); it2 != v->neighbors.end(); ++it2) {
            it2->pheromone = it2->pheromone * (1 - rho);
        }
    }
}

// Given a tour history deposit pheromone on the edges in the path
void deposit_pheromone(graph<string, int> g, list<string> path, double pheromone, double rho) {
    for (list<string>::iterator it = path.begin(); it != path.end(); ++it) {
        list<string>::iterator it2 = ++it;
        --it;
        if (it2 != path.end()) {
            double current_pheromone = g.get_edge_pheromone(*it, *it2);
            double new_pheromone = current_pheromone + (pheromone * rho);
            g.update_edge(*it, *it2, new_pheromone);
        }
    }
}

bool test_covergence(vector<ant_agent> vec_ants) {
    for (vector<ant_agent>::iterator it = vec_ants.begin(); it != vec_ants.end(); ++it) {
        int matching_lists = 0;
        for (vector<ant_agent>::iterator it2 = vec_ants.begin(); it2 != vec_ants.end(); ++it2) {
            if (it->get_tour_history().size() == it2->get_tour_history().size() &&
                        equal(it->get_tour_history().begin(), it->get_tour_history().end(), it2->get_tour_history().begin())) {
                matching_lists++;
            }
        }
        cout << matching_lists << endl;
        if (matching_lists > 8)
            return true;
    }
    return false;
}
