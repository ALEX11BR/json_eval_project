## Building the project
Run these commands:
```
cmake -S . -B build
cmake --build build
```
## Running the project
Run it like this (the first argument is the path to the JSON file; the second argument is the expression to evaluate from the file):
```
./build/json_eval test-util/input.json "a.b[0]"
```
## Running the tests
Run these commands:
```
cd build
ctest
```
