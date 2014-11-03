#############################################################################
##
#W    init.g                 The ferret package          Christopher Jefferson
#W
##


# Load C code
if Filename(DirectoriesPackagePrograms("ferret"), "hellod.so") <> fail then
  LoadDynamicModule(Filename(DirectoriesPackagePrograms("ferret"), "hellod.so"));
fi;

#############################################################################
##
#R  Read the declaration files.
##
ReadPackage( "ferret", "lib/yapb.gd" );
ReadPackage( "ferret", "lib/overloadmethods.gd" );

#E  init.g . . . . . . . . . . . . . . . . . . . . . . . . . . . .  ends here

