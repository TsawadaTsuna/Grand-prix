# Multithreaded Grand Prix Simulator

This is an implementation of a multithreaded grand prix simulator where every car will be an independent entity. It will simulate a race with L laps and N cars.

## Technical Features

- The number of laps introduced by the user must be greater or equal than 1.
- The number of cars introduced by the user must be greater or equal than 3 and less than the number of threads the machine has minus 1.
- Each car is a separate thread.
- Each thread is created using the OpenMP API for parallel execution.
- All cars use the same map data structure resource.
- Each car has a random speed, max speed and acceleration.
- Each car displays its id, name, speed, lap, position and racing time.
- Each car increases its speed at a random time.
- If a car is near another car it will increase or decrease its speed based on its lap percentage.
- At the end, the top 3 winners are displayed.

## Build and Run

### On Linux

```bash
# To compile the c file u can use this two ways
gcc -fopenmp grand-prix.c -o gp
make test
# To run the file compiled
./gp nlaps ncars
# Example
./gp 4 8

# To remove the executable created after compilation.
make clean
```

## Info

### Language

- [c](https://progforperf.github.io/Expert_C_Programming.pdf)

### Packages

- [OpenMP](https://www.openmp.org/)

### Structures

#### Global variables

- `int` nlaps - The number of laps
- `int` ncars - The number of cars (threads)
- `int` pl - An index for the order of the winners
- `**int` map - The map that contains the positions of the cars
- `*char[]` names - An array that contains various names
- `*char[]` colors - An array that contains colors in ANSI
- `char[]` characters - An array that contains the character a car can use

#### Car

- `*char` name - The name of the car
- `int` id - The unique identifier of the car
- `int` speed - The current speed of the car
- `int` maxSpeed - The max speed the car can reach
- `int` acceleration - The speed the car increase or decrease
- `int` lap - The current lap the car is
- `char` image - The char the car is represented
- `int` place - The current place the car is
- `double` racetime - The time the car is running
- `double` percentage - The percentage the car has traveled the current lap
- `int` finish - The condition for the car to finish running
- `*int` places - The places the cars are

### Explanation

Video [Explanation](link va aqui)
