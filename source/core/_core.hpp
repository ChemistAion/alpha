#pragma once

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOMINMAX
#define NODRAWTEXT
#define NOBITMAP
#define NOHELP

#define STRICT

#define UNICODE
#define _UNICODE

////////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

#include <cstring>
#include <cmath>

#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>

#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <intrin.h>

////////////////////////////////////////////////////////////////////////////////

#include <winsdkver.h>
#include <sdkddkver.h>
#include <windows.h>
#include <windowsx.h>

#include <shlwapi.h>

#include <tchar.h>
#include <dbghelp.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <comdef.h>

////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <chrono>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <execution>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <sstream>
#include <stdlib.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <map>
#include <malloc.h>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <optional>
#include <random>
#include <set>
#include <span>
#include <stop_token>
#include <queue>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <type_traits>

using namespace std::literals::chrono_literals;

////////////////////////////////////////////////////////////////////////////////

namespace Alpha
{
#ifdef _DEBUG
	constexpr bool cAlphaMode_Debug = true;
#else
	constexpr bool cAlphaMode_Debug = false;
#endif
}

#include "cista/include/cista/serialization.h"
//#include "cista/include/cista/mmap.h"
//#include "cista/include/cista/containers/variant.h"
//#include "cista/include/cista/targets/file.h"

#include "simdjson/include/simdjson.h"

////////////////////////////////////////////////////////////////////////////////

#include "utils/align.hpp"
#include "utils/decay.hpp"
#include "utils/detector.hpp"

#include "container/aligned_ptr.hpp"
#include "container/ring.hpp"
#include "container/sparse_set.hpp"
#include "container/sparse_map.hpp"

#include "async/synchron.hpp"
#include "async/future.hpp"
#include "async/task.hpp"
#include "async/executor.hpp"

#include "reflection/type.hpp"
#include "singleton/singleton.hpp"

#include "filesystem/entry.hpp"
#include "filesystem/volume.hpp"
#include "filesystem/stream.hpp"

#include "logger/sink.hpp"
#include "logger/logger.hpp"

//#include "platform/console.hpp"
#include "platform/window.hpp"

#include "timer/timer.hpp"

#include "json/value.hpp"
#include "json/accessor.hpp"
#include "config/value.hpp"
#include "config/json.hpp"
#include "config/config.hpp"


////////////////////////////////////////////////////////////////////////////////