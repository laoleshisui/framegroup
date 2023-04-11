%module framegroup

%inline %{
#include "../FrameGroupApi.h"
%}

typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;

%include "FrameGroupApi.h"