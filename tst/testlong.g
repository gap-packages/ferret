#
#
# This file runs a longer version of tests
#
#
LoadPackage( "ferret" );


FERRET_EXTRA_TESTS := true;
FERRET_TEST_COUNT := 100;

# Sanity check that we loaded ferret correctly
g := Solve([ConInGroup(AlternatingGroup(3))]);
if g <> AlternatingGroup(3) then
  Print("Failed to correctly load ferret");
  GAP_EXIT(1);
fi;

dirs := DirectoriesPackageLibrary( "ferret", "tst" );

TestDirectory(dirs, rec(exitGAP := true));

FORCE_QUIT_GAP(1);
