# This file runs tests repeatedly, to try to shake out
# any issues in memory management
LoadPackage( "io" );
LoadPackage( "ferret" );

dirs := DirectoriesPackageLibrary( "ferret", "tst" );

l1 := List([1..50], x ->Test(Filename(dirs, "teststabs.tst")));
l2 := List([1..100], x ->Test(Filename(dirs, "testbasic.tst")));

if ForAny(Concatenation(l1,l2), x -> (x=false)) then
    IO_exit(1);
else
    IO_exit(0);
fi;
