#pragma once
#include <string>

namespace Clonemmings
{
	constexpr uint32_t CONTROLPANELLABEL = 0;
	constexpr uint32_t LEVELCONTROLSLABEL = 1;
	constexpr uint32_t STARTLEVELBUTTON = 2;
	constexpr uint32_t PAUSELEVELBUTTON = 3;
	constexpr uint32_t OPENLEVELBUTTON = 4;
	constexpr uint32_t QUITBUTTON = 5;
	constexpr uint32_t GAMECONTROLLEVEL = 6;
	constexpr uint32_t RELEASERATEDRAGFLOAT = 7;
	constexpr uint32_t NUMFLOATERSLEFTLABEL = 8;
	constexpr uint32_t NUMBLOCKERSLEFTLABEL = 9;
	constexpr uint32_t NUMDIGGERSLEFTLABEL = 10;
	constexpr uint32_t NUMMINERSLEFTLABEL = 11;
	constexpr uint32_t MAKEFLOATERBUTTON = 12;
	constexpr uint32_t MAKEBLOCKERBUTTON = 13;
	constexpr uint32_t MAKEDIGGERBUTTON = 14;
	constexpr uint32_t MAKEMINERBUTTON = 15;
	constexpr uint32_t MAKEWALKERBUTTON = 16;
	constexpr uint32_t STATISTICSLABEL = 17;
	constexpr uint32_t CLONEMMINGSSAVEDLABEL = 18;
	constexpr uint32_t CLONEMMINGSLOSTLABEL = 19;
	constexpr uint32_t UNPAUSELEVELBUTTON = 20;
	constexpr uint32_t STOPLEVELBUTTON = 21;

#ifdef DEBUG
	#define CONTROLPANELLABELTEXT "Control Panel"
	#define LEVELCONTROLSLABELTEXT "Level Controls"
	#define STARTLEVELBUTTONTEXT "Start Level"
	#define STOPLEVELBUTTONTEXT "Stop Level"
	#define PAUSELEVELBUTTONTEXT "Pause Level"
	#define UNPAUSElEVELBUTTONTEXT "Unpause Level"
	#define OPENLEVELBUTTONTEXT "Open Level"
	#define QUITBUTTONTEXT "Quit"
	#define GAMECONTROLLEVELTEXT "Game Controls"
	#define RELEASERATEDRAGFLOATTEXT "Clonemming Release Rate"
	#define NUMFLOATERSLEFTLABELTEXT "Number of floaters left"
	#define NUMBLOCKERSLEFTLABELTEXT "Number of blockers left"
	#define NUMDIGGERSLEFTLABELTEXT "Number of diggers left"
	#define NUMMINERSLEFTLABELTEXT "Number of miners left"
	#define MAKEFLOATERBUTTONTEXT "Make Floater"
	#define MAKEBLOCKERBUTTONTEXT "Make Blocker"
	#define MAKEDIGGERBUTTONTEXT "Make Digger"
	#define MAKEMINERBUTTONTEXT "Make Miner"
	#define MAKEWALKERBUTTONTEXT "Make Walker"
	#define STATISTICSLABELTEXT "Level Statistics"
	#define CLONEMMINGSSAVEDLABELTEXT "Number of clonemmings saved"
	#define CLONEMMINGSLOSTLABELTEXT "Number of clonemmings lost"
#endif // DEBUG

}
