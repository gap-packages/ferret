#############################################################################
##
#A  advancedgraph.tst          ferret package                Chris Jefferson
##
##
gap> START_TEST("ferret package: advancedgraph.tst");
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> overload := FerretOverloadsEnabled();;
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> EnableFerretOverloads(false);
gap> # Functions which GAP implements inefficiently
gap> for i in [2..9] do
>      for j in [1..50] do
>        g := RandomGroupUpToSize(GlobalMersenneTwister, i);
>        for act in [OnDirectedGraph, OnEdgeColouredDirectedGraph] do
>          for start in [1,2] do
>            for normal in [1,2] do
>              CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act,
>                    rec(start_path_length := start, normal_path_length := normal));
>            od;
>          od;
>        od;
>     od;
>   od;
gap> EnableFerretOverloads(overload);
gap> STOP_TEST( "advancedgraph.tst", 10000 );
ferret package: advancedgraph.tst
#############################################################################
##
#E
