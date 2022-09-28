LoadPackage("ferret", false);
LoadPackage("atlasrep", false);

ReadPackage("ferret", "tst/random_obj.g");

if not(IsBound(FERRET_EXTRA_TESTS)) then
    FERRET_EXTRA_TESTS := false;
fi;

if not(IsBound(FERRET_TEST_COUNT)) then
    FERRET_TEST_COUNT := 50;
fi;

compare_full_groups := function(g, record)
    local perms;
    perms := record.generators;
    if(Size(perms) <> Size(Group(perms))) then
        Print("CFG: Size(p) = ", Size(perms), ", Size(Group(p)) = ", Size(Group(perms)),"\n");
        Print("CFG: p:", perms,"\n");
        return fail;
    fi;
    if(Size(Set(perms)) <> Size(perms)) then
        Print("CFG: Repeated members of p!","\n");
        Print(perms,"\n");
        return fail;
    fi;
    if(Set(perms) <> Set(Group(perms))) then
        Print("CFG: p not whole group!","\n");
        Print(Set(perms),"\n");
        Print(Set(Group(perms)),"\n");
        return fail;
    fi;
    if(Group(perms) <> g) then
        Print("CFG: Wrong group!","\n");
        Print(g,"\n");
        Print(Group(perms),"\n");
        return fail;
    fi;
    return true;
end;;


compare_gen_groups := function(g, record)
    local perms;
    perms := record.generators;
    if(Group(perms, ()) <> g) then
        Print("CGG: Wrong group!","\n");
        Print("CGG: Expected: ",g,"\n");
        Print("CGG: Found: ",Group(perms,()),"\n");
        return false;
    fi;
    if((Length(perms) > LargestMovedPoint(g)) and (Length(perms) > 1)) then
        Print("CGG: Too many generators! (LMP=)",LargestMovedPoint(g),"\n");
        Print(perms,"\n");
        return false;
    fi;
    return true;
end;;



proporderings := ["always", "alwaysbase", "never", "root", "firstnontrivial"];

# Checks all the ways of building up a single group -- can be extremely slow!
CheckGroup := function(g)
    local h, heuristics, only_gen, orbits, blocks, orbitals, output, record, f, orborders;
    # Clean off GAP name from group
    g := Group(GeneratorsOfGroup(g), ());
    if FERRET_EXTRA_TESTS then
        orborders := proporderings;
        heuristics := ["largest", "first", "smallest2", "randomsmallest", "random", "random"];
    else
        orborders := ["always"];
        heuristics := ["smallest", "smallest2", "randomsmallest", "random"];
    fi;

    for h in heuristics do
            for orbits in orborders do
                for blocks in proporderings do
                    for orbitals in proporderings do
                        record := rec(orbits := orbits, blocks := blocks, orbitals := orbitals);
                        output :=
                            Solve([ConInGroup(g, record)],
                                              rec( only_find_generators := true,
                                                   rbaseCellHeuristic := h,
                                                   recreturn := true) );
                        if not(compare_gen_groups(g, output)) then
                            Print("Check Group", [g,record,h]);
                        fi;

                         output :=
                            Solve([ConInGroup(g, record)],
                                              rec( only_find_generators := false,
                                                   rbaseCellHeuristic := h,
                                                   recreturn := true) );
                        if not(compare_full_groups(g, output)) then
                            Print("Check full group",[g,record,h]);
                        fi;
                    od;
                od;
            od;
    od;
    return true;
end;;

CheckStab := function(g, s, act, extra...)
    local permsStandard, permsExtra, stab, comp1, comp2, stabmap, optrec;
    
    if Size(extra) = 0 then
        stabmap := ConStabilize(s, act);
    elif Size(extra) = 1 then
        stabmap := ConStabilize(s, act, extra[1]);
    else
        ErrorNoReturn("Only 1, or no, extra arguments");
    fi;
    
    permsStandard := Solve([ConInGroup(g), stabmap], rec(recreturn := true));

    optrec := rec(blocks := Random(proporderings), orbitals := Random(proporderings));
    permsExtra := Solve([ConInGroup(g, optrec), stabmap], rec(recreturn := true));
    
    stab := Stabilizer(g, s, act);

    comp1 := compare_gen_groups(stab, permsStandard);

    if not(comp1) then
        Print("Fast check stab " , g , ",",s," with action ",act," and ", extra, " failed\n");
    fi;


    comp2 := compare_gen_groups(stab, permsExtra);

    if not(comp2) then
        Print("Fast check stab " , g , ",",s," with action ",act," and ", extra, " : ", optrec, " failed\n");
    fi;
    return (comp1 and comp2);
end;;

# Each of these functions should take a single integer, and return a pair, containing a constraint
# and a the group that constraint generates
ConstraintBuilders :=
[ 
    function(x)
        local g;
        g := RandomGroupUpToSize(GlobalMersenneTwister, x);
        return [ConInGroup(g), g];
    end,
    function(x)
        local s;
        s := RandomObj(GlobalMersenneTwister, x, OnSets);
        return [ConStabilize(s), Stabilizer(SymmetricGroup(x), s)];
    end
];

CheckSmallRandomPrimitives := function()
    return ForAll([5..10], x -> CheckStab(RandomGroupOfSize(GlobalMersenneTwister, x), [1,2,4],OnSets))
     and
           ForAll([3..6], x -> CheckStab(RandomGroupOfSize(GlobalMersenneTwister, x), [[1,2],[3,4]],OnSetsSets));

end;;

CheckRandomPrimitives := function()
    return ForAll([10..20], x -> CheckStab(RandomGroupOfSize(GlobalMersenneTwister, x), [2,4,6,8,9],OnSets))
     and
           ForAll([7..10], x -> CheckStab(RandomGroupOfSize(GlobalMersenneTwister, x), [[1,6],[2,4],[3,5]],OnSetsSets));

end;;
