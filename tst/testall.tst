#############################################################################
##
#A  testall.tst            YAPB package                Chris Jefferson
##
##
gap> START_TEST("ferret package: testall.tst");

# Note that you may use comments in the test file
# and also separate parts of the test by empty lines

# First load the package without banner (the banner must be suppressed to
# avoid reporting discrepancies in the case when the package is already
# loaded)
gap> LoadPackage("ferret",false);
true
gap> Read("test_functions.g");
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
gap> CheckSetStab(SymmetricGroup(5), []);
true
gap> CheckSetStab(SymmetricGroup(5), [2,3]);
true
gap> CheckSetStab(g1, []);
true
gap> CheckSetStab(g1, [1,2,3]);
true
gap> CheckSetStab(g1, [1,6]);
true
gap> CheckSetStab(makeRowColumnSymmetry(3,4), [1]);
true
gap> CheckSetStab(makeRowColumnSymmetry(3,4), [1,2]);
true
gap> CheckSetStab(makeRowColumnSymmetry(3,4), [4,7]);
true
gap> CheckSetStab(makeRowColumnSymmetry(7,7), [ 15, 19, 22, 25, 32, 33, 34 ]);
true
gap> CheckRandomPrimitives();
true
gap> STOP_TEST( "testall.tst", 10000 );
ferret package: testall.tst
#############################################################################
##
#E
