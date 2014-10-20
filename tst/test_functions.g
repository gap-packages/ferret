LoadPackage("ferret", false);
LoadPackage("atlas", false);

if not(IsBound(FERRET_TEST_COUNT)) then
    FERRET_TEST_COUNT := 50;
fi;

compare_full_groups := function(g, perms)
    if(Size(perms) <> Size(Group(perms))) then
        Print("Size(p) = ", Size(perms), ", Size(Group(p)) = ", Size(Group(perms)),"\n");
        Print("p:", perms,"\n");
        return fail;
    fi;
    if(Size(Set(perms)) <> Size(perms)) then
        Print("Repeated members of p!","\n");
        Print(perms,"\n");
        return fail;
    fi;
    if(Set(perms) <> Set(Group(perms))) then
        Print("p not whole group!","\n");
        Print(Set(perms),"\n");
        Print(Set(Group(perms)),"\n");
        return fail;
    fi;
    if(Group(perms) <> g) then
        Print("Wrong group!","\n");
        Print(g,"\n");
        Print(Group(perms),"\n");
        return fail;
    fi;
    return true;
end;;

heuristics := ["largest", "first", "smallest2", "randomsmallest", "random", "random", "random", "random"];

compare_gen_groups := function(g, perms)
    if(Group(perms) <> g) then
        Print("Wrong group!","\n");
        Print("Expected: ",g,"\n");
        Print("Found: ",Group(perms),"\n");
        return false;
    fi;
    if((Length(perms) > LargestMovedPoint(g)) and (Length(perms) > 1)) then
        Print("Too many generators! (LMP=)",LargestMovedPoint(g),"\n");
        Print(perms,"\n");
        return false;
    fi;
    return true;
end;;

CheckGroup := function(g)
    local permsfull, permsgen, permsfullStabChain, permsgenStabChain, permsfullBlock, permsgenBlock, h;
    for h in heuristics do
        permsfull := Solve([ConInGroup(g, "Inefficient")], rec( allperms := true, rbaseCellHeuristic := h));
        permsgen := Solve([ConInGroup(g, "Inefficient")], rec( rbaseCellHeuristic := h));
        permsfullStabChain := Solve([ConInGroup(g)], rec( allperms := true, rbaseCellHeuristic := h));
        permsgenStabChain := Solve([ConInGroup(g)], rec( rbaseCellHeuristic := h));

        permsfullBlock := Solve([ConInGroup(g, "BlockStabChain")], rec( allperms := true, rbaseCellHeuristic := h));
        permsgenBlock := Solve([ConInGroup(g, "BlockStabChain")], rec( rbaseCellHeuristic := h));

        Sort(permsfull);
        Sort(permsfullStabChain);
        Sort(permsfullBlock);

        if not(
               compare_full_groups(g, permsfullStabChain) and compare_gen_groups(g, permsgenStabChain) and
               compare_full_groups(g, permsfull) and compare_gen_groups(g, permsgen) and
               compare_full_groups(g, permsfullBlock) and compare_gen_groups(g, permsgenBlock)
            ) then
            Print("Check Group", g, " failed with heuristic ", h,"\n");
            return false;
        fi;
    od;
    return true;
end;;

FastCheckGroup := function(g)
    local permsgen, permsgenStabChain, permsgenBlock, h, comp1, comp2, comp3, res;
    res := true;
    for h in heuristics do
        permsgen := Solve([ConInGroup(g, "Inefficient")], rec( rbaseCellHeuristic := h));
        permsgenStabChain := Solve([ConInGroup(g)], rec( rbaseCellHeuristic := h));
        permsgenBlock := Solve([ConInGroup(g, "BlockStabChain")], rec( rbaseCellHeuristic := h));

        comp1 := compare_gen_groups(g, permsgenStabChain);
        comp2 := compare_gen_groups(g, permsgen);
        comp3 := compare_gen_groups(g, permsgenBlock);

        if not(comp1 and comp2 and comp3) then
            Print("Fast check group" , g , " failed with heuristic " , h, ":",
                  comp1,comp2,comp3,"\n");
        fi;

        res := res and comp1 and comp2 and comp3;
    od;

    return res;
end;;

FastCheckSetStab := function(g, s)
    local permsStabChain, permsBlock, stab, comp1, comp2;
    permsStabChain := Solve([ConInGroup(g), ConStabilize(s, OnSets)]);
    permsBlock := Solve([ConInGroup(g, "BlockStabChain"), ConStabilize(s, OnSets)]);
    stab := Stabilizer(g, s, OnSets);

    comp1 := compare_gen_groups(stab, permsStabChain);
    comp2 := compare_gen_groups(stab, permsBlock);

    if not(comp1 and comp2) then
        Print("Fast check set stab " , g , ",",s," failed:",
              comp1,comp2,"\n");
        Print(permsStabChain, "\n",permsBlock,"\n");
    fi;
    return (comp1 and comp2);
end;;

FastCheckSetStabNoBlock := function(g, s)
    local permsStabChain, permsBlock, stab, comp;
    permsStabChain := Solve([ConInGroup(g), ConStabilize(s, OnSets)]);
    stab := Stabilizer(g, s, OnSets);
    comp := (Group(permsStabChain) = stab);

    if not(comp) then
        Print("Fast check set stab (no block) " , g , ",",s," failed:",
              comp,"\n");
    fi;
    return comp;
end;;

FastCheckSetSetStab := function(g, s)
    local permsStabChain, permsBlock, stab, comp1, comp2;
    permsStabChain := Solve([ConInGroup(g), ConStabilize(s, OnSetsSets)]);
    permsBlock := Solve([ConInGroup(g, "BlockStabChain"), ConStabilize(s, OnSetsSets)]);
    stab := Stabilizer(g, s, OnSetsSets);

    comp1 := compare_gen_groups(stab, permsStabChain);
    comp2 := compare_gen_groups(stab, permsBlock);

    if not(comp1 and comp2) then
        Print("Fast check set set stab" , g , ",",s," failed:",
              comp1,comp2,"\n");
    fi;
    return (comp1 and comp2);

end;;

CheckSetStab := function(g, s)
    local perms, permsgen, permsStabChain, permsgenStabChain, max, h;
    max := LargestMovedPoint(g);
    if(Length(s) > 0) then
        max := Maximum(max, Maximum(s));
    fi;
    for h in heuristics do
        perms := Solve([ConInGroup(g, "Inefficient"), ConStabilize(s, OnSets)], rec( allperms := true, rbaseCellHeuristic := h));
        permsgen := Solve([ConInGroup(g), ConStabilize(s, OnSets)], rec( rbaseCellHeuristic := h));
        permsStabChain := Solve([ConInGroup(g, "Inefficient"), ConStabilize(s, OnSets)], rec( allperms := true, rbaseCellHeuristic := h));
        permsgenStabChain := Solve([ConInGroup(g), ConStabilize(s, OnSets)], rec( rbaseCellHeuristic := h));

        if not(compare_full_groups(Stabilizer(g, s, OnSets), permsStabChain) and
               compare_gen_groups(Stabilizer(g, s, OnSets), permsgenStabChain) and
               compare_full_groups(Stabilizer(g, s, OnSets), perms) and
               compare_gen_groups(Stabilizer(g, s, OnSets), permsgen)) then
          Print("CheckSetStab", g, " failed with heuristic ", h,"\n");
          return false;
        fi;
    od;
    return true;
end;;

makeRowColumnSymmetry := function(x,y)
    local perms,i,j,l;
    perms := [];

    for i in [1..(x-1)] do
        l := [1..x*y];
        for j in [1..y] do
            l[i    +(j-1)*x] := (i+1) + (j-1)*x;
            l[(i+1)+(j-1)*x] := i     + (j-1)*x;
        od;
        Append(perms, [PermList(l)]);
    od;

    for j in [1..(y-1)] do
        l := [1..x*y];
        for i in [1..x] do
            l[i+ j*x]    := i + (j-1)*x;
            l[i+(j-1)*x] := i + j*x;
        od;
        Append(perms, [PermList(l)]);
    od;
    return Group(perms);
end;;

randomPrimitiveGroup := function(size)
    return PrimitiveGroup(size, Random([1..NrPrimitiveGroups(size)]));
end;;


allTinyPrimitiveGroups := function(size)
    return Union(List([1..size], x -> List([1..NrPrimitiveGroups(x)], y -> PrimitiveGroup(x,y))));
end;;

CheckAllPrimitives := function(size)
    local g, i1, i2, p, s, t;
    Init(GlobalMersenneTwister, 1);
    for g in allTinyPrimitiveGroups(size) do
        for i1 in [2..50] do
            p := Random(SymmetricGroup(LargestMovedPoint(g)));
            s := [1];
            t := [];
            while not(Length(s) = Length(t) and Intersection(s,t) = [] and Length(s) > 0) do
                s := Filtered([1..LargestMovedPoint(g)+1], x -> Random([0,1]) = 0);
                t := Filtered([1..LargestMovedPoint(g)+1], x -> Random([0,1]) = 0);
            od;

            for i2 in [1..3] do
                if not(FastCheckSetStab(g^p, s) and
                       FastCheckSetSetStab(g^p, Set([Set(s),Set(t)]))) then
                    Print("Failure in: ", g," with ", p, ":", GeneratorsOfGroup(g), "\n");
                fi;
            od;
        od;
    od;
end;


CheckRandomPrimitives := function()
    return ForAll([10..20], x -> FastCheckSetStab(randomPrimitiveGroup(x), [2,4,6,8,9]))
     and
           ForAll([7..10], x -> FastCheckSetSetStab(randomPrimitiveGroup(x), [[1,6],[2,4],[3,5]]));

end;;

