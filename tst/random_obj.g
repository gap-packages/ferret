# Function to create a random of random objects, which we will use for testing


# Usage: randomgen, action, maxint
# Makes things that the action can be applied to in the range [1..maxint]



FT := rec(
  MergeUncolouredGraphs := function(graphlist)
	local graph, maxval, g, v;
	maxval := Maximum(List(graphlist, Length));
	graph := List([1..maxval], x -> []);
	for g in [1..Length(graphlist)] do
		for v in [1..Length(graphlist[g])] do
			Append(graph[v], List(graphlist[g][v], x -> [x,g]));
		od;
	od;
	return List(graph, Set);
end
);
	
  
RandomTuple := function(rgen, maxint)
  return List([1..Random(rgen, [0..maxint])], x -> Random(rgen,[1..maxint]));
end;


# Make sure inner lists do not share any values
RandomDisjointTupleTuple := function(rgen, maxint)
  local l, x, i, j, innersize, val, remaining;
  if Random(rgen,[1..3]) = 1 then
    # Make inner lists all the same size
    innersize := Random(rgen, [0..maxint]);
    if innersize = 0 then
      return List([1..Random(rgen,[0..maxint])], x->[]);
    fi;
    l := [];
    remaining := [1..maxint];
    for i in [1..Random(rgen, [0..Int(maxint/innersize)])] do
      x := [];
      for i in [1..innersize] do
        val := Random(rgen, remaining);
        remaining := Difference(remaining, [val]);
        Add(x, val);
      od;
      Add(l, x);
    od;
    return l;
  fi;

  l := [];
  remaining := [1..maxint];
  for i in [1..Random(rgen, [0..maxint])] do
    x := [];
    for i in [1..Random(0, Length(remaining))] do
      val := Random(rgen, remaining);
      remaining := Difference(remaining, [val]);
      Add(x, val);
    od;
    Add(l, x);
  od;
  return l;
end;

# In some algorithms it is interesting when the inner lists are the same size
# so we make sure we create these
RandomTupleTuple := function(rgen, maxint)
  if Random(rgen,[1..4]) = 1 then
    return RandomDisjointTupleTuple(rgen, maxint);
  else
    return List([1..Random([0..maxint])], x -> RandomTuple(rgen, maxint));
  fi;
end;

RandomObj := function(rgen, maxint, action)
  local edges, grp, graphlist;
  
  if action = OnPoints then
    return Random(rgen, [1..maxint]);
  fi;

  if action = OnPairs then
    return List([1,2],x->Random(rgen, [1..maxint]));
  fi;

  if action = OnTuples then
    return RandomTuple(rgen, maxint);
  fi;

  if action = OnSets then
    return Set(RandomTuple(rgen, maxint));
  fi;

  if action = OnTuplesTuples then
    return RandomTupleTuple(rgen, maxint);
  fi;

  if action = OnTuplesSets then
    return List(RandomTupleTuple(rgen, maxint), Set);
  fi;

  if action = OnSetsTuples then
    return Set(RandomTupleTuple(rgen, maxint));
  fi;

  if action = OnSetsSets then
    return Set(RandomTupleTuple(rgen, maxint), Set);
  fi;

  if action = OnSetsDisjointSets then
    return Set(RandomDisjointTupleTuple(rgen, maxint), Set);
  fi;

  if action = OnDirectedGraph then
    grp := PrimitiveGroup(maxint, Random(rgen, [1..NrPrimitiveGroups(maxint)]));
    edges := Set(Orbit(grp, [1,2], OnTuples));
    return List([1..maxint], x -> Filtered([1..maxint], y -> [x,y] in edges));
  fi;
  
  if action = OnEdgeColouredDirectedGraph then
    graphlist := List([1..Random(rgen, [1..5])], x -> RandomObj(rgen, maxint, OnDirectedGraph));
    return FT.MergeUncolouredGraphs(graphlist);
  fi;
   
  Error("Invalid Action");

end;

RandomGroupOfSize := function(rgen, size)
    return PrimitiveGroup(size, Random(rgen, [1..NrPrimitiveGroups(size)]));
end;;

RandomGroupUpToSize := function(rgen, size)
    local s;
    s := Random([1..size]);
    if s = 1 then
      return Group(());
    else
      return RandomGroupOfSize(rgen, s);
    fi;
end;;




##################
## Non-random, but still useful

makeRowColumnSymmetry := function(x,y)
    local perms,i,j,j2,l;
    perms := [];

    l := [1..x*y];
    for j in [1..y] do
        l[1 + (j-1)*x] := 2 + (j-1)*x;
        l[2 + (j-1)*x] := 1 + (j-1)*x;
    od;
    Append(perms, [PermList(l)]);

    l := [1..x*y];
    for i in [1..x] do
        for j in [1..y] do
            l[(i mod x)+1+(j-1)*x] := i + (j-1)*x;
        od;
    od;
    Append(perms, [PermList(l)]);

    l := [1..x*y];
    for i in [1..x] do
        l[i+x] := i;
        l[i  ] := i + x;
    od;
    Append(perms, [PermList(l)]);


    l := [];
    for j in [0..y-1] do
        for i in [1..x] do
            if j = 0 then
              j2 := y - 1;
            else
              j2 := j - 1;
            fi;
            l[i+ j*x] := i + j2*x;
        od;
    od;
    Append(perms, [PermList(l)]);

    return Group(perms);
end;;

allTinyPrimitiveGroups := function(size)
    return Union(List([1..size], x -> List([1..NrPrimitiveGroups(x)], y -> PrimitiveGroup(x,y))));
end;;
