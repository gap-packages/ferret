#
#
# This file runs package tests. It is also referenced in the package
# metadata in PackageInfo.g.
#
LoadPackage( "io" );
LoadPackage( "ferret" );

# Sanity check that we loaded ferret correctly
g := Solve([ConInGroup(AlternatingGroup(3))]);
if g <> AlternatingGroup(3) then
  Print("Failed to correctly load ferret");
  GAP_EXIT(1);
fi;

dirs := DirectoriesPackageLibrary( "ferret", "tst" );

HasSuffix := function(list, suffix)
  local len;
  len := Length(list);
  if Length(list) < Length(suffix) then return false; fi;
  return list{[len-Length(suffix)+1..len]} = suffix;
end;

# Load all tests in that directory
tests := DirectoryContents(dirs[1]);
tests := Filtered(tests, name -> HasSuffix(name, ".tst"));
Sort(tests);

# Convert tests to filenames
tests := List(tests, test -> Filename(dirs,test));

runtest := function(test)
    if Test(test, rec(compareFunction := "uptowhitespace")) then
        Print("Test '", test, "' succeeded\n");
        return true;
    else
        Print("Test '", test, "' failed\n");
        return false;
    fi;
end;

l := ParListByFork(tests, runtest, rec(NumberJobs := 4));

if ForAny(l, x -> (x=false)) then
    IO_exit(1);
else
    IO_exit(0);
fi;
