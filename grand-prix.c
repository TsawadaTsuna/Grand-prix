#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int nlaps;
int ncars;
int pl=0; //actual place first in array

struct Car{
    char *nombre;
    int id;
    int speed;
    int maxSpeed;
    int lap;
    int place;
    double racetime;
    double percentage;
    int finish;
    int **map;
    int *places;
};

struct Car * createCar(char *nombre, int id, int **map, int *places){
    struct Car *car = calloc(1,sizeof(struct Car));
    car->nombre = nombre;
    car->id = id;
    car->speed = rand() % (100 + 1 - 10) + 10;
    car->maxSpeed = rand() % (200 + 1 - 50) + 50;
    car->lap = 1;
    car->place = id;
    car->racetime =0;
    car->percentage=0;
    car->finish=0;
    car->map=map;
    car->places=places;
    return car;
}

void *carFunction(void *carro){
    struct Car *car = (struct Car *)carro;
    //printf("Hola\n");
    printf("Nombre: %s, id: %d, speed: %d, max speed: %d map: %p, places: %p\n",car->nombre,car->id,car->speed,car->maxSpeed,car->map,car->places);
    //Aqui va el codigo
    while(car->finish==0){
        car->racetime+=0.1;
        car->percentage += car->speed/100.0;
        if(car->percentage >= 100){
            car->lap+=1;
            car->percentage=0;
        }
        if (car->lap>nlaps){
            car->finish=1;
        }
        printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f\n",car->id,car->lap,car->percentage,car->speed,car->racetime);
    }
    car->places[pl]=car->id;
    pl++;
    //------------------
    //pthread_exit(NULL);
    return NULL;
}

//./gp nlaps ncars
int main(int argc, char **argv){
    srand(time(0));
    if(argc>2){
        nlaps = atoi(argv[1]);
        ncars = atoi(argv[2]);
    }else{
        printf("Pasa argumentos");
    }

    int place[ncars];
    int map[ncars][100];
    pthread_t cars[ncars];
    int t;
    for(long i =0;i<ncars;i++){
        struct Car *car= createCar("cari",(int)i,(int **)map,(int *)place);
        t=pthread_create(&cars[i],NULL,carFunction,(void *)car);
        if (t){
          printf("ERROR; return code from pthread_create() is %d\n", t);
          exit(-1);
       }
    }

    for(int i=0;i<ncars;i++){
        pthread_join(cars[i],NULL);
    }

    for(int i=0;i<ncars;i++){
        printf("Place %d: car #%d\n",i,place[i]);
    }

    pthread_exit(NULL);
}