#pragma once

#ifndef GLITCH_H
#define GLITCH_H

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned short		u_int16;
typedef unsigned long		u_long;
typedef short int			int16;
#define PF_TABLE_BITS	12
#define PF_TABLE_SZ_16	4096

#define PF_DEEP_COLOR_AWARE 1	// make sure we get 16bpc pixels; 
// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#include "Glitch_Strings.h"


/* Versioning information */

#define	MAJOR_VERSION	1
#define	MINOR_VERSION	0
#define	BUG_VERSION		0
#define	STAGE_VERSION	PF_Stage_DEVELOP
#define	BUILD_VERSION	1

enum
{
	GLITCH_INPUT = 0,

	GLITCH_SCANLINE,
	GLITCH_SCANLINE_POSITION,
	GLITCH_SCANLINE_HEIGHT,
	GLITCH_RANDOM_HEIGHT,
	GLITCH_AREA_COUNT,
	GLITCH_AREA_GLITCH_COUNT,
	GLITCH_COMPOSE,
	GLITCH_OPACITY,
	GLITCH_SEED_CONTROLL,
	GLITCH_AREA_SEED,

	GLITCH_TOPIC_COLORSHIFTER,
	GLITCH_COLORSHIFTER,
	GLITCH_COLORSHIFTER_FREQUENCY,
	GLITCH_COLORSHIFTER_TYPE,
	GLITCH_COLORSHIFTER_THRESHOLD,
	GLITCH_COLORSHIFTER_AMOUNT,
	GLITCH_TOPIC_COLORSHIFTER_END,

	GLITCH_TOPIC_COLOR,
	GLITCH_COLOR,
	GLITCH_COLOR_FREQUENCY,
	GLITCH_COLOR_RED,
	GLITCH_COLOR_GREEN,
	GLITCH_COLOR_BLUE,
	GLITCH_COLOR_BLACK,
	GLITCH_COLOR_WHITE,
	GLITCH_COLOR_ALPHA,
	GLITCH_COLOR_USE_CUSTOMCOLOR,
	GLITCH_COLOR_CUSTOMCOLOR,
	GLITCH_COLOR_UNMIX,
	GLITCH_COLOR_MIX,
	GLITCH_TOPIC_COLOR_END,

	GLITCH_TOPIC_SHIFTER,
	GLITCH_SHIFTER,
	GLITCH_SHIFTER_FREQUENCY,
	GLITCH_SHIFTER_DIRECTION,
	GLITCH_SHIFTER_BORDERTYPE,
	GLITCH_SHIFTER_MAX,
	GLITCH_TOPIC_SHIFTER_END,

	GLITCH_TOPIC_LAYER,
	GLITCH_LAYER,
	GLITCH_LAYER_FREQUENCY,
	GLITCH_LAYER_SOURCE,
	GLITCH_LAYER_SOURCE_POSITION,
	GLITCH_TOPIC_LAYER_END,

	GLITCH_TOPIC_FREEZER,
	GLITCH_FREEZER,
	GLITCH_FREEZER_FREQUENCY,
	GLITCH_FREEZER_TIME,
	GLITCH_TOPIC_FREEZER_END,

	GLITCH_NUM_PARAMS
};

typedef struct ParamInfo
{
	int scanline;
	int scanlineHeight;
	int scanlinePositionY;
	int areaCount;
	int areaGlitchCount;
	bool randomHeight;
	bool compose;
	int opacity;
	int seedControl;
	int areaSeed;

	bool colorShifter;
	unsigned long colorShifterFrequency;
	int colorShifterType;
	int colorShifterThreshold;
	int colorShifterAmount;

	bool color;
	unsigned long colorFrequency;
	bool colorRed;
	bool colorGreen;
	bool colorBlue;
	bool colorBlack;
	bool colorWhite;
	bool colorAlpha;
	bool colorUseCustomColor;
	PF_UnionablePixel colorCustomColor;
	bool colorUnmix;
	int colorMix;

	bool shifter;
	unsigned long shifterFrequency;
	int shifterDirection;
	int shifterBorderType;
	double shifterMax;

	bool layer;
	unsigned long layerFrequency;
	int layerSourcePosition;
	PF_LayerDef layerSource;

} ParamInfo;

#ifdef __cplusplus
extern "C" {
#endif

	DllExport	PF_Err
		EntryPointFunc(
		PF_Cmd			cmd,
		PF_InData		*in_data,
		PF_OutData		*out_data,
		PF_ParamDef		*params[],
		PF_LayerDef		*output,
		void			*extra);

#ifdef __cplusplus
}
#endif

#endif // GLITCH_H
