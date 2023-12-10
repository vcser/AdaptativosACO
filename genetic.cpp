#include <time.h>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <chrono>
#include <numeric>
#include <string>
#include <vector>
#include "greedy.cpp"
#include "utils.cpp"

using namespace std;

// Función para seleccionar padres usando FPS (Fitness Proportional Selection)
pair<pair<int, string>, pair<int, string>> selectParents(const vector<pair<int, string>>& population) {
    vector<double> probabilities;
    double totalInverseCost = 0.0;

    // Calcular la suma total de las inversas de los costos
    for (const pair<int, string>& individual : population) {
        totalInverseCost += 1.0 / static_cast<double>(individual.first);
    }

    // Calcular las probabilidades de selección como inversas de los costos
    for (const pair<int, string>& individual : population) {
        double probability = (1.0 / static_cast<double>(individual.first)) / totalInverseCost;
        probabilities.push_back(probability);
    }

    // Elegir dos padres utilizando FPS
    int parent1Index = 0;
    int parent2Index = 0;
    double r1 = static_cast<double>(rand()) / RAND_MAX;
    double r2 = static_cast<double>(rand()) / RAND_MAX;
    double cumulativeProbability = 0.0;

    for (int i = 0; i < population.size(); i++) {
        cumulativeProbability += probabilities[i];
        if (cumulativeProbability >= r1 && parent1Index == 0) {
            parent1Index = i;
        }
        if (cumulativeProbability >= r2 && parent2Index == 0) {
            parent2Index = i;
        }
    }

    return make_pair(population[parent1Index], population[parent2Index]);
}


// Función para realizar el crossover de dos padres
pair<int,string> crossover(pair<int,string> parent1, pair<int,string> parent2, vector<string> dataset) {
    // Realiza el crossover de 2 puntos
    int length = parent1.second.size();
    int point1 = rand() % (length - 1) + 1;
    int point2 = rand() % (length - point1) + point1;

    string childSequence = parent1.second.substr(0, point1) +
                          parent2.second.substr(point1, point2 - point1) +
                          parent1.second.substr(point2, length - point2);

    int childCost = calculateCost(childSequence, dataset);

    return make_pair(childCost,childSequence);
}

// Función para aplicar la mutación en un individuo
void mutate(pair<int,string>& individual, vector<string> dataset) {
    int index = rand() % individual.second.size();
    char list[4] = {'A', 'C', 'T', 'G'};
    int randomIndex = rand() % 4; // Genera una letra aleatoria (A, C, T, G)
    individual.second[index] = list[randomIndex];
    individual.first = calculateCost(individual.second, dataset);
}

int main(int argc, char const *argv[]) {
    // Definir tus parámetros
    int populationSize = 100;
    double mutationRate = 0.1;

    // Leer archivo
    srand(time(NULL));
    if (argc > 9) {
        if (argv[1][0] != '-' || argv[1][1] != 'i') {
            cout << "Debes ingresar el comando -i " << endl;
            return 0;
        }
        if(argv[3][0] != '-' ||argv[3][1] != 't'){
            cout <<"Debes ingresar el comando -t" <<endl;
            return 0;
        }
        if(argv[5][0] != '-'||argv[5][1] != 'p'){
            cout <<"Debes ingresar el comando -p"<<endl;
            return 0;
        }
        if(argv[7][0] != '-'||argv[7][1] != 'm'){
            cout <<"Debes ingresar el comando -m"<<endl;
            return 0;
        }
        if(argv[9][0] != '-'||argv[9][1] != 'p'||argv[9][2] !='a'){
            cout <<"Debes ingresar el comando -pa"<<endl;
            return 0;
        }
    } else {
        cout << "Debes ingresar el formato de ejecución <Greedy> -i "
                "<instancia-problema> -t <tiempo-maximo-segundos> -p <tamaño-población> -m <mutation-rate> -pa <probabilidad-aleatoriedad> " << endl;
        return 0;
    }

    fstream file;
    string path = argv[2];
    vector<string> dataset;
    int alpha = atoi(argv[10]);
    int timeLimit = atoi(argv[4]);
    populationSize = atoi(argv[6]);
    mutationRate = atof(argv[8]);
    cout<<"probabilidad: "<<alpha<<endl<<"time: "<<timeLimit<<endl<< "populationSize: " <<populationSize<<endl<<"mutationrate "<<mutationRate<<endl;
    file.open(path, ios::in);

    if (file.is_open()) {
        string data;
        while (getline(file, data)) {
            dataset.push_back(data);
        }
        file.close();
    }
    // Inicializa la población inicial utilizando tu algoritmo greedy
    pair<int, string> bestIndividual = make_pair(INT_MAX,"");

    vector< pair<int,string> > population;
    for (int i = 0; i < populationSize; i++) {
        pair<int, string> greedyResult = greedyAlgorithm(dataset, alpha);
        population.push_back(greedyResult);
    }
    auto startTime = chrono::high_resolution_clock::now();
    double getTime = 0.0;
    while (true) {
        // Comprueba el tiempo transcurrido en cada iteración.
        auto currentTime = chrono::high_resolution_clock::now();
        auto elapsedSeconds = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
        if (elapsedSeconds >= timeLimit) {
            break;  // Detener GRASP si se excede el límite de tiempo.
        }
        vector<pair<int,string>> newPopulation;
        for (int i = 0; i < populationSize / 2; i++) {
            // Seleccionar dos padres
            pair<pair<int,string>, pair<int,string>> parents = selectParents(population);

            // Realizar crossover
            pair<int,string> child1 = crossover(parents.first, parents.second, dataset);
            pair<int,string> child2 = crossover(parents.second, parents.first, dataset);

            // Aplicar mutación
            if (static_cast<double>(rand()) / RAND_MAX < mutationRate) {
                mutate(child1, dataset);
            }
            if (static_cast<double>(rand()) / RAND_MAX < mutationRate) {
                mutate(child2, dataset);
            }

            newPopulation.push_back(child1);
            newPopulation.push_back(child2);
        }
        
        pair<int,string> actualBest = *min_element(newPopulation.begin(), newPopulation.end(), [](const pair<int, string>& a, const pair<int, string>& b) {
        return a.first < b.first;
        });
        if(actualBest.first < bestIndividual.first){
            population = newPopulation;
            bestIndividual = actualBest;
            getTime = chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - startTime).count();
            cout<<"actualBest: "<<actualBest.first<<" "<<actualBest.second<<endl;
            cout<<"encontrado en el tiempo: "<<getTime<<endl;
        }
    }

    // Encuentra el mejor individuo en la población final

    //cout << "Mejor solución encontrada: " << bestIndividual.second << endl;
    cout << bestIndividual.first << endl;
    //cout << "tiempo total: "<< chrono::duration_cast<chrono::duration<double>>(chrono::high_resolution_clock::now() - startTime).count()<<endl;

    return 0;
}
