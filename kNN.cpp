#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <ctime>
#include <random> 
#include <cassert>

using namespace std;

const int TOTAL_PLANTS = 150;
const int NUM_CLASSES = 3;
const int FEATURES = 4;
const int TEST_SET_SIZE = 30; 


struct IrisPlant {
    double features[FEATURES]; 
    string classType;
};

// Глобални променливи за Min/Max стойности (за нормализация)
double minFeature[FEATURES];
double maxFeature[FEATURES];


// Използва се k-Nearest Neighbors
// Евклидовото разстояние е стандартната метрика (L2 норма).
double euclideanDistance(const IrisPlant& p1, const IrisPlant& p2) {
    double sum_sq = 0.0;
    for (int i = 0; i < FEATURES; ++i) {
        sum_sq += pow(p1.features[i] - p2.features[i], 2);
    }
    return sqrt(sum_sq);
}

struct DistanceHelper {
    IrisPlant plant;
    double distance;

    DistanceHelper(const IrisPlant& p, double d) : plant(p), distance(d) {}

    bool operator<(const DistanceHelper& other) const {
        return distance < other.distance;
    }
};


string classifyKNN(const IrisPlant& testPlant, const vector<IrisPlant>& trainingSet, int K) {
    if (trainingSet.empty()) return "N/A";

    vector<DistanceHelper> distances;
    distances.reserve(trainingSet.size());

    for (const auto& trainPlant : trainingSet) {
        distances.emplace_back(trainPlant, euclideanDistance(testPlant, trainPlant));
    }

    sort(distances.begin(), distances.end());

    map<string, int> classVotes;
    int maxVotes = 0;
    string predictedClass = "";

    for (int i = 0; i < min((int)distances.size(), K); ++i) {
        const string& classType = distances[i].plant.classType;
        classVotes[classType]++;

        // Актуализация на максималния брой гласове
        if (classVotes[classType] > maxVotes) {
            maxVotes = classVotes[classType];
            predictedClass = classType;
        }

    }

    return predictedClass;
}

double calculateAccuracy(const vector<IrisPlant>& testSet, const vector<IrisPlant>& trainingSet, int K) {
    if (testSet.empty()) return 0.0;

    int correctPredictions = 0;
    for (const auto& testPlant : testSet) {
        string predicted = classifyKNN(testPlant, trainingSet, K);
        if (predicted == testPlant.classType) {
            correctPredictions++;
        }
    }
    return (double)correctPredictions / testSet.size() * 100.0;
}

// 10-Fold Cross-Validation
vector<double> crossValidation(const vector<IrisPlant>& dataset, int K, int numFolds = 10) {
    vector<double> foldAccuracies;
    int foldSize = dataset.size() / numFolds;


    for (int fold = 0; fold < numFolds; ++fold) {
        vector<IrisPlant> trainFold;
        vector<IrisPlant> testFold;

        int start = fold * foldSize;
        int end = (fold + 1) * foldSize;


        for (int i = 0; i < dataset.size(); ++i) {
            if (i >= start && i < end) {
                testFold.push_back(dataset[i]); // Тестова част
            }
            else {
                trainFold.push_back(dataset[i]); // Обучаваща част
            }
        }

        // Поправка за последния fold, ако размерът не е точно делим
        if (fold == numFolds - 1 && dataset.size() % numFolds != 0) {
            for (int i = numFolds * foldSize; i < dataset.size(); ++i) {
                testFold.push_back(dataset[i]);
            }
        }

        double accuracy = calculateAccuracy(testFold, trainFold, K);
        foldAccuracies.push_back(accuracy);
    }

    return foldAccuracies;
}

// Min-Max Нормализация
// мащабират се стойностите в диапазона [0, 1].
void normalizeData(vector<IrisPlant>& plants) {
    for (int i = 0; i < FEATURES; ++i) {
        minFeature[i] = plants[0].features[i];
        maxFeature[i] = plants[0].features[i];
    }

    for (const auto& plant : plants) {
        for (int i = 0; i < FEATURES; ++i) {
            minFeature[i] = min(minFeature[i], plant.features[i]);
            maxFeature[i] = max(maxFeature[i], plant.features[i]);
        }
    }

    for (auto& plant : plants) {
        for (int i = 0; i < FEATURES; ++i) {
            double range = maxFeature[i] - minFeature[i];
            if (range > 1e-9) {
                plant.features[i] = (plant.features[i] - minFeature[i]) / range;
            }
            else {
                plant.features[i] = 0.0;
            }
        }
    }

    cout << "--- Preprocessing ---\n";
    cout << "The data has been normalized using Min-Max scaling.\n";
    cout << "Justification: kNN relies on distances. Different ranges of features\n";
    cout << "would lead to the dominance of the feature with the largest scope.\n";
    cout << "Normalization ensures equal influence of all features.\n";
    cout << "--------------------------------\n";
}

// Зареждане и Стратифицирано Разделяне на Данните (80% / 20%)
vector<IrisPlant> loadData(const string& filename) {
    vector<IrisPlant> plants;
    ifstream fin(filename);

    if (!fin.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return plants;
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        IrisPlant plant;
        stringstream ss(line);
        string segment;
        int featureIdx = 0;

        // Четене на 4-те характеристики и класа
        for (int i = 0; i < FEATURES; ++i) {
            getline(ss, segment, ',');
            plant.features[i] = stod(segment);
        }

        getline(ss, segment, '\r'); 
        plant.classType = segment;

        plants.push_back(plant);
    }
    fin.close();

    return plants;
}

// Стратифицирано разделяне на цялото множество на обучаващо и тестово (80/20)
// Връща {обучаващо, тестово}
pair<vector<IrisPlant>, vector<IrisPlant>> stratifiedSplit(const vector<IrisPlant>& allPlants) {
    map<string, vector<IrisPlant>> classes;
    for (const auto& plant : allPlants) {
        classes[plant.classType].push_back(plant);
    }

    vector<IrisPlant> trainSet;
    vector<IrisPlant> testSet;

    // Iris има 3 класа по 50 записа. 20% от 50 = 10 записа за тест от всеки клас.
    const int TEST_PER_CLASS = 10;

    // Разделяне стратифицирано
    for (auto& pair : classes) {
        vector<IrisPlant>& classVector = pair.second;
        // Разбъркване на всеки клас поотделно
        shuffle(classVector.begin(), classVector.end(), default_random_engine(time(0)));

        // Взимане на първите 10 за тест
        for (int i = 0; i < TEST_PER_CLASS; ++i) {
            testSet.push_back(classVector[i]);
        }
        // Останалите 40 за обучение
        for (int i = TEST_PER_CLASS; i < classVector.size(); ++i) {
            trainSet.push_back(classVector[i]);
        }
    }

    // Финално разбъркване на целите множества
    shuffle(trainSet.begin(), trainSet.end(), default_random_engine(time(0)));
    shuffle(testSet.begin(), testSet.end(), default_random_engine(time(0)));

    return { trainSet, testSet };
}


void solve() {
    int K;
    cout << "Enter K: ";
    if (!(cin >> K)) {
        cerr << "Invalid K.\n";
        return;
    }

    srand((unsigned)time(0));


    vector<IrisPlant> allPlants = loadData("iris.txt");
    if (allPlants.size() != TOTAL_PLANTS) {
        cerr << "Warning: " << allPlants.size() << " records were loaded (150 expected).\n";
        if (allPlants.empty()) return;
    }

    normalizeData(allPlants);


    auto sets = stratifiedSplit(allPlants);
    vector<IrisPlant> teachSet = sets.first; 
    vector<IrisPlant> testSet = sets.second;  

    if (K > teachSet.size()) {
        cerr << "Error: K (" << K << ") is greater than the size of the training set (" << teachSet.size() << ").\n";
        return;
    }

    double trainAccuracy = calculateAccuracy(teachSet, teachSet, K);

    cout << "\n=======================================================\n";
    cout << "1. Accuracy on the Training Set (K=" << K << ")\n";
    cout << "-------------------------------------------------------\n";
    printf("   Accuracy: %.2f%%\n", trainAccuracy);


   
    vector<double> cvAccuracies = crossValidation(teachSet, K, 10);

    double sumAccuracy = accumulate(cvAccuracies.begin(), cvAccuracies.end(), 0.0);
    double avgAccuracy = sumAccuracy / cvAccuracies.size();


    double sqDiffSum = 0.0;
    for (double acc : cvAccuracies) {
        sqDiffSum += pow(acc - avgAccuracy, 2);
    }
    double stdDev = sqrt(sqDiffSum / cvAccuracies.size());

    cout << "\n2. 10-Fold Cross-Validation Results\n";
    cout << "----------------------------------------------------------------------\n";
    for (size_t i = 0; i < cvAccuracies.size(); ++i) {
        printf("   Accuracy Fold %zu: %.2f%%\n", i + 1, cvAccuracies[i]);
    }
    printf("\n   Average Accuracy: %.2f%%\n", avgAccuracy);
    printf("   Standard Deviation: %.2f%%\n", stdDev);

    // 3.3. Оценка върху Тестовото Множество (Test Set Accuracy)
    double testAccuracy = calculateAccuracy(testSet, teachSet, K);

    cout << "\n3. Accuracy on the Test Set (K=" << K << ")\n";
    cout << "------------------------------------------------------\n";
    printf("   Accuracy: %.2f%%\n", testAccuracy);
    cout << "=======================================================\n";
    cout << "\nNext step: Implementing KD-Tree for faster searching.\n";
}

int main() {
  
    default_random_engine(time(0));

    solve();

    return 0;
}