function(mpi_test TESTNAME PROCS EXE)
  add_test(
    NAME ${TESTNAME}
    COMMAND ${MPIRUN} ${MPIRUN_PROCFLAG} ${PROCS} ${EXE} ${ARGN}
  )
endfunction(mpi_test)

mpi_test(sync_2D_2 2  ./synchronization /users/joshia5/new_mesh/tri8_2p.osh)
mpi_test(sync_2D_4 4  ./sync_4p /space/joshia5/omegah_examples/meshes/tri8_4p.osh)
