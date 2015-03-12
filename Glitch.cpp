#include "Glitch.h"
#include <stdlib.h>
#include <math.h>

#include "MT.h"

static PF_Err UpdateParameterUI(
	PF_InData *in_data,
	PF_OutData *out_data,
	PF_ParamDef *params[],
	PF_LayerDef *outputP);

static PF_Err About(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
	AEGP_SuiteHandler suites(in_data->pica_basicP);

	suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
		"%s v%d.%d\r%s",
		STR(StrID_Name),
		MAJOR_VERSION,
		MINOR_VERSION,
		STR(StrID_Description));

	ShellExecute(NULL, "open", "http://syuilo.com", NULL, NULL, SW_SHOW);

	return PF_Err_NONE;
}

static PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef	 *params[], PF_LayerDef *output)
{
	out_data->my_version = PF_VERSION(MAJOR_VERSION,
		MINOR_VERSION,
		BUG_VERSION,
		STAGE_VERSION,
		BUILD_VERSION);

	out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE |	// just 16bpc, not 32bpc
		PF_OutFlag_I_EXPAND_BUFFER |
		PF_OutFlag_I_HAVE_EXTERNAL_DEPENDENCIES |
		PF_OutFlag_SEND_UPDATE_PARAMS_UI;

	out_data->out_flags2 = PF_OutFlag2_SUPPORTS_QUERY_DYNAMIC_FLAGS;

	return PF_Err_NONE;
}

static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef	 *params[], PF_LayerDef *output)
{
	PF_Err		err = PF_Err_NONE;

	PF_ParamDef	def;

	// ------------------------
	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_POPUP("スキャンライン",
		2,
		1,
		"Random|Manual",
		1);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POINT("Scanline position",
		50,
		0,
		TRUE,
		2);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Scanline height",
		50,
		3);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("高さのばらつき",
		"ON",
		TRUE,
		0,
		4);

	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("ルート",
		1,
		32,
		1,
		8,
		3,
		5);

	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("再帰",
		1,
		32,
		1,
		8,
		3,
		6);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("合成",
		"ON",
		TRUE,
		0,
		7);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Glitch blend",
		0,
		8);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_POPUP("Seed control",
		2,
		1,
		"Automatic|Manual",
		9);

	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("Random seed",
		0,
		32768,
		0,
		1024,
		0,
		10);

	AEFX_CLR_STRUCT(def);
	PF_ADD_TOPIC("カラーシフタ",
		11);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("カラーシフタを使用",
		"Enabled",
		TRUE,
		0,
		12);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Frequency",
		40,
		13);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP("Type",
		4,
		4,
		"R - G, B (Rを中心にして、GとBに分離します)|G - R, B (Gを中心にして、RとBに分離します)|B - R, G (Bを中心にして、RとGに分離します)|Random",
		14);


	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("Threshold (しきい値)",
		0,
		255,
		0,
		255,
		0,
		15);

	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("Amount",
		2,
		64,
		2,
		32,
		8,
		16);

	AEFX_CLR_STRUCT(def);
	PF_END_TOPIC(17);

	AEFX_CLR_STRUCT(def);
	PF_ADD_TOPIC("カラーミキサ",
		18);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("カラーミキサを使用",
		"Enabled",
		TRUE,
		0,
		19);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Frequency",
		20,
		20);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("R信号",
		"ON",
		TRUE,
		0,
		21);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("G信号",
		"ON",
		TRUE,
		0,
		22);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("B信号",
		"ON",
		TRUE,
		0,
		23);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("Black信号",
		"ON",
		FALSE,
		0,
		24);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("White信号",
		"ON",
		FALSE,
		0,
		25);

	AEFX_CLR_STRUCT(def);
	PF_ADD_CHECKBOX("Alpha信号",
		"ON",
		FALSE,
		0,
		26);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("カスタムカラーを使用",
		"Enabled",
		FALSE,
		0,
		27);

	AEFX_CLR_STRUCT(def);
	PF_ADD_COLOR("カスタムカラー",
		255,
		0,
		0,
		28);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("ミックスではなく塗りつぶす",
		"ON",
		FALSE,
		0,
		29);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("シグナルの強さ",
		20,
		30);

	AEFX_CLR_STRUCT(def);
	PF_END_TOPIC(31);

	AEFX_CLR_STRUCT(def);
	PF_ADD_TOPIC("シフタ",
		32);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("シフタを使用",
		"Enabled",
		TRUE,
		0,
		33);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Frequency",
		50,
		34);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP("Direction",
		3,
		3,
		"Left|Right|Both",
		35);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP("Border",
		9,
		5,
		"None|Expand|Front|Mirror|Tile|Opposite mirrors|Scale‐up|Scale‐down|Render mirror",
		36);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Amount",
		5,
		37);

	AEFX_CLR_STRUCT(def);
	PF_END_TOPIC(38);

	AEFX_CLR_STRUCT(def);
	PF_ADD_TOPIC("レイヤーミックス",
		39);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("レイヤーミックスを使用",
		"Enabled",
		FALSE,
		0,
		40);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Frequency",
		30,
		41);

	AEFX_CLR_STRUCT(def);
	PF_ADD_LAYER("Source",
		PF_LayerDefault_NONE,
		42);

	AEFX_CLR_STRUCT(def);
	PF_ADD_POPUP("Source Position",
		2,
		1,
		"Source|Random",
		43);

	AEFX_CLR_STRUCT(def);
	PF_END_TOPIC(44);

	AEFX_CLR_STRUCT(def);
	PF_ADD_TOPIC("フリーザー",
		45);

	AEFX_CLR_STRUCT(def);
	def.flags = PF_ParamFlag_SUPERVISE;
	PF_ADD_CHECKBOX("フリーザーを使用",
		"Enabled",
		FALSE,
		0,
		46);

	AEFX_CLR_STRUCT(def);
	PF_ADD_PERCENT("Frequency",
		30,
		47);

	AEFX_CLR_STRUCT(def);
	PF_ADD_SLIDER("Time",
		1,
		1024,
		1,
		32,
		8,
		48);

	AEFX_CLR_STRUCT(def);
	PF_END_TOPIC(49);

	out_data->num_params = GLITCH_NUM_PARAMS;

	return err;
}

static ParamInfo initParamsInfo(PF_ParamDef *params[], PF_InData *in_data)
{
	ParamInfo niP;
	AEFX_CLR_STRUCT(niP);

	niP.scanline = params[GLITCH_SCANLINE]->u.pd.value;
	niP.scanlineHeight = FIX2INT(params[GLITCH_SCANLINE_HEIGHT]->u.sd.value);
	niP.scanlinePositionY = FIX2INT(params[GLITCH_SCANLINE_POSITION]->u.td.y_value);
	niP.areaCount = params[GLITCH_AREA_COUNT]->u.sd.value;
	niP.areaGlitchCount = params[GLITCH_AREA_GLITCH_COUNT]->u.sd.value;
	niP.randomHeight = params[GLITCH_RANDOM_HEIGHT]->u.bd.value;
	niP.compose = params[GLITCH_COMPOSE]->u.bd.value;
	niP.opacity = FIX2INT(params[GLITCH_OPACITY]->u.sd.value);
	niP.seedControl = params[GLITCH_SEED_CONTROLL]->u.pd.value;
	niP.areaSeed = params[GLITCH_AREA_SEED]->u.sd.value;

	niP.colorShifter = params[GLITCH_COLORSHIFTER]->u.bd.value;
	niP.colorShifterFrequency = FIX2INT(params[GLITCH_COLORSHIFTER_FREQUENCY]->u.pd.value);
	niP.colorShifterType = params[GLITCH_COLORSHIFTER_TYPE]->u.pd.value;
	niP.colorShifterThreshold = params[GLITCH_COLORSHIFTER_THRESHOLD]->u.sd.value;
	niP.colorShifterAmount = params[GLITCH_COLORSHIFTER_AMOUNT]->u.sd.value;

	niP.color = params[GLITCH_COLOR]->u.bd.value;
	niP.colorFrequency = FIX2INT(params[GLITCH_COLOR_FREQUENCY]->u.pd.value);
	niP.colorRed = params[GLITCH_COLOR_RED]->u.bd.value;
	niP.colorGreen = params[GLITCH_COLOR_GREEN]->u.bd.value;
	niP.colorBlue = params[GLITCH_COLOR_BLUE]->u.bd.value;
	niP.colorBlack = params[GLITCH_COLOR_BLACK]->u.bd.value;
	niP.colorWhite = params[GLITCH_COLOR_WHITE]->u.bd.value;
	niP.colorAlpha = params[GLITCH_COLOR_ALPHA]->u.bd.value;
	niP.colorUseCustomColor = params[GLITCH_COLOR_USE_CUSTOMCOLOR]->u.bd.value;
	niP.colorCustomColor = params[GLITCH_COLOR_CUSTOMCOLOR]->u.cd.value;
	niP.colorUnmix = params[GLITCH_COLOR_UNMIX]->u.bd.value;
	niP.colorMix = FIX2INT(params[GLITCH_COLOR_MIX]->u.sd.value);

	niP.shifter = params[GLITCH_SHIFTER]->u.bd.value;
	niP.shifterFrequency = FIX2INT(params[GLITCH_SHIFTER_FREQUENCY]->u.pd.value);
	niP.shifterDirection = params[GLITCH_SHIFTER_DIRECTION]->u.pd.value;
	niP.shifterBorderType = params[GLITCH_SHIFTER_BORDERTYPE]->u.pd.value;
	niP.shifterMax = FIX2INT(params[GLITCH_SHIFTER_MAX]->u.sd.value);

	niP.layer = params[GLITCH_LAYER]->u.bd.value;
	niP.layerFrequency = params[GLITCH_LAYER_FREQUENCY]->u.pd.value;
	//niP.layerSource = &params[GLITCH_LAYER_SOURCE]->u.ld;
	niP.layerSourcePosition = params[GLITCH_LAYER_SOURCE_POSITION]->u.pd.value;

	PF_ParamDef layer;

	PF_CHECKOUT_PARAM(in_data,
		GLITCH_LAYER_SOURCE,
		in_data->current_time,
		in_data->time_step,
		in_data->time_scale,
		&layer);

	niP.layerSource = layer.u.ld;

	return niP;
}

static PF_Err pSet(ParamInfo niP, PF_Pixel *data, A_long x, A_long y, A_long width, PF_Pixel pixel)
{
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;

	if (niP.opacity > 0) {
		A_long blend = (255 * niP.opacity / 100);

		if (pixel.red + blend < 255)
			pixel.red += blend;
		else
			pixel.red = 0;

		if (pixel.green + blend < 255)
			pixel.green += blend;
		else
			pixel.green = 0;

		if (pixel.blue + blend < 255)
			pixel.blue += blend;
		else
			pixel.blue = 0;
	}

	data[x + y * width] = pixel;
	return PF_Err_NONE;
}

static PF_Pixel pGet(PF_Pixel *data, A_long x, A_long y, A_long width)
{
	return data[x + y * width];
}

static PF_Err colorShifter(PF_Pixel *inData, PF_Pixel *outData, ParamInfo niP, A_long py, A_long height, A_long width, A_long inWidth, A_long outWidth)
{
	double w = niP.colorShifterAmount;

	int type = 0;

	if (niP.colorShifterType == 4)
		type = 1 + (rand() % 3);
	else
		type = niP.colorShifterType;

	for (A_long x = 0; x < width; x++) {
		for (A_long y = 0; y < height; y++) {
			A_long setx, sety;
			setx = x;
			sety = 1 + py + y;

			if ((sety >= 0) && (sety < (py + height))) {
				PF_Pixel sourcePixel;
				sourcePixel = pGet(inData, setx, sety, inWidth);

				for (int i = 1; i < w; i++) {
					PF_Pixel pixelA, pixelB;
					A_long lx = setx - i;
					A_long rx = setx + i + w;
					if (lx < 0)
						lx = 0;
					if (rx > width)
						rx = width;
					pixelA = pGet(inData, lx, sety, inWidth);
					pixelB = pGet(inData, rx, sety, inWidth);

					switch (type) {
						case 1:
							pixelA.blue = pGet(outData, lx, sety, outWidth).blue;

							if (sourcePixel.red > niP.colorShifterThreshold) {
								if (pixelA.green + sourcePixel.red < 255)
									pixelA.green += sourcePixel.red;
								else
									pixelA.green = 255;

								if (pixelB.blue + sourcePixel.red < 255)
									pixelB.blue += sourcePixel.red;
								else
									pixelB.blue = 255;
							}
							break;
						case 2:
							pixelA.blue = pGet(outData, lx, sety, outWidth).blue;

							if (sourcePixel.green > niP.colorShifterThreshold) {
								if (pixelA.red + sourcePixel.green < 255)
									pixelA.red += sourcePixel.green;
								else
									pixelA.red = 255;

								if (pixelB.blue + sourcePixel.green < 255)
									pixelB.blue += sourcePixel.green;
								else
									pixelB.blue = 255;
							}
							break;
						case 3:
							pixelA.green = pGet(outData, lx, sety, outWidth).green;

							if (sourcePixel.blue > niP.colorShifterThreshold) {
								if (pixelA.red + sourcePixel.blue < 255)
									pixelA.red += sourcePixel.blue;
								else
									pixelA.red = 255;

								if (pixelB.green + sourcePixel.blue < 255)
									pixelB.green += sourcePixel.blue;
								else
									pixelB.green = 255;
							}
							break;
					}

					pSet(niP, outData, lx, sety, outWidth, pixelA);
					pSet(niP, outData, rx, sety, outWidth, pixelB);
				}
			}
		}
	}

	return PF_Err_NONE;
}

static PF_Err color(PF_Pixel *data, ParamInfo niP, A_long py, A_long height, A_long width, A_long outWidth)
{
	/*
		Color ID:
		Alpha: -1
		Black: 0
		White: 1
		Red: 2
		Green: 3
		Blue: 4
		<Custom>: 5
		*/
	int useColors[7];
	int colorCount = 0;
	if (niP.colorAlpha) {
		useColors[colorCount] = -1;
		colorCount++;
	}
	if (niP.colorBlack) {
		useColors[colorCount] = 0;
		colorCount++;
	}
	if (niP.colorWhite) {
		useColors[colorCount] = 1;
		colorCount++;
	}
	if (niP.colorRed) {
		useColors[colorCount] = 2;
		colorCount++;
	}
	if (niP.colorGreen) {
		useColors[colorCount] = 3;
		colorCount++;
	}
	if (niP.colorBlue) {
		useColors[colorCount] = 4;
		colorCount++;
	}
	if (niP.colorUseCustomColor) {
		useColors[colorCount] = 5;
		colorCount++;
	}

	if (colorCount == 0)
		return PF_Err_NONE;

	int colorRandom = genrand_int32() % colorCount;
	int color = useColors[colorRandom];

	A_long mix;
	mix = (255 * niP.colorMix / 100);

	for (A_long x = 0; x < width; x++) {
		for (A_long y = 0; y < height; y++) {
			A_long setx, sety;
			setx = x;
			sety = py + y;

			if ((setx >= 0) && (setx < width) && (sety >= 0) && (sety < (py + height))) {
				PF_Pixel pixel;
				pixel = pGet(data, setx, sety, outWidth);
				if (!niP.colorUnmix)
					switch (color) {
						case -1:
							if (pixel.alpha + mix < 255)
								pixel.alpha += mix;
							else
								pixel.alpha = 255;
							break;
						case 0:
							if (pixel.red - mix > 0)
								pixel.red -= mix;
							else
								pixel.red = 0;
							if (pixel.green - mix > 0)
								pixel.green -= mix;
							else
								pixel.green = 0;
							if (pixel.blue - mix > 0)
								pixel.blue -= mix;
							else
								pixel.blue = 0;
							break;
						case 1:
							if (pixel.red + mix < 255)
								pixel.red += mix;
							else
								pixel.red = 255;
							if (pixel.green + mix < 255)
								pixel.green += mix;
							else
								pixel.green = 255;
							if (pixel.blue + mix < 255)
								pixel.blue += mix;
							else
								pixel.blue = 255;
							break;
						case 2:
							if (pixel.red + mix < 255)
								pixel.red += mix;
							else
								pixel.red = 255;
							break;
						case 3:
							if (pixel.green + mix < 255)
								pixel.green += mix;
							else
								pixel.green = 255;
							break;
						case 4:
							if (pixel.blue + mix < 255)
								pixel.blue += mix;
							else
								pixel.blue = 255;
							break;
						case 5:
							if (pixel.red + niP.colorCustomColor.red < 255)
								pixel.red += niP.colorCustomColor.red;
							else
								pixel.red = 255;
							if (pixel.green + niP.colorCustomColor.green < 255)
								pixel.green += niP.colorCustomColor.green;
							else
								pixel.green = 255;
							if (pixel.blue + niP.colorCustomColor.blue < 255)
								pixel.blue += niP.colorCustomColor.blue;
							else
								pixel.blue = 255;
							break;
				} else
					switch (color) {
						case -1:
							pixel.alpha = 255;
							break;
						case 0:
							pixel.red = 0;
							pixel.green = 0;
							pixel.blue = 0;
							break;
						case 1:
							pixel.red = 255;
							pixel.green = 255;
							pixel.blue = 255;
							break;
						case 2:
							pixel.red = 255;
							pixel.green = 0;
							pixel.blue = 0;
							break;
						case 3:
							pixel.red = 0;
							pixel.green = 255;
							pixel.blue = 0;
							break;
						case 4:
							pixel.red = 0;
							pixel.green = 0;
							pixel.blue = 255;
							break;
						case 5:
							pixel.red = niP.colorCustomColor.red;
							pixel.green = niP.colorCustomColor.green;
							pixel.blue = niP.colorCustomColor.blue;
							break;
				}
				pSet(niP, data, setx, sety, outWidth, pixel);
			}
		}
	}

	return PF_Err_NONE;
}

static PF_Err shifter(PF_Pixel *inData, PF_Pixel *outData, ParamInfo niP, A_long py, A_long height, A_long width, A_long inWidth, A_long outWidth)
{
	A_long ps;

	switch (niP.shifterDirection) {
		case 1: // Left
			ps = (A_long)(width * niP.shifterMax / 100);
			break;
		case 2: // Right
			ps = -((A_long)(width * niP.shifterMax / 100));
			break;
		case 3: // Both
			if ((genrand_int32() % 2) == 1)
				ps = (A_long)(width * niP.shifterMax / 100);
			else
				ps = -((A_long)(width * niP.shifterMax / 100));
			break;
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			A_long setx, sety;
			setx = (x + ps);
			sety = (py + y);

			PF_Pixel pixel;

			if ((setx >= 0) && (setx < width)) {
				pixel = pGet(inData, setx, sety, inWidth);
			} else {
				switch (niP.shifterBorderType) {
					case 1: // None
						pixel = pGet(inData, x, sety, inWidth);
						break;
					case 2: // Expand
						if (ps >= 0)
							pixel = pGet(inData, width - 1, sety, inWidth);
						else
							pixel = pGet(inData, 0, sety, inWidth);
						break;
					case 3: // Front
						if (ps >= 0)
							pixel = pGet(outData, 0, sety, outWidth);
						else
							pixel = pGet(outData, width, sety, outWidth);
						break;
					case 4: // Mirror
						if (ps >= 0)
							pixel = pGet(inData, x + (width - ps), sety, inWidth);
						else
							pixel = pGet(inData, x - (width + ps), sety, inWidth);
						break;
					case 5: // Tile
						if (ps >= 0)
							pixel = pGet(inData, x - (width - ps), sety, inWidth);
						else
							pixel = pGet(inData, x + (width + ps), sety, inWidth);
						break;
					case 6:// Opposite mirrors
						if (ps >= 0)
							pixel = pGet(outData, x + ps, sety, outWidth);
						else
							pixel = pGet(outData, x - ps, sety, outWidth);
						break;
					case 7: // Scale‐up
						if (ps >= 0)
							pixel = pGet(outData, x / 2, sety, outWidth);
						else
							pixel = pGet(outData, (-x) / (-2), sety, outWidth);
						break;
					case 8: // Scale‐down
						pixel = pGet(outData, x + x, sety, outWidth);
						break;
					case 9: // Render mirror
						pixel = pGet(outData, width - x, sety, outWidth);
						break;
				}
			}

			pSet(niP, outData, x, sety, outWidth, pixel);
		}
	}

	return PF_Err_NONE;
}

static PF_Err layerMix(PF_Pixel *inData, PF_Pixel *outData, ParamInfo niP, A_long py, A_long height, A_long width, A_long inWidth, A_long imageWidth, A_long imageHeight, A_long outWidth)
{
	PF_LayerDef source = niP.layerSource;

	if (source.data == NULL)
		return PF_Err_NONE;

	if (source.width <= 0)
		return PF_Err_NONE;

	if (source.height <= 0)
		return PF_Err_NONE;

	if (source.width != imageWidth || source.height != imageHeight)
		return PF_Err_NONE;

	A_long sourceWindth = source.width;
	A_long sourceHeight = source.height;

	int sourceY = (genrand_int32() % (sourceHeight - height));

	for (A_long x = 0; x < width; x++) {
		for (A_long y = 0; y < height; y++) {
			PF_Pixel pixel;
			switch (niP.layerSourcePosition) {
				case 1:
					pixel = pGet((PF_Pixel *)source.data, x, py + y, inWidth);
					break;
				case 2:
					pixel = pGet((PF_Pixel *)source.data, x, sourceY + y, inWidth);
					break;
			}
			pSet(niP, outData, x, py + y, outWidth, pixel);
		}
	}

	return PF_Err_NONE;
}

static PF_Err Render(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output)
{
	PF_Err err = PF_Err_NONE;
	AEGP_SuiteHandler	suites(in_data->pica_basicP);

	PF_EffectWorld *input = &params[GLITCH_INPUT]->u.ld;
	PF_Boolean is16Bit PF_WORLD_IS_DEEP(output);

	ParamInfo niP = initParamsInfo(params, in_data);
	if (niP.scanlineHeight <= 1)
		return err;

	A_long width = input->width;
	A_long height = input->height;

	A_long inWidth, outWidth;
	if (is16Bit == TRUE) {
		outWidth = output->rowbytes / sizeof(PF_Pixel16);
	} else {
		outWidth = output->rowbytes / sizeof(PF_Pixel);
		inWidth = input->rowbytes / sizeof(PF_Pixel);
	}

	PF_Pixel *outData = (PF_Pixel *)output->data;
	PF_Pixel *inData = (PF_Pixel *)input->data;

	if (niP.compose)
		PF_COPY(input, output, NULL, NULL);

	switch (niP.seedControl) {
		case 1:
			init_genrand(in_data->current_time / in_data->time_step);
			break;
		case 2:
			init_genrand(niP.areaSeed);
			break;
	}

	// 壊す数
	int a = 0;

	switch (niP.scanline) {
		case 1:
			a = (genrand_int32() % niP.areaCount);
			break;
		case 2:
			a = 1;
			break;
	}

	// Area
	for (int i = 0; i < a; i++) {
		int naibua = 0;
		int area_py = 0;
		int area_height = 0;

		naibua = 1 + (genrand_int32() % niP.areaGlitchCount);

		if (niP.randomHeight) {
			area_height = 2 + (genrand_int32() % ((height * niP.scanlineHeight / 100) - 3));
		} else {
			area_height = (height * niP.scanlineHeight / 100);
		}

		switch (niP.scanline) {
			case 1:
				if (height == area_height)
					area_py = 0;
				else
					area_py = (genrand_int32() % (height - area_height));
				break;
			case 2:
				area_py = niP.scanlinePositionY;
				break;
		}

		if (area_py >= height)
			area_py = height - 1;
		if (area_height + area_py >= height)
			area_height = (height - area_py);
		if (area_height + area_py >= height)
			area_py = height - area_height;

		// Glitch
		for (int j = 0; j < naibua; j++) {
			int glitch_height;
			int glitch_py;

			if (niP.randomHeight) {
				glitch_height = 0 + (genrand_int32() % area_height);
			} else {
				glitch_height = area_height - 1;
			}

			glitch_py = area_py + (genrand_int32() % (area_height - glitch_height));

			if (niP.layer && ((genrand_int32() % 100) <= niP.layerFrequency))
				layerMix(inData, outData, niP, glitch_py, glitch_height, width, inWidth, width, height, outWidth);

			if (niP.colorShifter && ((genrand_int32() % 100) <= niP.colorShifterFrequency))
				colorShifter(inData, outData, niP, glitch_py, glitch_height, width, inWidth, outWidth);

			//PF_COPY(output, input, NULL, NULL);

			if (niP.color && ((genrand_int32() % 100) <= niP.colorFrequency))
				color(outData, niP, glitch_py, glitch_height, width, outWidth);

			PF_COPY(output, input, NULL, NULL);

			if (niP.shifter && ((genrand_int32() % 100) <= niP.shifterFrequency))
				shifter(inData, outData, niP, glitch_py, glitch_height, width, inWidth, outWidth);
		}
	}

	return err;
}

static PF_Err MakeParamCopy(
	PF_ParamDef *actual[],	/* >> */
	PF_ParamDef copy[])		/* << */
{
	for (A_short iS = 0; iS < GLITCH_NUM_PARAMS; ++iS) {
		AEFX_CLR_STRUCT(copy[iS]); // clean params are important!
	}
	copy[GLITCH_SCANLINE] = *actual[GLITCH_SCANLINE];
	copy[GLITCH_SCANLINE_POSITION] = *actual[GLITCH_SCANLINE_POSITION];
	copy[GLITCH_SCANLINE_HEIGHT] = *actual[GLITCH_SCANLINE_HEIGHT];
	copy[GLITCH_RANDOM_HEIGHT] = *actual[GLITCH_RANDOM_HEIGHT];
	copy[GLITCH_AREA_COUNT] = *actual[GLITCH_AREA_COUNT];
	copy[GLITCH_AREA_GLITCH_COUNT] = *actual[GLITCH_AREA_GLITCH_COUNT];
	copy[GLITCH_COMPOSE] = *actual[GLITCH_COMPOSE];
	copy[GLITCH_OPACITY] = *actual[GLITCH_OPACITY];
	copy[GLITCH_SEED_CONTROLL] = *actual[GLITCH_SEED_CONTROLL];
	copy[GLITCH_AREA_SEED] = *actual[GLITCH_AREA_SEED];
	copy[GLITCH_TOPIC_COLORSHIFTER] = *actual[GLITCH_TOPIC_COLORSHIFTER];
	copy[GLITCH_COLORSHIFTER] = *actual[GLITCH_COLORSHIFTER];
	copy[GLITCH_COLORSHIFTER_FREQUENCY] = *actual[GLITCH_COLORSHIFTER_FREQUENCY];
	copy[GLITCH_COLORSHIFTER_TYPE] = *actual[GLITCH_COLORSHIFTER_TYPE];
	copy[GLITCH_COLORSHIFTER_THRESHOLD] = *actual[GLITCH_COLORSHIFTER_THRESHOLD];
	copy[GLITCH_COLORSHIFTER_AMOUNT] = *actual[GLITCH_COLORSHIFTER_AMOUNT];
	copy[GLITCH_TOPIC_COLORSHIFTER_END] = *actual[GLITCH_TOPIC_COLORSHIFTER_END];
	copy[GLITCH_TOPIC_COLOR] = *actual[GLITCH_TOPIC_COLOR];
	copy[GLITCH_COLOR] = *actual[GLITCH_COLOR];
	copy[GLITCH_COLOR_FREQUENCY] = *actual[GLITCH_COLOR_FREQUENCY];
	copy[GLITCH_COLOR_RED] = *actual[GLITCH_COLOR_RED];
	copy[GLITCH_COLOR_GREEN] = *actual[GLITCH_COLOR_GREEN];
	copy[GLITCH_COLOR_BLUE] = *actual[GLITCH_COLOR_BLUE];
	copy[GLITCH_COLOR_BLACK] = *actual[GLITCH_COLOR_BLACK];
	copy[GLITCH_COLOR_WHITE] = *actual[GLITCH_COLOR_WHITE];
	copy[GLITCH_COLOR_ALPHA] = *actual[GLITCH_COLOR_ALPHA];
	copy[GLITCH_COLOR_USE_CUSTOMCOLOR] = *actual[GLITCH_COLOR_USE_CUSTOMCOLOR];
	copy[GLITCH_COLOR_CUSTOMCOLOR] = *actual[GLITCH_COLOR_CUSTOMCOLOR];
	copy[GLITCH_COLOR_UNMIX] = *actual[GLITCH_COLOR_UNMIX];
	copy[GLITCH_COLOR_MIX] = *actual[GLITCH_COLOR_MIX];
	copy[GLITCH_TOPIC_COLOR_END] = *actual[GLITCH_TOPIC_COLOR_END];
	copy[GLITCH_TOPIC_SHIFTER] = *actual[GLITCH_TOPIC_SHIFTER];
	copy[GLITCH_SHIFTER] = *actual[GLITCH_SHIFTER];
	copy[GLITCH_SHIFTER_FREQUENCY] = *actual[GLITCH_SHIFTER_FREQUENCY];
	copy[GLITCH_SHIFTER_DIRECTION] = *actual[GLITCH_SHIFTER_DIRECTION];
	copy[GLITCH_SHIFTER_BORDERTYPE] = *actual[GLITCH_SHIFTER_BORDERTYPE];
	copy[GLITCH_SHIFTER_MAX] = *actual[GLITCH_SHIFTER_MAX];
	copy[GLITCH_TOPIC_SHIFTER_END] = *actual[GLITCH_TOPIC_SHIFTER_END];
	copy[GLITCH_TOPIC_LAYER] = *actual[GLITCH_TOPIC_LAYER];
	copy[GLITCH_LAYER] = *actual[GLITCH_LAYER];
	copy[GLITCH_LAYER_FREQUENCY] = *actual[GLITCH_LAYER_FREQUENCY];
	copy[GLITCH_LAYER_SOURCE] = *actual[GLITCH_LAYER_SOURCE];
	copy[GLITCH_LAYER_SOURCE_POSITION] = *actual[GLITCH_LAYER_SOURCE_POSITION];
	copy[GLITCH_TOPIC_LAYER_END] = *actual[GLITCH_TOPIC_LAYER_END];
	return PF_Err_NONE;
}

static PF_Err UpdateParameterUI(
	PF_InData *in_data,
	PF_OutData *out_data,
	PF_ParamDef *params[],
	PF_LayerDef *outputP)
{
	PF_Err err = PF_Err_NONE;
	AEGP_SuiteHandler suites(in_data->pica_basicP);
	ParamInfo niP = initParamsInfo(params, in_data);

	PF_ParamDef param_copy[GLITCH_NUM_PARAMS];
	ERR(MakeParamCopy(params, param_copy));

	switch (niP.scanline) {
		case 1:
			param_copy[GLITCH_SCANLINE_POSITION].ui_flags = PF_PUI_DISABLED;
			param_copy[GLITCH_AREA_COUNT].ui_flags = PF_PUI_CONTROL;
			break;
		case 2:
			param_copy[GLITCH_SCANLINE_POSITION].ui_flags = PF_PUI_CONTROL;
			param_copy[GLITCH_AREA_COUNT].ui_flags = PF_PUI_DISABLED;
			break;
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_SCANLINE_POSITION,
		&param_copy[GLITCH_SCANLINE_POSITION]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_AREA_COUNT,
		&param_copy[GLITCH_AREA_COUNT]));

	switch (niP.seedControl) {
		case 1:
			param_copy[GLITCH_AREA_SEED].ui_flags = PF_PUI_DISABLED;
			break;
		case 2:
			param_copy[GLITCH_AREA_SEED].ui_flags = PF_PUI_CONTROL;
			break;
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_AREA_SEED,
		&param_copy[GLITCH_AREA_SEED]));

	if (niP.colorShifter) {
		param_copy[GLITCH_COLORSHIFTER_AMOUNT].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLORSHIFTER_THRESHOLD].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLORSHIFTER_FREQUENCY].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLORSHIFTER_TYPE].ui_flags = PF_PUI_CONTROL;
	} else {
		param_copy[GLITCH_COLORSHIFTER_AMOUNT].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLORSHIFTER_THRESHOLD].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLORSHIFTER_FREQUENCY].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLORSHIFTER_TYPE].ui_flags = PF_PUI_DISABLED;
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLORSHIFTER_AMOUNT,
		&param_copy[GLITCH_COLORSHIFTER_AMOUNT]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLORSHIFTER_THRESHOLD,
		&param_copy[GLITCH_COLORSHIFTER_THRESHOLD]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLORSHIFTER_FREQUENCY,
		&param_copy[GLITCH_COLORSHIFTER_FREQUENCY]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLORSHIFTER_TYPE,
		&param_copy[GLITCH_COLORSHIFTER_TYPE]));

	if (niP.color) {
		param_copy[GLITCH_COLOR_FREQUENCY].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_ALPHA].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_BLACK].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_WHITE].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_RED].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_GREEN].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_BLUE].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_USE_CUSTOMCOLOR].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_CUSTOMCOLOR].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_MIX].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_COLOR_UNMIX].ui_flags = PF_PUI_CONTROL;
	} else {
		param_copy[GLITCH_COLOR_FREQUENCY].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_ALPHA].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_BLACK].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_WHITE].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_RED].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_GREEN].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_BLUE].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_USE_CUSTOMCOLOR].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_CUSTOMCOLOR].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_MIX].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_COLOR_UNMIX].ui_flags = PF_PUI_DISABLED;
	}

	if (niP.colorUseCustomColor) {
		if (niP.color) {
			param_copy[GLITCH_COLOR_CUSTOMCOLOR].ui_flags = PF_PUI_CONTROL;
		} else {
			param_copy[GLITCH_COLOR_CUSTOMCOLOR].ui_flags = PF_PUI_DISABLED;
		}
	} else {
		param_copy[GLITCH_COLOR_CUSTOMCOLOR].ui_flags = PF_PUI_DISABLED;
	}

	if (niP.colorUnmix) {
		param_copy[GLITCH_COLOR_MIX].ui_flags = PF_PUI_DISABLED;
	} else {
		if (niP.color) {
			param_copy[GLITCH_COLOR_MIX].ui_flags = PF_PUI_CONTROL;
		} else {
			param_copy[GLITCH_COLOR_MIX].ui_flags = PF_PUI_DISABLED;
		}
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_FREQUENCY,
		&param_copy[GLITCH_COLOR_FREQUENCY]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_ALPHA,
		&param_copy[GLITCH_COLOR_ALPHA]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_BLACK,
		&param_copy[GLITCH_COLOR_BLACK]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_WHITE,
		&param_copy[GLITCH_COLOR_WHITE]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_RED,
		&param_copy[GLITCH_COLOR_RED]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_RED,
		&param_copy[GLITCH_COLOR_RED]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_GREEN,
		&param_copy[GLITCH_COLOR_GREEN]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_BLUE,
		&param_copy[GLITCH_COLOR_BLUE]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_USE_CUSTOMCOLOR,
		&param_copy[GLITCH_COLOR_USE_CUSTOMCOLOR]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_CUSTOMCOLOR,
		&param_copy[GLITCH_COLOR_CUSTOMCOLOR]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_UNMIX,
		&param_copy[GLITCH_COLOR_UNMIX]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_COLOR_MIX,
		&param_copy[GLITCH_COLOR_MIX]));

	if (niP.shifter) {
		param_copy[GLITCH_SHIFTER_FREQUENCY].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_SHIFTER_BORDERTYPE].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_SHIFTER_DIRECTION].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_SHIFTER_MAX].ui_flags = PF_PUI_CONTROL;
	} else {
		param_copy[GLITCH_SHIFTER_FREQUENCY].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_SHIFTER_BORDERTYPE].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_SHIFTER_DIRECTION].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_SHIFTER_MAX].ui_flags = PF_PUI_DISABLED;
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_SHIFTER_FREQUENCY,
		&param_copy[GLITCH_SHIFTER_FREQUENCY]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_SHIFTER_BORDERTYPE,
		&param_copy[GLITCH_SHIFTER_BORDERTYPE]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_SHIFTER_DIRECTION,
		&param_copy[GLITCH_SHIFTER_DIRECTION]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_SHIFTER_MAX,
		&param_copy[GLITCH_SHIFTER_MAX]));

	if (niP.layer) {
		param_copy[GLITCH_LAYER_FREQUENCY].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_LAYER_SOURCE].ui_flags = PF_PUI_CONTROL;
		param_copy[GLITCH_LAYER_SOURCE_POSITION].ui_flags = PF_PUI_CONTROL;
	} else {
		param_copy[GLITCH_LAYER_FREQUENCY].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_LAYER_SOURCE].ui_flags = PF_PUI_DISABLED;
		param_copy[GLITCH_LAYER_SOURCE_POSITION].ui_flags = PF_PUI_DISABLED;
	}

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_LAYER_FREQUENCY,
		&param_copy[GLITCH_LAYER_FREQUENCY]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_LAYER_SOURCE,
		&param_copy[GLITCH_LAYER_SOURCE]));

	ERR(suites.ParamUtilsSuite1()->PF_UpdateParamUI(in_data->effect_ref,
		GLITCH_LAYER_SOURCE_POSITION,
		&param_copy[GLITCH_LAYER_SOURCE_POSITION]));

	out_data->out_flags |=
		PF_OutFlag_REFRESH_UI |
		PF_OutFlag_FORCE_RERENDER;

	return PF_Err_NONE;
}

DllExport
PF_Err EntryPointFunc(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output, void *extra)
{
	PF_Err		err = PF_Err_NONE;

	try {
		switch (cmd) {
			case PF_Cmd_ABOUT:
				err = About(in_data, out_data, params, output);
				break;

			case PF_Cmd_GLOBAL_SETUP:
				err = GlobalSetup(in_data, out_data, params, output);
				break;

			case PF_Cmd_PARAMS_SETUP:
				err = ParamsSetup(in_data, out_data, params, output);
				break;

			case PF_Cmd_SEQUENCE_SETUP:
				/*err = UpdateParameterUI(in_data,
					out_data,
					params,
					output);*/
				break;

			case PF_Cmd_USER_CHANGED_PARAM:
				err = UpdateParameterUI(in_data,
					out_data,
					params,
					output);
				break;

			case PF_Cmd_UPDATE_PARAMS_UI:
				err = UpdateParameterUI(in_data,
					out_data,
					params,
					output);
				break;

			case PF_Cmd_RENDER:
				err = Render(in_data, out_data, params, output);
				break;
		}
	} catch (PF_Err &thrown_err) {
		//err = thrown_err;
	}
	return err;
}
