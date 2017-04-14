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
_PATH_SO:=Filename(DirectoriesPackagePrograms("ferret"), "ferret.so");
if _PATH_SO <> fail then
    LoadDynamicModule(_PATH_SO);
fi;
Unbind(_PATH_SO);

ReadPackage( "ferret", "lib/helper_functions.g" );
ReadPackage( "ferret", "lib/yapb.gi" );
ReadPackage( "ferret", "lib/overloadmethods.gi" );

#E  read.g . . . . . . . . . . . . . . . . . . . . . . . . . . . .  ends here
