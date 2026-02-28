#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <chrono>

using namespace std;

const int MAX_POPULATION_SIZE = 500; // Намален размер за скорост при N=10000
const int MAX_GENERATIONS = 200;    // Достатъчно за добра конвергенция
int N;
double MaxCapacity;
double* weights, * prices;

struct Knapsack {
    vector<bool> configuration;
    double fitness;
    double totalWeight;

    bool operator<(const Knapsack& other) const {
        return fitness > other.fitness; // За низходящо сортиране
    }
};

void generatePopulation(vector<Knapsack>& population) {
    for (int i = 0; i < MAX_POPULATION_SIZE; i++) {
        Knapsack k;
        k.fitness = k.totalWeight = 0;
        k.configuration.resize(N);
        for (int j = 0; j < N; j++) {
            bool included = rand() % 2;
            k.configuration[j] = included;
            if (included) {
                k.fitness += prices[j];
                k.totalWeight += weights[j];
            }
        }
        if (k.totalWeight > MaxCapacity) k.fitness = 0;
        population.push_back(k);
    }
}

void crossover(const Knapsack& p1, const Knapsack& p2, Knapsack& c1, Knapsack& c2) {
    int point = rand() % N;
    c1.configuration.resize(N);
    c2.configuration.resize(N);
    c1.fitness = c1.totalWeight = c2.fitness = c2.totalWeight = 0;

    for (int i = 0; i < N; i++) {
        c1.configuration[i] = (i < point) ? p1.configuration[i] : p2.configuration[i];
        c2.configuration[i] = (i < point) ? p2.configuration[i] : p1.configuration[i];

        if (c1.configuration[i]) { c1.fitness += prices[i]; c1.totalWeight += weights[i]; }
        if (c2.configuration[i]) { c2.fitness += prices[i]; c2.totalWeight += weights[i]; }
    }
    if (c1.totalWeight > MaxCapacity) c1.fitness = 0;
    if (c2.totalWeight > MaxCapacity) c2.fitness = 0;
}

void mutate(Knapsack& k) {
    if ((rand() % 100) < 5) { // 5% вероятност за мутация
        int idx = rand() % N;
        if (k.configuration[idx]) {
            k.fitness -= prices[idx];
            k.totalWeight -= weights[idx];
            k.configuration[idx] = 0;
        }
        else {
            k.fitness += prices[idx];
            k.totalWeight += weights[idx];
            k.configuration[idx] = 1;
        }
        if (k.totalWeight > MaxCapacity) k.fitness = 0;
        else if (k.fitness < 0) k.fitness = 0;
    }
}

int main() {
    srand(time(0));
    if (!(cin >> MaxCapacity >> N)) return 0;

    prices = new double[N];
    weights = new double[N];
    for (int i = 0; i < N; i++) cin >> weights[i] >> prices[i];

    auto start_time = chrono::high_resolution_clock::now();

    vector<Knapsack> population;
    generatePopulation(population);

    double bestOverall = 0;

    for (int gen = 0; gen < MAX_GENERATIONS; gen++) {
        sort(population.begin(), population.end());

        if (population[0].fitness > bestOverall) bestOverall = population[0].fitness;

        // Печат на 10 междинни стойности (през равни интервали)
        if (gen % (MAX_GENERATIONS / 10) == 0) {
            cout << (long long)bestOverall << endl;
        }

        for (int i = MAX_POPULATION_SIZE / 2; i < MAX_POPULATION_SIZE - 1; i += 2) {
            int p1 = rand() % (MAX_POPULATION_SIZE / 2);
            int p2 = rand() % (MAX_POPULATION_SIZE / 2);
            crossover(population[p1], population[p2], population[i], population[i + 1]);
        }

        for (int i = 0; i < MAX_POPULATION_SIZE; i++) mutate(population[i]);
    }

    sort(population.begin(), population.end());
    if (population[0].fitness > bestOverall) bestOverall = population[0].fitness;

    cout << endl << (long long)bestOverall << endl;

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> diff = end_time - start_time;
    // cout << "Time: " << diff.count() << "s" << endl; // Опция за време

    delete[] prices; delete[] weights;
    return 0;
}

