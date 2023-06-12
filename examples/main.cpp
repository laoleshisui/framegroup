#include "Template.h"

#include <mutex>
#include <condition_variable>


std::mutex mutex = {};
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;
    
DECLARE_CAPTUTER(a)
DECLARE_CAPTUTER(b)
DECLARE_CAPTUTER(c)
DECLARE_CAPTUTER(d)
DECLARE_CAPTUTER(e)

DECLARE_RENDER(01)
DECLARE_RENDER(02)
DECLARE_RENDER(03)
DECLARE_RENDER(04)
DECLARE_RENDER(05)

DECLARE_CAPTUTER_RENDER(A)
DECLARE_CAPTUTER_RENDER(B)
DECLARE_CAPTUTER_RENDER(C)
DECLARE_CAPTUTER_RENDER(D)
DECLARE_CAPTUTER_RENDER(E)


int main(){

    RUN(a)
    RUN(b)
    RUN(c)
    RUN(d)
    RUN(e)

    RUN(01)
    RUN(02)
    RUN(03)
    RUN(04)
    RUN(05)

    RUN(A)
    RUN(B)
    RUN(C)
    RUN(D)
    RUN(E)

    cv.wait_for(ul, std::chrono::duration(std::chrono::milliseconds(5000)));
    CORE_LOG(INFO)<< "main() finished.";     
}