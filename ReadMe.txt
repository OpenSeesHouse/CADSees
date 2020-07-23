
2020/07/18 (1399/4/28 Iranian Date)


Welcome! and thank you for your interest in the CADSees application!

CADSees is an Open-Source pre/post-processor of OpenSees designed for AutoCAD application.
The current version is based on the  2021 version of AutoCAD's x64 architecture.


To successfully build the code (to use the built files see items 6-11)

1-install Visual Studio 2019 and .NetFramework 4.8

2-install OBjectArx SDK version 2021 in C:\OBjectArx

3-insatll boost_1_73_0 in C:\

4-Currently, outputs are created in folder: C:\Program files\CADSees. Since this folder is placed in C:\ drive,
 you need to run Visual Studio "as administrator". To neutralize this need, go to project settings (all projects)
 and change "Linker->General->Output File" to another path.

5-Since CADSees project depends on the outputs of the CSSObjects project, you have to first build this project.

6-After successful building of the projects, run "CADSees_reg-2021.reg" placed in the root of the repository to
 add the required entries to the Windows registry. This activates the preferred on-demand loading of the dll files 
 created in "C:\Program files\CADSees" folder.

7-To have autoCAD automatically load the files, copy the contents of the  "test outputs" folder to the path.
 In this folder, there are also sample OpenSees models (.ops files) you can use for testing the application.

8-After running AutoCAD, create a new drawing and run the "importOPS" command. At the first time, the toolbar placed
 in the "C:\Program files\CADSees" will be added to the environment and you can invoke the commands from this toolbar
 in future runs. If CADSees is loaded successfully, you will see the start screen with the "CivilSoftScience" logo.

9-Browse for the ".ops" file containing OpenSees model definition. This file should NOT contain TCL commands. To produce this
 raw OpenSees model from your TCL scripts, place the "logCommands" command at the beginning of your script to save 
 the raw (no-script) model once you run it.

10-"logCommands" command is added to OpenSees for enhanced use of CADSees and is available in the CSS version of OpenSees
 downloadable from link below. An OpenSees installer is also provided at this page. Please also see the readMe file provided
 at this path:
 https://drive.google.com/drive/folders/1kMREGdIJY-zqTUqZnUoGjyO0HgPScF6M?usp=sharing)

11-Syntax of the "logCommands" command:
 logCommands <-file $fileName [default = commands.ops]>
 Note: to load the output file from autoCAD, it should have .ops extension.
 

