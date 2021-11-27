#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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
    /*
    for (int i=0;i<ncars;i++){
        for(int j=0;j<100;j++){
            printf("%d, ",map[i][j]);
        }
        printf("\n");
    }
    */
    return car;
}

void *carFunction(void *carro){
    struct Car *car = (struct Car *)carro;
    //printf("Hola\n");
    //printf("Nombre: %s, id: %d, speed: %d, max speed: %d map: %p, places: %p\n",car->nombre,car->id,car->speed,car->maxSpeed,car->map,car->places);
    //Aqui va el codigo
    int **map = car->map;
    while(car->finish==0){
        int prePos = car->percentage;
        //int prep = map + (car->id*100+prePos)*sizeof(int);
        //map[car->id][(int)prePos]=-1;
        //&prep=-1;
        car->racetime+=0.1;
        car->percentage += car->speed/100.0;
        //map[car->id][(int)car->percentage%100]=car->id;
        //int pos = map + (car->id*100+((int)car->percentage%100))*sizeof(int);
        //&pos = car->id;
        if(car->percentage >= 100){
            car->lap+=1;
            car->percentage=0;
        }
        if (car->lap>nlaps){
            car->finish=1;
        }
        //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f\n",car->id,car->lap,car->percentage,car->speed,car->racetime);
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
        printf("Pasa argumentos\n");
        return -1;
    }

    int max_threads = omp_get_max_threads();
    if (ncars>max_threads){
        printf("Pasar porfavor un numero de carros menor al maximo de threads que es %d\n",max_threads);
        return -1;
    }

    int place[ncars];
    int map[ncars][100];
    for (int i=0;i<ncars;i++){
        for(int j=0;j<100;j++){
            if (j==0){
                map[i][j] = i;
            }else{
                map[i][j] = -1;
            }
        }
    }
    
    /*
    for (int i=0;i<ncars;i++){
        for(int j=0;j<100;j++){
            printf("%d, ",map[i][j]);
        }
        printf("\n");
    }
    */
    
    #pragma omp parallel num_threads(ncars)
    {
        int id = omp_get_thread_num();
        //srand(time(id));
        struct Car *car= createCar("cari",id,(int **)map,(int *)place);
        //incar++;
        carFunction((void *)car);
    }

    for(int i=0;i<ncars;i++){
        printf("Place %d: car #%d\n",i,place[i]);
    }
    return 0;
    //pthread_exit(NULL);
}
