#############################################################################
##
#A  test_minimage.tst            YAPB package                Chris Jefferson
##
##
gap> START_TEST("ferret package: test_minimage.tst");

# Note that you may use comments in the test file
# and also separate parts of the test by empty lines

# First load the package without banner (the banner must be suppressed to
# avoid reporting discrepancies in the case when the package is already
# loaded)
gap> LoadPackage("ferret",false);
true
gap> Read("test_functions.g");
gap> CheckMinimalImageTransformations();
gap> # CheckMinimalImagePartialPerm();
gap> CheckMinimalImagePerm();
gap> STOP_TEST( "test_minimage.tst", 10000 );
ferret package: test_minimage.tst
#############################################################################
##
#E
