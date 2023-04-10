# ProxyLibraryGenerator

- Generate dummy DLL which calls an actual DLL
- This makes it easier to intercept calls to a library

## Usage example

1. Make a copy of the DLL you want to intercept
	- ``COPY path\to\app\interesting.dll path\to\app\really_interesting.dll``
2. Compile the ProxyLibraryGenerator
3. Run and generate the source & project files
	- ``ProxyLibraryGenerator.exe library="path\to\app\really_interesting.dll" output="path\to\generated_files"``
		- Or if you have a more specific use case (i.e. executable) in mind
	- ``ProxyLibraryGenerator.exe library="path\to\app\really_interesting.dll" executable="path\to\app\example.exe" output="path\to\generated_files``
4. Open the output folder
5. Open the .vcxproj project file using Visual Studio
	- It will generate the "solution file" for the project automatically
6. Compile the proxy DLL
7. Overwrite the original DLL
	- ``COPY path\to\generated_files\interesting.dll path\to\app\interesting.dll``
	- NOTE: you could add a post build step in the generated project which does this for you
8. Run the application of interest
9. Attach debugger in the instance of Visual Studio where you compiled the proxy DLL
