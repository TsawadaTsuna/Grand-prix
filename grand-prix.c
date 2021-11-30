#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

int nlaps;
int ncars;
int **map;
int pl = 0; //actual place first in array

#define DEFAULT "\e[0m"

char *names[] = {
    "Valtteri Bottas", "Lewis Hamilton",
    "Fernando Alonso", "Esteban Ocon",
    "Nikita Mazepin", "Mick Schumacher",
    "Lando Norris", "Daniel Ricciardo",
    "Kimi Räikkönen", "Antonio Giovinazzi",
    "Sergio Pérez", "Max Verstappen",
    "Lance Stroll", "Sebastian Vettel",
    "Carlos Sainz", "Charles Leclerc",
    "Pierre Gasly", "Yuki Tsunoda",
    "Nicholas Latifi", "George Russell"
};

char *colors[] = {
    "\e[0;31m", "\e[0;32m",
    "\e[0;33m", "\e[0;36m",
    "\e[0;37m"
};

char characters[] = {
    '@', '#', '*', 'A', 'X', '&', 'Z', '!', 'Q', 'C', '+'
};

struct Car {
    char *name;
    int id;
    int speed;
    int maxSpeed;
    int acceleration;
    int lap;
    char image;
    int place;
    double racetime;
    double percentage;
    int finish;
    int *places;
};

struct Car *createCar(char *name, int id, int *places) {
    struct Car *car = calloc(1, sizeof(struct Car));
    car->name = name;
    car->id = id;
    car->speed = rand() % (100 + 1 - 50) + 50;
    car->maxSpeed = rand() % (200 + 1 - 150) + 150;
    car->acceleration = rand() % (8 + 1 - 3) + 3;
    car->lap = 1;
    car->image = characters[rand() % (10 + 1 - 0) + 0];
    car->place = id + 1;
    car->racetime = 0;
    car->percentage = 0;
    car->finish = 0;
    car->places = places;
    return car;
}

void printMap(struct Car **carros) {
    system("clear");
    for (int i = 0; i < ncars; i++) {
        char *carril = calloc(100, sizeof(char));
        for (int j = 0; j < 100; j++) {
            if (map[i][j] != -1) {
                carril[j] = carros[i]->image;
            } else {
                carril[j] = '-';
            }
        }
        if (!carros[i]->finish) {
            printf("%s%s Lap %d\n", colors[i % 5], carril, carros[i]->lap);
        } else {
            printf("%s%s Finish\n", colors[i % 5], carril);
        }
    }
    printf("\n");

    for (int i = 0; i < ncars; i++) {
        if (!carros[i]->finish) {
            printf("%sCar #%d: lap: %d, name: %s, speed: %d, racetime: %.2f, position: %d\n", DEFAULT, carros[i]->id, carros[i]->lap, carros[i]->name, carros[i]->speed, carros[i]->racetime, carros[i]->place);
        } else {
            printf("%sCar #%d: lap: Complete, name: %s, speed: %d, racetime: %.2f, position: %d\n", DEFAULT, carros[i]->id, carros[i]->name, carros[i]->speed, carros[i]->racetime, carros[i]->place);
        }
    }
    printf("\n");
}

void *carFunction(void *carro, struct Car **carros) {
    struct Car *car = (struct Car *)carro;
    if (car->id == ncars) {
        while (pl < ncars) {
            printMap(carros);
            sleep(1);
        }
    } else {
        int cAcc = 0;
        while (!car->finish) {
            int prePos = (int)car->percentage;
            map[car->id][prePos] = -1;
            car->racetime += 0.1;
            car->percentage += car->speed / 100.0;
            map[car->id][(int)car->percentage % 100] = car->id;
            for (int i = 0; i < ncars; i++) {
                if (carros[i]->percentage - car->percentage > 0 &&
                    carros[i]->percentage - car->percentage < 2 &&
                    carros[i]->speed < car->speed) {
                    srand(time(0));
                    double r = ((double)rand() / RAND_MAX) + 1;
                    if (r > 0.9) {
                        int newSpeed = carros[i]->speed;
                        car->speed = newSpeed;
                    }
                }
            }

            if (cAcc == 4) {
                int newASpeed = car->speed + car->acceleration;
                if (newASpeed > car->maxSpeed) {
                    car->speed = car->maxSpeed;
                } else {
                    car->speed = newASpeed;
                }
                cAcc = 0;
            } else {
                cAcc++;
            }

            if (car->percentage >= 100) {
                car->lap += 1;
                car->percentage = 0;
            }
            for (int i = 0; i < ncars; i++) {
                if (carros[i]->place < car->place) {
                    if (car->lap > carros[i]->lap) {
                        int tmp = car->place;
                        car->place = carros[i]->place;
                        carros[i]->place = tmp;
                    } else if (car->lap == carros[i]->lap &&
                        car->percentage > carros[i]->percentage) {
                        int tmp = car->place;
                        car->place = carros[i]->place;
                        carros[i]->place = tmp;
                    }
                }
            }
            if (car->lap > nlaps) {
                car->finish = 1;
            }
            sleep(1);
        }
        car->places[pl] = car->id;
        pl++;
    }
}

// compile
// gcc -fopenmp grand-prix.c -o gp

// execute
// ./gp nlaps ncars
int main(int argc, char **argv) {
    if (argc < 2){
        printf("ERROR: Not enough arguments\n");
        exit(-1);
    }
    nlaps = atoi(argv[1]);
    ncars = atoi(argv[2]);
    if (nlaps < 1){
        printf("ERROR: The number of laps should be 1 or more\n");
        exit(-1);
    }
    if (ncars < 3){
        printf("ERROR: The number of cars should be 3 or more\n");
        exit(-1);
    }
    int max_threads = omp_get_max_threads() - 1;
    if (ncars > max_threads){
        printf("WARNING: The number of cars should be less than %d for true parallelism\n", max_threads);
        exit(1);
    }
    //printf("laps: %d, cars: %d\n", nlaps, ncars);

    map = calloc(ncars, sizeof(int *));
    for (int i = 0; i < ncars; i++) {
        map[i] = calloc(100, sizeof(int));
    }
    for (int i = 0; i < ncars; i++) {
        for (int j = 0; j < 100; j++) {
            if (j == 0) {
                map[i][j] = i;
            } else {
                map[i][j] = -1;
            }
        }
    }

    srand(time(0));

    int place[ncars];
    struct Car *carsArray[ncars + 1];
    for (int id = 0; id < ncars + 1; id++) {
        struct Car *car = createCar(names[id], id, (int *)place);
        carsArray[id] = car;
    }

    #pragma omp parallel num_threads(ncars + 1)
    {
        int id = omp_get_thread_num();
        carFunction((void *)carsArray[id], (struct Car **)carsArray);
    }

    printf("WINNERS!! :\n");
    for (int i = 0; i < 3; i++) {
        printf("#%d Place : car #%d\n", i + 1, place[i]);
    }
    return 0;
}
