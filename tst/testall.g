#
# This file runs package tests. It is also referenced in the package
# metadata in PackageInfo.g.
#
LoadPackage( "ferret" );

# Sanity check that we loaded ferret correctly
g := Solve([ConInGroup(AlternatingGroup(3))]);
if g <> AlternatingGroup(3) then
  Print("Failed to correctly load ferret");
  GAP_EXIT(1);
fi;

dirs := DirectoriesPackageLibrary( "ferret", "tst" );

TestDirectory(dirs, rec(exitGAP := true));

FORCE_QUIT_GAP(1);
