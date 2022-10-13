#############################################################################
##
#A  teststabs.tst          ferret package                Chris Jefferson
##
##
gap> START_TEST("ferret package: teststabs.tst");
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> overload := FerretOverloadsEnabled();;
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> EnableFerretOverloads(false);
gap> # Functions which GAP implements efficiently
gap> for i in [2..20] do
>      for j in [1..50] do
>        g := RandomGroupUpToSize(GlobalMersenneTwister, i);
>        for act in [OnPoints, OnSets, OnSetsDisjointSets, OnTuplesSets, OnTuplesTuples] do
>          CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act);
>        od;
>     od;
>   od;
gap> EnableFerretOverloads(overload);
gap> STOP_TEST( "teststabs.tst", 10000 );
ferret package: teststabs.tst
#############################################################################
##
#E
