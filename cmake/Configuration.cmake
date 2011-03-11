#------------Based on LCGCMT_59-------------------------
if(WIN32)
  set(LCG_home E:/local/lib/lcg)
  set(LCG_releases ${LCG_home}/external)
else()
  set(LCG_home /afs/cern.ch/sw/lcg)
  set(LCG_releases ${LCG_home}/app/releases)
endif()
set(LCG_external ${LCG_home}/external)

set(LCG_platform ${BINARY_TAG})
set(LCG_basesystem ${BINARY_TAG_PREFIX})
set(LCG_system ${LCG_basesystem}-opt)

# please keep alphabetic order and the structure (tabbing).
# it makes it much easier to edit/read this file !

SET(LCG_config_version  60a)

SET(COOL_config_version   COOL_2_8_9)
SET(CORAL_config_version  CORAL_2_3_14b)
SET(POOL_config_version   POOL_2_9_12)
SET(RELAX_config_version  RELAX_1_2_2a)
SET(ROOT_config_version   5.28.00a)

# externals:

SET(AIDA_config_version             3.2.1)

# keep the follwing two in sync !
SET(4suite_config_version           1.0.2p1)
SET(bjam_config_version             3.1.13)
SET(blas_config_version             20070405)
SET(Boost_config_version            1.44.0)
SET(Boost_file_version              1_44)
SET(bz2lib_config_version           1.0.2)
SET(CASTOR_config_version           2.1.8-10)
SET(cernlib_config_version          2006a)
SET(cgal_config_version             3.3.1)
SET(cgsigsoap_config_version        1.3.3-1)
SET(CLHEP_config_version            1.9.4.7)
SET(cmake_config_version            2.6.4)
SET(cmt_config_version              v1r20p20081118)
SET(coin3d_config_version           3.1.3.p1)
SET(CppUnit_config_version          1.12.1_p1)
SET(cx_oracle_config_version        5.0.1)
SET(Cygwin_config_version           1.5)
SET(david_config_version            1_36a)
SET(dawn_config_version             3_88a)
SET(dcache_client_config_version    1.9.3p1)
SET(dcache_config_version_dcap      1.9.3-7)
SET(dcache_config_version_srm       1.8.0-15p7)
SET(doxygen_config_version          1.7.0)
SET(dpm_config_version              1.7.4-7sec)
SET(EDGRLS_config_version           2.3.3)
SET(Expat_config_version            1.95.8)
SET(fftw_config_version             3.1.2)
SET(Frontier_Client_config_version  2.8.0)
SET(fastjet_config_version          2.4.2p5)
SET(GCCXML_config_version           0.9.0_20100114)
SET(genshi_config_version           0.6)
SET(gfal_config_version             1.11.8-2)
SET(globus_config_version           4.0.7-VDT-1.10.1)
SET(graphviz_config_version         2.24.0)
SET(GSL_config_version              1.10)
SET(HepMC_config_version            2.03.11)
SET(HepPDT_config_version           2.06.01)
SET(HippoDraw_config_version        1.9.0)
SET(ipython_config_version          0.8.4)
SET(javasdk_config_version          1.6.0)
SET(javajni_config_version          ${javasdk_config_version})
SET(json_config_version             2.1.1)
SET(kcachegrind_config_version      0.4.6)
SET(ktjet_config_version            1.08)
SET(lapack_config_version           3.1.1)
SET(lcgdmcommon_config_version      1.7.4-7sec)
SET(lcginfosites_config_version     2.6.2-1)
SET(lcgutils_config_version         1.7.6-1)
SET(lcov_config_version             1.9)
SET(lfc_config_version              1.7.4-7sec)
SET(libsvm_config_version           2.86)
SET(libunwind_config_version        0.99)
SET(matplotlib_config_version       0.99.1.2)
SET(minuit_config_version           5.27.02)
SET(MathLib_config_version          1_0_0)
SET(mock_config_version             0.6.0)
SET(multiprocessing_config_version  2.6.2.1)
SET(myproxy_config_version          4.2-VDT-1.10.1)
SET(mysql_config_version            5.1.45)
SET(mysql_python_config_version     1.2.2)
SET(neurobayes_config_version       10.8)
SET(neurobayes_expert_config_version 10.8)
SET(numpy_config_version            1.3.0)
SET(oracle_config_version           11.2.0.1.0p2)
SET(processing_config_version       0.51)
SET(py_config_version               1.2.1)
SET(pydot_config_version            1.0.2)
SET(pyanalysis_config_version       1.2)
SET(pygraphics_config_version       1.1p1)
SET(pyminuit_config_version         0.0.1)
SET(pytools_config_version          1.4)
SET(Python_config_version           2.6.5)
SET(Python_config_version_twodigit  2.6)
SET(pyqt_config_version             4.7)
SET(pyparsing_config_version        1.5.1)
SET(pyxml_config_version            0.8.4p1)
SET(QMtest_config_version           2.4.1)
SET(Qt_config_version               4.6.3.p1)
SET(qwt_config_version              5.2.1)
SET(readline_config_version         2.5.1p1)
SET(roofit_config_version           3.10p1)
SET(rx_config_version               1.5)
SET(setuptools_config_version       0.6c11)
SET(scipy_config_version            0.7.1)
SET(scons_config_version            0.96.1)
SET(sip_config_version              4.10)
SET(siscone_config_version          1.3.3)
SET(slc4_compat_config_version      1.0)
SET(soqt_config_version             1.5.0.p1)
SET(sqlalchemy_config_version       0.6.3)
SET(sqlite_config_version           3.6.22)
SET(storm_config_version            0.16.0)
SET(sympy_config_version            0.6.7)
SET(tcmalloc_config_version         1.5)
SET(tcsh_config_version             6.16.00)
SET(uuid_config_version             1.38p1)
SET(valgrind_config_version         3.6.0)
SET(vdt_config_version              1.2.3_globusrls215b)
SET(vomsapic_config_version         1.9.17-1)
SET(vomsapicpp_config_version       1.9.17-1)
SET(vomsapi_noglobus_config_version 1.9.17-1)
SET(vomsclients_config_version      1.9.17-1)
SET(XercesC_config_version          3.1.1p1)
SET(xqilla_config_version           2.2.4)
SET(zlib_config_version             1.2.3p1)
