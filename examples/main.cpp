#include "Template.h"

#include <mutex>
#include <condition_variable>


std::mutex mutex;
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;
    
DECLARE_CAPTUTER(a)
DECLARE_CAPTUTER(b)
DECLARE_CAPTUTER(c)
DECLARE_CAPTUTER(d)
DECLARE_CAPTUTER(e)

DECLARE_RENDER(02)
DECLARE_RENDER(03)
DECLARE_RENDER(04)
DECLARE_RENDER(05)
DECLARE_RENDER(06)
DECLARE_RENDER(07)
DECLARE_RENDER(08)
DECLARE_RENDER(09)
DECLARE_RENDER(10)

int main(){

    RUN(a)
    RUN(b)
    RUN(c)
    RUN(d)
    RUN(e)

    RUN(02)
    RUN(03)
    RUN(04)
    RUN(05)
    RUN(06)
    RUN(07)
    RUN(08)
    RUN(09)
    RUN(10)

    cv.wait(ul);
}