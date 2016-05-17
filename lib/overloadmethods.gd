#############################################################################
##
##
#W  overloadmethods.gd    ferret Package                  Chris Jefferson
##
##  Replace built in GAP methods.
##
#Y  Copyright (C) 2014     University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##


#############################################################################
##  <#GAPDoc Label="EnableFerretOverloads">
##  <ManSection>
##  <Func Name="EnableFerretOverloads" Arg="[active]"/>
##
##  <Description>
##  if <A>active</A> (a bool) is true, then enable
##  Ferret specialisations of Intersection and Stabilizer.
##  Call with <A>active</A> false to disable.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
DeclareGlobalFunction( "EnableFerretOverloads" );

#############################################################################
##  <#GAPDoc Label="FerretOverloadsEnabled">
##  <ManSection>
##  <Func Name="FerretOverloadsEnabled" Arg=""/>
##
##  <Description>
##  Return if Ferret specialisations of Intersection and Stabilizer
##  are currently enabled.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
DeclareGlobalFunction( "FerretOverloadsEnabled" );

#############################################################################
##  <#GAPDoc Label="InfoFerretOverloads">
##  <ManSection>
##  <InfoClass Name="InfoFerretOverloads"/>
##
##  <Description>
##  is the info class for showing if Ferret is being called when GAP methods
##  such as Intersection or Stabilizer are called. 
##  (see&nbsp;<Ref Sect="Info Functions"/>).
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
##
DeclareInfoClass( "InfoFerretOverloads" );
