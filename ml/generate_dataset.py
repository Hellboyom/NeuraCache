import csv
import random
import time


OUTPUT_FILE = "ml/access_dataset.csv"

KEY_TYPES = [
    "hot",
    "warm",
    "cold",
    "bursty"
]


def generate_dataset(
    rounds=10000,
    seed=42
):
    random.seed(seed)

    access_counts = {}
    last_access = {}

    rows = []

    current_time = time.time()

    for step in range(rounds):

        pattern = random.random()

        if pattern < 0.50:
            key = "hot_" + str(
                random.randint(0, 4)
            )

        elif pattern < 0.75:
            key = "warm_" + str(
                random.randint(0, 9)
            )

        elif pattern < 0.90:
            key = "cold_" + str(
                random.randint(0, 49)
            )

        else:
            key = "bursty_" + str(
                random.randint(0, 4)
            )

        previous_count = \
            access_counts.get(key, 0)

        previous_time = \
            last_access.get(
                key,
                current_time
            )

        time_since_access = (
            current_time -
            previous_time
        )

        recent_accesses = min(
            previous_count,
            10
        )

# The label represents whether
# this key is accessed again
# within the next observation window.

        future_access = 0

        if key.startswith("hot_"):
            future_access = (
                1
                if random.random() < 0.85
                else 0
            )

        elif key.startswith("warm_"):
            future_access = (
                1
                if random.random() < 0.60
                else 0
            )

        elif key.startswith("bursty_"):
            future_access = (
                1
                if random.random() < 0.70
                else 0
            )

        else:
            future_access = (
                1
                if random.random() < 0.15
                else 0
            )

        rows.append([
            previous_count,
            recent_accesses,
            time_since_access,
            future_access
        ])

        access_counts[key] = \
            previous_count + 1

        last_access[key] = current_time

        current_time += 0.01

    return rows


def main():

    rows = generate_dataset()

    with open(
        OUTPUT_FILE,
        "w",
        newline=""
    ) as file:

        writer = csv.writer(file)

        writer.writerow([
            "access_count",
            "recent_accesses",
            "time_since_access",
            "future_access"
        ])

        writer.writerows(rows)

    print(
        "Dataset generated:",
        OUTPUT_FILE
    )

    print(
        "Rows:",
        len(rows)
    )


if __name__ == "__main__":
    main()