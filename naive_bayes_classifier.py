import pandas as pd
import numpy as np
from io import StringIO
import requests

def load_data():
    url = "https://archive.ics.uci.edu/ml/machine-learning-databases/voting-records/house-votes-84.data"
    response = requests.get(url)
    columns = ['class'] + [f'vote_{i}' for i in range(1, 17)]
    df = pd.read_csv(StringIO(response.text), names=columns)
    return df

class NaiveBayesCustom:
    def __init__(self, lambda_param=1.0):
        self.lambda_param = lambda_param
        self.prior = {}
        self.conditional = {}
        self.classes = []
        self.features = []

    def fit(self, X, y):
        self.classes = y.unique()
        self.features = X.columns
        n_samples = len(y)
        
        for cls in self.classes:
            cls_count = sum(y == cls)
            self.prior[cls] = np.log(cls_count / n_samples)
            
            self.conditional[cls] = {}
            for col in self.features:
                self.conditional[cls][col] = {}
                possible_vals = ['y', 'n', '?'] if '?' in X[col].values else ['y', 'n']
                counts = X[y == cls][col].value_counts()
                
                for val in possible_vals:
                    val_count = counts.get(val, 0)
                    # Laplace Smoothing formula
                    prob = (val_count + self.lambda_param) / \
                           (cls_count + self.lambda_param * len(possible_vals))
                    self.conditional[cls][col][val] = np.log(prob)

    def predict(self, X):
        predictions = []
        for _, row in X.iterrows():
            best_cls = None
            max_log_prob = -float('inf')
            
            for cls in self.classes:
                log_prob = self.prior[cls]
                for col in self.features:
                    val = row[col]
                    # If value exists in our training dictionary, add its log prob
                    if val in self.conditional[cls][col]:
                        log_prob += self.conditional[cls][col][val]
                
                if log_prob > max_log_prob:
                    max_log_prob = log_prob
                    best_cls = cls
            predictions.append(best_cls)
        return np.array(predictions)

def run_experiment():
    print("Select mode: 0 (keep '?' as 3rd value) or 1 (impute with mode)")
    mode_input = input("Choice: ")
    
    print("Enter lambda value for Laplace smoothing (e.g., 1.0):")
    l_param = float(input("Lambda: "))

    df = load_data()
    
    
    if mode_input == '1':
        for col in df.columns[1:]:
            # Impute '?' with the mode of the class
            df.loc[df[col] == '?', col] = df.groupby('class')[col].transform(
                lambda x: x.mode()[0] if not x.mode().empty else 'n'
            )

    
    df = df.sample(frac=1, random_state=42).reset_index(drop=True)
    train_dfs, test_dfs = [], []
    for cls in df['class'].unique():
        cls_df = df[df['class'] == cls]
        idx = int(len(cls_df) * 0.8)
        train_dfs.append(cls_df.iloc[:idx])
        test_dfs.append(cls_df.iloc[idx:])
    
    train_df = pd.concat(train_dfs).sample(frac=1, random_state=42)
    test_df = pd.concat(test_dfs).sample(frac=1, random_state=42)
    
    X_train, y_train = train_df.drop('class', axis=1), train_df['class']
    X_test, y_test = test_df.drop('class', axis=1), test_df['class']

    model = NaiveBayesCustom(lambda_param=l_param)
    model.fit(X_train, y_train)

    train_acc = np.mean(model.predict(X_train) == y_train)

    folds = np.array_split(train_df, 10)
    cv_accs = []
    for i in range(10):
        cv_tst = folds[i]
        cv_trn = pd.concat([f for j, f in enumerate(folds) if i != j])
        m = NaiveBayesCustom(lambda_param=l_param)
        m.fit(cv_trn.drop('class', axis=1), cv_trn['class'])
        cv_accs.append(np.mean(m.predict(cv_tst.drop('class', axis=1)) == cv_tst['class']))

    test_acc = np.mean(model.predict(X_test) == y_test)

    # изход
    print(f"\n1. Train Set Accuracy:\n    Accuracy: {train_acc*100:.2f}%")
    print("\n10-Fold Cross-Validation Results:")
    for idx, acc in enumerate(cv_accs):
        print(f"    Accuracy Fold {idx+1}: {acc*100:.2f}%")
    
    print(f"\n    Average Accuracy: {np.mean(cv_accs)*100:.2f}%")
    print(f"    Standard Deviation: {np.std(cv_accs)*100:.2f}%")
    print(f"\n2. Test Set Accuracy:\n    Accuracy: {test_acc*100:.2f}%")

if __name__ == "__main__":
    run_experiment()