#include <vector>
#include <string>

using namespace std;

int calculateCost(string result, vector<string> dataset){
    int cost = 0;
    vector<int> distances(dataset.size(),0);
    //se rellena el vector de distancias
    for (int i = 0; i < result.size(); i++){
        for (int sequence = 0; sequence < dataset.size(); sequence++){
            if(result[i] != dataset[sequence][i]){
                distances[sequence]++;
            }
        }
    }
    //se calcula el costo
    for (auto value :distances) {
        cost += value * value;
    }
    return cost;
}