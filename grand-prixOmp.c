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

void printMap(void *carro){
    struct Car *car = (struct Car *)carro;
    //(int[ncars][100]) car->map;

    for (int i=0;i<ncars;i++){
        for(int j=0;j<100;j++){
            printf("%d, ",car->map[i][j]);
        }
        printf("\n");
    }
}

void *carFunction(void *carro, struct Car **carros){
    struct Car *car = (struct Car *)carro;
    printf("Nombre: %s, id: %d, speed: %d, max speed: %d map: %p, places: %p\n",car->nombre,car->id,car->speed,car->maxSpeed,car->map,car->places);
    while(car->finish==0){
        //int prePos = car->percentage;
        //car->map[car->id][(int)prePos]=-1;
        car->racetime+=0.1;
        car->percentage += car->speed/100.0;
        //car->map[car->id][(int)car->percentage%100]=car->id;
        //carros[car->id]->percentage=(int)car->percentage;
        for(int i=0;i<ncars;i++){
            if(carros[i]->percentage - car->percentage>0 && carros[i]->percentage - car->percentage<2 && carros[i]->speed < car->speed){
                srand(time(0));
                double r=rand();
                if (r>0.9){
                    //Codigo para igualar la velocidad del nuevo carro
                    int newSpeed = carros[i]->speed;
                    car->speed=newSpeed;
                }
            }
        }
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
    struct Car *carsArray[ncars];
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
    for(int id=0;id<ncars;id++){
        struct Car *car= createCar("cari",id,(int **)map,(int *)place);
        carsArray[id]=car;
    }

    #pragma omp parallel num_threads(ncars)
    {
        int id = omp_get_thread_num();
        //srand(time(id));
        //struct Car *car= createCar("cari",id,(int **)map,(int *)place);
        //carsArray[id]=car;
        //incar++;
        carFunction((void *)carsArray[id],(struct Car**) carsArray);
        //printMap((void *)car);
    }

    for(int i=0;i<ncars;i++){
        printf("Place %d: car #%d\n",i,place[i]);
    }
    
    return 0;
    //pthread_exit(NULL);
}
