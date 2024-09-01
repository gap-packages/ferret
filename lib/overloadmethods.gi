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

_FERRET_ENABLE_OVERLOADS := true;

InstallGlobalFunction(EnableFerretOverloads, function(b)
    # This method is longer than it looks like it needs to be
    # in case b is not a Boolean
    if b then
        _FERRET_ENABLE_OVERLOADS := true;
    else
        _FERRET_ENABLE_OVERLOADS := false;
    fi;
end);

InstallGlobalFunction(FerretOverloadsEnabled, function()
    return _FERRET_ENABLE_OVERLOADS;
end);

_YAPB_fastIsNaturalOrSymmetricGroup := function(G)
    return  ( HasIsNaturalSymmetricGroup(G) and IsNaturalSymmetricGroup(G) ) or
       ( HasIsNaturalAlternatingGroup(G) and IsNaturalAlternatingGroup(G) );
end;

# Based on PermGroupStabilizerOp in oprtperm.gi.

PermGroupStabilizerFerretOp := function(arg)
    local   K,          # stabilizer <K>, result
            S,  base,
	    G,d,gens,acts,act,dom;

 # get arguments, ignoring a given domain
 G:=arg[1];
 K:=Length(arg);
 act:=arg[K];
 acts:=arg[K-1];
 gens:=arg[K-2];
 d:=arg[K-3];

    # If we are not currently trying ferret, exit straight away!
    if not(_FERRET_ENABLE_OVERLOADS) then
      return CallFuncList(PermGroupStabilizerOp, arg);
    fi;
    Info(InfoFerretOverloads, 2, "Considering ferret for Stabilizer: ", arg);
    if gens <> acts  then
        Info(InfoFerretOverloads, 2, "Rejected: generators and actions are different");
        #TODO: Check whether  acts is permutations and one could work in the
        #permutation image (even if G is not permgroups)
        TryNextMethod();
    fi;

    # These are easy and GAP has special methods to do them
    if _YAPB_fastIsNaturalOrSymmetricGroup(G) and act = OnSets and ForAll( d, IsInt )
            then
        Info(InfoFerretOverloads, 2, "Rejected: Sets in the natural symmetric group");
        TryNextMethod();
    fi;

    # First of all, lets dump some easy cases we don't want to handle
    if act = OnPoints or act = OnPairs or act = OnTuples then
        Info(InfoFerretOverloads, 2, "Rejected: acting on points, pairs or tuples");
        return CallFuncList(PermGroupStabilizerOp, arg);
    fi;

    # action on sets of points, use a backtrack
    if act = OnSets  and ForAll( d, IsInt )  then
      if Length(d)=1 then
        K:=Stabilizer(G,d[1]);
      else
        Info(InfoFerretOverloads, 1, "Using ferret for Stabilizer(.., OnSets)");
        K:=Solve([ConInGroup(G), ConStabilize(d, OnSets)]);
      fi;

    # action on sets of pairwise disjoint sets
    elif act = OnSetsDisjointSets
      and IsList(d) and ForAll(d,i->ForAll(i,IsInt)) then
        Info(InfoFerretOverloads, 1, "Using ferret for Stabilizer(.., OnSetsDisjointSets)");
        K := Solve([ConInGroup(G), ConStabilize(d, OnSetsDisjointSets)]);
    # action on sets of sets
    elif act = OnSetsSets
      and IsList(d) and ForAll(d,i->ForAll(i,IsInt)) then
        Info(InfoFerretOverloads, 1, "Using ferret");
        K := Solve([ConInGroup(G), ConStabilize(d, OnSetsSets)]);
    # action on tuples of sets
    elif act = OnTuplesSets
      and IsList(d) and ForAll(d,i->ForAll(i,IsInt)) then
        Info(InfoFerretOverloads, 1, "Using ferret for Stabilizer(.., OnTuplesSets)");
        K := Solve([ConInGroup(G), List(d, x -> ConStabilize(x, OnSets))]);
    # action on sets of tuples
    elif act = OnSetsTuples
      and IsList(d) and ForAll(d,i->ForAll(i,IsInt)) then
        Info(InfoFerretOverloads, 1, "Using ferret for Stabilizer(.., OnSetsTuples)");
        K := Solve([ConInGroup(G), ConStabilize(d, OnSetsTuples)]);
    # action on tuples of tuples
    elif act = OnTuplesTuples
      and IsList(d) and ForAll(d,i->ForAll(i,IsInt)) then
        return Stabilizer(G, Flat(d), OnTuples);
    # other action
    else
        TryNextMethod();
    fi;

    # return the stabilizer
    return K;
end;

## These overloads are taken from oprtperm.gi, allowing us to hook in before the original
## permutation backtrack code

InstallOtherMethod( StabilizerOp, "permutation group with generators list",
       true,
        [ IsPermGroup, IsObject,
          IsList,
          IsList,
          IsFunction ],
  # the objects might be a group element: rank up
  {} -> RankFilter(IsMultiplicativeElementWithInverse)
  # and we are better even if the group is solvable
  +RankFilter(IsSolvableGroup) + 1,
  PermGroupStabilizerFerretOp);

InstallOtherMethod( StabilizerOp, "permutation group with domain",true,
        [ IsPermGroup, IsObject,
	  IsObject,
          IsList,
          IsList,
          IsFunction ],
  # the objects might be a group element: rank up
  {} -> RankFilter(IsMultiplicativeElementWithInverse)
  # and we are better even if the group is solvable
  +RankFilter(IsSolvableGroup) + 1,
  PermGroupStabilizerFerretOp);



# This function replaces Intersection2 for perm groups
  InstallMethod( Intersection2, "perm groups (from Ferret Package)", IsIdenticalObj,
    [ IsPermGroup, IsPermGroup ], 1,
  function( G, H )
  local   Omega,  P,  rbase,  L,mg,mh,i;

      if not(_FERRET_ENABLE_OVERLOADS) then
        TryNextMethod();
      fi;
      Info(InfoFerretOverloads, 2, "Using ferret for Intersection2");

      if IsIdenticalObj( G, H )  then
        return G;
      fi;

        # These are easy and GAP has special methods to do them
      if _YAPB_fastIsNaturalOrSymmetricGroup(G) and _YAPB_fastIsNaturalOrSymmetricGroup(H) then
        TryNextMethod();
      fi;

      # Tighten bounds if possible
      mg := LargestMovedPoint(G);
      mh := LargestMovedPoint(H);

      if mg <> mh then
        if mg < mh then
          H := Stabilizer(H, [mg+1..mh], OnTuples);
        else
          G := Stabilizer(G, [mh+1..mg], OnTuples);
        fi;
      fi;

      # Handle some trivial cases
      if IsSubset(G,H) then
        return H;
      elif IsSubset(H,G) then
        return G;
      fi;

      return Solve([ConInGroup(G), ConInGroup(H)]);
  end );
