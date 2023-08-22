#pragma once

#define BOARD_MAX_NUM 8
#define STAGE_MAX_NUM 4

bool isStage[STAGE_MAX_NUM];

bool obstacles_stage_1[BOARD_MAX_NUM][BOARD_MAX_NUM];
bool obstacles_stage_2[BOARD_MAX_NUM][BOARD_MAX_NUM];
bool obstacles_stage_3[BOARD_MAX_NUM][BOARD_MAX_NUM];
bool obstacles_stage_4[BOARD_MAX_NUM][BOARD_MAX_NUM];
bool obstacles_stage_test[BOARD_MAX_NUM][BOARD_MAX_NUM];

void AddObstacleStageFirst()
{
	obstacles_stage_1[3][3] = true;
	obstacles_stage_1[3][5] = true;
	obstacles_stage_1[6][2] = true;
	obstacles_stage_1[5][6] = true;
}

void AddObstacleStageSecond()
{
	obstacles_stage_2[3][6] = true;
	obstacles_stage_2[5][1] = true;
	obstacles_stage_2[6][5] = true;
	obstacles_stage_2[0][7] = true;
	obstacles_stage_2[1][0] = true;
}

void AddObstacleStageThird()
{
	obstacles_stage_3[1][0] = true;
	obstacles_stage_3[2][5] = true;
	obstacles_stage_3[3][2] = true;
	obstacles_stage_3[3][6] = true;
	obstacles_stage_3[6][7] = true;
	obstacles_stage_3[7][4] = true;
}

void AddObstacleStageFourth()
{
	obstacles_stage_4[0][7] = true;
	obstacles_stage_4[1][0] = true;
	obstacles_stage_4[2][6] = true;
	obstacles_stage_4[3][3] = true;
	obstacles_stage_4[5][4] = true;
	obstacles_stage_4[6][1] = true;
	obstacles_stage_4[7][3] = true;
	obstacles_stage_4[7][5] = true;
}