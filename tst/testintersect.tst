#############################################################################
##
#A  testintersect.tst            YAPB package              Chris Jefferson
##
##
gap> START_TEST("ferret package: testintersect.tst");
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> Intersection(Group((1,2),(3,4,5)), Group((2,3,4)), SymmetricGroup(5));
#I  Using ferret for Intersection2
#I  Considering ferret for Stabilizer
#I  Using ferret for Intersection2
#I  Considering ferret for Stabilizer
Group(())
gap> EnableFerretOverloads(false);
gap> for i in [2..20] do
>      for j in [1..50] do
>        grps := List([1..Random([1..6])], x -> RandomGroupUpToSize(i));
>        if Intersection(grps) <> Solve(List(grps, ConInGroup)) then
>          Print("bad intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, "BlockStabChain"))) then
>          Print("bad block intersection: ", grps);
>        fi;
>     od;
>   od;
gap> SetInfoLevel(InfoFerretOverloads, 0);
gap> EnableFerretOverloads(true);
gap> for i in [2..20] do
>      for j in [1..50] do
>        grps := List([1..Random([1..6])], x -> RandomGroupUpToSize(i));
>        if Intersection(grps) <> Solve(List(grps, ConInGroup)) then
>          Print("bad intersection: ", grps);
>        fi;
>        if Intersection(grps) <> Solve(List(grps, x -> ConInGroup(x, "BlockStabChain"))) then
>          Print("bad block intersection: ", grps);
>        fi;
>     od;
>   od;
gap> STOP_TEST( "testintersect.tst", 10000 );
ferret package: testintersect.tst
#############################################################################
##
#E
