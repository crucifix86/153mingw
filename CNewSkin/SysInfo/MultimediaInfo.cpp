//------------------------------------------------------------------------------
// MultimediaInfo.cpp
//    
//   This file contains MultimediaInfo.
// 
//   Copyright (c) 2001 J. Michael McGarrah ( mcgarrah@mcgarware.com )
// 
#include "MultimediaInfo.h"
#include <windows.h>
#include <mmreg.h>

#include <sstream>
using namespace std;

MultimediaInfo::MultimediaInfo()
{
   determineMultimediaInfo();
}

MultimediaInfo::MultimediaInfo(const MultimediaInfo& source)
{
   assign(source);
}

MultimediaInfo& MultimediaInfo::operator=(const MultimediaInfo& right)
{
   if (this != &right)
   {
      assign(right);
   }

   return (*this);
}

MultimediaInfo::~MultimediaInfo()
{
   // nothing to do yet
}

void MultimediaInfo::assign(const MultimediaInfo& source)
{
   m_bIsInstalled = source.m_bIsInstalled;
	m_bHasVolCtrl = source.m_bHasVolCtrl;
	m_bHasSeparateLRVolCtrl = source.m_bHasSeparateLRVolCtrl;
	m_strCompanyName = source.m_strCompanyName;
	m_strProductName = source.m_strProductName;
}

void MultimediaInfo::determineMultimediaInfo()
{

	HRESULT hRes = S_OK;
	MMRESULT mmRes = MMSYSERR_NOERROR;
	UINT numDevices = 0;
	UINT cbCaps;
	WAVEOUTCAPS wavCaps;

	// Get the number of audio output devices installed on your system.
	numDevices = waveOutGetNumDevs ();
	m_bIsInstalled = (numDevices > 0) ? TRUE : FALSE;

	// If the device is present, only then we will proceed to get other information.
	if (m_bIsInstalled)
	{
		hRes = E_FAIL;
		cbCaps = sizeof (AUXCAPS);
		mmRes = waveOutGetDevCaps (0, &wavCaps, cbCaps);
		if (mmRes == MMSYSERR_NOERROR) {
			hRes = S_OK;
			m_strProductName = wavCaps.szPname;
			m_bHasSeparateLRVolCtrl = (wavCaps.dwSupport & WAVECAPS_VOLUME) ? TRUE : FALSE;
			m_bHasVolCtrl = (wavCaps.dwSupport & AUXCAPS_VOLUME) ? TRUE : FALSE;
			m_strCompanyName = GetAudioDevCompanyName (wavCaps.wMid);
		}
	}

    return; 
}


std::string MultimediaInfo::GetAudioDevCompanyName(int wCompany)
{
	string stCompany;

	stCompany = ("Unknown");
	switch (wCompany) {
		case MM_MICROSOFT:
			stCompany = ("Microsoft Corporation");
			break;
		case MM_CREATIVE:
			stCompany = ("Creative Labs, Inc");
			break;
		case MM_MEDIAVISION:
			stCompany = ("Media Vision, Inc.");
			break;
		case MM_FUJITSU:
			stCompany = ("Fujitsu Corp.");
			break;
		case MM_ARTISOFT:
			stCompany = ("Artisoft, Inc.");
			break;
		case MM_TURTLE_BEACH:
			stCompany = ("Turtle Beach, Inc.");
			break;
		case MM_IBM:
			stCompany = ("IBM Corporation");
			break;
		case MM_VOCALTEC:
			stCompany = ("Vocaltec LTD.");
			break;
		case MM_ROLAND:
			stCompany = ("Roland");
			break;
		case MM_DSP_SOLUTIONS:
			stCompany = ("DSP Solutions, Inc.");
			break;
		case MM_NEC:
			stCompany = ("NEC");
			break;
		case MM_ATI:
			stCompany = ("ATI");
			break;
		case MM_WANGLABS:
			stCompany = ("Wang Laboratories, Inc");
			break;
		case MM_TANDY:
			stCompany = ("Tandy Corporation");
			break;
		case MM_VOYETRA:
			stCompany = ("Voyetra");
			break;
		case MM_ANTEX:
			stCompany = ("Antex Electronics Corporation");
			break;
		case MM_ICL_PS:
			stCompany = ("ICL Personal Systems");
			break;
		case MM_INTEL:
			stCompany = ("Intel Corporation");
			break;
		case MM_GRAVIS:
			stCompany = ("Advanced Gravis");
			break;
		case MM_VAL:
			stCompany = ("Video Associates Labs, Inc.");
			break;
		case MM_INTERACTIVE:
			stCompany = ("InterActive Inc");
			break;
		case MM_YAMAHA:
			stCompany = ("Yamaha Corporation of America");
			break;
		case MM_EVEREX:
			stCompany = ("Everex Systems, Inc");
			break;
		case MM_ECHO:
			stCompany = ("Echo Speech Corporation");
			break;
		case MM_SIERRA:
			stCompany = ("Sierra Semiconductor Corp");
			break;
		case MM_CAT:
			stCompany = ("Computer Aided Technologies");
			break;
		case MM_APPS:
			stCompany = ("APPS Software International");
			break;
		case MM_DSP_GROUP:
			stCompany = ("DSP Group, Inc");
			break;
		case MM_MELABS:
			stCompany = ("MicroEngineering Labs");
			break;
		case MM_COMPUTER_FRIENDS:
			stCompany = ("Computer Friends, Inc.");
			break;
		case MM_ESS:
			stCompany = ("ESS Technology");
			break;
		case MM_AUDIOFILE:
			stCompany = ("Audio, Inc.");
			break;
		case MM_MOTOROLA:
			stCompany = ("Motorola, Inc.");
		case MM_CANOPUS:
			stCompany = ("Canopus, co., Ltd.");
			break;
		case MM_EPSON:
			stCompany = ("Seiko Epson Corporation");
			break;
		case MM_TRUEVISION:
			stCompany = ("Truevision");
			break;
		case MM_AZTECH:
			stCompany = ("Aztech Labs, Inc.");
			break;
		case MM_VIDEOLOGIC:
			stCompany = ("Videologic");
			break;
		case MM_SCALACS:
			stCompany = ("SCALACS");
			break;
		case MM_KORG:
			stCompany = ("Korg Inc.");
			break;
		case MM_APT:
			stCompany = ("Audio Processing Technology");
			break;
		case MM_ICS:
			stCompany = ("Integrated Circuit Systems, Inc.");
			break;
		case MM_ITERATEDSYS:
			stCompany = ("Iterated Systems, Inc.");
			break;
		case MM_METHEUS:
			stCompany = ("Metheus");
			break;
		case MM_LOGITECH:
			stCompany = ("Logitech, Inc.");
			break;
		case MM_WINNOV:
			stCompany = ("Winnov, Inc.");
			break;
		case MM_NCR:
			stCompany = ("NCR Corporation");
			break;
		case MM_EXAN:
			stCompany = ("EXAN");
			break;
		case MM_AST:
			stCompany = ("AST Research Inc.");
			break;
		case MM_WILLOWPOND:
			stCompany = ("Willow Pond Corporation");
			break;
		case MM_SONICFOUNDRY:
			stCompany = ("Sonic Foundry");
			break;
		case MM_VITEC:
			stCompany = ("Vitec Multimedia");
			break;
		case MM_MOSCOM:
			stCompany = ("MOSCOM Corporation");
			break;
		case MM_SILICONSOFT:
			stCompany = ("Silicon Soft, Inc.");
			break;
		case MM_SUPERMAC:
			stCompany = ("Supermac");
			break;
		case MM_AUDIOPT:
			stCompany = ("Audio Processing Technology");
			break;
		case MM_SPEECHCOMP:
			stCompany = ("Speech Compression");
			break;
		case MM_AHEAD:
			stCompany = ("Ahead, Inc.");
			break;
		case MM_DOLBY:
			stCompany = ("Dolby Laboratories");
			break;
		case MM_OKI:
			stCompany = ("OKI");
			break;
		case MM_AURAVISION:
			stCompany = ("AuraVision Corporation");
			break;
		case MM_OLIVETTI:
			stCompany = ("Ing C. Olivetti & C., S.p.A.");
			break;
		case MM_IOMAGIC:
			stCompany = ("I/O Magic Corporation");
			break;
		case MM_MATSUSHITA:
			stCompany = ("Matsushita Electric Industrial Co., LTD.");
			break;
		case MM_CONTROLRES:
			stCompany = ("Control Resources Limited");
			break;
		case MM_XEBEC:
			stCompany = ("Xebec Multimedia Solutions Limited");
			break;
		case MM_NEWMEDIA:
			stCompany = ("New Media Corporation");
			break;
		case MM_NMS:
			stCompany = ("Natural MicroSystems");
			break;
		case MM_LYRRUS:
			stCompany = ("Lyrrus Inc.");
			break;
		case MM_COMPUSIC:
			stCompany = ("Compusic");
			break;
		case MM_OPTI:
			stCompany = ("OPTI Computers Inc.");
			break;
		case MM_ADLACC:
			stCompany = ("Adlib Accessories Inc.");
			break;
		case MM_COMPAQ:
			stCompany = ("Compaq Computer Corp.");
			break;
		case MM_DIALOGIC:
			stCompany = ("Dialogic Corporation");
			break;
		case MM_INSOFT:
			stCompany = ("InSoft, Inc.");
			break;
		case MM_MPTUS:
			stCompany = ("M.P. Technologies, Inc.");
			break;
		case MM_WEITEK:
			stCompany = ("Weitek");
			break;
		case MM_LERNOUT_AND_HAUSPIE:
			stCompany = ("Lernout & Hauspie");
			break;
		case MM_QCIAR:
			stCompany = ("Quanta Computer Inc.");
			break;
		case MM_APPLE:
			stCompany = ("Apple Computer, Inc.");
			break;
		case MM_DIGITAL:
			stCompany = ("Digital Equipment Corporation");
			break;
		case MM_MOTU:
			stCompany = ("Mark of the Unicorn");
			break;
		case MM_WORKBIT:
			stCompany = ("Workbit Corporation");
			break;
		case MM_OSITECH:
			stCompany = ("Ositech Communications Inc.");
			break;
		case MM_MIRO:
			stCompany = ("miro Computer Products AG");
			break;
		case MM_CIRRUSLOGIC:
			stCompany = ("Cirrus Logic");
			break;
		case MM_ISOLUTION:
			stCompany = ("ISOLUTION  B.V.");
			break;
		case MM_HORIZONS:
			stCompany = ("Horizons Technology, Inc");
			break;
		case MM_CONCEPTS:
			stCompany = ("Computer Concepts Ltd");
			break;
		case MM_VTG:
			stCompany = ("Voice Technologies Group, Inc.");
			break;
		case MM_RADIUS:
			stCompany = ("Radius");
			break;
		case MM_ROCKWELL:
			stCompany = ("Rockwell International");
			break;
		//case MM_XYZ:
		//	stCompany = ("Co. XYZ for testing");
		//	break;
		case MM_OPCODE:
			stCompany = ("Opcode Systems");
			break;
		case MM_VOXWARE:
			stCompany = ("Voxware Inc");
			break;
		case MM_NORTHERN_TELECOM:
			stCompany = ("Northern Telecom Limited");
			break;
		case MM_APICOM:
			stCompany = ("APICOM");
			break;
		case MM_GRANDE:
			stCompany = ("Grande Software");
			break;
		case MM_ADDX:
			stCompany = ("ADDX");
			break;
		case MM_WILDCAT:
			stCompany = ("Wildcat Canyon Software");
			break;
		case MM_RHETOREX:
			stCompany = ("Rhetorex Inc");
			break;
		case MM_BROOKTREE:
			stCompany = ("Brooktree Corporation");
			break;
		case MM_ENSONIQ:
			stCompany = ("ENSONIQ Corporation");
			break;
		case MM_FAST:
			stCompany = ("///FAST Multimedia AG");
			break;
		case MM_NVIDIA:
			stCompany = ("NVidia Corporation");
			break;
		case MM_OKSORI:
			stCompany = ("OKSORI Co., Ltd.");
			break;
		case MM_DIACOUSTICS:
			stCompany = ("DiAcoustics, Inc.");
			break;
		case MM_GULBRANSEN:
			stCompany = ("Gulbransen, Inc.");
			break;
		case MM_KAY_ELEMETRICS:
			stCompany = ("Kay Elemetrics, Inc.");
			break;
		case MM_CRYSTAL:
			stCompany = ("Crystal Semiconductor Corporation");
			break;
		case MM_SPLASH_STUDIOS:
			stCompany = ("Splash Studios");
			break;
		case MM_QUARTERDECK:
			stCompany = ("Quarterdeck Corporation");
			break;
		case MM_TDK:
			stCompany = ("TDK Corporation");
			break;
		case MM_DIGITAL_AUDIO_LABS:
			stCompany = ("Digital Audio Labs, Inc.");
			break;
		case MM_SEERSYS:
			stCompany = ("Seer Systems, Inc.");
			break;
		case MM_PICTURETEL:
			stCompany = ("PictureTel Corporation");
			break;
		case MM_ATT_MICROELECTRONICS:
			stCompany = ("AT&T Microelectronics");
			break;
		case MM_OSPREY:
			stCompany = ("Osprey Technologies, Inc.");
			break;
		case MM_MEDIATRIX:
			stCompany = ("Mediatrix Peripherals");
			break;
		case MM_SOUNDESIGNS:
			stCompany = ("SounDesignS M.C.S. Ltd.");
			break;
		case MM_ALDIGITAL:
			stCompany = ("A.L. Digital Ltd.");
			break;
		case MM_SPECTRUM_SIGNAL_PROCESSING:
			stCompany = ("Spectrum Signal Processing, Inc.");
			break;
		case MM_ECS:
			stCompany = ("Electronic Courseware Systems, Inc.");
			break;
		case MM_AMD:
			stCompany = ("AMD");
			break;
		case MM_COREDYNAMICS:
			stCompany = ("Core Dynamics");
			break;
		case MM_CANAM:
			stCompany = ("CANAM Computers");
			break;
		case MM_SOFTSOUND:
			stCompany = ("Softsound, Ltd.");
			break;
		case MM_NORRIS:
			stCompany = ("Norris Communications, Inc.");
			break;
		case MM_DDD:
			stCompany = ("Danka Data Devices");
			break;
		case MM_EUPHONICS:
			stCompany = ("EuPhonics");
			break;
		case MM_PRECEPT:
			stCompany = ("Precept Software, Inc.");
			break;
		case MM_CRYSTAL_NET:
			stCompany = ("Crystal Net Corporation");
			break;
		case MM_CHROMATIC:
			stCompany = ("Chromatic Research, Inc");
			break;
		case MM_VOICEINFO:
			stCompany = ("Voice Information Systems, Inc");
			break;
		case MM_VIENNASYS:
			stCompany = ("Vienna Systems");
			break;
		case MM_CONNECTIX:
			stCompany = ("Connectix Corporation");
			break;
		case MM_GADGETLABS:
			stCompany = ("Gadget Labs LLC");
			break;
		case MM_FRONTIER:
			stCompany = ("Frontier Design Group LLC");
			break;
		case MM_VIONA:
			stCompany = ("Viona Development GmbH");
			break;
		case MM_CASIO:
			stCompany = ("Casio Computer Co., LTD");
			break;
		case MM_DIAMONDMM:
			stCompany = ("Diamond Multimedia");
			break;
		case MM_S3:
			stCompany = ("S3");
			break;
		case MM_FRAUNHOFER_IIS:
			stCompany = ("Fraunhofer");
			break;
		default:
			stCompany = ("Unknown");
			break;
	}

	return stCompany;
}
