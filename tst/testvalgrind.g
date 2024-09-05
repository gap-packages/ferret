# This file is designed to just run one test, 'testvalgrind.tst', which we run
# through the 'valgrind' system, to check for memory issues

if Test( Filename(DirectoriesPackageLibrary( "ferret", "tst/tstfiles" ),"testvalgrind.tst"), rec(showProgress := true) ) = true then
  FORCE_QUIT_GAP(0);
else
  FORCE_QUIT_GAP(1);
fi;
