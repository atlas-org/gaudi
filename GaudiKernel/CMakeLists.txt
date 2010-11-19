############################################################################
# CMakeLists.txt file for building GaudiKernel
# @author Pere Mato, CERN (semi-automaticaly generated)
############################################################################
GAUDI_USE_PACKAGE(Boost)
GAUDI_USE_PACKAGE(CppUnit)
GAUDI_USE_PACKAGE(ROOT)

#---Libraries---------------------------------------------------------------
GAUDI_LINKER_LIBRARY(GaudiKernel Lib/*.cpp LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES})

#---Executables-------------------------------------------------------------
GAUDI_EXECUTABLE(genconf Util/genconf.cpp LIBRARIES GaudiKernel)

#---Tests-------------------------------------------------------------------
GAUDI_UNIT_TEST(DirSearchPath_test ../tests/src/DirSearchPath_test.cpp LIBRARIES GaudiKernel)
GAUDI_UNIT_TEST(test_SerializeSTL ../tests/src/test_SerializeSTL.cpp LIBRARIES GaudiKernel cppunit)

#---Dictionaries------------------------------------------------------------
REFLEX_BUILD_DICTIONARY(GaudiKernel dict/dictionary.h  dict/dictionary.xml LIBRARIES GaudiKernel)

#---Installation------------------------------------------------------------
GAUDI_INSTALL_HEADERS(GaudiKernel)
GAUDI_INSTALL_PYTHON_MODULES()
GAUDI_INSTALL_SCRIPTS()

#---This is needed by SET_RUNTIME_PATH() :-(-----------------------------------
SET( GAUDI_LIBRARY_DIRS ${GAUDI_LIBRARY_DIRS} ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE )
