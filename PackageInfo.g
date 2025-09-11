SetPackageInfo( rec(

PackageName := "ferret",
Subtitle := "Backtrack Search in Permutation Groups",
Version := "1.0.15",
Date := "11/09/2025", # dd/mm/yyyy format
License := "MPL-2.0",

Persons := [
  rec(
    IsAuthor := true,
    IsMaintainer := true,
    FirstNames := "Christopher",
    LastName := "Jefferson",
    WWWHome := "https://heather.cafe/",
    Email := "caj21@st-andrews.ac.uk",
    GitHubUsername := "ChrisJefferson",
    PostalAddress := Concatenation(
               "St Andrews\n",
               "Scotland\n",
               "UK" ),
    Place := "St Andrews",
    Institution := "University of St Andrews",
  ),
],

PackageWWWHome := "https://gap-packages.github.io/ferret/",

ArchiveURL     := Concatenation("https://github.com/gap-packages/ferret/",
                                "releases/download/v", ~.Version,
                                "/ferret-", ~.Version),
README_URL     := Concatenation( ~.PackageWWWHome, "README" ),
PackageInfoURL := Concatenation( ~.PackageWWWHome, "PackageInfo.g" ),

ArchiveFormats := ".tar.gz",

##  Status information. Currently the following cases are recognized:
##    "accepted"      for successfully refereed packages
##    "submitted"     for packages submitted for the refereeing
##    "deposited"     for packages for which the GAP developers agreed
##                    to distribute them with the core GAP system
##    "dev"           for development versions of packages
##    "other"         for all other packages
##
Status := "deposited",

SourceRepository := rec(
  Type := "git",
  URL := "https://github.com/gap-packages/ferret"
),
IssueTrackerURL := Concatenation( ~.SourceRepository.URL, "/issues" ),

AbstractHTML   :=
" The <span class=\"pkgname\">Ferret</span> package\
 provides a C++ reimplementation of Jeffery Leon's\
 Partition Backtrack framework for solving problems\
 in permutation groups",

PackageDoc := rec(
  BookName  := "Ferret",
  ArchiveURLSubset := ["doc"],
  HTMLStart := "doc/chap0_mj.html",
  PDFFile   := "doc/manual.pdf",
  SixFile   := "doc/manual.six",
  LongTitle := "Backtrack Search in Permutation Groups",
),

Dependencies := rec(
  GAP := ">= 4.12",
  NeededOtherPackages := [ [ "GAPDoc", ">= 1.5" ] ],
  SuggestedOtherPackages := [ ],
  ExternalConditions := [ ],
),

AvailabilityTest := function()
  if not IsKernelExtensionAvailable("ferret") then
    LogPackageLoadingMessage(PACKAGE_WARNING,
                             ["the kernel module is not compiled, ",
                              "the package cannot be loaded."]);
    return false;
  fi;
  return true;
end,

TestFile := "tst/testall.g",

Keywords := [ "Permutation Groups", "Partition Backtrack" ],

AutoDoc := rec(
    TitlePage := rec(
        Copyright := """
&copyright; by Christopher Jefferson
"""
    )
),

));


