#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

int nlaps;
int ncars;
int pl=0; //actual place first in array
int **map;
char **carriles;

struct Car{
    char *nombre;
    int id;
    int speed;
    int maxSpeed;
    int lap;
    int place;
    int acceleration;
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
    car->acceleration = rand() % (5 + 1 - 1) + 1;
    car->lap = 1;
    car->place = id+1;
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

void printMap(){
    //system("clear");
    for(int i=0;i<ncars;i++){
        printf("%s",carriles[i]);
    }
    printf("\n");
    //sleep(3);
}



void *carFunction(void *carro, struct Car **carros){
    struct Car *car = (struct Car *)carro;
    //printf("Nombre: %s, id: %d, speed: %d, max speed: %d map: %p, places: %p\n",car->nombre,car->id,car->speed,car->maxSpeed,car->map,car->places);
    int cAcc =0;
    //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
    while(car->finish==0){
        int prePos = car->percentage;
        map[car->id][(int)prePos]=-1;
        car->racetime+=0.1;
        car->percentage += car->speed/100.0;
        map[car->id][(int)car->percentage%100]=car->id;
        for(int i=0;i<ncars;i++){
            if(carros[i]->percentage - car->percentage>0 && carros[i]->percentage - car->percentage<2 && carros[i]->speed < car->speed){
                srand(time(0));
                double r=((double) rand() / (RAND_MAX)) + 1;
                if (r>0.9){
                    //Codigo para igualar la velocidad del nuevo carro
                    int newSpeed = carros[i]->speed;
                    car->speed=newSpeed;
                    //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
                }
            }
        }

        if (cAcc==10){
            int newASpeed = car->speed + car->acceleration;
            if(newASpeed>car->maxSpeed){
                car->speed=car->maxSpeed;
            }else{
                car->speed=newASpeed;
            }
            //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
            cAcc=0;
        }else{
            cAcc++;
        }

        if(car->percentage >= 100){
            car->lap+=1;
            car->percentage=0;
        }
        for(int i=0;i<ncars;i++){
            if(carros[i]->place < car->place){
                if(car->lap > carros[i]->lap){
                    int tmp = car->place;
                    car->place=carros[i]->place;
                    carros[i]->place=tmp;
                    //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
                }else if(car->lap == carros[i]->lap && car->percentage > carros[i]->percentage){
                    int tmp = car->place;
                    car->place=carros[i]->place;
                    carros[i]->place=tmp;
                    //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
                }
            }
        }
        if (car->lap>nlaps){
            car->finish=1;
        }

        char * carril = calloc(101,sizeof(char));
        for(int i=0;i<100;i++){
            if(i==(int) car->percentage){
                //itoa(car->id,carril[i],10);
                //sprintf(carril[i],"%d",car->id);
                carril[i]='X';
            }else{
                carril[i]='-';
            }
        }
        carril[100]='\n';
        carriles[car->id]=carril;
        //sleep(2);
        //printf("carril: %s",carriles[car->id]);
        //free(carril);
        //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
    }
    //sleep(5);
    //printf("Car #%d: lap: %d, percentage: %f, speed: %d, racetime: %f, position: %d\n",car->id,car->lap,car->percentage,car->speed,car->racetime,car->place);
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

    if (ncars<3){
        printf("Pasar porfavor al menos 3 carros\n");
        return -1;
    }

    int place[ncars];
    //int map[ncars][100];
    struct Car *carsArray[ncars];
    map = calloc(ncars,sizeof(int *));
    for(int i=0;i<ncars;i++){
        map[i]=calloc(100,sizeof(int));
    }
    carriles=calloc(ncars,sizeof(char *));

    for (int i=0;i<ncars;i++){
        carriles[i]="X---------------------------------------------------------------------------------------------------\n";
    }

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
        //printMap();
        //printMap();
    }

    for(int i=0;i<3;i++){
        printf("Place %d: car #%d\n",i,place[i]);
    }
    /*
    for (int i=0;i<ncars;i++){
        for(int j=0;j<100;j++){
            printf("%d, ",map[i][j]);
        }
        printf("\n");
    }
    */
    return 0;
    //pthread_exit(NULL);
}
