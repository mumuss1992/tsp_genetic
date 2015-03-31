#include <iostream>
#include <algorithm> // sort, next_permutation
#include "tsp.h"
using namespace std;


Edge::Edge(int src, int dest, int weight) // constructor of Edge
{
	// assigns the parameters
	this->src = src;
	this->dest = dest;
	this->weight = weight;
}


Graph::Graph(int V, int initial_vertex) // constructor of Graph
{
	if(V < 1) // checks if number of vertexes is less than 1
	{
		cout << "Error: number of vertexes <= 0\n";
		exit(1);
	}
	
	this->V = V; // assigns the number of vertices
	this->initial_vertex = initial_vertex; // assigns initial vertex
	this->total_edges = 0; // initially the total of edges is 0
}


void Graph::addEdge(int src, int dest, int weight) // add a edge
{
	Edge edge(src, dest, weight); // creates a edge
	edges.push_back(edge); // adds edge in vector of edges
	map_edges[make_pair(src, dest)] = weight; // adds edge in the map
	total_edges++; // increments number total of edges
}


void Graph::showGraph() // shows all connections of the graph
{	
	map<pair<int, int>, int>::iterator it;
	for(it = map_edges.begin(); it != map_edges.end(); ++it)
		cout << it->first.first << " linked to vertex " << it->first.second << " with weight " << it->second << endl;
}


int Graph::existsEdge(int src, int dest) // checks if exists a edge
{
	map<pair<int, int>,int>::iterator it = map_edges.find(make_pair(src, dest));
	
	if(it != map_edges.end())
		return it->second; // returns cost
	return -1;
}


Genetic::Genetic(Graph* graph, int size_population, int iterations, int mutation_rate) // constructor of Genetic
{
	if(size_population < 1) // checks if size of population is less than 1
	{
		cout << "Error: size_population < 1\n";
		exit(1);
	}
	else if(mutation_rate < 0 || mutation_rate > 100) // checks if mutation rate is less than 0
	{
		cout << "Error: mutation_rate must be >= 0 and <= 100\n";
		exit(1);
	}
	this->graph = graph;
	this->size_population = size_population;
	this->real_size_population = 0;
	this->iterations = iterations;
	this->mutation_rate = mutation_rate;
}



// checks if is a valid solution, then return total cost of path else return -1
int Genetic::isValidSolution(vector<int>& solution)
{
	int total_cost = 0;

	// checks if connections are valid
	for(int i = 0; i < graph->V; i++)
	{
		if(i + 1 <  graph->V)
		{
			int cost = graph->existsEdge(solution[i], solution[i+1]);
			
			// checks if exists connection
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
		}
		else
		{
			int cost = graph->existsEdge(solution[i], solution[0]);
			
			// checks if exists connection
			if(cost == -1)
				return -1;
			else
				total_cost += cost;
			break;
		}
	}
	return total_cost;
}


bool Genetic::existsChromosome(const vector<int> & v)
{
	// checks if exists in the population
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // gets the vector
		if(equal(v.begin(), v.end(), vec.begin())) // compares vectors
			return true;
	}
	return false;
}


void Genetic::initialPopulation() // generates the initial population
{
	vector<int> parent;
	
	// inserts initial vertex in the parent
	parent.push_back(graph->initial_vertex);
	
	// creates the parent
	for(int i = 1; i < graph->V; i++)
	{
		if(i != graph->initial_vertex)
			parent.push_back(i);
	}
		
	int total_cost = isValidSolution(parent);
	
	if(total_cost != -1) // checks if the parent is valid
	{
		population.push_back(make_pair(parent, total_cost)); // inserts in the population
		real_size_population++; // increments real_size_population
	}
	
	// makes permutations until "size_population"
	for(int i = 0; i < size_population; i++)
	{
		vector<int> permutation;
		
		next_permutation(parent.begin() + 1, parent.begin() + graph->V); // gets permutation
		for(int j = 0; j < graph->V; j++) // creates the permutation
			permutation.push_back(parent[j]);
			
		int total_cost = isValidSolution(permutation); // checks if solution is valid
		
		// checks if permutation is a valid solution and if not exists
		if(total_cost != -1 && !existsChromosome(permutation))
		{
			population.push_back(make_pair(permutation, total_cost)); // add in population
			real_size_population++; // increments real_size_population in the unit
			if(real_size_population == size_population) // checks the goal
				break; // left the loop
		}
	}
	
	// checks if real_size_population is 0
	if(real_size_population == 0)
	{
		cout << "Empty initial population ;( Try again runs the algorithm...\n";
		exit(1);
	} 
	else
		sort(population.begin(), population.end(), sort_pred()); // sort population
		
}


void Genetic::showPopulation()
{
	cout << "\nShowing solutions...\n\n";
	for(vector<pair<vector<int>, int> >::iterator it=population.begin(); it!=population.end(); ++it)
	{
		const vector<int>& vec = (*it).first; // gets the vector
		
		for(int i = 0; i < graph->V; i++)
			cout << vec[i] << " ";
		cout << graph->initial_vertex;
		cout << " | Cost: " << (*it).second << "\n";
	}
	cout << "\nPopulation size: " << real_size_population << endl;
}


void Genetic::crossOver(vector<int>& parent)
{
	// generates points
	int point1 = rand() % (graph->V - 1) + 1; // rand 1 until (graph->V - 1)
	int point2 = rand() % (graph->V - point1) + point1;
	
	if(point1 == point2)
	{
		if(point1 - 1 > 1)
			point1--;
		else if(point2 + 1 < graph->V)
			point2++;
		else
		{
			// point1 or point2 ?? random...
			int point = rand() % 10 + 1; // number in the range 1 to 10
			
			if(point <= 5)
				point1--;
			else
				point2++;
		}
	}
	
	vector<int> child;
	
	// generates child - inverting subsequence of the vertexes
	
	for(int i = 0; i < point1; i++)
		child.push_back(parent[i]);
		
	for(int i = point2; i >= point1; i--)
		child.push_back(parent[i]);
	
	for(int i = point2 + 1; i < graph->V; i++)
		child.push_back(parent[i]);
		
	// mutation ...
	int mutation = rand() % 100 + 1; // random number in [1,100]
	if(mutation <= mutation_rate) // checks if the random number <= mutation rate
	{
		// makes a mutation: changes positions of the cities
		int city1, city2, aux;
		
		city1 = rand() % (graph->V - 1) + 1;
		city2 = rand() % (graph->V - 1) + 1;
		
		// makes trading
		aux = child[city1];
		child[city1] = child[city2];
		child[city2] = aux;
	}
	
	int total_cost = isValidSolution(child);
	
	// checks if is a valid solution and not exists in the population
	if(total_cost != -1 && !existsChromosome(child))
	{
		// add child in the population
		insertBinarySearch(child, total_cost); // uses binary search to insert
		real_size_population++; // increments the real_size_population
	}
}


// inserts in the vector using binary search
void Genetic::insertBinarySearch(vector<int>& child, int total_cost)
{
	int imin = 0;
	int imax = real_size_population - 1;
	
	while(imax >= imin)
	{
		int imid = imin + (imax - imin) / 2;
		
		if(total_cost == population[imid].second)
		{
			population.insert(population.begin() + imid, make_pair(child, total_cost));
			return;
		}
		else if(total_cost > population[imid].second)
			imin = imid + 1;
		else
			imax = imid - 1;
	}
	population.insert(population.begin() + imin, make_pair(child, total_cost));
}


// runs the genetic algorithm
void Genetic::run()
{
	initialPopulation(); // gets initial population

	for(int i = 0; i < iterations; i++)
	{
		int  old_size_population = real_size_population;
		
		/* selects two parents (if exists) who will participate 
			of the reproduction process */
		if(real_size_population >= 2)
		{	
			if(real_size_population == 2)
			{
				// applying crossover in the two parents
				crossOver(population[0].first);
				crossOver(population[1].first);
			}
			else
			{
				// real_size_population > 2
				
				int parent1, parent2;
			
				do
				{
					// select two random parents
					parent1 = rand() % real_size_population;
					parent2 = rand() % real_size_population;
				}while(parent1 == parent2);
				
				// applying crossover in the two parents
				crossOver(population[parent1].first);
				crossOver(population[parent2].first);
			}
			
			// gets difference to check if the population grew 
			int diff_population = real_size_population - old_size_population;
			
			if(diff_population == 2)
			{
				// removes the two worst parents
				population.pop_back();
				population.pop_back();
				
				// decrements the real_size_population in 2 units
				real_size_population -= 2;
			}
			else if(diff_population == 1)
			{
				population.pop_back(); // removes the worst parent
				real_size_population--; // decrements the real_size_population in the unit
			}
		} 
		else // population contains only 1 parent
		{
			// applying crossover in the parent
			crossOver(population[0].first);
			
			if(real_size_population > old_size_population)
			{
				population.pop_back(); // removes the worst parent
				real_size_population--; // decrements the real_size_population in the unit
			}
		}
	}
	
	showPopulation(); // shows the population
	
	cout << "\nBest solution: ";
	const vector<int>& vec = population[0].first;
	for(int i = 0; i < graph->V; i++)
		cout << vec[i] << " ";
	cout << graph->initial_vertex;
	cout << " | Cost: " << population[0].second;
}
