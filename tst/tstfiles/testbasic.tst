#############################################################################
##
#A  testbasic.tst          ferret package                Chris Jefferson
##
##
gap> START_TEST("ferret package: testbasic.tst");

# Note that you may use comments in the test file
# and also separate parts of the test by empty lines

# First load the package without banner (the banner must be suppressed to
# avoid reporting discrepancies in the case when the package is already
# loaded)
gap> LoadPackage("ferret",false);
true
gap> ReadPackage("ferret", "tst/test_functions.g");
true
gap> SetInfoLevel(InfoFerretOverloads, 9);
gap> overload := FerretOverloadsEnabled();;
gap> EnableFerretOverloads(false);
gap> CheckGroup(Group((1,2)));
true
gap> CheckGroup(Group((1,2),(3,4)));
true
gap> CheckGroup(Group((1,4)(2,5)(3,6), (1,2)(4,5)));
true
gap> g1 := Group((1,4)(2,5)(3,6), (1,2)(4,5), (2,3)(5,6));;
gap> CheckGroup(g1);
true
gap> CheckGroup(makeRowColumnSymmetry(3,4));
true
gap> CheckGroup(makeRowColumnSymmetry(4,4));
true
gap> CheckStab(SymmetricGroup(5), [], OnSets);
true
gap> CheckStab(SymmetricGroup(5), [2,3], OnSets);
true
gap> CheckStab(g1, [], OnSets);
true
gap> CheckStab(g1, [1,2,3], OnSets);
true
gap> CheckStab(g1, [1,6], OnSets);
true
gap> CheckStab(makeRowColumnSymmetry(3,4), [1], OnSets);
true
gap> CheckStab(makeRowColumnSymmetry(3,4), [1,2], OnSets);
true
gap> CheckStab(makeRowColumnSymmetry(3,4), [4,7], OnSets);
true
gap> CheckStab(makeRowColumnSymmetry(7,7), [ 15, 19, 22, 25, 32, 33, 34 ], OnSets);
true
gap> CheckRandomPrimitives();
true
gap> EnableFerretOverloads(overload);
gap> STOP_TEST( "testbasic.tst", 10000 );
ferret package: testbasic.tst
#############################################################################
##
#E
