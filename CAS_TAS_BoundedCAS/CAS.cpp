#include <iostream>
#include <pthread.h>
#include <atomic>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fstream>
#include <algorithm>
#include <random>

using namespace std;

int n, k;
float lambda1, lambda2;
atomic<int> lock(0);
FILE *fout = fopen("CAS-Log.txt", "w");
time_t waitingtime = 0;
time_t maxwaitingtime = 0;

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
int put(int m)
{
    return m;
}

void *testingcas(void *para)
{
    int threadid = *(int *)para;
    int i;
    vector<int> times;
    default_random_engine creator;
    exponential_distribution<double> d1(1.0 / lambda1);
    exponential_distribution<double> d2(1.0 / lambda2);
    i = 0;

    while (i < k)
    {
        time_t reqEnterTime = time(NULL);
        times = func_Time(reqEnterTime);
        fprintf(fout, " %d th CS Request at %d:%d:%d  by thread %d \n", i + 1, times[0], times[1], times[2], threadid);
        while (1)
        {
            int a = 0;
            int b = 1;
            if (lock.compare_exchange_strong(a, b))
            {
                break;
            }
        }
        // entry section

        time_t startTime = time(NULL);
        waitingtime = waitingtime + startTime - reqEnterTime;
        maxwaitingtime = max(maxwaitingtime, startTime - reqEnterTime);
        times = func_Time(startTime);
        fprintf(fout, " %d th CS Entered at %d:%d:%d  by thread %d \n", i + 1, times[0], times[1], times[2], threadid);
        usleep(d1(creator) * 1000000);
        // critical section

        time_t exitTime = time(NULL);
        times = func_Time(exitTime);
        fprintf(fout, " %d th CS Exited  at %d:%d:%d  by thread %d \n", i + 1, times[0], times[1], times[2], threadid);
        lock = 0;
        usleep(d2(creator) * 1000000);
        i = i + 1;
    }
    return 0;
}

int main()
{
    int i;
    ifstream fin;
    fin.open("inp-params.txt");
    fin >> n >> k >> lambda1 >> lambda2;

    fprintf(fout, "CAS Output\n");

    pthread_t threadid[n];
    pthread_attr_t attributes[n];
    int arrayofids[n];
    i = 0;
    while (i < n)
    {
        int k = put(i);
        arrayofids[k] = k + 1;
        pthread_attr_init(&attributes[k]);
        pthread_create(&threadid[k], &attributes[k], testingcas, &arrayofids[k]);
        i = i + 1;
    }
    i = 0;
    while (i < n)
    {
        k = put(i);
        pthread_join(threadid[k], NULL);
        i = i + 1;
    }
    FILE *fpi = fopen("CAS-Stats.txt", "w");
    cout << " Average waiting time :" << float(waitingtime) / (n * k) << endl;
    cout << " Max waiting time :" << maxwaitingtime << endl;
    float e = float(waitingtime) / (n * k);
    int max = maxwaitingtime;
    fprintf(fpi, "Avg waiting time: %f\n Max Waiting time: %d\n", e, max);
    return 0;
}
