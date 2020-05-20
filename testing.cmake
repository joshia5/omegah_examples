function(mpi_test TESTNAME PROCS EXE)
  add_test(
    NAME ${TESTNAME}
    COMMAND ${MPIRUN} ${MPIRUN_PROCFLAG} ${PROCS} ${EXE} ${ARGN}
  )
endfunction(mpi_test)

mpi_test(synchronization_2D 4  ./synchronization  ${CMAKE_SOURCE_DIR}/meshes/tri8_4p.osh)
mpi_test(partitioning_2D 4  ./partitioning  ${CMAKE_SOURCE_DIR}/meshes/tri8_4p.osh)
mpi_test(ghosting_2D 4  ./ghosting  ${CMAKE_SOURCE_DIR}/meshes/cube7k_4p.osh)
