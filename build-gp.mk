# build & test automation

APP_NAME=grand-prix
APP_EXE=gp

build:
	gcc -fopenmp ${APP_NAME}.c -o ${APP_EXE}
test: build
	@echo This program will be manually executed
	@echo ./${APP_EXE} \#laps \#cars
