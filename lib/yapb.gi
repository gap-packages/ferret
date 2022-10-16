#############################################################################
##
##
#W  yapb.gi                Ferret Package                Chris Jefferson
##
##  Installation file for functions of the Ferret package.
##
#Y  Copyright (C) 2013-2014 University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##

####
# Functions and variables beginning '_YAPB' are only called
# from C++ by YAPB.
####

_YAPB_Globals := [];

# Have a copy of this in each thread in HPCGAP
# This store references to variables we are using inside ferret,
# to make sure they don't get garbage collected.
if IsBound( MakeThreadLocal )  then
  MakeThreadLocal("_YAPB_Globals");
fi;

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

_YAPB_getOrbitPart := function(g, maxval)
  return OrbitsDomain(Group(g.generators), [1..maxval]);
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

_YAPB_getInfoFerret := function()
  return InfoLevel(InfoFerret);
end;

_YAPB_getInfoFerretDebug := function()
  return InfoLevel(InfoFerretDebug);
end;


_YAPB_fillRepElements := function(G, orb)
  local val, g, reps, buildorb, gens;
  reps := [];
  reps[orb[1]] := ();
  buildorb := [orb[1]];
  gens := GeneratorsOfGroup(G);
  for val in buildorb do
  	for g in gens do
	  if not IsBound(reps[val^g]) then
	  	reps[val^g] := reps[val] * g;
		Add(buildorb, val^g);
	  fi;
	od;
  od;
  return reps;
end;

_YAPB_stabTime := 0;

_YAPB_getOrbitalList := function(sc, maxval)
	local G, cutoff,
        orb, orbitsG, iorb, graph, graphlist, val, p, i, orbsizes, orbpos, innerorblist, orbitsizes,
		    biggestOrbit, skippedOneLargeOrbit, orbreps, stabtime;
	
  if IsGroup(sc) then
    G := sc;
  else
    G := GroupStabChain(sc);
  fi;
  
  cutoff := infinity;

	graphlist := [];
	orbitsG := Orbits(G,[1..maxval]);
	
	orbsizes := [];
	orbpos := [];
	# Efficiently store size of orbits of values
	for orb in [1..Length(orbitsG)] do
		for i in orbitsG[orb] do
			orbsizes[i] := Size(orbitsG[orb]);
			orbpos[i] := orb;
		od;
	od;

    stabtime := NanosecondsSinceEpoch();
	innerorblist := List(orbitsG, o -> Orbits(Stabilizer(G, o[1]), [1..LargestMovedPoint(G)]));
    _YAPB_stabTime := _YAPB_stabTime + (NanosecondsSinceEpoch() - stabtime);

  orbitsizes := List([1..Length(orbitsG)], 
	  x -> List(innerorblist[x], y -> Size(orbitsG[x])*Size(y)));
	
	biggestOrbit := Maximum(Flat(orbitsizes));

	skippedOneLargeOrbit := false;

	for i in [1..Size(orbitsG)] do
		orb := orbitsG[i];
    orbreps := [];
		for iorb in innerorblist[i] do
			if (Size(orb) * Size(iorb) = biggestOrbit and not skippedOneLargeOrbit) then
				skippedOneLargeOrbit := true;
			else
				if (Size(orb) * Size(iorb) <= cutoff) and
				# orbit size unchanged
				not(Size(iorb) = orbsizes[iorb[1]]) and
				# orbit size only removed one point
				not(orbpos[orb[1]] = orbpos[iorb[1]] and Size(iorb) + 1 = orbsizes[iorb[1]]) and
        # don't want to take the fixed point orbit
        not(orb[1] = iorb[1] and Size(iorb) = 1)
					then
					graph := List([1..LargestMovedPoint(G)], x -> []);
          if IsEmpty(orbreps) then
            orbreps := _YAPB_fillRepElements(G, orb);
          fi;
					for val in orb do
						p := orbreps[val]; 
						graph[val] := List(iorb, x -> x^p);
					od;
					Add(graphlist, graph);
				fi;
			fi;
		od;
	od;
	return graphlist;
end;

#####
# END OF FUNCTIONS CALLED FROM C++ CODE
#####

#############################################################################
##
##
##  <#GAPDoc Label="ConStabilize">
##  <ManSection>
##  <Func Name="ConStabilize" Arg="object, action" Label="for an object and an action"/>
##  <Func Name="ConStabilize" Arg="object, n" Label="for a transformation or partial perm"/>
##
##  <Description>
##  This function creates a Constraint which can be given to <Ref Func="Solve" />.
##  It does not perform any useful actions by itself
##  <P/>
##  In the first form this represents the group which stabilises <A>object</A>
##  under <A>action</A>.
##  The currently allowed actions are <C>OnSets</C>, <C>OnSetsSets</C>, <C>OnSetsDisjointSets</C>,
##  <C>OnSetsTuples</C>, <C>OnTuples</C>, <C>OnPairs</C> and <C>OnDirectedGraph</C>.
##  <P/>
##  In the second form it represents the stabilizer of a partial perm
##  or transformation in the symmetric group on <A>n</A> points.
##
##
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallMethod(ConStabilize, [IsList, IsFunction],
function(list, op)
  if op = OnSets then
    return rec(constraint := "SetStab",
               arg := list,
               max := MaximumList(list, 0));
  fi;

  if op = OnSetsDisjointSets then
    return rec(constraint := "SetSetStab",
               arg := list,
               max := MaximumList(List(list, x -> MaximumList(x, 0)),0));
  fi;

  if op = OnSetsSets then
    return rec(constraint := "OverlappingSetSetStab",
               arg := list,
               max := MaximumList(List(list, x -> MaximumList(x, 0)),0));
  fi;

  if op = OnSetsTuples then
    return rec(constraint := "SetTupleStab",
               arg := list,
               max := MaximumList(List(list, x -> MaximumList(x, 0)),0));
  fi;

  if op = OnTuples or op = OnPairs then
    return rec(constraint := "ListStab",
               arg := list,
               max := MaximumList(list, 0));
  fi;

  if op = OnTuplesTuples then
      return rec(constraint := "ListStab",
                 arg := Concatenation(list),
                 max := MaximumList(Concatenation(list), 0));
  fi;

  if op = OnTuplesSets then
     return List(list, x -> ConStabilize(x, OnSets));
  fi;

  if op = OnDirectedGraph then
    return rec(constraint := "DirectedGraph",
               arg := list,
               max := Length(list));
  fi;
  
  if op = OnEdgeColouredDirectedGraph then
      return rec(constraint := "EdgeColouredDirectedGraph",
                 arg := list,
                 max := Length(list));
  fi;

  Error("Do not understand:", op);
end);

InstallMethod(ConStabilize, [IsList, IsFunction, IsRecord],
  function(list, op, useroptions)
    local con, options;
    
    if Size(RecNames(useroptions)) = 0 then
      return ConStabilize(list, op);
    fi;
  
    if op = OnEdgeColouredDirectedGraph then
      con := rec(constraint := "EdgeColouredDirectedGraph",
                 arg := list,
                 max := Length(list));
    elif op = OnDirectedGraph then
      con := rec(constraint := "DirectedGraph",
               arg := list,
               max := Length(list));
    else
      ErrorNoReturn("No record argument allowed for " + op);
    fi;
    
     useroptions := _FerretHelperFuncs.fillUserValues(
          rec(start_path_length := 1,
              normal_path_length := 1), useroptions);
     
     con.start_path_length := useroptions.start_path_length;
     con.normal_path_length := useroptions.normal_path_length;
     
     return con;
  end);
  
  

InstallMethod(ConStabilize, [IsPosInt, IsFunction],
function(i, op)
  if op = OnPoints then
    return rec(constraint := "SetStab",
               arg := [i],
               max := i);
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
##  This function creates a Constraint which can be given to <Ref Func="Solve" />.
##  It does not perform any useful actions by itself
##  <P/>
##  Represents the set of permutations in a permutation group <A>G</A>, as an
##  argument for
##  <Ref Func="Solve" />.
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallMethod(ConInGroup, [IsPermGroup],
function(G)
  return ConInGroup(G, rec() );
end);

# Inefficient, StabChain, BlockStabChain, OrbStabChain, BlockOrbStabChain, UnorderedStabChain

InstallMethod(ConInGroup, [IsPermGroup, IsRecord],
function(G, useroptions)

  useroptions := _FerretHelperFuncs.fillUserValues(
          rec(orbits := "always", blocks := "never", orbitals := "never"), useroptions);

  # We special case the identity group, because it is a pain
  if IsTrivial(G) then
    return rec(constraint:="FixAllPoints", max := 1);
  else
    return rec(constraint:= "Generators_StabChain",
               arg := G,
               orbits := useroptions.orbits,
               blocks := useroptions.blocks,
               orbitals := useroptions.orbitals,
               max := LargestMovedPoint(G));
  fi;
end);


InstallGlobalFunction( OnDirectedGraph, function(graph, perm)
  local newgraph, list, i, j;
  newgraph := [];
  for i in [1..Length(graph)] do
    list := [];
    for j in [1..Length(graph[i])] do
      Add(list, (graph[i][j])^perm);
    od;
    if i^perm > Length(graph) then
      ErrorNoReturn("perm invalid on graph");
    fi;
    newgraph[i^perm] := Set(list);
  od;
  return newgraph;
end );

InstallGlobalFunction( OnEdgeColouredDirectedGraph, function(graph, perm)
  local newgraph, list, i, j;
  newgraph := [];
  for i in [1..Length(graph)] do
    list := [];
    for j in [1..Length(graph[i])] do
      Add(list, [(graph[i][j][1])^perm, graph[i][j][2]]);
    od;
    if i^perm > Length(graph) then
      ErrorNoReturn("perm invalid on graph");
    fi;
    newgraph[i^perm] := Set(list);
  od;
  return newgraph;
end );

_FERRET_DEFAULT_OPTIONS := MakeImmutable(rec(searchValueHeuristic := "RBase",
                               searchFirstBranchValueHeuristic := "RBase",
                               rbaseCellHeuristic := "smallest",
                               rbaseValueHeuristic := "smallest",
                               stats := false,
                               nodeLimit := false,
                               recreturn := false,
                               only_find_generators := true,
                               just_rbase := false
                               ));

#############################################################################
##
##
##  <#GAPDoc Label="Solve">
##  <ManSection>
##  <Func Name="Solve" Arg="constraints [,rec]"/>
##
##  <Description>
##  Finds the intersection of the list <A>Constraints</A>.
##
##  Each member of <A>constraints</A> should be a group or coset
##  generated by one of <Ref Func="ConInGroup" /> or
##  <Ref Func="ConStabilize" Label="for an object and an action"/>.
##
##  The optional second argument allows configuration options to be
##  passed in. These follow options are supported:
##
##  <List>
##  <Mark><C>rbaseCellHeuristic</C> (default "smallest")</Mark>
##  <Item>
##      The cell to be branched on. This is the option which will most
##      effect the time taken to search. the default is usually best.
##
##      Other options are: "First" (first cell), "Largest" (largest cell),
##      "smallest2" (the 2nd smallest cell), "random" (a random cell)
##      and "randomsmallest" (one of the smallest cells, chosen randomly)
##  </Item>
##  <Mark><C>rbaseValueHeuristic</C> (default "smallest")</Mark>
##  <Item>
##      Choose which cell to branch on within a cell. While this will
##      generally make a big difference to search, it is hard to predict
##      the best value, and small changes to the problem will change the
##      best heuristic. Options are the same as <C>rbaseCellHeuristic</C>.
##  </Item>
##  <Mark><C>searchValueHeuristic</C> (default <K>RBase</K>)</Mark>
##  <Item>
##      The order to branch during search. In general the best order
##      is very hard to predict. Options are "RBase", "InvRBase",
##      "Random", "Sorted" or "Nosort" (which uses the order the values
##      naturally end up in by the algorithm).
##  </Item>
##  <Mark><C>searchFirstBranchValueHeuristic</C> (default <K>RBase</K>)</Mark>
##  <Item>
##      Choose the search order used just on the left-most branches of
##      search. Allows the same options as <C>searchValueHeuristic</C>
##  </Item>
##  <Mark><C>stats</C> (default <C>false</C>)</Mark>
##  <Item>
##      Change the return value to provide a range of information about how
##      search performed (implies <C>recreturn</C>). This information will
##      change between releases.
##  </Item>
##  <Mark><C>nodeLimit</C> (default <C>false</C>) </Mark>
##  <Item>
##      Either <B>false</B>, or an integer which places a limit on the amount
##      of search which should be performed. WARNING: When this option is set
##      to an integer, Ferret will return the current best answer when the limit
##      is reached, which may be a subgroup of the actual result. To know if this
##      limit was reached, set <C>stats</C> to <B>true</B>, and check the nodes.
##  </Item>
##  <Mark><C>recreturn</C> (default <C>false</C>) </Mark>
##  <Item>
##      Return a record containing private information, rather than the group.
##  </Item>
##  <Mark><C>only_find_generators</C> (default <C>true</C>)</Mark>
##  <Item>
##      By default only find the generators of the group. If false, then
##      find all members of the group. This option is only useful for testing.
##      If 'true', then sets 'recreturn' to true.
##  </Item>
##  </List>
##
##
##  </Description>
##  </ManSection>
##  <#/GAPDoc>
InstallGlobalFunction( Solve, function( arg )
  local maxpoint, record, l, options, useroptions,
        constraints, name, buildStabChain, retgrp;
  l := Length(arg);
  if l = 0 or l > 2 then
    Error( "usage: Solve(<C>[, <options>])");
  fi;

  constraints := Filtered(Flat(arg[1]), x -> x.max > 0);

  if Length(constraints) = 0 then
    Error("Cannot create infinite group in Solve");
  fi;

  maxpoint := Maximum(List(constraints, x -> x.max));

  # YAPB++ requires at least 2 points. We solve this in this way
  # because it makes sure we return all the various things
  # users might want (an rbase, etc).
  if maxpoint < 2 then
    Add(constraints, rec(constraint:="FixAllPoints", max := 2));
    maxpoint := 2;
  fi;

  if l = 2 then
    useroptions := arg[2];
  else
    useroptions := rec();
  fi;

  options := _FerretHelperFuncs.fillUserValues(_FERRET_DEFAULT_OPTIONS,
                                               useroptions);

  options.size := maxpoint;

  if options.stats or options.just_rbase then
    options.recreturn := true;
  fi;

_YAPB_stabTime := 0;
  record := YAPB_SOLVE(constraints, options);
  if IsBound(record.timing) then
    record.timing[1].stabInOrbFinding := _YAPB_stabTime;
fi;
  _YAPB_clearRefs();

  # We now stitch together a stabilizer chain from the return values of YAPB++
  buildStabChain := function(gens, gen_map)
    local sc, current_val, start_of_range, i;
    current_val := gen_map[1][1];
    start_of_range := 1;
    sc := EmptyStabChain(gens, ());
    for i in [2..Length(gen_map)] do
      if gen_map[i][1] <> current_val then
        InsertTrivialStabilizer(sc, current_val);
        AddGeneratorsExtendSchreierTree(sc, gens{[start_of_range..i-1]});
        current_val := gen_map[i][1];
        start_of_range := i;
      fi;
    od;
    InsertTrivialStabilizer(sc, current_val);
    AddGeneratorsExtendSchreierTree(sc, gens{[start_of_range..Length(gens)]});
    return sc;
  end;

  if options.recreturn then
    return record;
  else
    if record.generators = [()] then # just to avoid having to make code work in this case
      retgrp := Group(());
      StabChainMutable(retgrp);
    else
      retgrp := Group(record.generators);
      SetStabChainMutable(retgrp, buildStabChain(record.generators, record.generators_map));
    fi;

    Size(retgrp);
    return retgrp;
  fi;

end );
#E  files.gi  . . . . . . . . . . . . . . . . . . . . . . . . . . . ends here
