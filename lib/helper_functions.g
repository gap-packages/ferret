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

_FerretHelperFuncs := rec(

# Copies 'useroptions' over values of 'options' with the same name.
fillUserValues := function(options, useroptions)
  local name, ret;
  
  ret := rec();

  for name in RecNames(options) do
    if IsBound(useroptions.(name)) then
      options.(name) := useroptions.(name);
      Unbind(useroptions.(name));
    fi;
  od;
  
  if useroptions <> rec() then
    Error("Unknown options: ", useroptions);
  fi;
 end
 );