#include <iostream>
#include <pthread.h>
#include <atomic>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fstream>
#include <stdlib.h>
#include <random>

using namespace std;

int n, k;
float lambda1, lambda2;
atomic<int> lock(0);
vector<bool> waitingarray(100, false);
FILE *fout = fopen("CAS-BOUNDED-Log.txt", "w");
time_t waitingtime = 0;
time_t maxwaitingtime = 0;

void print_fu()
{
    fprintf(fout, "CAS-Bounded Output\n");
}
int put(int m)
{
    return m;
}
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

void *testingcasbounded(void *para)
{
    int i;
    int m = 1000000;
    int threadid = *(int *)para;
    vector<int> times;
    default_random_engine creator;
    exponential_distribution<double> d1(1.0 / lambda1);
    exponential_distribution<double> d2(1.0 / lambda2);
    i = 0;

    while (i < k)
    {
        waitingarray[threadid - 1] = true;
        bool keyvalue = false;
        time_t reqEnterTime = time(NULL);
        times = func_Time(reqEnterTime);
        fprintf(fout, " %d th CS Request at %d:%d:%d  by thread %d \n", i + 1, times[0], times[1], times[2], threadid);
        while (waitingarray[threadid - 1] && !keyvalue)
        {
            int a = 0;
            int b = 1;
            keyvalue = lock.compare_exchange_strong(a, b);
        }
        waitingarray[threadid - 1] = false;
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
        fprintf(fout, " %d th CS Exited at %d:%d:%d  by thread %d \n", i + 1, times[0], times[1], times[2], threadid);
        int x = threadid % n;
        while ((x != threadid - 1) && !waitingarray[x])
        {
            x = (x + 1) % n;
        }
        if (x == threadid - 1)
        {
            lock = 0;
        }
        else
        {
            waitingarray[x] = false;
        }
        int r = put(m);
        usleep(d2(creator) * r);
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

    pthread_t threadid[n];
    pthread_attr_t attributes[n];
    int arrayofids[n];
    print_fu();

    i = 0;
    while (i < n)
    {
        arrayofids[i] = i + 1;
        pthread_attr_init(&attributes[i]);
        pthread_create(&threadid[i], &attributes[i], testingcasbounded, &arrayofids[i]);
        i++;
    }

    i = 0;
    while (i < n)
    {
        pthread_join(threadid[i], NULL);
        i = i + 1;
    }
    FILE *fpi = fopen("CAS-Bounded-Stats.txt", "w");
    cout << " Average waiting time :" << float(waitingtime) / (n * k) << endl;
    cout << " Max waiting time :" << maxwaitingtime << endl;
    float e = float(waitingtime) / (n * k);
    int max = maxwaitingtime;
    fprintf(fpi, "Avg waiting time: %f\n Max Waiting time: %d\n", e, max);
    return 0;
}
