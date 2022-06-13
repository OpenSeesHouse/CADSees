// (C) Copyright 2013 by Microsoft 
//
//#define CAD14
using System;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.ApplicationServices;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.Customization;
//using Registry = Microsoft.Win32.Registry;
//using RegistryKey = Microsoft.Win32.RegistryKey;
using Autodesk.AutoCAD.DatabaseServices;
// This line is not mandatory, but improves loading performances
[assembly: ExtensionApplication(typeof(CADSeesLoader.MyPlugin))]

namespace CADSeesLoader
{

	// This class is instantiated by AutoCAD once and kept alive for the 
	// duration of the session. If you don't do any one time initialization 
	// then you should remove this class.
	public class MyPlugin : IExtensionApplication
	{

		void IExtensionApplication.Initialize()
		{
			// Add one time initialization here
			// One common scenario is to setup a callback function here that 
			// unmanaged code can call. 
			// To do this:
			// 1. Export a function from unmanaged code that takes a function
			//    pointer and stores the passed in value in a global variable.
			// 2. Call this exported function in this function passing delegate.
			// 3. When unmanaged code needs the services of this managed module
			//    you simply call acrxLoadApp() and by the time acrxLoadApp 
			//    returns  global function pointer is initialized to point to
			//    the C# delegate.
			// For more info see: 
			// http://msdn2.microsoft.com/en-US/library/5zwkzwf4(VS.80).aspx
			// http://msdn2.microsoft.com/en-us/library/44ey4b32(VS.80).aspx
			// http://msdn2.microsoft.com/en-US/library/7esfatk4.aspx
			// as well as some of the existing AutoCAD managed apps.

			// Initialize your plug-in application here
			// Opening the registry key
			Editor ed = Application.DocumentManager.MdiActiveDocument.Editor;
			//ed.WriteMessage("\n1");
			RegistryKey sk1 = Registry.CurrentUser.OpenSubKey(HostApplicationServices.Current.UserRegistryProductRootKey, true)
				.OpenSubKey("Applications", true).OpenSubKey("CADSees", true);
			var pathObj = sk1.GetValue("LOADER");
			if (pathObj == null)
				return;
			string path = (string)pathObj;
			int index = path.LastIndexOf("\\");
			path = path.Remove(index);
			string isFrst = (string)sk1.GetValue("ISFIRSTLOAD");
			if (isFrst.Equals("TRUE"))
			{
				var cuiPath = (string)sk1.GetValue("TOOLFOLDER") + "\\CADSeesToolbar.cuix";
				string mainCuiFile = (string)Application.GetSystemVariable("MENUNAME");
				mainCuiFile += ".cuix";
				CustomizationSection cs = new CustomizationSection(mainCuiFile);
				int res2 = cs.PartialCuiFiles.Add(cuiPath);
				CustomizationSection myCui = new CustomizationSection(cuiPath);
				cs.Save();
				Application.ReloadAllMenus();
				sk1.SetValue("ISFIRSTLOAD", "FALSE", Microsoft.Win32.RegistryValueKind.String);
			}
			string arxPath = path + "\\CADSees.arx";
			string dbxPath = path + "\\CSSObjects.dbx";
#if DEBUG
			dbxPath = path + "\\Objects\\CSSObjects.dbx";
#endif
			DynamicLinker DL = SystemObjects.DynamicLinker;
			DL.LoadModule(dbxPath, false, false);
			DL.LoadModule(arxPath, false, false);
			Welcome form = new Welcome();
			form.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			form.ShowDialog();
			//System.Threading.Thread.Sleep(2500);
			//form.Dispose();
			//ed.WriteMessage("\naaaaaaaaaaaaaaa");
		}

		void IExtensionApplication.Terminate()
		{
			// Do plug-in application clean up here
		}

	}

}
