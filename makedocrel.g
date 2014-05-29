###########################################################################
##
#W makedocrel.g           From the Congruence package       Alexander Konovalov
##
###########################################################################

ExtractMyManualExamples:=function( pkgname, main, files )
local path, tst, i, s, name, output, ch, a;
path:=Directory(
        Concatenation(PackageInfo(pkgname)[1].InstallationPath, "/doc") );
Print("Extracting manual examples for ", pkgname, " package ...\n" );
tst:=ExtractExamples( path, main, files, "Chapter" );
Print(Length(tst), " chapters detected\n");
for i in [ 1 .. Length(tst) ] do
  Print( "Chapter ", i, " : \c" );
  if Length( tst[i] ) > 0 then
    s := String(i);
    if Length(s)=1 then
      # works for <100 chapters
      s:=Concatenation("0",s);
    fi;
    name := Filename(
              Directory(
                Concatenation( PackageInfo(pkgname)[1].InstallationPath,
                               "/tst" ) ),
                Concatenation( LowercaseString(pkgname), s, ".tst" ) );
    output := OutputTextFile( name, false ); # to empty the file first
    SetPrintFormattingStatus( output, false ); # to avoid line breaks
    ch := tst[i];
    AppendTo(output, "# ", pkgname, ", chapter ",i,"\n");
    for a in ch do
      AppendTo(output, "\n# ",a[2], a[1]);
    od;
    Print("extracted ", Length(ch), " examples \n");
  else
    Print("no examples \n" );
  fi;
od;
end;

###########################################################################

CONGRUENCEMANUALFILES:=[
"../PackageInfo.g","../lib/yapb.gd"
];

MakeGAPDocDoc( "doc",     # path to the directory containing the main file
               "manual",  # the name of the main file (without extension)
                          # list of (probably source code) files relative
                          # to path which contain pieces of documentation
                          # which must be included in the document
               CONGRUENCEMANUALFILES,
               "ferret",# the name of the book used by GAP's online help
               "../../..",# optional: relative path to the main GAP root
                          # directory to produce HTML files with relative
                          # paths to external books.
               "MathJax"  # optional: use "MathJax", "Tth" and/or "MathML"
                          # to produce additional variants of HTML files
               );;

# Copy the *.css and *.js files from the styles directory of the GAPDoc
# package into the directory containing the package manual.
CopyHTMLStyleFiles( "doc" );

# Create the manual.lab file which is needed if the main manuals or another
# package is referring to your package
GAPDocManualLab( "ferret" );;

ExtractMyManualExamples( "ferret", "manual.xml", CONGRUENCEMANUALFILES );

QUIT;
###########################################################################
##
#E
##
