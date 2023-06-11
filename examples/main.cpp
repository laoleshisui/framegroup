#include "Template.h"

#include <mutex>
#include <condition_variable>


std::mutex mutex;
std::unique_lock<std::mutex> ul(mutex);
std::condition_variable cv;
    
DECLARE_CAPTUTER(01)

DECLARE_RENDER(02)

int main(){

    RUN(01)
    
    RUN(02)

    cv.wait(ul);
}