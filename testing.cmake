function(mpi_test TESTNAME PROCS EXE)
  add_test(
    NAME ${TESTNAME}
    COMMAND ${MPIRUN} ${MPIRUN_PROCFLAG} ${PROCS} ${EXE} ${ARGN}
  )
endfunction(mpi_test)

mpi_test(adjacencies_2D 1  ./adjacencies  ${CMAKE_SOURCE_DIR}/meshes/tri8.osh)
mpi_test(adjacencies_cube7k_3D 1  ./adjacencies  ${CMAKE_SOURCE_DIR}/meshes/cube7k.osh)
mpi_test(adjacencies_xgc_3D 1  ./adjacencies  /lore/cwsmith/geometries/xgc3d/linear_extrusion_tetra_16.osh)
mpi_test(tags_2D 1  ./tags  ${CMAKE_SOURCE_DIR}/meshes/tri8.osh)
