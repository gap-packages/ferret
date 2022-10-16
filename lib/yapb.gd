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

DeclareOperation("ConStabilize", [IsObject, IsObject]);
DeclareOperation("ConStabilize", [IsObject, IsObject, IsRecord]);

#DeclareOperation("ConStabilize", [IsTransformation, IsPosInt]);
#DeclareOperation("ConStabilize", [IsPartialPerm, IsPosInt]);
#DeclareOperation("ConStabilize", [IsList, IsFunction]);

DeclareOperation("ConInGroup", [IsPermGroup, IsRecord]);
DeclareOperation("ConInGroup", [IsPermGroup]);

DeclareGlobalFunction("OnDirectedGraph");
DeclareGlobalFunction("OnEdgeColouredDirectedGraph");


#############################################################################
##
#F  Solve( <list> ) . . . . . . . .  solve a list of constraints
##
DeclareGlobalFunction( "Solve" );


#############################################################################
##  <#GAPDoc Label="InfoFerret">
##  <ManSection>
##  <InfoClass Name="InfoFerret"/>
##
##  <Description>
##  is the info class for showing Ferret's search information.
##  Available levels are 1 (basic stats), 2 (basic search tree),
##  3 ( all information)
##  (see&nbsp;<Ref Sect="Info Functions"/>).
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
DeclareInfoClass( "InfoFerret" );

#############################################################################
##  <#GAPDoc Label="InfoFerretDebug">
##  <ManSection>
##  <InfoClass Name="InfoFerretDebug"/>
##
##  <Description>
##  is the info class which shows Ferret's internal behaviour.
##  This info level is not of general interest, and is only enabled.
##  when ferret is compiled using the PRINT option.
##  (see&nbsp;<Ref Sect="Info Functions"/>).
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
DeclareInfoClass( "InfoFerretDebug" );

#E  files.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
