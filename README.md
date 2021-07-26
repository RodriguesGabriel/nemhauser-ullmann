# Nemhauser-Ullmann

## How to generate new datasets
    python generate_dataset [number of items] [min value] [max value] [output path]

## How to execute
    make
    ./bin/sequential < datasets/in > datasets/out
    ./bin/parallel < datasets/in > datasets/out
    ./bin/parallel_merge < datasets/in > datasets/out

Be aware that "parallel_merge" is not returning a correct output as of now but its performance should be similar.

Thread pool classes are not being used.