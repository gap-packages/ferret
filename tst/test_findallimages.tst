#############################################################################
##
#A  test_findallimages.tst       YAPB package                Chris Jefferson
##
##
gap> START_TEST("ferret package: test_findallimages.tst");
gap> LoadPackage("ferret",false);
true
gap> LoadPackage("semigroups", false);
true
gap> t3 := ReadGenerators("trans-3");;
gap> t4 := ReadGenerators("trans-4");;
gap> t5 := ReadGenerators("trans-5");;
gap> mt3 := AllMinimalTransformations(SymmetricGroup(3), 3);;
gap> mt4 := AllMinimalTransformations(SymmetricGroup(4), 4);;
gap> mt5 := AllMinimalTransformations(SymmetricGroup(5), 5);;
gap> t3 = List(mt3, x -> [x]);
true
gap> t4 = List(mt4, x -> [x]);
true
gap> t5 = List(mt5, x -> [x]);
true
gap> STOP_TEST( "test_findallimages.tst", 10000 );
ferret package: test_findallimages.tst
#############################################################################
##
#E
