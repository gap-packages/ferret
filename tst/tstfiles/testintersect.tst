#############################################################################
##
#A  testintersect.tst          ferret package              Chris Jefferson
##
##
gap> START_TEST("ferret package: testintersect.tst");
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> overload := FerretOverloadsEnabled();;
gap> EnableFerretOverloads(true);
gap> SetInfoLevel(InfoFerretOverloads, 9);

# Unstable test
#gap> Intersection(Group((1,2),(3,4,5),(8,9),(10,11)), Group((2,3),(4,5),(8,9)), Group((1,2),(6,7)));
#I  Using ferret for Intersection2
#I  Considering ferret for Stabilizer
#I  Using ferret for Intersection2
#I  Considering ferret for Stabilizer
#Group(())
gap> EnableFerretOverloads(false);
gap> for i in [2..20] do
>      for j in [1..50] do
>        grps := List([1..Random([1..6])], x -> RandomGroupUpToSize(GlobalMersenneTwister, i));
>        if Intersection(grps) <> Solve(List(grps, ConInGroup)) then
>          Print("bad intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, rec(blocks:="always")))) then
>          Print("bad block intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, rec(orbitals:="always")))) then
>          Print("bad orbital intersection: ", grps);
>        fi;
>     od;
>   od;
gap> SetInfoLevel(InfoFerretOverloads, 0);
gap> EnableFerretOverloads(true);
gap> for i in [2..20] do
>      for j in [1..50] do
>        grps := List([1..Random([1..6])], x -> RandomGroupUpToSize(GlobalMersenneTwister, i));
>        if Intersection(grps) <> Solve(List(grps, ConInGroup)) then
>          Print("bad intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, rec(blocks:="always")))) then
>          Print("bad block intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, rec(orbitals:="always")))) then
>          Print("bad orbital intersection: ", grps);
>        fi;
>     od;
>   od;
gap> EnableFerretOverloads(overload);
gap> STOP_TEST( "testintersect.tst", 10000 );
ferret package: testintersect.tst
#############################################################################
##
#E
