(* PacletInfo file template
 * Contains placeholders (@xxx@) that will be replaced at configuration time by CMake (in the function called install_paclet_files).
 * The PacletInfo.wl file with proper values substituted will be then copied to the final paclet layout.
 *)
Paclet[
	"Name" -> "@CMAKE_PROJECT_NAME@",
	"Version" -> "@CMAKE_PROJECT_VERSION@",
	"Description" -> "@CMAKE_PROJECT_DESCRIPTION@",
	"WolframVersion" -> "12.1+",
	"Updating" -> Automatic,
	"Extensions" -> {
		{"Kernel", Root -> "Kernel", Context -> "@CMAKE_PROJECT_NAME@`"},
		{"LibraryLink"}
	}
]