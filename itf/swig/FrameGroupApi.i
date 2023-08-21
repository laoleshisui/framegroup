%module(directors="1") framegroup

%inline %{
//#include "itf/FrameGroupApi.h"
#include <src/api/FrameRenderApi.h>
#include <src/api/FrameCapturerApi.h>
#include <src/api/FrameGroupApi.h>
%}

typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;

//%include "itf/FrameGroupApi.h"

%feature("director") FrameRenderObserver;
%include <src/api/FrameRenderApi.h>

%include <stl.i>
namespace std {
  %template(StringVector) std::vector < string >;
}
%include <src/api/FrameCapturerApi.h>

%feature("director") FrameGroupObserver;
%include <src/api/FrameGroupApi.h>