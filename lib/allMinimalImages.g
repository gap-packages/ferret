LoadPackage("ferret");


gather := [];

acceptTransform := function(m, sollength)
    local f;
    f := rec(
      filter := function(l)
        local len;
#        Print("Filter:", l);
        len := Length(l);
        if Int( (l[len] - 1) / m) <> len - 1 then
#            Print("f\n");
            return false;
        fi;
#        Print("t\n");
        
        return true;
      end,
        
      record := function(l)
#          Print("record:",l);
          
          if Length(l) = sollength then
              Add(gather, Transformation(_unbooleaniseList(l, m)));
#              Print("f\n");
              
              return false;
          fi;
#          Print("t\n");
          
          return true;
      end);
    return f;
end;

acceptPartialPerm := function(m, sollength)
    local f;
    f := rec(
      filter := function(l)
        local len;
        len := Length(l);
        if Int( (l[len] - 1) / m) <> len - 1 then
            return false;
        fi;
        
        l := _unbooleaniseList(l, m);
        if not(IsDuplicateFreeList(Filtered(l, x -> x<>m))) then
            return false;
        fi;
        return true;
    end,
      record := function(l)
        l := _unbooleaniseList(l, m);
        # the 'mod m' is just to turn m into 0.
        if Length(l) = sollength then
            Add(gather, PartialPerm(List(l, x -> x mod m)));
            return false;
        fi;
        return true;
        
    end);

    return f;
end;

recurseMinimalImage := function(baseG, G, l, max, accept)
    local orbmins, i, min;

    if Length(l) = 0 then
        min := 1;
    else 
        if not(CanonicalImage(baseG, l, rec(action := OnSets, result := GetBool, stabilizer := Group(())))) then
            return;
        fi;
        
        if not(accept.record(l)) then
            return;
        fi;
        
        if l[Length(l)] = max then
            return;
        fi;

        min := l[Length(l)] + 1;
    fi;

    orbmins := List(Orbits(G, [min..max]), x -> Minimum(x));
    Sort(orbmins);
    for i in orbmins do
        if i >= min then
            Add(l, i);
            if accept.filter(l) then
                recurseMinimalImage(baseG, Stabilizer(G,i), l, max, accept);
            fi;
            
            Remove(l);
        fi;
    od;
end;

AllMinimalSetsFiltered := function(G, max, accept)
    gather := [];
    recurseMinimalImage(G, G, [], max, accept);
    return gather;
end;

AllMinimalListsFiltered := function(G, n, filter)
    return AllMinimalSetsFiltered(_rowColGen(G,n), n*n, filter);
end;

AllMinimalTransformations := function(G, n)
    return AllMinimalSetsFiltered(_rowColGen(G,n), n*n, acceptTransform(n,n));
end;

AllMinimalPartialPerms := function(G, n)
    return AllMinimalSetsFiltered(_rowColGen(G,n+1), (n+1)*(n+1), acceptPartialPerm(n+1, n));
end;


# AllMinimalTransformations: AllMinimalLists(G, n, acceptTransform)
# AllMinimalPartialPerms: AllMinimalLists(G, n, acceptPartialPerm)

AllMinimalOrderedPairs := function(G, n, generator)
    local t, l, stabs, stabs_set, inner_images, out, i, j;
    
    l := AllMinimalListsFiltered(G, n, generator);
    
    stabs := List(l, x -> Group(Solve([ConStabilize(x, n), ConInGroup(G)])));
    stabs_set := Set(stabs);
    inner_images := List(stabs_set, x -> AllMinimalListsFiltered(x, n, generator));
    out := [];
    for i in [1..Length(l)] do
        for j in inner_images[Position(stabs_set, stabs[i])] do
            Add(out, [l[i], j]);
        od;
    od;
    return out;
end;

AllMinimalUnorderedPairs := function(G, n, filter)
    local pairs, seconds, seconds_image, out, p, mimage;
    pairs := AllMinimalOrderedPairs(G, n, filter);
    seconds := Set(pairs, x -> x[2]);
    seconds_image := List(seconds, x -> CanonicalImage(G, x, rec(result := GetImage, action := OnPoints, stabilizer := Group(()))));

    # Now we try to find the minimum unordered pairs [a,b].
    # We use the following facts:
    # 1) MinimumImage(G, a) = a;
    # 2) In the minimum image of [a,b], the smallest of
    #    MinimumImage(G, a) and MinimumImage(G, b) certainly appears
    #
    # So, go as follows:
    # a) a < MinimumImage(G,b) : Is minimal, include.
    # b) a > MinimumImage(G,b) : Is not minimal,
    #                      covered where MinimumImage(G,b) appears first in pair.
    # So this leaves a = MinimumImage(G, b).
    # In this case, need to compare the two ways of ordering the pair
    # We could do this more cleverly, but it doesn't happen too often!
    # if so, then include.

    out := [];
    for p in pairs do
        mimage := seconds_image[PositionSorted(seconds, p[2])];
        if p[1] < mimage then
            Add(out, p);
        elif p[1] = mimage and p[1] < p[2] and 
          MinimalImage(G, [p[1],p[2]], OnTuples) <= 
          MinimalImage(G, [p[2],p[1]], OnTuples) then
            Add(out, p);
        fi;
    od;
    return out;
end;



AllOrderedPairsTransformations_Slow := function(G, n)
    local g;
    g := _rowColGen(G,n);
    return AllMinimalSetsFiltered(_cajGroupCopy(g, LargestMovedPoint(g), 2), n*(n+1)*2, acceptTransform(n+1, 2*n));
end;

AllUnorderedPairsTransformations_Slow := function(G, n)
    local g;
    g := _rowColGen(G, n);
    return AllMinimalSetsFiltered(_cajWreath(g, LargestMovedPoint(g), 2), n*(n+1)*2, acceptTransform(n+1,  2*n));
end;
