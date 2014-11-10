#############################################################################
##
#W    read.g                 The Ferret package          Christopher Jefferson
#W
##

#############################################################################
##
#R  Read the install files.
##

# Load C code
if Filename(DirectoriesPackagePrograms("ferret"), "hellod.so") <> fail then
  LoadDynamicModule(Filename(DirectoriesPackagePrograms("ferret"), "hellod.so"));
fi;

ReadPackage( "ferret", "lib/yapb.gi" );
ReadPackage( "ferret", "lib/overloadmethods.gi" );

#E  read.g . . . . . . . . . . . . . . . . . . . . . . . . . . . .  ends here
