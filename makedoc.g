##  This builds the documentation of the Example package
##  Needs: GAPDoc package, latex, pdflatex, mkindex
##  
if fail = LoadPackage("AutoDoc", ">= 2016.01.21") then
    Error("AutoDoc 2016.01.21 or newer is required");
fi;

AutoDoc(rec(scaffold := rec( MainPage := false )));
