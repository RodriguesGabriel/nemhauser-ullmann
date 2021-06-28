from random import randint
from sys import argv


def generate_dataset(n: int, start: int, end: int, output: str):
    with open(output, "w") as out:
        out.write(f"{n} {int(randint(n * start, n * end) / 2)}\n")
        for i in range(n):
            out.write(f"{randint(start, end) + i} {randint(start, end) + i}\n")


if __name__ == "__main__":
    if len(argv) != 5:
        print("Usage: python generate_dataset [n] [start] [end] [output path]")
    else:
        generate_dataset(int(argv[1]), int(argv[2]), int(argv[3]), argv[4])
