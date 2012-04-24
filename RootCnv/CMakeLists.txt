require(GaudiKernel GaudiUtils)

find_package(ROOT COMPONENTS RIO Tree TreePlayer MathCore)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS __POOL_COMPATIBILITY)

gaudi_add_library(RootCnvLib *.cpp
                  LINK_LIBRARIES GaudiKernel GaudiUtilsLib ROOT
                  INCLUDE_DIRS GaudiKernel GaudiUtilsLib
                  PUBLIC_HEADERS RootCnv)

gaudi_add_module(RootCnv components/Components.cpp
                 LINK_LIBRARIES RootCnvLib)

# Note: the set_property above is needed because that's the way REFLEX_BUILD_LIBRARIES
#       takes the "-D" flags, and it make the following lines unneeded.
#set_property(TARGET RootCnvLib RootCnv RootCnvDict
#             APPEND PROPERTY COMPILE_DEFINITIONS __POOL_COMPATIBILITY)

#---Dictionaries------------------------------------------------------------
gaudi_add_dictionary(RootCnv dict/RootCnv_dict.h  dict/RootCnv_dict.xml LINK_LIBRARIES RootCnvLib)

add_executable(gaudi_merge merge/merge.cpp)
target_link_libraries(gaudi_merge RootCnvLib)