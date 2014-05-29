#############################################################################
##
##
#W  files.gd                   ferret Package                  Chris Jefferson
##
##  Declaration file for types for MinimalImage and CanonicalImage.
##
#Y  Copyright (C) 2014     University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##


#############################################################################
##
## Two transformations of a group used when calculating MinimialImages of
## Transformations, Permutations and PartialPermutations
##

DeclareAttribute( "rowcolsquareGroup", IsPermGroup );
DeclareAttribute( "rowcolsquareGroup2", IsPermGroup );


#############################################################################
##
#F  MinimialImage
##
DeclareOperation( "MinimalImage", [IsPermGroup, IsObject] );

## MinimalImage for lists (takes OnSet, OnSetSets, etc.)

DeclareOperation( "MinimalImage", [IsPermGroup, IsObject, IsFunction] );

#############################################################################
##
#F  CanonicalImage
##
DeclareOperation( "CanonicalImage", [IsPermGroup, IsObject, IsObject] );

DeclareOperation( "MinimalImageUnorderedPair", [IsPermGroup, IsObject]);
DeclareOperation( "MinimalImageUnorderedPair", [IsPermGroup, IsObject, IsFunction]);
DeclareOperation( "MinimalImageOrderedPair", [IsPermGroup, IsObject]);
DeclareOperation( "MinimalImageOrderedPair", [IsPermGroup, IsObject, IsFunction]);

  
DeclareGlobalFunction("GetPerm");
DeclareGlobalFunction("GetImage");
DeclareGlobalFunction("GetBool");



#E  files.gd  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
