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
##  <#GAPDoc Label="ConStabilize">
##
##  <ManSection>
##  <Func Name="ConStabilize" Arg="s"/>
##  <Returns>
##    nothing
##  </Returns>
##  <Description>
##  Closes scscp connection <A>s</A>, which must be an object in the
##  category  on the underlying
##  input/output TCP stream of <A>s</A>.
##  <Example>
##  <![CDATA[
##  gap> SetInfoLevel( InfoSCSCP, 0 );
##  gap> s:=NewSCSCPconnection("localhost",26133);
##  < connection to localhost:26133 session_id=localhost:26133:52918 >
##  gap> CloseSCSCPconnection(s);
##  ]]>
##  </Example>
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
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

## Get stats from most recent call to SolveStats
DeclareGlobalFunction( "SolveStats" );


#E  files.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
