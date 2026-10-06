import csv
import math

from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
from sklearn.dummy import DummyClassifier


DATASET_FILE = "ml/access_dataset.csv"


def load_dataset():
    X = []
    y = []

    with open(DATASET_FILE, "r") as file:
        reader = csv.DictReader(file)

        for row in reader:
            X.append({
                "access_count": float(row["access_count"]),
                "recent_accesses": float(row["recent_accesses"]),
                "time_since_access": float(row["time_since_access"])
            })

            y.append(int(row["future_access"]))

    return X, y


def heuristic_prediction(row):
    access_count = row["access_count"]
    recent_accesses = row["recent_accesses"]
    time_since_access = row["time_since_access"]

    score = (
        0.4 * min(access_count / 10.0, 1.0)
        + 0.5 * min(recent_accesses / 10.0, 1.0)
        + 0.1 * math.exp(-time_since_access)
    )

    return 1 if score >= 0.5 else 0


def main():

    X, y = load_dataset()

    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,
        random_state=42,
        stratify=y
    )

    # --------------------------------------------------
    # 1. Baseline
    # --------------------------------------------------

    baseline = DummyClassifier(
        strategy="most_frequent"
    )

    baseline.fit(
        [
            [
                row["access_count"],
                row["recent_accesses"],
                row["time_since_access"]
            ]
            for row in X_train
        ],
        y_train
    )

    baseline_predictions = baseline.predict(
        [
            [
                row["access_count"],
                row["recent_accesses"],
                row["time_since_access"]
            ]
            for row in X_test
        ]
    )

    baseline_accuracy = accuracy_score(
        y_test,
        baseline_predictions
    )

    # --------------------------------------------------
    # 2. Existing heuristic
    # --------------------------------------------------

    heuristic_predictions = [
        heuristic_prediction(row)
        for row in X_test
    ]

    heuristic_accuracy = accuracy_score(
        y_test,
        heuristic_predictions
    )

    # --------------------------------------------------
    # 3. Machine learning model
    # --------------------------------------------------

    train_features = [
        [
            row["access_count"],
            row["recent_accesses"],
            row["time_since_access"]
        ]
        for row in X_train
    ]

    test_features = [
        [
            row["access_count"],
            row["recent_accesses"],
            row["time_since_access"]
        ]
        for row in X_test
    ]

    model = LogisticRegression(
        max_iter=1000
    )

    model.fit(
        train_features,
        y_train
    )

    ml_predictions = model.predict(
        test_features
    )

    ml_accuracy = accuracy_score(
        y_test,
        ml_predictions
    )

    # --------------------------------------------------
    # Results
    # --------------------------------------------------

    print()
    print("======================================")
    print("     NeuraCache Predictor Evaluation")
    print("======================================")
    print()

    print(
        f"Baseline accuracy:   {baseline_accuracy:.4f}"
    )

    print(
        f"Heuristic accuracy:  {heuristic_accuracy:.4f}"
    )

    print(
        f"ML accuracy:         {ml_accuracy:.4f}"
    )

    print()

    print("Improvement over baseline:")
    print(
        f"ML:        {(ml_accuracy - baseline_accuracy) * 100:.2f} percentage points"
    )

    print(
        f"Heuristic: {(heuristic_accuracy - baseline_accuracy) * 100:.2f} percentage points"
    )

    print()

    if ml_accuracy > heuristic_accuracy:
        print("RESULT: ML beats the heuristic predictor.")
    else:
        print("RESULT: Heuristic predictor remains stronger.")

    print()
    print("======================================")


if __name__ == "__main__":
    main()