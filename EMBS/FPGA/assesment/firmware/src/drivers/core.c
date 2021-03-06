#include "core.h"
#include "fsl.h"

extern u8 distances[12][12];


void send_to_core(struct world_state * state) {
	int i, j;

	putfslx(state->width, 0, FSL_DEFAULT);
	putfslx(state->height, 0, FSL_DEFAULT);
	putfslx(state->waypoints_count, 0, FSL_DEFAULT);
	putfslx(state->ignores_walls, 0, FSL_DEFAULT);

	for (i = 0; i < state->waypoints_count; i++) {
		putfslx(state->waypoints[i].x, 0, FSL_DEFAULT);
		putfslx(state->waypoints[i].y, 0, FSL_DEFAULT);
	}

	for (i = 0; i < 12 - state->waypoints_count; i++) {
		putfslx(0, 0, FSL_DEFAULT);
		putfslx(0, 0, FSL_DEFAULT);
	}

	for (i = 0; i < state->walls_count; i++) {
		putfslx(state->walls[i].x, 0, FSL_DEFAULT);
		putfslx(state->walls[i].y, 0, FSL_DEFAULT);
		putfslx(state->walls[i].direction, 0, FSL_DEFAULT);
		putfslx(state->walls[i].length, 0, FSL_DEFAULT);
	}

	for (i = 0; i < 60 - state->walls_count; i++) {
		putfslx(0, 0, FSL_DEFAULT);
		putfslx(0, 0, FSL_DEFAULT);
		putfslx(0, 0, FSL_DEFAULT);
		putfslx(0, 0, FSL_DEFAULT);


	}
}

void get_from_core(struct world_state* state) {
	int i, j, t, data;

	for (t = 0; t < state->waypoints_count; t++) {

		getfslx(data, 0, FSL_DEFAULT);
		//don't really care about start waypoint's x coord, but it seemingly fixes an error in the ip core

		getfslx(data, 0, FSL_DEFAULT);
		//don't really care about start waypoint's y coord, but it seemingly fixes an error in the ip core


		for (i = 0; i < state->height; i++) {
			for (j = 0; j < state->width; j++) {
				getfslx(state->paths[t][i][j], 0, FSL_DEFAULT); // read and transpose
			}
		}

		if(state->size == WORLD_SIZE_SMALL) {
			xil_printf("\n\rDistance matrix for %2d:\n\r", t);

			for (i = 0; i < state->width; i++) {
				for (j = 0; j < state->height; j++) {
					xil_printf("%3d ", state->paths[t][j][i]);
				}

				xil_printf("\n\r");
			}
		}
	}

	if(state->size != WORLD_SIZE_SMALL) {
		xil_printf("\n\rDistance matrices recieved, but not displayed.\n\r");
	}

	xil_printf("\n-------------------");
	for (t = 0; t < state->waypoints_count; t++) {
		xil_printf("\n\rDistances for %2d: ", t);
		for (i = 0; i < state->waypoints_count; i++) {
			getfslx(distances[t][i], 0, FSL_DEFAULT);

			xil_printf("%3d ", distances[t][i]);
		}
	}
	xil_printf("\n\r\n-------------------\n\r\nWaiting for shortest path\n\r\n");

	getfslx(state->shortest_path, 0, FSL_DEFAULT);

	xil_printf("Found shortest path %d.\n\rRoute waypoints: ", state->shortest_path);

	for (t = 0; t < state->waypoints_count; t++) {
		getfslx(state->route[t], 0, FSL_DEFAULT);

		xil_printf("%d ", state->route[t]);
	}
	xil_printf("\n\r\n-------------------\n\r\n");



}
