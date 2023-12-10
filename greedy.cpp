#include <time.h>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

pair<int, string> greedyAlgorithm(const vector<string>& dataset, int alpha) {
    vector<int> distances(dataset.size(), 0);
    string result = "";
    int cost = 0;

    // Elegir como primera letra la más frecuente o tomar una letra aleatoria
    if (alpha > rand() % 100 + 1) {
        char list[4] = {'A', 'C', 'T', 'G'};
        int randomIndex = rand() % 4;
        char chosen = list[randomIndex];
        result += chosen;
        for (int i = 0; i < dataset.size(); i++) {
            if (dataset[i][0] != chosen) {
                distances[i]++;
            }
        }
    } else {
        map<char, int> m;
        for (int i = 0; i < dataset.size(); i++) {
            m[dataset[i][0]]++;
        }

        auto best = max_element(
            m.begin(), m.end(),
            [](auto &a, auto &b) -> bool { return a.second < b.second; });
        result += best->first;
        for (int i = 0; i < dataset.size(); i++) {
            if (dataset[i][0] != best->first) {
                distances[i]++;
            }
        }
    }

    for (auto value : distances) {
        cost += value * value;
    }

    // Elegir siguientes letras
    for (int i = 1; i < dataset[0].size() - 1; i++) {
        int min_cost = INT_MAX;
        char best_letter;
        vector<int> best_distances;

        if (alpha > rand() % 100 + 1) {
            char list[4] = {'A', 'C', 'T', 'G'};
            int randomIndex = rand() % 4;
            char chosen = list[randomIndex];
            cost = 0;
            best_letter = chosen;
            best_distances = distances;
            for (int sequence = 0; sequence < dataset.size(); sequence++) {
                if (dataset[sequence][i] != chosen) {
                    best_distances[sequence]++;
                }
            }
            for (auto value : best_distances) {
                cost += value * value;
            }
        } else {
            for (char letter : {'A', 'C', 'T', 'G'}) {
                vector<int> distances_copy = distances;
                cost = 0;
                for (int sequence = 0; sequence < dataset.size(); sequence++) {
                    if (dataset[sequence][i] != letter) {
                        distances_copy[sequence]++;
                    }
                }
                for (auto value : distances_copy) {
                    cost += value * value;
                }
                if (cost < min_cost) {
                    min_cost = cost;
                    best_letter = letter;
                    best_distances = distances_copy;
                }
            }
        }
        result += best_letter;
        distances = best_distances;
    }

    return make_pair(cost, result);
}