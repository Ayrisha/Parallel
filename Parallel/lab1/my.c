#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define DT 0.05

typedef struct
{
    double x, y;
} vector;

int bodies, timeSteps;
double *masses, GravConstant;
vector *positions, *velocities, *accelerations;

vector addVectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};

    return c;
}

vector scaleVector(double b, vector a)
{
    vector c = {b * a.x, b * a.y};

    return c;
}

vector subtractVectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};

    return c;
}

double mod(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

void initiateSystem(char *fileName)
{
    int i;
    FILE *fp = fopen(fileName, "r");

    fscanf(fp, "%lf%d%d", &GravConstant, &bodies, &timeSteps);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));

    for (i = 0; i < bodies; i++)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}


vector minus(vector a){
    vector c = {-a.x, -a.y};

    return c;
}

void simulate()
{
    vector** vectorArray = (vector**)malloc(bodies * sizeof(vector*));
    vector c = {0.0, 0.0};

    for (int i = 0; i < bodies; i++) {
        vectorArray[i] = (vector*)malloc(bodies * sizeof(vector));
    }

    #pragma omp parallel for num_threads(1) private(j)
    for (int i = 0; i < bodies; i++) {
        for (int j = 0; j < bodies; j++) {
            vectorArray[i][j] = c;
        }
    }

    #pragma omp parallel for num_threads(1)
    for (int i = 0; i < bodies; i++)
    {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
        for (int j = 0; j < bodies; j++)
        {
            if (i != j)
            {
                if (vectorArray[i][j].x == 0.0 && vectorArray[i][j].y == 0.0){
                    accelerations[i] = addVectors(accelerations[i], scaleVector(GravConstant * masses[j] / pow(mod(subtractVectors(positions[i], positions[j])), 3), subtractVectors(positions[j], positions[i])));
                    #pragma omp critical
                    {
                        vectorArray[i][j] = accelerations[i];
                        vectorArray[j][i] = minus(accelerations[i]);
                    }
                }
                else{
                    accelerations[i] = addVectors(accelerations[i], vectorArray[i][j]);
                }
            }
        }
        #pragma omp barrier
        positions[i] = addVectors(positions[i], scaleVector(DT,velocities[i]));
        velocities[i] = addVectors(velocities[i], scaleVector(DT, accelerations[i]));
    }


    for (int i = 0; i < bodies; i++) {
        free(vectorArray[i]);
    }
    free(vectorArray);
}

int main(int argC, char *argV[])
{
    clock_t begin = clock();
    int i, j;

    if (argC != 2)
        printf("Usage : %s <file name containing system configuration data>", argV[0]);
    else
    {
        initiateSystem(argV[1]);
        FILE *fp = fopen("outputmy.txt", "w");
        fprintf(fp, "Body   :     x              y           vx              vy   ");
        for (i = 0; i < timeSteps; i++)
        {
            fprintf(fp, "\nCycle %d\n", i + 1);
            simulate();
            for (j = 0; j < bodies; j++)
                fprintf(fp, "Body %d : %lf\t%lf\t%lf\t%lf\n", j + 1, positions[j].x, positions[j].y, velocities[j].x, velocities[j].y);
        }
        fclose(fp);
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%lf", time_spent);
    return 0;
}
