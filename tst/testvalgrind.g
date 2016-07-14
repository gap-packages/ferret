# This file is designed to just run one test, 'testvalgrind.tst', which we run
# through the 'valgrind' system, to check for memory issues

LoadPackage("io");
if Test( Filename(DirectoriesPackageLibrary( "ferret", "tst/tstfiles" ),"testvalgrind.tst"), rec(showProgress := true) ) = true then
  IO_exit(0);
else
  IO_exit(1);
fi;
