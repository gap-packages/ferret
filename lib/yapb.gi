#############################################################################
##
##
#W  yapb.gi                Ferret Package                Chris Jefferson
##
##  Installation file for functions of the Ferret package.
##
#Y  Copyright (C) 1999,2001 University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##

####
# Functions and variables beginning '_YAPB' are only called
# from C++ by YAPB.
####

_YAPB_Globals := [];

_YAPB_addRef := function(obj)
  Add(_YAPB_Globals, obj);
end;

_YAPB_checkRef := function(obj)
  return ForAny(_YAPB_Globals, x -> IsIdenticalObj(x, obj));
end;

_YAPB_clearRefs := function()
  _YAPB_Globals := [];
end;

_YAPB_getPermTo := function(sc, i)
  local current_perm, current_pos, base_val;
  base_val := sc.orbit[1];
  if not(IsBound(sc.transversal[i])) then
    return fail;
  fi;
  current_perm := sc.transversal[i];
  current_pos := i^current_perm;
  while (current_pos <> base_val)
  do
    current_perm := current_perm * sc.transversal[current_pos];
    current_pos := i^current_perm;
  od;
  return current_perm;
end;

_YAPB_inGroup := function(p, g)
  return (p in g);
end;

_YAPB_isGroupConj := function(p, g)
  return g^p = g;
end;

_YAPB_getOrbitPart := function(g, i)
  return OrbitsDomain(Group(g.generators), [1..i]);
end;

_YAPB_getBlockList := function(sc)
  local blocks, g, orbs, b, o;
  g := Group(sc.generators);
  orbs := Orbits(g);
  blocks := [];
  for o in orbs
  do
    b := RepresentativesMinimalBlocks(g,o);
    if b[1] <> o then
      Append(blocks, b);
    fi;
  od;

  return List(blocks, x->Orbit(g,Set(x),OnSets));
end;

_YAPB_FixedOrbits := function(group, domainsize, fixedpoints)
    return OrbitsDomain(Stabilizer(group, fixedpoints, OnTuples), [1..domainsize]);
end;

_YAPB_RepresentElement := function(group, list1, list2)
    local res;
    res := RepresentativeAction(group, list1, list2, OnTuples);
    if(res = fail) then
        return fail;
    fi;
    return ListPerm(res);
end;

#####
# END OF FUNCTIONS CALLED FROM C++ CODE
#####

#############################################################################
##
##
##  <#GAPDoc Label="ConStabilize">
##  <ManSection>
##  <Heading>ConStabilize</Heading>
##  <Func Name="ConStabilize" Arg="object, action" Label="for an object and an action"/>
##  <Func Name="ConStabilize" Arg="object, n" Label="for a transformation or partial perm"/>
##  
##  <Description>
##  In the first form this represents the group which stabilises <A>object</A>
##  under <A>action</A>.
##  The currently allowed actions are OnSets, OnSetsSets, OnSetsDisjointSets,
##  OnTuples, OnPairs and OnDirectedGraph.
##
##  In the second form it represents the stabilizer of a partial perm
##  or transformation in the symmetric group on <A>n</A> points.
##
##  Both of these methods are for constructing arguments for the
##  <Ref Func="Solve" /> method.
##
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallMethod(ConStabilize, [IsList, IsFunction],
function(list, op)
  if op = OnSets then
    return rec(constraint := "SetStab",
               arg := list,
               max := MaximumList(list, 1));
  fi;

  if op = OnSetsDisjointSets then
    return rec(constraint := "SetSetStab",
               arg := list,
               max := MaximumList(List(list, x -> MaximumList(x, 1))));
  fi;

  if op = OnSetsSets then
    return rec(constraint := "OverlappingSetSetStab",
               arg := list,
               max := MaximumList(List(list, x -> MaximumList(x, 1))));
  fi;

  if op = OnTuples or op = OnPairs then
    return rec(constraint := "ListStab",
               arg := list,
               max := MaximumList(list, 1));
  fi;
  
  if op = OnTuplesTuples then
      return rec(constraint := "ListStab",
                 arg := Concatenation(list),
                 max := MaximumList(Concatenation(list), 1));
  fi;

  if op = OnDirectedGraph then
    return rec(constraint := "DirectedGraph",
               arg := list,
               max := Length(list));
  fi;

  Error("Do not understand:", op);
end);


InstallMethod(ConStabilize, [IsTransformation, IsPosInt],
function(t, i)
  return ConStabilize(List([1..i], x -> [x^t]), OnDirectedGraph);
end);

InstallMethod(ConStabilize, [IsPartialPerm, IsPosInt],
function(pp, m)
  local l, i;
  l := List([1..m], x -> []);
  for i in [1..m] do
    if i^pp <> 0 then
      Add(l[i], i^pp);
    fi;
  od;
  return ConStabilize(l, OnDirectedGraph);
end);

#############################################################################
##
##
##  <#GAPDoc Label="ConInGroup">
##  <ManSection>
##  <Func Name="ConInGroup" Arg="G"/>
##
##  <Description>
##  Represents the permutation group <A>G</A>, as an argument for
##  <Ref Func="Solve" />.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallMethod(ConInGroup, [IsPermGroup],
function(G)
  return ConInGroup(G, "OrbStabChain");
end);

# Inefficient, StabChain, BlockStabChain, OrbStabChain, BlockOrbStabChain, UnorderedStabChain

InstallMethod(ConInGroup, [IsPermGroup, IsString],
function(G, s)
  return rec(constraint:=Concatenation("Generators_", s),
             arg := G,
             max := LargestMovedPoint(G));
end);


InstallGlobalFunction( OnDirectedGraph, function()
  Error("This is not a true function, just a place holder");
end );


LoadDynamicModule(Filename(DirectoriesPackagePrograms("ferret"), "hellod.so"));


_SOLVE_STATS := rec();

#############################################################################
##
##
##  <#GAPDoc Label="SolveStats">
##  <ManSection>
##  <Func Name="SolveStats" Arg=""/>
##
##  <Description>
##  Returns performance stats for the most recent call to
##  <Ref Func="Solve" />.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallGlobalFunction(SolveStats, function()
  return _SOLVE_STATS;
end);

#############################################################################
##
##
##  <#GAPDoc Label="Solve">
##  <ManSection>
##  <Func Name="Solve" Arg="constraints [,rec]"/>
##
##  <Description>
##  Finds the intersection of the list <A>constraints</A>.
##
##  Each memory of <A>constraints</A> should be a group or coset
##  generated by one of <Ref Func="ConInGroup" /> or 
##  <Ref Func="ConStabilize" />.
##
##  The optional second argument allows configuration options to be
##  passed in.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallGlobalFunction( Solve, function( arg )
  local maxpoint, perms, l, options, useroptions, constraints;
  l := Length(arg);
  if l = 0 or l > 2 then
    Error( "usage: Solve(<C>[, <options>])");
  fi;

  constraints := Flat(arg[1]);
  maxpoint := Maximum(List(constraints, x -> x.max));

  options := rec(searchValueHeuristic := "RBase",
                 searchFirstBranchValueHeuristic := "RBase",
                 rbaseCellHeuristic := "smallest",
                 rbaseValueHeuristic := "smallest",
                 size := maxpoint,
                 stats := true
                 );

  if l = 2 then
    useroptions := arg[2];
  else
    useroptions := rec();
  fi;

  if IsBound(useroptions.rbaseValueHeuristic) then
    options.rbaseValueHeuristic := useroptions.rbaseValueHeuristic;
    Unbind(useroptions.rbaseValueHeuristic);
  fi;

  if IsBound(useroptions.rbaseCellHeuristic) then
    options.rbaseCellHeuristic := useroptions.rbaseCellHeuristic;
    Unbind(useroptions.rbaseCellHeuristic);
  fi;

  if IsBound(useroptions.searchValueHeuristic) then
    options.searchValueHeuristic := useroptions.searchValueHeuristic;
    Unbind(useroptions.searchValueHeuristic);
  fi;

  if IsBound(useroptions.searchFirstBranchValueHeuristic) then
    options.searchFirstBranchValueHeuristic := useroptions.searchFirstBranchValueHeuristic;
    Unbind(useroptions.searchFirstBranchValueHeuristic);
  fi;

  if IsBound(useroptions.allperms) then
    options.allperms := useroptions.allperms;
    Unbind(useroptions.allperms);
  fi;

  if IsBound(useroptions.stats) then
    options.stats := useroptions.stats;
    Unbind(useroptions.stats);
  fi;

  if IsBound(useroptions.justrbase) then
    options.justrbase := useroptions.justrbase;
    Unbind(useroptions.justrbase);
  fi;

  if useroptions <> rec() then
    Error("Unknown options: ", useroptions);
  fi;

  perms := YAPB_SOLVE(constraints, options);
  _YAPB_clearRefs();

  _SOLVE_STATS := perms.stats;
  return perms.generators;
end );


#E  files.gi  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
