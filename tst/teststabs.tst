#############################################################################
##
#A  teststabs.tst            YAPB package                Chris Jefferson
##
##
gap> START_TEST("ferret package: teststabs.tst");
gap> LoadPackage("ferret",false);
true
gap> Read("test_functions.g");
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> EnableFerretOverloads(false);
# Functions which GAP implements efficently
gap> for i in [2..20] do
>      for j in [1..50] do
>        g := RandomGroupOfSize(i);
>        for act in [OnPoints, OnSets, OnSetsDisjointSets, OnTuplesSets, OnTuplesTuples] do
>          CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act);
>        od;
>     od;
>   od;
gap> for i in [2..7] do
>      for j in [1..20] do
>        g := RandomGroupOfSize(i);
>        for act in [OnSetsSets] do
>          CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act);
>        od;
>     od;
>   od;
gap> STOP_TEST( "teststabs.tst", 10000 );
ferret package: teststabs.tst
#############################################################################
##
#E