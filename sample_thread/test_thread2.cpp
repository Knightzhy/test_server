#include <iostream>
#include <stdint.h>
#include <thread>
#include <unistd.h>
#include <atomic>

using namespace std;

atomic<int64_t> count(0);

void output()
{
    cout << count << endl;
}

void work()
{
    for (int64_t i = 0; i< 10000000000 ; i++) {
        count++;
    }
}

int main()
{
    for (int i = 0; i < 1 ; i++) {
        thread t(work);
        t.detach();
    }
    sleep(1);

    // get count
    output();

    sleep(9);

    // get count
    output();

    return 0;
}
