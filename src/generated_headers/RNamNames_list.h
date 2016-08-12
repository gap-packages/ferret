#ifndef RNAM
#define RNAM(x) extern UInt RName_ ## x;
#endif


RNAM(SetStab)
RNAM(ListStab)
RNAM(arg)
RNAM(constraint)
RNAM(size)
RNAM(only_find_generators)
RNAM(canonical)
RNAM(stats)
RNAM(just_rbase)

RNAM(searchValueHeuristic)
RNAM(searchFirstBranchValueHeuristic)
RNAM(rbaseCellHeuristic)
RNAM(rbaseValueHeuristic)

RNAM(nodeLimit)

RNAM(generators)
RNAM(genlabels)
RNAM(identity)
RNAM(labels)
RNAM(orbit)
RNAM(stabilizer)
RNAM(translabels)
RNAM(transversal)


#undef RNAM
