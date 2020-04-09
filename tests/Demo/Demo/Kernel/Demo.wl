BeginPackage["Demo`"];

Squared;

Begin["`Private`"];

$BaseDemoDirectory = FileNameDrop[$InputFileName, -2];
Get[FileNameJoin[{$BaseDemoDirectory, "LibraryResources", "LibraryLinkUtilities.wl"}]];


Squared[x_] := x^2;

End[];

EndPackage[];