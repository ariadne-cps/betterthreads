set(LIBRARY_NAME BETTERTHREADS_SRC)

include(CodeCoverage)
append_coverage_compiler_flags()

add_library(${LIBRARY_NAME} OBJECT
        thread.cpp
        buffered_thread.cpp
        thread_pool.cpp
        workload_advancement.cpp
        task_manager.cpp
        )

foreach(WARN ${LIBRARY_EXCLUSIVE_WARN})
    target_compile_options(${LIBRARY_NAME} PUBLIC -W${WARN})
endforeach()
