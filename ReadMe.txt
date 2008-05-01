------------------------------------------------------------------------------------------
SimpleLib

Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
http://www.toptensoftware.com

Copyright for contributed code remains with the original authors, as marked in the code.

This code has been released for use "as is".  Any redistribution or modification however 
is strictly prohibited.   See the readme.txt file for complete terms and conditions.
------------------------------------------------------------------------------------------


This file describes SimpleLib which is a set of C++ library containing many useful
functions, classes, macros etc...

SimpleLib is divided into 5 parts, each of which is dependant on the previous.  SimpleLib.h
is the most basic and is just an include file.  All other parts need to be compiled to libs
and linked against.

For latest version please see http://www.toptensoftware.com/simplelib

SimpleLib.h
-----------

* Collections + strings + misc - see http://www.toptensoftware.com/simplelib
* Cross platform compatible
* Completely inline - no libs to build/link against


SimpleLibUtils - possibly cross platform compatible .lib of miscellaneous useful stuff
--------------

* CalcCRC- CRC Calculations
* CommandLineParser - Command Line Parser
* FormatBinary - Format binary data to hex
* IncrementString - increment a string (eg: Button1->Button2)
* LoadSaveTextFile - Load/save text files as ansi/unicode
* ParseHtmlColor - Parse HTML color strings to RGB (eg: "#0000FF" or "Blue")
* ParseUtils - Miscellaneous text parsing functions
* PathLibrary - split and join paths, extract titles, change file extensions, check if qualified etc...
* ProfileFile - load and save ini files + more
* SmartStringCompare - compares strings but understands integer numbers
* SplitCommandLine - very simple command line parser/splitter
* SplitString various string split and unsplit functions
* StringReplace - String search and replace
* StringSearch - String search
* Tokenizer - C++ tokenizer
* Wildcard - Wildcard matching functions  (eg: blah*Blah?)


SimpleLibWin - library of Win32 utilities
------------

* AutoSizeControls - automatically resize controls when parent window resizes
* BufferedPaintDC - double buffered screen paint dc
* CallbackTimer - timers with LPARAM user data and without needing window handle
* ClipDC - helper to clip down then restore a DC
* CommandState - extension to WM_COMMAND to support checked/unchecked menu/toolbar updating
* CopyStream - copy one IStream to another
* CreateFontEasy - easy font creation
* DataExchange - MFC style dialog exchange without MFC
* DocView - misc stuff for building doc/view architecture without MFC
* DoWebRequest - get a web page
* DragDetectEx - better version of DragDetect
* FadeWindow - fade then destroy or hide a window
* FileStream - Open a file as a IStream
* FileUtils - miscellaneous file functions (eg: recursive folder creation, filename of module)
* FindFiles - Wrapper aroudn FindFirst/NextFile, returns string vector
* FormatError - build string from an HRESULT
* GdiMisc - miscellanoue GDI stuff (alpha fill, fill v/h line)
* GdiMiscEx - various image blt, strected edge image blts etc...
* GdiPlusLoader - loads GDI+ dll
* GdiText - text measure/draw helpers
* GeoOps - helpers for working with Points, Rects etc, including operators
* IsInputMessage - check if a Win32 message is an input message
* LoadSaveTextStream - load/save text to a stream
* LoadString - helper to load a string resource
* MessageBox - message box wrappers
* MessageLoop - message loop class
* MouseTracker - tracks mouse movements with various implementations
* MRUFileList - list of MRU files
* NotifyIcon - shell tray icon wrapper
* PostableObject - post messages to objects that aren't windows
* ProfileFileBinary - save/loaded binary format structured ini files
* ProfileSettings - helpers for setting/getting profile settings from registry
* ProfileStream - save/load binary string from a CProfileFile
* ReflectMessages - like ATL's message reflection without bugs
* RegistryStream - save/load stream to registry
* RegistryUtils - Registry helper functions
* ResourceContentProvider - content from resource into C++ tokenizer
* RunModalLoop - run a modal message loop
* SaveWindowPlacemenet - save/load window position to registry
* SelectObject - Select object to hDC and automatically restore it
* SetLayeredWindowAttributesEx - transparent/region windows
* ShellUtils - helpers for shell integration
* SimpleDdeClient - useful for shell DDE integration
* SimpleFileDialog - wrapper around common file dialog
* SimpleMemoryStream - implements IStream on a block of memory
* SmartHandle - automatically closing GDI handle, registry HKEY, Win32 HANDLE etc...
* Splitter - simple splitter window stuff for dialogs
* StreamHelpers - various functions to simplify using IStreams
* StreamImpl - provides QI implementation for an IStream implementation
* SubStream - provides an IStream which is sub-section of another stream
* ThemeActivate - stuff to help with XP themed windows
* ThemedDrawings - helpers for doing XP themed drawing
* Threading - thread, event, semaphore wrappers
* TimerVector - A collection of timed callbacks
* TranslateColor - simpler than OleTranslateColor
* VersionInfo - various helpers for extracting file version info
* WindowImplEx - better


SimpleLibAtlWin - Win32 utilities built on top of ATL
---------------

* ADXDialog - ATL integration of DataExchange
* CheckList - ATL check list box
* FrameWindow - Simple Docking frame window functionality for ATL apps
* ListViewUtils - helpers for working with listview controls
* LoadIconEx - load small/large icons
* MdiWindow - MDI with ATL
* MenuBar/MenuBarApi - menu bar as a child window control
* MenuIcons - simply add icons to menus
* PropertySheet - implement property sheets in ATL apps
* RegisteredWindow - implement Win32 Registered window classes in ATL
* ResizeablePanel - resizable dock panel for FrameWindow
* SimpleToolTip - custom tool tip implementation
* ToolBarCtrlEx - wrapper/helper for working with toolbars
* WindowImplEx - enhanced version of CWindowImpl


SimpleLibAtlCom - library of COM related utilities built on top of ATL
---------------

* AreFontsEqual - compare to IFonts (??)
* AtlDrawInfo - better draw info for ActiveX controls
* BorderStyle - border style adjusters for ActiveX controls
* CoCreateScriptEngine - load a script engine while avoiding virus scanner
* ComControlEx - enhanced ComControl implementation
* ComError - better com error reporting
* ComObjectOwner - owned COM object implementation
* ComObjectSink - simple sink interface for CComObject classes
* ComPtrVector - a vector (collection) of com objects with support for indexed lookups + _NewEnum enumerators
* DispatchProperty - get/set dispatch properties
* EnumVariantImpl - for automation enumerators
* FontHolder - wrapper/holder for IFont/IFontDisp
* FormatBSTR - sprintf style formatting without CRT
* GetControlIcon - get the icon for an ActiveX control
* IsControlInDesignMode - check if a control is in design/run mode
* Now - get current date in VARIANT format
* ObjectIdentity - compare objects for identity
* ObjectWithNamedValues - simple attachable properties for a COM object
* ObjectWithWeakOwner - implement objects with weak owners (for building object hierarchies without circular reference probs)
* OleCommandTargetImpl - ATL style IOleCommandTarget implementation
* OleKeyMouse - ActiveX key/mouse mappings
* PerThreadSingleton - COM object that is singleton on each thread on which its created
* PropertyBagOnXML - implement IPropertyBag as XML
* PropertyNotifyHandler - COM property notification listener
* SafeArray - simple safe array wrapper
* SimpleDataObject - IDataObject implementation
* SimpleDragDrop - drag/drop/clipboard stuff
* SimpleEventSink/SimpleEventSinkBase - sink dispatch events easily in C++
* SimpleSink - Simple sink implementation
* SpecifyPropertyPagesImpl2 - different property pages in run/design mode
* StandardError - Clear IErrorInfo for standard error result codes
* SubclassedControl - ActiveX control of a subclassed window
* TypeInfoHelpers - C++ ITypeLib/ITypeInfo wrappers
* VariantGrid - a 2D grid of variants
* VariantParams - helpers for working with optional parameters
* XMLArchive - wrapper around XML DOM for simplified saving/loading in XML





Licensing SimpleLib
-------------------

SimpleLib is provided on the following terms and conditions:

* SimpleLib is completely free for unrestricted use in any commercial, retail, freeware, 
shareware, open source or other software project.
* SimpleLib is provided on an as-is basis, with no warranty to suitability or quality.
* SimpleLib is provided with no support.  Topten Software will usually provide some 
assistance in using SimpleLib but is not obliged to do so.
* The source code for SimpleLib may not be redistributed in either its original or modified 
form.  Projects using SimpleLib that are distributed with source code should refer
to http://www.toptensoftware.com/simplelib as the original source for SimpleLib.  This 
limitation will not apply in the case that Topten Software ceases to exist, or in the case
that Topten Software grants written permission.
* Copyright for SimpleLib remains with Topten Software, except for code contributed by
other developers (which is clearly marked in the code base) where copyright remains with
the respective contributors.
* There is no need to acknowledge use of SimpleLib in applications that use it.
