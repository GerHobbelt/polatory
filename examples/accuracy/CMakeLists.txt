set(TARGET accuracy)

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET} PRIVATE
    Boost::program_options
    polatory
)

if(MSVC)
    polatory_target_contents(${TARGET} ${POLATORY_DLLS})
endif()
