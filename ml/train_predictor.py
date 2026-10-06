import csv

from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, classification_report
from sklearn.dummy import DummyClassifier


DATASET_FILE = "ml/access_dataset.csv"


def load_dataset():
    X = []
    y = []

    with open(DATASET_FILE, "r") as file:
        reader = csv.DictReader(file)

        for row in reader:
            X.append([
                float(row["access_count"]),
                float(row["recent_accesses"]),
                float(row["time_since_access"])
            ])

            y.append(
                int(row["future_access"])
            )

    return X, y


def main():

    X, y = load_dataset()

    print("Dataset loaded")
    print("Samples:", len(X))

    X_train, X_test, y_train, y_test = train_test_split(
        X,
        y,
        test_size=0.2,
        random_state=42,
        stratify=y
    )

    # Baseline model.
    # This always predicts the most common class.
    baseline = DummyClassifier(
        strategy="most_frequent"
    )

    baseline.fit(X_train, y_train)

    baseline_predictions = baseline.predict(X_test)

    baseline_accuracy = accuracy_score(
        y_test,
        baseline_predictions
    )

    print()
    print("Baseline accuracy:", baseline_accuracy)

    # Actual machine learning model.
    model = LogisticRegression(
        max_iter=1000
    )

    model.fit(X_train, y_train)

    predictions = model.predict(X_test)

    accuracy = accuracy_score(
        y_test,
        predictions
    )

    print()
    print("ML model accuracy:", accuracy)

    print()
    print("Classification report:")

    print(
        classification_report(
            y_test,
            predictions
        )
    )

    print("Model coefficients:")

    feature_names = [
        "access_count",
        "recent_accesses",
        "time_since_access"
    ]

    for name, coefficient in zip(
        feature_names,
        model.coef_[0]
    ):
        print(
            f"{name}: {coefficient}"
        )


if __name__ == "__main__":
    main()
