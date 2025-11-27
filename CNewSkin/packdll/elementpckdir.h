#ifndef _ELEMENTPCKDIR_H_
#define _ELEMENTPCKDIR_H_

const char *	g_szPckDir1[] = {
	"configs",
	"facedata",
	"gfx",
	"grasses",
	"help\\script",
	"interfaces",
	"building",
	"models",
	"sfx",
	"shaders",
	"surfaces",
	"textures",
	"trees",
	"litmodels",
	"loddata",
};

const char *	g_szPckDir2[] = {
	"configs",
	"facedata",
	"gfx",
	"grasses",
	"help\\script",
	"interfaces",
	"script",
	"building",
	"models",
	"sfx",
	"shaders",
	"surfaces",
	"textures",
	"trees",
	"litmodels",
	"loddata",
};

const char *	g_szNoPckDir[][2] = {
	"cursors", "", // r means recursive for param 2
	"data", "",
	"font", "",
	"help\\voice", "r",
	"maps", "r",
	"userdata\\userpic", "",
	"userdata\\character", "r",
	"userdata\\server", "",
	"music", "r",
	"info", "",
	"bbsfiles", "r",
	".", "s",		// s means file must be in the sep files list
	"reportbugs", "s",
};

const char *	g_szSepFiles[] =
{
	"dbserver.conf",
	"elementclient.exe",
	"dbghelp.dll",
	"elementskill.dll",
	"ftdriver.dll",
	"ifc22.dll",
	"immwrapper.dll",
	"speedtreert.dll",
	"unicows.dll",
	"zlibwapi.dll",
	"reportbugs.exe",
	"mfc42.dll",
	"modelview.exe",
};

#endif
