LoadPackage("ferret", false);
LoadPackage("atlas", false);

ReadPackage("ferret", "tst/random_obj.g");

if not(IsBound(FERRET_TEST_COUNT)) then
    FERRET_TEST_COUNT := 50;
fi;

compare_full_groups := function(g, record)
    local perms;
    perms := record.generators;
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


compare_gen_groups := function(g, record)
    local perms;
    perms := record.generators;
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

heuristics := ["largest", "first", "smallest2", "randomsmallest", "random", "random", "random", "random"];

# Checks all the ways of building up a single group -- can be extremely slow!
CheckGroup := function(g)
    local permsfull, permsgen, permsfullStabChain, permsgenStabChain, permsfullBlock, permsgenBlock, h;
    for h in heuristics do
        permsfull := Solve([ConInGroup(g, "Inefficient")],
                           rec( only_find_generators := false, rbaseCellHeuristic := h, recreturn := true));
        permsgen := Solve([ConInGroup(g, "Inefficient")], rec( rbaseCellHeuristic := h, recreturn := true));
        permsfullStabChain := Solve([ConInGroup(g)],
                              rec( only_find_generators := false, rbaseCellHeuristic := h, recreturn := true));
        permsgenStabChain := Solve([ConInGroup(g)], rec( rbaseCellHeuristic := h, recreturn := true));

        permsfullBlock := Solve([ConInGroup(g, "BlockStabChain")],
                                rec( only_find_generators := false, rbaseCellHeuristic := h, recreturn := true));
        permsgenBlock := Solve([ConInGroup(g, "BlockStabChain")], rec( rbaseCellHeuristic := h, recreturn := true));

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

CheckStab := function(g, s, act)
    local permsStabChain, permsBlock, stab, comp1, comp2;
    permsStabChain := Solve([ConInGroup(g), ConStabilize(s, act)], rec(recreturn := true));
    permsBlock := Solve([ConInGroup(g, "BlockStabChain"), ConStabilize(s, act)], rec(recreturn := true));
    stab := Stabilizer(g, s, act);

    comp1 := compare_gen_groups(stab, permsStabChain);
    comp2 := compare_gen_groups(stab, permsBlock);

    if not(comp1 and comp2) then
        Print("Fast check stab " , g , ",",s," with action ",act," failed:",
              comp1,comp2,"\n");
        Print(permsStabChain, "\n",permsBlock,"\n");
    fi;
    return (comp1 and comp2);
end;;

# Each of these functions should take a single integer, and return a pair, containing a constraint
# and a the group that constraint generates
ConstraintBuilders :=
[ 
    function(x)
        local g;
        g := RandomGroupUpToSize(x);
        return [ConInGroup(g), g];
    end,
    function(x)
        local s;
        s := RandomObj(GlobalMersenneTwister, x, OnSets);
        return [ConStabilize(s), Stabilizer(SymmetricGroup(x), s)];
    end
];

CheckSmallRandomPrimitives := function()
    return ForAll([5..10], x -> CheckStab(RandomGroupOfSize(x), [1,2,4],OnSets))
     and
           ForAll([3..6], x -> CheckStab(RandomGroupOfSize(x), [[1,2],[3,4]],OnSetsSets));

end;;

CheckRandomPrimitives := function()
    return ForAll([10..20], x -> CheckStab(RandomGroupOfSize(x), [2,4,6,8,9],OnSets))
     and
           ForAll([7..10], x -> CheckStab(RandomGroupOfSize(x), [[1,6],[2,4],[3,5]],OnSetsSets));

end;;

