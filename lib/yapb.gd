#############################################################################
##
##
#W  files.gd                   Example Package                  Werner Nickel
##
##  Declaration file for functions of the Example package.
##
#Y  Copyright (C) 1999,2001 University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##


DeclareCategory("IsConstraint", IsComponentObjectRep);

DeclareRepresentation( "IsSimpleConstraint", IsConstraint, []);

###########################################################################
##
##

#! @Description
#!   Insert documentation for you function here
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

## Get stats from most recent call to SolveStats
DeclareGlobalFunction( "SolveStats" );


#E  files.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
