#############################################################################
##
#W    init.g                 The ferret package          Christopher Jefferson
#W
##

#############################################################################

# load kernel function if it is installed
if not LoadKernelExtension("ferret") then
  Error("failed to load the ferret package kernel extension");
fi;

##
#R  Read the declaration files.
##
ReadPackage( "ferret", "lib/yapb.gd" );
ReadPackage( "ferret", "lib/overloadmethods.gd" );

#E  init.g . . . . . . . . . . . . . . . . . . . . . . . . . . . .  ends here

