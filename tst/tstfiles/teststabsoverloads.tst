#############################################################################
##
#A  teststabsoverloads.tst          ferret package            Chris Jefferson
##
##
gap> START_TEST("ferret package: teststabsoverloads.tst");
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> overload := FerretOverloadsEnabled();;
gap> EnableFerretOverloads(true);
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> FerretOverloadsEnabled();
true
gap> g := Group((1,2),(2,3,4,5),(6,7));;

# Unstable tests
#gap> Stabilizer(g, [1,2,3], OnSets) = Group([ (2,3), (1,2), (4,5), (6,7) ]);
#I  Considering ferret for Stabilizer
#I  Using ferret for Stabilizer(.., OnSets)
#true
#gap> Stabilizer(g, [[1,2,3],[2,3,4]], OnSetsSets) = Group([(1,4),(2,3),(6,7)]);
#I  Considering ferret for Stabilizer
#I  Using ferret
#true
#gap> Stabilizer(g, [[1,],[2,3],[4]], OnSetsDisjointSets) = Group([(1,4),(2,3),(6,7)]);
#I  Considering ferret for Stabilizer
#I  Using ferret for Stabilizer(.., OnSetsDisjointSets)
#true
#gap> Stabilizer(g, [[1,],[2,3],[4]], OnTuplesSets) = Group([(2,3),(6,7)]);
#I  Considering ferret for Stabilizer
#I  Using ferret for Stabilizer(.., OnTuplesSets)
#true
gap> Stabilizer(g, [1,2,3], OnTuples) = Group([(4,5),(6,7)]);
#I  Considering ferret for Stabilizer: [ Group( [ (1,2), (2,3,4,5), (6,7) ] ), [ 1, 2, 3 ], [ (1,2), (2,3,4,5), (6,7) ], [ (1,2), (2,3,4,5), (6,7) ], function ( tuple, elm )
<<kernel code>> from src/listfunc.c:OnTuples
end ]
#I  Rejected: acting on points, pairs or tuples
true
gap> Stabilizer(g, [[1,2],[3]], OnTuplesTuples) = Group([(4,5),(6,7)]);
#I  Considering ferret for Stabilizer: [ Group( [ (1,2), (2,3,4,5), (6,7) ] ), [ [ 1, 2 ], [ 3 ] ], [ (1,2), (2,3,4,5), (6,7) ], [ (1,2), (2,3,4,5), (6,7) ], function ( e, g )
return List( e, function ( i )
return OnTuples( i, g );
end );
end ]
#I  Considering ferret for Stabilizer: [ Group( [ (1,2), (2,3,4,5), (6,7) ] ), [ 1, 2, 3 ], [ (1,2), (2,3,4,5), (6,7) ], [ (1,2), (2,3,4,5), (6,7) ], function ( tuple, elm )
<<kernel code>> from src/listfunc.c:OnTuples
end ]
#I  Rejected: acting on points, pairs or tuples
true
gap> Stabilizer(SymmetricGroup(5), [1,2,3,4], OnSets) = SymmetricGroup(4);
true
gap> Stabilizer(SymmetricGroup(5), [1,2,3], OnTuples) = SymmetricGroup([4..5]);
true
gap> Stabilizer(SymmetricGroup(5), [[1,2,3,4],[5]], OnSetsSets) = SymmetricGroup(4);
#I  Considering ferret for Stabilizer: [ SymmetricGroup( [ 1 .. 5 ] ), [ [ 1, 2, 3, 4 ], [ 5 ] ], [ (1,2,3,4,5), (1,2) ], [ (1,2,3,4,5), (1,2) ], function ( e, g )
return Set( e, function ( i )
return OnSets( i, g );
end );
end ]
#I  Using ferret
true
gap> Stabilizer(SymmetricGroup(5), [[1,2,3,4],[5]], OnSetsDisjointSets) = SymmetricGroup(4);
#I  Considering ferret for Stabilizer: [ SymmetricGroup( [ 1 .. 5 ] ), [ [ 1, 2, 3, 4 ], [ 5 ] ], [ (1,2,3,4,5), (1,2) ], [ (1,2,3,4,5), (1,2) ], function ( e, g )
return Set( e, function ( i )
return OnSets( i, g );
end );
end ]
#I  Using ferret for Stabilizer(.., OnSetsDisjointSets)
true
gap> SetInfoLevel(InfoFerretOverloads, 0);
gap> # Functions which GAP implements efficiently
gap> for i in [2..20] do
>      for j in [1..15] do
>        g := RandomGroupUpToSize(GlobalMersenneTwister, i);
>        for act in [OnPoints, OnSets, OnSetsDisjointSets, OnTuplesSets, OnTuplesTuples] do
>          CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act);
>        od;
>     od;
>   od;
gap> # Functions which GAP implements inefficiently
gap> for i in [2..7] do
>      for j in [1..5] do
>        g := RandomGroupUpToSize(GlobalMersenneTwister, i);
>        for act in [OnSetsSets] do
>          CheckStab(g, RandomObj(GlobalMersenneTwister, i, act), act);
>        od;
>     od;
>   od;
gap> EnableFerretOverloads(overload);
gap> STOP_TEST( "teststabsoverloads.tst", 10000 );
