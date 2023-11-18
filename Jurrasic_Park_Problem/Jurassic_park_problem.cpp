#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include <unistd.h>
#include <random>
#define N 1000000

using namespace std;

sem_t sem;

int arr[N][2];

int P, C, k;
float l1, l2;

FILE *fout = fopen("log.txt", "w");
time_t waitingtime = 0;
time_t cwaitingtime = 0;

// a function to give the local time
vector<int> func_Time(time_t param)
{
    vector<int> times;
    tm *TIME;
    TIME = localtime(&param);
    times.push_back(TIME->tm_hour);
    times.push_back(TIME->tm_min);
    times.push_back(TIME->tm_sec);
    return times;
}

// the thread function
void *Pthreadf(void *param)
{
    int i, j = 0;
    int Pid = *(int *)param;
    vector<int> times;
    default_random_engine creator;
    exponential_distribution<double> d1(1.0 / l1);
    exponential_distribution<double> d2(1.0 / l2);

    // got the passengerid of the pass
    //  add the time later

    time_t enterMtime = time(NULL);
    times = func_Time(enterMtime);

    // cout << "Passenger" << Pid << " enters the mueseum at " << times[0] << ":" << times[1] << ":" << times[2] << endl;
    fprintf(fout, "Passenger%d enters the mueseum at %d:%d:%d\n", Pid, times[0], times[1], times[2]);
    while (i < k)
    {
        time_t Rreqtime = time(NULL);
        times = func_Time(Rreqtime);
        // cout << "Passenger" << Pid << " made a ride request at " << times[0] << ":" << times[1] << ":" << times[2] << endl;
        fprintf(fout, "Passenger%d made a ride request at %d:%d:%d\n", Pid, times[0], times[1], times[2]);
        sem_wait(&sem);

        // assigning the car
        for (j = 0; j < C; j++)
        {
            if (arr[j][1] == 0)
            {
                // cout << "Car" << arr[j][0] << " accepts passenger" << Pid << "'s request" << endl;
                fprintf(fout, "Car%d accepts passenger%d's request\n", arr[j][0], Pid);
                arr[j][1] = 1;
                break;
            }
        }
        time_t startRtime = time(NULL);
        times = func_Time(startRtime);
        // cout << "Passenger" << Pid << " starts riding the car"<<arr[j][0]<<" at " << times[0] << ":" << times[1] << ":" << times[2] << endl;
        fprintf(fout, "Passenger%d starts riding the car%d at %d:%d:%d\n", Pid, arr[j][0], times[0], times[1], times[2]);
        // cout << "Car" << arr[j][0] << " is riding passenger" << Pid << endl;
        fprintf(fout, "Car%d is riding passenger%d\n", arr[j][0], Pid);
        usleep(d2(creator) * 1000000);
        // cout << "Car" << arr[j][0] << " finishes passenger" << Pid <<"'s ride"<< endl;
        fprintf(fout, "Car%d finsihes riding passenger%d's ride\n", arr[j][0], Pid);
        arr[j][1] = 0;
        sem_post(&sem);
        time_t finishRtime = time(NULL);
        times = func_Time(finishRtime);
        cwaitingtime = cwaitingtime + finishRtime - startRtime;
        cout << "Passenger" << Pid << " finished riding the car" << arr[j][0] << " at " << times[0] << ":" << times[1] << ":" << times[2] << endl;
        fprintf(fout, "Passenger%d finished riding the car%d at %d:%d:%d\n", Pid, arr[j][0], times[0], times[1], times[2]);
        i = i + 1;

        usleep(d1(creator) * 1000000);
    }
    time_t Passend = time(NULL);
    waitingtime = waitingtime + Passend - enterMtime;
    return 0;
}

// main function
int main()
{
    int i, j;
    ifstream inp;
    inp.open("inp-params.txt");
    inp >> P >> C >> l1 >> l2 >> k;
    cout << k;
    sem_init(&sem, 0, C);
    for (j = 0; j < C; j++)
    {
        arr[j][0] = j + 1;
        arr[j][1] = 0;
    }

    pthread_t threadidP[P];
    pthread_attr_t attributes[P];
    int arrayofPids[P];
    for (i = 0; i < P; i++)
    {
        arrayofPids[i] = i + 1;
        pthread_attr_init(&attributes[i]);
        pthread_create(&threadidP[i], &attributes[i], Pthreadf, &arrayofPids[i]);
    }
    for (i = 0; i < P; i++)
    {
        pthread_join(threadidP[i], NULL);
    }

    sem_destroy(&sem);
    FILE *fpi = fopen("Stats.txt", "w");
    cout << "avg time for the Passengers to complete their rides is " << ((float)waitingtime / P) << "seconds" << endl;
    cout << "avg time of all Cars to complete their tour is " << ((float)cwaitingtime / C) << "seconds" << endl;
    float e = (float)waitingtime / P;
    float f = (float)cwaitingtime / C;
    fprintf(fpi, "Avg time for the Passengers to complete their rides is: %f\navg time of all Cars to complete their tour is: %f\n", e, f);

    return 0;
}