#############################################################################
##
#A  testslow.tst            Example package                Alexander Konovalov
##
##  To create a test file, place GAP prompts, input and output exactly as
##  they must appear in the GAP session. Do not remove lines containing
##  START_TEST and STOP_TEST statements.
##
##  The first line starts the test. START_TEST reinitializes the caches and
##  the global random number generator, in order to be independent of the
##  reading order of several test files. Furthermore, the assertion level
##  is set to 2 by START_TEST and set back to the previous value in the
##  subsequent STOP_TEST call.
##
##  The argument of STOP_TEST may be an arbitrary identifier string.
##
gap> START_TEST("ferret package: testslow.tst");

# Note that you may use comments in the test file
# and also separate parts of the test by empty lines

# First load the package without banner (the banner must be suppressed to
# avoid reporting discrepancies in the case when the package is already
# loaded)
gap> LoadPackage("ferret",false);
true
gap> Read("test_functions.g");
gap> g := AtlasGroup("Fi23",NrMovedPoints,31671);;
gap> FastCheckSetStabNoBlock(g, [1..1]);
true
gap> FastCheckSetStabNoBlock(g, [1..10]);
true
gap> FastCheckSetStabNoBlock(g, [1..20]);
true
gap> STOP_TEST( "testslow.tst", 10000 );
ferret package: testslow.tst
#############################################################################
##
#E

# LoadPackage("atlas"); SetInfoLevel(InfoBckt, 10); g := AtlasGroup("Fi23",NrMovedPoints,31671);; Stabilizer(g, [1..10], OnSets);

# LoadPackage("atlas"); LoadPackage("ferret"); g := AtlasGroup("Fi23",NrMovedPoints,31671);;  s := Solve([ConInGroup(g), ConStabilize([1..10], OnSets)]);

# LoadPackage("atlas"); LoadPackage("ferret"); g := AtlasGroup("Fi23",NrMovedPoints,31671);;  s := Stabilizer(g, [[1..10],[11..20]], OnSetsSets);

# LoadPackage("atlas"); LoadPackage("ferret"); g := AtlasGroup("Fi23",NrMovedPoints,31671);;  s := Solve([ConInGroup(g), ConStabilize([[1..10],[11..20]])], OnSetsSets);
