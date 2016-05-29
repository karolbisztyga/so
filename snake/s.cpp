#include <iostream>
#include <string>
#include <bitset>
#include <math.h>

const int screen_dimension = 8;
const int snake_length = 4;

void generate_image(
	int** square_screen,
	int screen_dimension,
	int snake_shift,
	int snake_length);
void clearImage(int** square_screen, int screen_dimension);
void printScreen(int** square_screen, int screen_dimension);
void manage_snake_animation(
	int** screen,
	int screen_size,
	int snake_start_length,
	int snake_growth_step_delay);

int** create_screen(int screen_size);
void delete_screen(int** screen, int screen_size);

char screen_row_to_char(char *row, int screen_size)
{
	char row_as_char = 0;

	for (int i = 0; i < screen_size; ++i)
	{
		if (row[i] == 1) {
			// TODO
			row_as_char = 0;
		}
	}

	return row_as_char;
}

int main() 
{
	int** screen = create_screen(screen_dimension);
	clearImage(screen, screen_dimension);
	
	manage_snake_animation(screen, screen_dimension, 3, 4);

	int a;
	std::cin >> a;

	delete_screen(screen, screen_dimension);
}

enum animation_phase {
	FORWARD, BACKWARD
};

void change_animation_phase(animation_phase &phase)
{
	switch (phase) {
	case FORWARD:
		phase = BACKWARD;
		return;
	case BACKWARD:
		phase = FORWARD;
	}
}

void change_snake_shift(int &shift, animation_phase phase)
{
	switch (phase) {
	case FORWARD:
		++shift;
		return;
	case BACKWARD:
		--shift;
	}
}

void manage_snake_animation(
	int** screen, 
	int screen_size, 
	int snake_start_length, 
	int snake_growth_step_delay)
{
	int snake_head_position = 0;

	int screen_resolution = pow(screen_size, 2);

	int snake_shift = -snake_start_length;
	int snake_length = snake_start_length;

	animation_phase phase = FORWARD;

	for (int time = 1; time <= 170; ++time) {
		if (time % snake_growth_step_delay == 0
			&& snake_head_position > 0 
			&& snake_head_position < screen_resolution) {
			++snake_length;
			--snake_shift;
		}

		change_snake_shift(snake_shift, phase);
		change_snake_shift(snake_head_position, phase);

		generate_image(screen, screen_size, snake_shift, snake_length);
		printScreen(screen, screen_size);
		std::cout << "\n";

		//if (snake_shift != 0 && snake_shift % screen_resolution == 0) {
		if ((phase == FORWARD && snake_shift == screen_resolution)
				|| (phase == BACKWARD && snake_shift == -snake_length)) {
			change_animation_phase(phase);
			snake_length = snake_start_length;
			if (phase == FORWARD) {
				snake_shift = -snake_start_length;
				snake_head_position = 0;
			}
		}
	}
}

enum screen_side {
	top = 0, right, bottom, left
};

void change_side(screen_side &current_screen_side) 
{
	if (current_screen_side == left) {
		current_screen_side = top;
	}
	else {
		current_screen_side = (screen_side)(current_screen_side + 1);
	}
}

struct screen_position {
	int row;
	int col;
};

screen_position get_initial_position(int screen_dimension, screen_side side, int step)
{
	screen_position initial_position = { 0, 0 };
	int current_side_length = screen_dimension - (2 * step) - 1;

	switch (side)
	{
	case top:
		initial_position.row = step;
		initial_position.col = step;
		break;
	case right:
		initial_position.row = step;
		initial_position.col = screen_dimension - step - 1;
		break;
	case bottom:
		initial_position.row = screen_dimension - step - 1;
		initial_position.col = screen_dimension - step - 1;
		break;
	case left:
		initial_position.row = screen_dimension - step - 1;
		initial_position.col = step;
		break;
	}

	return initial_position;
}

int manage_side(int** square_screen, int screen_dimension, screen_side side, int step, int snake_to_draw, int shift_to_draw)
{
	screen_position current_position = get_initial_position(screen_dimension, side, step);
	int current_side_length = screen_dimension - (2 * step) - 1;

	int i = 0;
	for (; i < current_side_length; ++i) {
		if (snake_to_draw == 0) {
			break;
		}

		square_screen[current_position.row][current_position.col] = shift_to_draw > 0 ? 0 : 1;

		switch (side)
		{
		case top:
			++current_position.col;
			break;
		case right:
			++current_position.row;
			break;
		case bottom:
			--current_position.col;
			break;
		case left:
			--current_position.row;
			break;
		}

		if (shift_to_draw > 0) {
			--shift_to_draw;
		}
		else {
			--snake_to_draw;
		}
	}

	return i;
}

void overflow_below_zero_subtraction(int &from, int &value)
{
	int overflow = 0;

	from -= value;
	
	if (from < 0) {
		overflow = -from;
		from = 0;
	}

	value = overflow;
}

void generate_image(
	int** square_screen, 
	int screen_dimension, 
	int snake_shift, 
	int snake_length)
{
	int snake_to_draw = snake_shift < 0 ? snake_length + snake_shift : snake_length;
	int shift_to_draw = snake_shift < 0 ? 0 : snake_shift;

	int drawn = 0;

	screen_side current_side = top;

	int currentRow = 0,
		currentCol = 0;

	int steps_count = screen_dimension / 2;

	clearImage(square_screen, screen_dimension);
	for (int step = 0; step < steps_count; ++step) {
		if (snake_to_draw == 0) {
			break;
		}

		//for each side:
		for (int side_step = 0; side_step < 4; ++side_step) {
			if (snake_to_draw == 0) {
				break;
			}

			drawn = manage_side(
				square_screen,
				screen_dimension,
				current_side,
				step,
				snake_to_draw,
				shift_to_draw);

			overflow_below_zero_subtraction(shift_to_draw, drawn);
			overflow_below_zero_subtraction(snake_to_draw, drawn);

			change_side(current_side);
		}
	}
}

void clearImage(int** square_screen, int screen_dimension)
{
	for (int i = 0; i < screen_dimension; ++i) {
		for (int j = 0; j < screen_dimension; ++j) {
			square_screen[i][j] = 0;
		}
	}
}

void printScreen(int** square_screen, int screen_dimension)
{
	std::string separator = " ";

	for (int i = 0; i < screen_dimension; ++i) {
		for (int j = 0; j < screen_dimension; ++j) {
			std::cout << square_screen[i][j] << (j != screen_dimension - 1 ? separator : "");
		}
		std::cout << std::endl;
	}
}

int** create_screen(int screen_size)
{
	int** screen = new int*[screen_size];
	for (int i = 0; i < screen_size; ++i) {
		screen[i] = new int[screen_size];
	}

	return screen;
}

void delete_screen(int** screen, int screen_size)
{
	for (int i = 0; i < screen_size; ++i) {
		delete screen[i];
	}
	delete screen;
}