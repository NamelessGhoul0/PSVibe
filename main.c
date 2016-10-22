/*
	PSVibe
	Copyright (C) 2016, DragonSnake

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vita2d.h>

#define WHITE	RGBA8(255, 255, 255, 255)
#define RED		RGBA8(255, 0, 0, 255)

int main() {
	vita2d_pgf *pgf;
	SceCtrlData ctrl;
	SceCtrlActuator vibe;
	SceCtrlPortInfo ctrl_info;
	int small_strength = 0;
	int large_strength = 0;
	int max_range = 255;
	float small_percentage;
	float large_percentage;

	// init
	vita2d_init();
	vita2d_set_clear_color(RGBA8(32, 32, 32, 255)); // dark gray
	pgf = vita2d_load_default_pgf();

	int model;
	model = sceKernelGetModel();

	// check model to ensure this is running on a PSTV
	if (model != SCE_KERNEL_MODEL_VITATV) {
		// render text
		vita2d_start_drawing();
		vita2d_clear_screen();
		vita2d_pgf_draw_textf(pgf, 20, 40, WHITE, 1.0f, "PSVibe: PSTV edition v2 by DragonSnake");

		vita2d_pgf_draw_textf(pgf, 20, 80, WHITE, 1.0f, "Follow @DragonSnake0 on Twitter for more");

		vita2d_pgf_draw_textf(pgf, 20, 120, RED, 1.0f, "Error: PSVita detected!");
		vita2d_pgf_draw_textf(pgf, 20, 140, RED, 1.0f, "PSVibe will only run on PSTV / VitaTV consoles");
		vita2d_pgf_draw_textf(pgf, 20, 160, RED, 1.0f, "PSVitas dont have any vibration mechanism");
		vita2d_pgf_draw_textf(pgf, 20, 180, RED, 1.0f, "sorry about that :(");

		vita2d_pgf_draw_textf(pgf, 20, 220, WHITE, 1.0f, "Exiting in 5 seconds...");
		vita2d_end_drawing();
		vita2d_swap_buffers();

		sceKernelDelayThread(5 * 1000 * 1000);
		vita2d_fini();
		// freeing pgf crashes for some reason
		//vita2d_free_pgf(pgf);
		sceKernelExitProcess(0);
	}

	// get controller type
	sceCtrlGetControllerPortInfo(&ctrl_info);

	while (1) {
		sceCtrlPeekBufferPositive(0, &ctrl, 1);

		// check for exit combo
		if (ctrl.buttons == (SCE_CTRL_START | SCE_CTRL_SELECT)) {
			vita2d_fini();
			// freeing pgf crashes for some reason
			//vita2d_free_pgf(pgf);
			sceKernelExitProcess(0);
		}

		// start render
		vita2d_start_drawing();
		vita2d_clear_screen();

		// set motor strength
		switch (ctrl_info.port[1]) {
			case SCE_CTRL_TYPE_DS3:
				if (ctrl.buttons & SCE_CTRL_RIGHT)
					small_strength = 1;
				if (ctrl.buttons & SCE_CTRL_LEFT)
					small_strength = 0;
				if (ctrl.buttons & SCE_CTRL_UP)
					large_strength++;
				if (ctrl.buttons & SCE_CTRL_DOWN)
					large_strength--;

				// make sure strength doesnt go out of bounds
				// DS3 range is 0-1 for small motor & 40-255 for large motor
				if (large_strength == 0)
					large_strength = 40;
				if (large_strength < 40)
					large_strength++;
				if (large_strength > 255)
					large_strength--;

				// calculate the percentage of motor strength
				small_percentage = (float)small_strength / 1.0 * 100.0;
				vita2d_pgf_draw_textf(pgf, 20, 220, WHITE, 1.0f, "small motor strength = %.2f%%", small_percentage);

				large_percentage = (float)(large_strength - 40.0) / (max_range - 40.0) * 100.0;
				vita2d_pgf_draw_textf(pgf, 20, 240, WHITE, 1.0f, "large motor strength = %.2f%%", large_percentage);
				break;
			case SCE_CTRL_TYPE_DS4:
				if (ctrl.buttons & SCE_CTRL_RIGHT)
					small_strength++;
				if (ctrl.buttons & SCE_CTRL_LEFT)
					small_strength--;
				if (ctrl.buttons & SCE_CTRL_UP)
					large_strength++;
				if (ctrl.buttons & SCE_CTRL_DOWN)
					large_strength--;

				// make sure strength doesnt go out of bounds
				// DS4 range is 0 to 255 for both motors
				if (small_strength < 0)
					small_strength++;
				if (small_strength > 255)
					small_strength--;
				if (large_strength < 0)
					large_strength++;
				if (large_strength > 255)
					large_strength--;

				// calculate the percentage of motor strength
				small_percentage = (float)small_strength / max_range * 100.0;
				vita2d_pgf_draw_textf(pgf, 20, 220, WHITE, 1.0f, "small motor strength = %.2f%%", small_percentage);

				large_percentage = (float)large_strength / max_range * 100.0;
				vita2d_pgf_draw_textf(pgf, 20, 240, WHITE, 1.0f, "large motor strength = %.2f%%", large_percentage);
				break;
			default:
				break;
		}

		vita2d_pgf_draw_textf(pgf, 20, 40, WHITE, 1.0f, "PSVibe: PSTV edition v2 by DragonSnake");

		vita2d_pgf_draw_textf(pgf, 20, 80, WHITE, 1.0f, "Follow @DragonSnake0 on Twitter for more");

		vita2d_pgf_draw_textf(pgf, 20, 120, WHITE, 1.0f, "Press UP / DOWN to control large motor speed");
		vita2d_pgf_draw_textf(pgf, 20, 140, WHITE, 1.0f, "Press LEFT / RIGHT to control small motor speed");

		// display controller type on screen
		switch (ctrl_info.port[1]) {
			case SCE_CTRL_TYPE_DS3:
				vita2d_pgf_draw_textf(pgf, 20, 180, WHITE, 1.0f, "Detected controller type: DS3");
				break;
			case SCE_CTRL_TYPE_DS4:
				vita2d_pgf_draw_textf(pgf, 20, 180, WHITE, 1.0f, "Detected controller type: DS4");
		}

		vita2d_pgf_draw_textf(pgf, 20, 280, WHITE, 1.0f, "Press START + SELECT to exit");

		// end render
		vita2d_end_drawing();
		vita2d_swap_buffers();

		// setup actuator
		vibe.small = small_strength;
		vibe.large = large_strength;

		// vibrate!
		sceCtrlSetActuator(1, &vibe);
	}
    return 0;
}
