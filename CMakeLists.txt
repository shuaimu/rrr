cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -DCOROUTINE -DCOROUTINE_COUNT")

include_directories(.)

add_library(rrr
    base/all.hpp
    base/basetypes.cpp
    base/basetypes.hpp
    base/debugging.cpp
    base/debugging.hpp
    base/logging.cpp
    base/logging.hpp
    base/misc.cpp
    base/misc.hpp
    base/strop.cpp
    base/strop.hpp
    base/threading.cpp
    base/threading.hpp
    base/unittest.cpp
    base/unittest.hpp
    misc/alarm.hpp
    misc/alock.cpp
    misc/alock.hpp
    misc/cpuinfo.hpp
    misc/dball.hpp
    misc/marshal.cpp
    misc/marshal.hpp
    misc/rand.cpp
    misc/rand.hpp
    misc/recorder.cpp
    misc/recorder.hpp
    misc/stat.hpp
    mq/buf.cpp
    mq/buf.h
    mq/client.cpp
    mq/client.h
    mq/polling.cpp
    mq/polling.h
    mq/rpc.cpp
    mq/rpc.h
    mq/rpc_comm.h
    mq/server.cpp
    mq/server.h
    pylib/simplerpc/_pyrpc.cpp
    rpc/client.cpp
    rpc/client.hpp
    rpc/polling.cpp
    rpc/polling.hpp
    rpc/server.cpp
    rpc/server.hpp
    rpc/utils.cpp
    rpc/utils.hpp
    utils/hostname.h
    utils/logger.h
    utils/mht.h
    utils/mlock.h
    utils/mpr_dag.h
    utils/mpr_hash.h
    utils/mpr_queue.h
    utils/mpr_thread_pool.h
    utils/mtime.h
    utils/safe_assert.h
    event/fastStackAllocator.cpp
    event/scheduler.cpp
    event/coroPool.cpp
    rrr.h
    rrr.hpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Threads)
find_package(PkgConfig REQUIRED)
pkg_check_modules(APR REQUIRED apr-1)

IF(APPLE)
ELSE()
pkg_check_modules(PYTHON2 REQUIRED python2)
ENDIF()

link_directories(
	${PYTHON2_LIBRARY_DIRS}
	${APR_LIBRARY_DIRS}
	)

include_directories(
	${PYTHON2_INCLUDE_DIRS}
	${APR_INCLUDE_DIRS}
	.
	)

add_executable(func event/testcase/function.cpp)
add_executable(blank_coroutine event/testcase/blank_coroutine.cpp)
add_executable(switch event/testcase/switch.cpp)
add_executable(coro_pool_test event/testcase/coro_pool_test.cpp)
add_executable(test_scheduler event/testcase/test_scheduler.cpp)

target_link_libraries(
    test_scheduler
    rrr   ${CMAKE_THREAD_LIBS_INIT}   "boost_coroutine"   "boost_context")

target_link_libraries(
	coro_pool_test
	rrr	${CMAKE_THREAD_LIBS_INIT}	"boost_coroutine"	"boost_context"	)

target_link_libraries(
	switch
	rrr	${CMAKE_THREAD_LIBS_INIT}	"boost_coroutine"	"boost_context"	)

target_link_libraries(
	blank_coroutine
	rrr	${CMAKE_THREAD_LIBS_INIT}	"boost_coroutine"	"boost_context"	)

target_link_libraries(
	func	rrr	${CMAKE_THREAD_LIBS_INIT}	"boost_coroutine"	"boost_context"	)
