#############################################################################
##
##
#W  helper_functions.g                                        Chris Jefferson
##
##  This file contains a number of functions which I find of use. I share
##  this file amongst multiple packages, and will try to keep it in sync.
##  The name of the class is changed for each package, to deal with different
##  versions of packages
##
#Y  Copyright (C) 2014     University of St. Andrews, North Haugh,
#Y                          St. Andrews, Fife KY16 9SS, Scotland
##


_FerretHelperFuncs := MakeImmutable(rec(

  # Simple helper to support optional arguments
  optArg := function(Val, default)
    if Length(Val) = 0 then
      return default;
    fi;
    if Length(Val) = 1 then
      return Val[1];
    fi;
    ErrorNoReturn("Only one optional argument!");
  end,

# Copies 'useroptions' over values of 'options' with the same name.
fillUserValues := function(options, useroptions)
  local name, ret;
  ret := rec();
  
  useroptions := ShallowCopy(useroptions);

  for name in RecNames(options) do
    if IsBound(useroptions.(name)) then
      ret.(name) := useroptions.(name);
      Unbind(useroptions.(name));
    else
      ret.(name) := options.(name);
    fi;
  od;

  if useroptions <> rec() then
    Error("Unknown options: ", useroptions);
  fi;

  return ret;
 end));
