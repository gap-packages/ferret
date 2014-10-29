#############################################################################
##
##
#W  yapb.gd                Ferret Package                Chris Jefferson
##
##  Declaration file for functions of the Ferret package.
##
#Y  Copyright (C) 1999,2001 University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##


DeclareCategory("IsConstraint", IsComponentObjectRep);

DeclareRepresentation( "IsSimpleConstraint", IsConstraint, []);

###########################################################################
##
##

DeclareOperation("ConStabilize", [IsObject]);
DeclareOperation("ConStabilize", [IsObject, IsObject]);

#DeclareOperation("ConStabilize", [IsTransformation, IsPosInt]);
#DeclareOperation("ConStabilize", [IsPartialPerm, IsPosInt]);
#DeclareOperation("ConStabilize", [IsList, IsFunction]);

DeclareOperation("ConInGroup", [IsPermGroup, IsString]);
DeclareOperation("ConInGroup", [IsPermGroup]);

DeclareGlobalFunction("OnDirectedGraph");


#############################################################################
##
#F  Solve( <list> ) . . . . . . . .  solve a list of constraints
##
DeclareGlobalFunction( "Solve" );



#E  files.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
