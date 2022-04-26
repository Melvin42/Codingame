#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct  s_cells
{
	int         index;
	int         richness;
	int         neigh_0;
	int         neigh_1;
	int         neigh_2;
	int         neigh_3;
	int         neigh_4;
	int         neigh_5;
}               t_cells;

typedef struct  s_trees
{
	int         cell_index;
	int         size;
	bool        is_mine;
	bool        is_dormant;
}               t_trees;

typedef struct  s_possible_moves
{
	char        action[10];
	int         tree;
	int         seed;
	int         tree_around;
	int         my_tree_around;
	int         score;
}               t_possible_moves;

typedef struct s_shadow
{
	int *shadow_now;
	int *shadow_next1;
	int *shadow_next2;
	int *shadow_next3;
	int *shadow_next4;
	int *shadow_next5;
}               t_shadow;

typedef struct  s_flags
{
	int         end;
	int         good_seed;
	int         good_grow;
	int         stop_plant;
	int         stop_grow_little;
	int         stop_grow_moyen;
	int         num_seed;
	int         num_size_one;
	int         num_size_two;
	int         num_size_three;
	int         num_size_three_ennemy;
	int         num_tree_green;
	int         num_tree_yellow;
	int         num_tree_orange;
	int         total_is_mine;
}               t_flags;

int    complete_tree_end(t_trees *trees, int num, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, int sun, int nutrients)
{
	int complete = -1;
	if (flags->end == 1)
		return (0);
	int i = -1;
	while (++i < num)
	{
		if (trees[i].is_mine == 1 && trees[i].size == 3 && complete == -1)
		{
			if ((trees[i].cell_index >= 19 && trees[i].cell_index <= 36) && nutrients == 1)// && ((sun + 1) % 3 != 0))
				continue;
			if ((trees[i].cell_index >= 19 && trees[i].cell_index <= 36) && nutrients == 0)
				continue;
			complete = i;
		}
	}
	if (complete != -1)
	{
		printf("COMPLETE %d\n", trees[complete].cell_index);
		flags->end = 1;
		return (1);
	}
	return (0);
}

int     set_shadow_value(t_trees *trees, int i, t_shadow *shadow)
{
	int shadow_value = 0;

	//    if (shadow->shadow_now[trees[i].cell_index] == 3)
	//        shadow_value++;
	if (shadow->shadow_next1[trees[i].cell_index] == 3)
		shadow_value++;
	if (shadow->shadow_next2[trees[i].cell_index] == 3)
		shadow_value++;
	if (shadow->shadow_next3[trees[i].cell_index] == 3)
		shadow_value++;
	/*
	   if (shadow->shadow_next4[trees[i].cell_index] == 3)
	   shadow_value++;
	   if (shadow->shadow_next5[trees[i].cell_index] == 3)
	   shadow_value++;
	   */
	return (shadow_value);
}

int    sun_next_turn(t_trees *trees, int number_of_trees, t_shadow *shadow, int sun)
{
	int total;
	int i;

	total = 0;
	i = -1;
	while (++i < number_of_trees)
	{
		if (trees[i].is_mine == 1 && shadow->shadow_next1[trees[i].cell_index] > trees[i].size)
			total += trees[i].size;
	}
	return (total + sun);
}

int    complete_tree(t_trees *trees, int num, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, int sun, int day, t_shadow *shadow)
{
	int complete = -1;
	int shadow_value;
	if (flags->end == 1 || day < 11)
		return (0);
	int i = -1;
	while (++i < num)
	{
		shadow_value = set_shadow_value(trees, i, shadow);
		if (trees[i].is_mine == 1 && trees[i].size == 3
				&& shadow->shadow_next1[trees[i].cell_index] == 3)
			//   && trees[complete].cell_index <= trees[i].cell_index)
		{
			complete = i;
			break;
		}
		/*
		   if (trees[i].is_mine == 1 && trees[i].size == 3 && flags->num_size_three >= 6)
		   {
		   complete = i;
		   break;
		   }
		   */
	}
	if (complete != -1)
	{
		if (day < 17 && (sun_next_turn(trees, num, shadow, sun) - 4) <= 10)
			return (0);
		i = -1;
		while (++i < number_of_possible_moves)
		{
			if (move[i].action[0] == 'C' && (move[i].tree == trees[complete].cell_index))
			{
				printf("COMPLETE %d\n", trees[complete].cell_index);
				flags->end = 1;
				return (1);
			}
		}
	}
	return (0);
}

int    grow_tree(t_trees *trees, int num, int size, int min, int max, t_flags *flags, t_possible_moves *move, int number_of_possible_moves)
{
	if (flags->end == 1)
		return (0);
	int i = -1;
	int j;
	while (++i < num)
	{
		j = -1;
		if (trees[i].is_mine == 1 && trees[i].size == size && (trees[i].cell_index >= min && trees[i].cell_index <= max))
		{
			while (++j < number_of_possible_moves)
			{
				if (move[j].action[0] == 'G' && (move[j].tree == trees[i].cell_index))
				{                    
					printf("GROW %d\n", trees[i].cell_index);
					flags->end = 1;
					return (1);
				}
			}
		}
	}
	return (0);
}

int    grow_print_all(t_trees *trees, int num, int size, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, t_shadow *shadow)
{
	flags->good_seed = -1;
	if (flags->end == 1)
		return (0);
	int i = -1;
	int j;
	while (++i < num)
	{
		j = -1;
		if (trees[i].is_mine == 1 && trees[i].size == size)
		{
			while (++j < number_of_possible_moves)
			{
				if (move[j].action[0] == 'G' && (move[j].tree == trees[i].cell_index))
				{
					if (flags->good_grow == -1)
						flags->good_grow = i;
					else
					{                
						int flag = 0; 
						// a finir
						if (shadow->shadow_next1[move[i].tree] == trees[i].size - 1 && flag == 0)
						{
							flags->good_grow = i;
							flag = 1;
						}
						printf("GROW %d\n", trees[i].cell_index);
						flags->end = 1;
						return (1);
					}
				}
			}
		}
	}
	return (0);
}

int    grow_print_all_one(t_trees *trees, int num, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, t_shadow *shadow)
{
	int grow = -1;
	if (flags->end == 1)
		return (0);
	int i = -1;
	while (++i < num)
	{
		if (trees[i].is_mine == 1 && trees[i].size == 1
				&& shadow->shadow_next1[trees[i].cell_index] == 1)
			// && trees[grow].cell_index <= trees[i].cell_index)
		{
			grow = i;
			break;
		}
	}
	if (grow != -1)
	{
		i = -1;
		while (++i < number_of_possible_moves)
		{
			if (move[i].action[0] == 'G' && move[i].tree == trees[grow].cell_index)
			{
				printf("GROW %d\n", trees[grow].cell_index);
				flags->end = 1;
				return (1);
			}
		}
	}
	return (0);
}

int    grow_print_all_two(t_trees *trees, int num, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, t_shadow *shadow)
{
	int grow = -1;
	if (flags->end == 1)
		return (0);
	int i = -1;
	while (++i < num)
	{
		if (trees[i].is_mine == 1 && trees[i].size == 2
				&& shadow->shadow_next1[trees[i].cell_index] == 2)
			//   && trees[grow].cell_index <= trees[i].cell_index)
		{
			grow = i;
			break;
		}
	}
	if (grow != -1)
	{
		i = -1;
		while (++i < number_of_possible_moves)
		{
			if (move[i].action[0] == 'G' && move[i].tree == trees[grow].cell_index)
			{
				printf("GROW %d\n", trees[grow].cell_index);
				flags->end = 1;
				return (1);
			}
		}
	}
	return (0);
}

int     grow_all(t_trees *trees, int number_of_trees, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, int sun, int day, t_shadow *shadow)
{
	if (flags->end == 1 || day == 23)
		return (0);
	if (flags->stop_grow_little == 0 && flags->num_size_two + 3 < 6)
	{
		if (grow_print_all_one(trees, number_of_trees, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
		if (grow_print_all(trees, number_of_trees, 1, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
	}
	if (flags->stop_grow_moyen == 0)
	{
		if (grow_print_all_two(trees, number_of_trees, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
		if (grow_print_all(trees, number_of_trees, 2, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
	}
	if (flags->stop_grow_little == 0)
	{    
		if (grow_print_all(trees, number_of_trees, 0, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
	}
	if (flags->stop_grow_little == 0)
	{
		if (grow_print_all_one(trees, number_of_trees, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
		if (grow_print_all(trees, number_of_trees, 1, flags, move, number_of_possible_moves, shadow) == 1)
			return (0);
	}
	return (0);
}

int    grow_orange(t_trees *trees, int number_of_trees, t_flags *flags, t_possible_moves *move, int number_of_possible_moves, int sun, int day)
{
	if (flags->end == 1)
		return (0);
	if (flags->stop_grow_moyen == 0)
	{
		if (grow_tree(trees, number_of_trees, 2, 18, 36, flags, move, number_of_possible_moves) == 1)
			return (0);
	}
	if (flags->stop_grow_little == 0 && flags->num_size_two + 3 < 6)
	{
		if (grow_tree(trees, number_of_trees, 1, 18, 36, flags, move, number_of_possible_moves) == 1)
			return (0);
	}
	if (flags->stop_grow_little == 0 && flags->num_size_one == 0)
	{    
		if (grow_tree(trees, number_of_trees, 0, 18, 36, flags, move, number_of_possible_moves) == 1)
			return (0);
	}
	return(0);
}

void    check_dist_two(t_possible_moves *move, t_trees *trees, int number_of_trees, t_cells *cell, int i, int neigh)
{
	int j = -1;

	move[i].tree_around++;
	while (++j < number_of_trees)
	{
		if (trees[j].cell_index == cell[neigh].neigh_0)
			move[i].tree_around++;
		if (trees[j].cell_index == cell[neigh].neigh_1)
			move[i].tree_around++;
		if (trees[j].cell_index == cell[neigh].neigh_2)
			move[i].tree_around++;
		if (trees[j].cell_index == cell[neigh].neigh_3)
			move[i].tree_around++;
		if (trees[j].cell_index == cell[neigh].neigh_4)
			move[i].tree_around++;
		if (trees[j].cell_index == cell[neigh].neigh_5)
			move[i].tree_around++;
	}
}

void    check_dist_one(t_possible_moves *move, t_trees *trees, int number_of_trees, t_cells *cell, int i, int neigh)
{
	int j = -1;
	move[i].tree_around++;
	while (++j < number_of_trees)
	{
		if (trees[j].cell_index == cell[neigh].neigh_0)
			move[i].tree_around++;
		//check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_0].index);
		if (trees[j].cell_index == cell[neigh].neigh_1)
			move[i].tree_around++;
		// check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_1].index);
		if (trees[j].cell_index == cell[neigh].neigh_2)
			move[i].tree_around++;
		// check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_2].index);
		if (trees[j].cell_index == cell[neigh].neigh_3)
			move[i].tree_around++;
		// check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_3].index);
		if (trees[j].cell_index == cell[neigh].neigh_4)
			move[i].tree_around++;
		// check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_4].index);
		if (trees[j].cell_index == cell[neigh].neigh_5)
			move[i].tree_around++;
		// check_dist_two(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_5].index);
	}
}

int check_seed_diag(t_possible_moves *move, int score[2], t_cells *cell, int i, t_flags *flags, t_shadow *shadow)
{
	int tab[37][6] = {{10, 12, 14, 16, 18, 8}, {9, 3, 5, 17, 36, 20}, {23, 11, 4, 6, 7, 21}, {24 ,26, 13, 5, 1, 9}, {11, 27, 29, 15, 6, 2}, 
		{3, 13, 30, 32, 17, 1}, {2, 4, 15, 33, 35, 7}, {21, 2, 6, 35, -1, -1}, {22, 10, 0, 18, 19, -1}, {-1, 24, 3, 1, 20, -1},
		{-1, 25, 12, 0, 8, 22}, {-1, -1, 27, 4, 2, 23}, {25, -1, 28, 14, 0, 10}, {26, -1, -1, 30, 5, 3}, {12, 28, -1, 31, 16, 0},
		{4, 29, -1, -1, 33, 6}, {0, 14, 31, -1, 34, 18}, {1, 5, 32, -1, -1, 36}, {8, 0, 16, 34, -1, 19}, {-1, 8, 18, -1, -1, -1},
		{-1, 9, 1, 36, -1, -1}, {-1, 23, 2, 7, -1, -1}, {-1, -1, 10, 8, -1, -1}, {-1, -1, 11, 2, 21, -1}, {-1, -1, 26, 3, 9, -1},
		{-1, -1, -1, 12, 10, -1}, {-1, -1, -1, 13, 3, 24}, {-1, -1, -1, 29, 4, 11}, {-1, -1, -1, -1, 14, 12}, {27, -1, -1, -1, 15, 4},
		{13, -1, -1, -1, 32, 5}, {14, -1, -1, -1, -1, 16}, {5, 30, -1, -1, -1, 17}, {6, 15, -1, -1, -1, 35}, {18, 16, -1, -1, -1, -1},
		{7, 6, 33, -1, -1, -1}, {20, 1, 17, -1, -1, -1}};
	int j = -1;
	while (++j < 6)
	{
		if (tab[move[i].tree][j] == move[i].seed)
		{
			if (flags->good_seed == -1)
				flags->good_seed = i;
			else
			{
				int flag = 0;

				if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
						&& shadow->shadow_next1[move[i].seed] == 0 
						&& shadow->shadow_next2[move[i].seed] == 0
						&& shadow->shadow_next3[move[i].seed] == 0 && flag == 0)
					// && move[i].tree_around <= move[flags->good_seed].tree_around)
				{
					flags->good_seed = i;
					flag = 1;
				}
				if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
						&& shadow->shadow_next1[move[i].seed] == 0 
						&& shadow->shadow_next2[move[i].seed] == 0 && flag == 0)
					//&& move[i].tree_around <= move[flags->good_seed].tree_around)
				{
					flags->good_seed = i;
					flag = 1;
				}
				if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
						&& shadow->shadow_next1[move[i].seed] == 0 && flag == 0
						&& move[i].tree_around <= move[flags->good_seed].tree_around)
				{
					flags->good_seed = i;
					flag = 1;
				}
				if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
						&& move[i].tree_around <= move[flags->good_seed].tree_around && flag == 0)
				{
					flags->good_seed = i;
					flag = 1;
				}
			}
		}
	}
	return (0);
}

int    plant_seeds(t_possible_moves *move, int number_of_possible_moves, t_flags *flags, t_trees *trees, int number_of_trees, t_cells *cell, t_shadow *shadow)
{
	if (flags->stop_plant == 1 || flags->end == 1)
		return (0);
	int i = -1;
	int j;
	while (++i < number_of_possible_moves)
		if (move[i].action[0] == 'S')
		{
			j = -1;
			while (++j < number_of_trees)
			{
				if (trees[j].cell_index == cell[move[i].seed].neigh_0)
					move[i].tree_around++;
				//   check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_0].index);
				if (trees[j].cell_index == cell[move[i].seed].neigh_1)
					move[i].tree_around++;
				//   check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_1].index);
				if (trees[j].cell_index == cell[move[i].seed].neigh_2)
					move[i].tree_around++;
				//   check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_2].index);
				if (trees[j].cell_index == cell[move[i].seed].neigh_3)
					move[i].tree_around++;
				//  check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_3].index);
				if (trees[j].cell_index == cell[move[i].seed].neigh_4)
					move[i].tree_around++;
				//  check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_4].index);
				if (trees[j].cell_index == cell[move[i].seed].neigh_5)
					move[i].tree_around++;
				//  check_dist_one(move, trees, number_of_trees, cell, i, cell[cell[move[i].seed].neigh_5].index);
			}
		}    
	i = -1;
	while (++i < number_of_possible_moves)
		if (move[i].action[0] == 'S')
		{
			j = -1;
			while (++j < number_of_trees)
			{
				if (trees[j].cell_index == cell[move[i].seed].neigh_0 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
				if (trees[j].cell_index == cell[move[i].seed].neigh_1 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
				if (trees[j].cell_index == cell[move[i].seed].neigh_2 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
				if (trees[j].cell_index == cell[move[i].seed].neigh_3 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
				if (trees[j].cell_index == cell[move[i].seed].neigh_4 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
				if (trees[j].cell_index == cell[move[i].seed].neigh_5 && trees[j].is_mine == 1)
					move[i].my_tree_around++;
			}
		}
	int score[2] = {37, -1};
	flags->good_seed = -1;
	i = -1;
	while (++i < number_of_possible_moves)
	{
		if (move[i].action[0] == 'S')
		{
			if (score[1] == -1)
			{
				score[0] = move[i].tree_around;
				check_seed_diag(move, score, cell, i, flags, shadow);
				score[1] = flags->good_seed;
			}
			else
			{
				score[0] = move[i].tree_around;
				check_seed_diag(move, score, cell, i, flags, shadow);
				score[1] = flags->good_seed;
			}
		}
	}
	if (move[score[1]].action[0] == 'S' && score[1] != -1)
	{
		if (move[score[1]].my_tree_around == 0)
		{
			printf("%s %d %d\n", move[score[1]].action, move[score[1]].tree, move[score[1]].seed);
			flags->end = 1;
		}
	}
	flags->good_seed = -1;
	if (flags->end == 0)
	{
		i = -1;
		while (++i < number_of_possible_moves)
		{
			if (move[i].action[0] == 'S' && move[i].my_tree_around == 0)
			{

				if (flags->good_seed == -1)
					flags->good_seed = i;
				else
				{
					int flag = 0;
					if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
							&& shadow->shadow_next1[move[i].seed] == 0 
							&& shadow->shadow_next2[move[i].seed] == 0
							&& shadow->shadow_next3[move[i].seed] == 0 && flag == 0)
						//   && move[i].tree_around <= move[flags->good_seed].tree_around)
					{
						flags->good_seed = i;
						flag = 1;
					}
					if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
							&& shadow->shadow_next1[move[i].seed] == 0 
							&& shadow->shadow_next2[move[i].seed] == 0 && flag == 0)
						//   && move[i].tree_around <= move[flags->good_seed].tree_around)
					{
						flags->good_seed = i;
						flag = 1;
					}
					if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
							&& shadow->shadow_next1[move[i].seed] == 0 && flag == 0
							&& move[i].tree_around <= move[flags->good_seed].tree_around)
					{
						flags->good_seed = i;
						flag = 1;
					}
					if (cell[move[flags->good_seed].seed].richness <= cell[move[i].seed].richness
							&& move[i].tree_around <= move[flags->good_seed].tree_around && flag == 0)
					{
						flags->good_seed = i;
						flag = 1;
					}
				}

			}
		}
		if (flags->good_seed != -1)
		{
			printf("%s %d %d\n", move[flags->good_seed].action, move[flags->good_seed].tree, move[flags->good_seed].seed);
			flags->end = 1;
			return (0);
		}
	}
	return (0);
}

int *ft_sun_dir_all(t_trees *trees, int number_of_trees, t_cells *cell, int tab[10][7])
{
	int *shadow_tab;
	shadow_tab = NULL;
	if (shadow_tab)
		free(shadow_tab);
	if (!(shadow_tab = malloc(sizeof(int) * 37)))
		return (NULL);
	int i = -1;
	int j;
	int k;
	while (++i < 7)
	{
		j = -1;
		while (++j < 7)
		{
			k = -1;
			while (++k < number_of_trees)
			{
				if (trees[k].cell_index == tab[i][j])
				{
					int count = trees[k].size + 1;
					while (--count > 0)
						shadow_tab[tab[i + count][j]] = trees[k].size;
				}
			}
		}
	}
	return (shadow_tab);
}

int shadow_calc(t_shadow *shadow, t_trees *trees, int number_of_trees, t_cells *cell, int sun, int day)
{
	int tab_sun_dir0[10][7] = {{25, 26, 27, 28, 29, 30, 31}, {24, 11, 12, 13, 14, 15, 32}, {23, 10, 3, 4, 5, 16, 33}, {22, 9, 2, 0, 6, 17, 34},
		{-1, 21, 8, 1, 18, 35, -1}, {-1, -1, 20, 7, 36, -1, -1}, {-1, -1, -1, 19, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int tab_sun_dir1[10][7] = {{28, 29, 30, 31, 32, 33, 34}, {27, 13, 14, 15, 16, 17, 35}, {26, 12, 4, 5, 6, 18, 36}, {25, 11, 3, 0, 1, 7, 19},
		{-1, 24, 10, 2, 8, 20, -1},{-1, -1, 23, 9, 21, -1, -1},{-1, -1, -1, 22, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int tab_sun_dir2[10][7] = {{31, 32, 33, 34, 35, 36, 19}, {30, 15, 16, 17, 18, 7, 20}, {29, 14, 5, 6, 1, 8, 21}, {28, 13, 4, 0, 2, 9, 22},
		{-1, 27, 12, 3, 10, 23, -1}, {-1, -1, 26, 11, 24, -1, -1}, {-1, -1, -1, 25, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int tab_sun_dir3[10][7] = {{34, 35, 36, 19, 20, 21, 22}, {33, 17, 18, 7, 8, 9, 23}, {32, 16, 6, 1, 2, 10, 24}, {31, 15, 5, 0, 3, 11, 25},
		{-1, 30, 14, 4, 12, 26, -1}, {-1, -1, 29, 13, 27, -1, -1}, {-1, -1, -1, 28, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int tab_sun_dir4[10][7] = {{19, 20, 21, 22, 23, 24, 25}, {36, 7, 8, 9, 10, 11, 26}, {35, 18, 1, 2, 3, 12, 27}, {34, 17, 6, 0, 4, 13, 28},
		{-1, 33, 16, 5, 14, 29, -1}, {-1, -1 , 32, 15, 30, -1, -1}, {-1, -1, -1, 31, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int tab_sun_dir5[10][7] = {{28, 27, 26, 25, 24, 22}, {29, 13, 12, 11, 10, 9, 21}, {30, 14, 4, 3, 2, 8, 20}, {31, 15, 5, 0, 1, 7, 19},
		{-1, 32, 16, 6, 18, 36, -1}, {-1, -1, 33, 17, 35, -1, -1}, {-1, -1, -1, 34, -1, -1, -1},
		{-1, -1, -1, 1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1, -1}};
	int sun_dir_now = day % 6;
	int sun_dir_next1 = (day + 1) % 6;
	int sun_dir_next2 = (day + 2) % 6;
	int sun_dir_next3 = (day + 3) % 6;
	int sun_dir_next4 = (day + 4) % 6;
	int sun_dir_next5 = (day + 5) % 6;

	if (sun_dir_now == 0)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_now == 1)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_now == 2)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_now == 3)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_now == 4)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_now == 5)
		shadow->shadow_now = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	if (sun_dir_next1 == 0)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_next1 == 1)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_next1 == 2)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_next1 == 3)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_next1 == 4)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_next1 == 5)
		shadow->shadow_next1 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	if (sun_dir_next2 == 0)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_next2 == 1)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_next2 == 2)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_next2 == 3)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_next2 == 4)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_next2 == 5)
		shadow->shadow_next2 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	if (sun_dir_next3 == 0)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_next3 == 1)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_next3 == 2)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_next3 == 3)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_next3 == 4)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_next3 == 5)
		shadow->shadow_next3 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	if (sun_dir_next4 == 0)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_next4 == 1)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_next4 == 2)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_next4 == 3)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_next4 == 4)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_next4 == 5)
		shadow->shadow_next4 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	if (sun_dir_next5 == 0)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir0);
	else if (sun_dir_next5 == 1)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir1);
	else if (sun_dir_next5 == 2)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir2);
	else if (sun_dir_next5 == 3)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir3);
	else if (sun_dir_next5 == 4)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir4);
	else if (sun_dir_next5 == 5)
		shadow->shadow_next5 = ft_sun_dir_all(trees, number_of_trees, cell, tab_sun_dir5);
	return (0);
}

int    split_possible_moves(char *src, t_possible_moves *dst)
{
	int i = -1;

	while (isalpha(src[++i]))
		dst->action[i] = src[i];
	dst->action[i] = '\0';
	if (dst->action[0] == 'W')
		return (0);
	i++;
	dst->tree = atoi(&src[i]);
	if (dst->action[0] == 'G')
		return (0);
	if (dst->tree / 10 == 0)
		i += 2;
	else
		i += 3;
	dst->seed = atoi(&src[i]);
	return (0); 
}

void    calc_trees_num(t_flags *flags, t_trees *trees, int number_of_trees)
{
	int i = -1;
	while (++i < number_of_trees)
	{
		if (trees[i].is_mine == 1 && trees[i].size == 0)
			flags->num_seed++;
		if (trees[i].is_mine == 1 && trees[i].size == 1)
			flags->num_size_one++;
		if (trees[i].is_mine == 1 && trees[i].size == 2)
			flags->num_size_two++;
		if (trees[i].is_mine == 1 && trees[i].size == 3)
			flags->num_size_three++;
		if (trees[i].is_mine == 0 && trees[i].size == 3)
			flags->num_size_three_ennemy++;
	}
}

void    count_num_trees(t_flags *flags, t_trees *trees, int number_of_trees)
{
	int i = -1;
	while (++i < number_of_trees)
	{
		if (trees[i].cell_index <= 6 && trees[i].is_mine == 1)
			flags->num_tree_green++;
		else if ((trees[i].cell_index >= 7 && trees[i].cell_index <= 18) && trees[i].is_mine == 1)
			flags->num_tree_yellow++;
		else if (trees[i].cell_index >= 19 && trees[i].is_mine == 1)
			flags->num_tree_orange++;
	}
}

void    count_my_trees(t_flags *flags, t_trees *trees, int number_of_trees)
{
	int i = -1;

	while (++i < number_of_trees)
		if (trees[i].is_mine == 1)
			flags->total_is_mine++;
}

int main()
{
	t_cells     cell[37];
	t_flags     flags;
	t_shadow    shadow;
	int i;

	memset(&flags, 0, sizeof(flags));

	int number_of_cells;
	scanf("%d", &number_of_cells);
	for (int i = 0; i < number_of_cells; i++) {
		scanf("%d%d%d%d%d%d%d%d", &cell[i].index, &cell[i].richness, &cell[i].neigh_0,
				&cell[i].neigh_1, &cell[i].neigh_2, &cell[i].neigh_3,
				&cell[i].neigh_4, &cell[i].neigh_5);
	}
	int flag21 = 0;
	int flag_rush = 0;
	while (1)
	{
		/*
		   clock_t start, end;
		   start = clock();
		   end = clock();
		   fprintf(stderr, "%f\n", (end - start) * 1000.f / CLOCKS_PER_SEC);
		   */

		flags.end = 0;
		flags.num_seed = 0;
		flags.num_size_one = 0;
		flags.num_size_two = 0;
		flags.num_size_three = 0;
		flags.num_size_three_ennemy = 0;
		flags.num_tree_green = 0;
		flags.num_tree_yellow = 0;
		flags.num_tree_orange = 0;
		flags.total_is_mine = 0;
		int day;
		scanf("%d", &day);
		int nutrients;
		scanf("%d", &nutrients);
		int sun;
		int score;
		scanf("%d%d", &sun, &score);
		int opp_sun;
		int opp_score;
		bool opp_is_waiting;
		int _opp_is_waiting;
		scanf("%d%d%d", &opp_sun, &opp_score, &_opp_is_waiting);
		opp_is_waiting = _opp_is_waiting;
		int number_of_trees;
		scanf("%d", &number_of_trees);
		t_trees trees[number_of_trees];
		memset(trees, 0, sizeof(trees));
		for (int i = 0; i < number_of_trees; i++) {
			int _is_mine;
			int _is_dormant;
			scanf("%d%d%d%d", &trees[i].cell_index, &trees[i].size, &_is_mine, &_is_dormant);
			trees[i].is_mine = _is_mine;
			trees[i].is_dormant = _is_dormant;
		}
		calc_trees_num(&flags, trees, number_of_trees);
		count_num_trees(&flags, trees, number_of_trees);
		count_my_trees(&flags, trees, number_of_trees);
		int number_of_possible_moves;
		scanf("%d", &number_of_possible_moves); fgetc(stdin);
		t_possible_moves possible_moves[number_of_possible_moves];
		memset(possible_moves, 0, sizeof(possible_moves));
		for (int i = 0; i < number_of_possible_moves; i++) {
			char possible_move[32];
			scanf("%[^\n]", possible_move); fgetc(stdin);
			split_possible_moves(possible_move, &possible_moves[i]);
		}

		shadow_calc(&shadow, trees, number_of_trees, cell, sun, day);
		fprintf(stderr, "sun = %d\n", sun_next_turn(trees, number_of_trees, &shadow, sun));
		if (day == 0)
		{
			printf("WAIT\n");
			continue;
		}
		else if (day == 1)
		{
			grow_orange(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, day);
			if (flags.end == 0)
				printf("WAIT\n");
			continue;
		}
		else if (day == 2)
			grow_orange(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, day);
		fprintf(stderr, "mine %d ennemy %d\n", flags.num_size_three, flags.num_size_three_ennemy);
		/*
		   if (opp_score - score >= 20)
		   complete_tree_end(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, nutrients);
		   */
		/*
		   if (opp_score - score > 0)
		   {
		   complete_tree_end(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, nutrients);
		   }
		   */
		/*
		   if (nutrients <= 12 && opp_score -score <= 0)
		   flag_rush = 1;
		   if (flag_rush == 1)
		   complete_tree_end(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, nutrients);
		   */
		if (day == 21)
		{
			if (opp_score > score - nutrients) // ajouter number of tree et nurtient?
				flag21 = 1;
			if (flag21 == 1)
				complete_tree_end(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, nutrients);
			flags.stop_grow_little = 1;
		}
		if(day >= 22)
		{
			if (sun_next_turn(trees, number_of_trees, &shadow, sun) - (7 + flags.num_size_three) >= flags.num_size_three / 4)
				flags.stop_grow_moyen = 1;
			complete_tree_end(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, nutrients);
		}
		if (flags.num_size_three)
			complete_tree(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, day, &shadow);
		if (flags.end == 0 && flags.num_seed == 0 && flags.num_size_one < 2 && nutrients > 6)
			plant_seeds(possible_moves, number_of_possible_moves, &flags, trees, number_of_trees, cell, &shadow);
		if (flags.end == 0)
		{
			grow_all(trees, number_of_trees, &flags, possible_moves, number_of_possible_moves, sun, day, &shadow);
		}
		if (day == 23 && (opp_score + (opp_sun / 3) == (score + (sun / 3) - 1)))
			plant_seeds(possible_moves, number_of_possible_moves, &flags, trees, number_of_trees, cell, &shadow);
		if (flags.end == 0)
			printf("WAIT\n");
	}
	return 0;
}
