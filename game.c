#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct
{
    int blood;
    int strength;
    int coins;
    int level;
    int evasionRate;
} info;
// for monster

typedef struct
{
    int potion_of_swiftness;
    int totem_of_undying;
    int golden_apple;
    int potion_of_good_vision;
} inventory;
// inventory

typedef struct
{
    int default_blood;
    int blood;
    int strength;
    double hunger;
    int coins;
    int level;
    int rp;           // player row coordinate
    int cp;           // player column coordinate
    int rv;           // village row coordinate
    int cv;           // village column coordinate
    int boot;         // for calculating time of the day
    int day;          // what day it is
    inventory things; // like backpack
    inventory effect; // what effects are on me
} adventurer;
// for hero

typedef struct
{
    int blood;
    int strength;
    int level;
} village;
// for village

char **create_map(int *row, int *col)
{
    printf("Input the number of row and column for the map (suggestion square and 50*50 or up):\n");
    scanf("%d %d", row, col);
    char **arr = (char **)malloc(*row * sizeof(char *));
    for (int i = 0; i < *row; i++)
        arr[i] = (char *)malloc(*col * sizeof(char));

    for (int i = 0; i < *row; i++)
        for (int j = 0; j < *col; j++)
            arr[i][j] = '.';
    return arr;
}

int check_boundary(int *row, int *col, int r, int c) // row temp and col temp to see if input is in the correct range
{
    if (r >= *row || r < 0 || c >= *col || c < 0)
    {
        printf("the location is outside the map\n");
        return 1; // true
    }
    else
        return 0; // false
}

int check_availability(char **map, int *row, int *col, int r, int c)
{
    if (map[r][c] != '.') // check if occupied
    {
        printf("the location is occupied\n");
        return 1; // true
    }
    else
        return 0; // false
}

int comp_check_availability(char **map, int *row, int *col, int r, int c)
{
    if (map[r][c] != '.') // check if occupied
        return 1;         // true
    else
        return 0; // false
}

void print_map(char **arr, int *row, int *col, adventurer *hero, village *village)
{
    // village stat
    printf("\n\nYour village's stats:\n");
    printf("Blood: %d\tStrength: %d\tLevel: %d\n\n", village->blood, village->strength, village->level);

    // player stat
    printf("Your stats:\n");
    printf("Blood: %d\tStrength: %d\tHunger: %lf\tCoins: %d\tLevel: %d\n\n", hero->blood, hero->strength, hero->hunger, hero->coins, hero->level);

    // what's in your inventory
    printf("Your Inventory:\n");
    printf("Potion of  swiftness: %d\tTotem of undying: %d\tGolden apple: %d\tPotion of good vision: %d\n\n", hero->things.potion_of_swiftness, hero->things.totem_of_undying, hero->things.golden_apple, hero->things.potion_of_good_vision);

    // effects on you
    printf("Your effects:(1: means has effect, 0: means no effect)\n");
    printf("Potion of  swiftness: %d\tTotem of undying: %d\tPotion of good vision: %d\n\n", hero->effect.potion_of_swiftness, hero->effect.totem_of_undying, hero->effect.potion_of_good_vision);

    // show map
    printf("== MAP ==\n");
    for (int i = hero->rp - 2 - hero->effect.potion_of_good_vision; i <= hero->rp + 2 + hero->effect.potion_of_good_vision; i++)
    {
        for (int j = hero->cp - 2 - hero->effect.potion_of_good_vision; j <= hero->cp + 2 + hero->effect.potion_of_good_vision; j++)
        {
            if (i < 0 || i >= *row)
                continue;
            if (j < 0 || j >= *col)
                continue;
            if (arr[i][j] != 'p' && arr[i][j] != 'v' && arr[i][j] != 'd' && arr[i][j] != 'c' && arr[i][j] != 'm' && arr[i][j] != 'f' && arr[i][j] != 'h')
                printf("%c", '.');
            else
                printf("%c", arr[i][j]);
        }
        printf("\n");
    }
    // reminder
    printf("You are the center of the map\n");
    printf("== MAP ==\n\n");

    // coordinates
    printf("(1 indexed)Your coordinates are (%d, %d)\n", hero->cp + 1, hero->rp + 1);
    printf("(1 indexed)Your village's coordinates are (%d, %d)\n\n", hero->cv + 1, hero->rv + 1);

    // time
    printf("Time of the day: %d:00\n", 24 - (hero->boot / (hero->effect.potion_of_swiftness + 1)));
}

void setup_village(char **map, int *row, int *col, adventurer *hero) // input village coordinates
{
    printf("Input the row and column for the village location:\n");
    scanf("%d %d", &hero->rv, &hero->cv);
    if (check_boundary(row, col, hero->rv, hero->cv))
        setup_village(map, row, col, hero);
    map[hero->rv][hero->cv] = 'v';
}

void setup_monster(char **map, int *row, int *col) // input monster coordinates
{
    int monster_num = (*row) * (*col) * 1 / 8; // how many monsters
    printf("There are %d monsters in total.\n The location of the monsters are randomly generated.\n", monster_num);
    for (int i = 0; i < monster_num;) // check input and coordinates for monster(s)
    {
        int rowm, colm;                                         // monster row and monster column
        rowm = rand() % (*row);                                 // random generate row
        colm = rand() % (*col);                                 // random generate column
        if (comp_check_availability(map, row, col, rowm, colm)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowm][colm] = 'm'; // set coordinates to monster
    }
}

void setup_food(char **map, int *row, int *col) // input cow coordinates
{
    int food_num = (*row) * (*col) * 1 / 8; // how many chicken/cow
    printf("There are %d cows/food in total.\n The location of the cows are randomly generated.\n", food_num);
    for (int i = 0; i < food_num;)
    {
        int rowf, colf;                                         // food row and food column
        rowf = rand() % (*row);                                 // random generate row
        colf = rand() % (*col);                                 // random generate column
        if (comp_check_availability(map, row, col, rowf, colf)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowf][colf] = 'c'; // set coordinates to food
    }
}

void setup_dealer(char **map, int *row, int *col)
{
    int dealers_num = (*row) * (*col) * 1 / 12; // setup how many
    printf("There are %d dealers in total.\n The location of the dealers are randomly generated.\n", dealers_num);
    for (int i = 0; i < dealers_num;)
    {
        int rowd, cold;                                         // dealer row and dealer column
        rowd = rand() % (*row);                                 // random generate row
        cold = rand() % (*col);                                 // random generate column
        if (comp_check_availability(map, row, col, rowd, cold)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowd][cold] = 'd'; // set coordinates to dealer
    }
}

void setup_fast_travel(char **map, int *row, int *col)
{
    int fast_travel_num = (*row) * (*col) * 1 / 48; // how many fast travel stations
    printf("There are %d fast travel stations in total.\n The location of the stations are randomly generated.\n", fast_travel_num);
    for (int i = 0; i < fast_travel_num;)
    {
        int rowf, colf;                                         // fast travel row and fast travel col
        rowf = rand() % (*row);                                 // random genrate row;
        colf = rand() % (*col);                                 // random genrate col
        if (comp_check_availability(map, row, col, rowf, colf)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowf][colf] = 'f'; // set coordinates to fast travel stations
    }
}

void setup_clinic(char **map, int *row, int *col)
{
    int clinic_num = (*row) * (*col) * 1 / 48; // how many health stations
    printf("There are %d health stations in total.\n The location of the stations are randomly generated.\n", clinic_num);
    for (int i = 0; i < clinic_num;)
    {
        int rowh, colh;                                         // health row and health col
        rowh = rand() % (*row);                                 // random genrate row;
        colh = rand() % (*col);                                 // random genrate col
        if (comp_check_availability(map, row, col, rowh, colh)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowh][colh] = 'h'; // set coordinates to health stations
    }
}

void setup_gun(char **map, int *row, int *col)
{
    int gun_num = (*row) * (*col) / 24; // how many guns
    printf("There are %d guns in total.\n The location of the guns are randomly generated.\n", gun_num);
    for (int i = 0; i < gun_num;)
    {
        int rowg, colg;                                         // gun row and gun col
        rowg = rand() % (*row);                                 // random genrate row;
        colg = rand() % (*col);                                 // random genrate col
        if (comp_check_availability(map, row, col, rowg, colg)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowg][colg] = 'g'; // set coordinates to guns for computer but map will not show(hidden)
    }
}

void setup_shield(char **map, int *row, int *col)
{
    int shield_num = (*row) * (*col) / 24; // how many shields
    printf("There are %d shields in total.\n The location of the shields are randomly generated.\n", shield_num);
    for (int i = 0; i < shield_num;)
    {
        int rows, cols;                                         // shield row and shield col
        rows = rand() % (*row);                                 // random genrate row;
        cols = rand() % (*col);                                 // random genrate col
        if (comp_check_availability(map, row, col, rows, cols)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rows][cols] = 's'; // set coordinates to shields for computer but map will not show(hidden)
    }
}

void setup_trap(char **map, int *row, int *col)
{
    int trap_num = (*row) * (*col) / 24; // how many traps
    printf("There are %d traps in total.\n The location of the traps are randomly generated.\n", trap_num);
    for (int i = 0; i < trap_num;)
    {
        int rowt, colt;                                         // trap row and trap col
        rowt = rand() % (*row);                                 // random genrate row;
        colt = rand() % (*col);                                 // random genrate col
        if (comp_check_availability(map, row, col, rowt, colt)) // check if available, if not random again
            continue;
        i++;                   // put here so if continue 'i' will not plus 1
        map[rowt][colt] = 't'; // set coordinates to traps for computer but map will not show(hidden)
    }
}

int check_blood(adventurer *hero)
{
    // check if your blood = 0 and have totem of undying or not
    if (hero->blood <= 0 && hero->effect.totem_of_undying == 0) // don't have totem of undying
    {
        printf("You DEAD"); // dead
        return 1;
    }
    else if (hero->blood <= 0 && hero->effect.totem_of_undying == 1) // if you have totem of undying
    {
        hero->blood = 5;                                                            // give you five more blood
        printf("triggered totem of undying, now you have %d blood\n", hero->blood); // description
        hero->effect.totem_of_undying--;                                            // used effect
    }
    return 0;
}

int check_hunger(adventurer *hero)
{
    if (hero->hunger <= 0) // check if too hungry
    {
        hero->hunger = 0;
        printf("You are hungry! You will lose blood!\nPlease EAT!!!\n");
        hero->blood--; // lose blood
        return 1;
    }
    return 0;
}

int check_time(char **map, int *row, int *col, adventurer *hero, village *village)
{
    if (hero->boot == 0) // check if night time
    {
        free(map);                                                                     // free malloced map
        printf("It's a new day, things are respawned except your village and you.\n"); // new day
        map = create_map(row, col);                                                    // new map, same size
        map[hero->rv][hero->cv] = 'v';                                                 // set village coordinate to the same place
        map[hero->rp][hero->cp] = 'p';                                                 // player is sent back to village
        hero->effect.potion_of_good_vision = 0;                                        // no more effect for potion of good vision
        hero->effect.potion_of_swiftness = 0;                                          // no more effect for potion of swiftness
        int monster_count = 0;                                                         // count nearby monsters
        for (int i = hero->rp - 2 - hero->effect.potion_of_good_vision * 3; i <= hero->rp + 2 + hero->effect.potion_of_good_vision * 3; i++)
        {
            for (int j = hero->cp - 2 - hero->effect.potion_of_good_vision * 3; j <= hero->cp + 2 + hero->effect.potion_of_good_vision * 3; j++)
            {
                if (i < 0 || i >= *row)
                    continue;
                if (j < 0 || j >= *col)
                    continue;
                if (map[i][j] == 'm')
                    monster_count++; // if nearby monster ++
            }
        }

        village->blood -= monster_count * 10; // nearby monster attack
        village->level++;                     // village level +1
        hero->level++;                        // player level+1
        hero->day++;                          // new day
        return 1;
    }
    return 0;
}

int evasion(info *monster, adventurer *hero)
{
    int e = rand() % 101;          // randomize to compare with monster evasion rate
    if (e <= monster->evasionRate) // if e is smaller than the monster evades successfully
    {
        printf("monster has evaded\n");
        hero->blood = hero->blood - monster->level; // monster attack
        return 1;
    }
    return 0; // evasion unsuccessful
}

int battle_result(info *monster, adventurer *hero)
{
    srand(time(NULL)); // set random's seed
    do
    {
        monster->evasionRate = rand() % 101;                              // random monster's evasion rate (0-100)
        printf("monster's evasionRate is: %d%%\n", monster->evasionRate); // show monster evasion_rate
        if (hero->blood <= 0)                                             // if blood<=0, you dead
        {
            printf("You DEAD!\n");
            return 0;
        }
    } while (monster->level > 0 && evasion(monster, hero)); // if monster keeps evading then the monster can continue its attack

    // monster evade unsuccessfull
    if (monster->strength <= hero->strength) // if your attack(strength) >= monster blood(strength/blood/level all the same for monster) ==> you win
    {
        printf("You Win!\n");
        return 1;
    }
    else // otherwise you lose and monster will attack
    {
        printf("You Lose!\n");
        hero->blood -= monster->strength; // monster attack
        return 2;
    }
}

char encounter_monster(adventurer *hero)
{
    info monster = {0, 0, 0, 0, 0};                                                                                            // set monster stats
    monster.level = (rand() % 4 + 1);                                                                                          // random level 1-4
    monster.blood = monster.strength = (monster.level * hero->day);                                                            // set monster blood and strength
    monster.coins = monster.blood * 5;                                                                                         // monster coins
    printf("You are fighting Monster %d!\n", monster.strength);                                                                // show what monster you are fighting
    printf("Monster %d has %d blood, %d strength, %d coins\n", monster.level, monster.blood, monster.strength, monster.coins); // show monster stats
    printf("do you want to fight [0]No [1]Yes:\n");
    int decide; // decide to fight or not
    scanf("%d", &decide);
    if (decide) // decide yes
    {
        hero->hunger -= monster.level; // hunger - monsterlevel
        if (check_hunger(hero))        // see if hunger = 0,if yes then lose
            return 'l';
        if (check_blood(hero)) // see if blood = 0, if yes then dead
            return 'D';
        switch (battle_result(&monster, hero)) // battle
        {
        case 0: // you are dead (blood=0)
            return 'D';
        case 1:                           // you win
            hero->coins += monster.coins; // you get monster coins
            (hero->strength)++;           // your strength++
            return 'w';
        case 2: // you lose
            return 'l';
        }
    } // if decide not to fight, set to you lose
    return 'l';
}

char encounter_food(adventurer *hero)
{
    if (hero->hunger >= 20) // check if you are full already
    {
        printf("You are full. You don't need to eat.\n");
        return 'l';
    }
    (hero->hunger) += 2;
    if (hero->hunger > 20) // if add too full then hunger = 20
        hero->hunger = 20;
    printf("You have %lf hunger\n", hero->hunger); // show hunger
    return 'w';
}

int check_coins(adventurer *hero, int worth)
{
    if (hero->coins <= 0)
        hero->coins = 0;
    if (hero->coins < worth)
    {
        printf("Sorry you don't have enough money.\n");
        return 0;
    }
    return 1;
}

void in_village(adventurer *hero)
{
    int count_sword = 0;
    while (1)
    {
        printf("What do you want to do in the village (1:buy a magical sword; 2: go to the bar; 3.eat meal; 4:sell; 5.leave the village?):\n");
        info sword = {0, 1, 3, 0, 0}; // set sword with strength 1 and 3 coins
        info beer = {1, 0, 2, 0, 0};  // set beer with blood 1 and 2 coins
        int dowhat;                   // decide
        scanf("%d", &dowhat);
        switch (dowhat)
        {
        case 1:
            if (count_sword >= 1) // already bought once during this visit
            {
                printf("You already brought a sword.\n");
                break;
            }
            else
            {
                printf("The magical sword is %d coins\n", sword.coins); // show how much sword is
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, sword.coins)) // otherwise if yes then check if you have enough money to buy
                    break;
                hero->coins -= sword.coins;                                                                  // if yes then buy
                (hero->strength)++;                                                                          // strength+1
                printf("Nice sword! Now you have %d strength and %d coins!\n", hero->strength, hero->coins); // compliment
                count_sword++;                                                                               // bought sword 1 time
                break;
            }
        case 2:
            if ((hero->blood) >= hero->default_blood) // check if blood is full
            {
                printf("Your blood is full. You don't need a beer.\n");
                break;
            }
            else
            {
                printf("The magical beer is %d coins\n", beer.coins); // show how much one beer is
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, beer.coins)) // check if enough money to buy
                    break;
                hero->coins -= beer.coins;
                (hero->blood)++;                                                                      // blood +1
                printf("Nice beer! Now you have %d blood and %d coins!\n", hero->blood, hero->coins); // compiment
                break;
            }
        case 3:
            if ((hero->hunger) >= 20) // check if full
            {
                printf("You are full. You don't need a meal.\n");
                break;
            }
            else
            {
                int meal_coins = 5;                                    // one meal cost 5 coins
                printf("The magical feast is %d coins\n", meal_coins); // show how much
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, meal_coins)) // check if enough coins to buy
                    break;
                hero->coins -= meal_coins;
                (hero->hunger) += 5;   // hunger +5
                if (hero->hunger > 20) // add to full
                    hero->hunger = 20;
                printf("Nice meal! Now you have %d hunger and %d coins!\n", hero->hunger, hero->coins); // compiment
                break;
            }
        case 4:
            while (1)
            {
                printf("What do you want to sell?(1:sell potion of swiftness; 2: sell totem of undying; 3:sell golden apple; 4.sell potion of good vision; 5:leave)\n(Note: each item is sold for 5 coins no matter what)\n"); // ask what to sell
                int decide;
                scanf("%d", &decide);
                switch (decide)
                {
                case 1:                                        // sell potion of swiftness
                    if (hero->things.potion_of_swiftness == 0) // check if you have it
                    {
                        printf("You don't have it, stop scamming your village.\n");
                        break;
                    }
                    hero->things.potion_of_swiftness--;           // sold
                    hero->coins += 10;                            // add 10 coins
                    printf("Now you have %d coins", hero->coins); // show how many coins left
                    break;
                case 2:                                     // sell totem of undying
                    if (hero->things.totem_of_undying == 0) // check if you have it
                    {
                        printf("You don't have it, stop scamming your village.\n");
                        break;
                    }
                    hero->things.totem_of_undying--;              // sold
                    hero->coins += 10;                            // add 10 coins
                    printf("Now you have %d coins", hero->coins); // show how many coins left
                    break;
                case 3:                                 // sell golden apple
                    if (hero->things.golden_apple == 0) // check if you have it
                    {
                        printf("You don't have it, stop scamming your village.\n");
                        break;
                    }
                    hero->things.golden_apple--;                  // sold
                    hero->coins += 10;                            // add 10 coins
                    printf("Now you have %d coins", hero->coins); // show how many coins left
                    break;
                case 4:                                          // sell potion of good vision
                    if (hero->things.potion_of_good_vision == 0) // check if you have it
                    {
                        printf("You don't have it, stop scamming your village.\n");
                        break;
                    }
                    hero->things.potion_of_good_vision--;         // sold
                    hero->coins += 10;                            // add 10 coins
                    printf("Now you have %d coins", hero->coins); // show how many coins left
                    break;
                case 5:
                    printf("Thanks for selling\n"); // say bye bye
                    break;
                default: // invalid action
                    printf("Invalid action!\n");
                    break;
                }
                if (decide == 5)
                    break;
            }
        case 5: // leave
            printf("You are welcome to come back anytime!\n");
            return;
        default: // invalid action
            printf("Invalid action! 1:buy a magical sword; 2: go to the bar; 3.eat meal; 4:sale; 5.leave the village\n");
            break;
        }
    }
}

char encounter_station(adventurer *hero)
{
    printf("You are now in the fast travel station\n");                // description
    printf("Do you want to travel back to village? [0]No [1]Yes :\n"); // decide
    int decide;
    scanf("%d", &decide);
    if (decide) // decide yes
    {
        return 'f';
    }
    return 'l'; // decide no
}

char encounter_clinic(adventurer *hero)
{
    printf("You are now in the health station\n"); // description
    if (hero->blood >= hero->default_blood)        // check if blood is full
        printf("You are already healthy you don't need it");
    else               // if no
        hero->blood++; // add +1 blood
    return 'l';
}

void encounter_shield(adventurer *hero)
{
    if (hero->blood >= hero->default_blood) // check if healthy
    {
        printf("You are healthy. You don't need a shield\n");
        return;
    }

    (hero->blood)++;                            // get blood +1
    printf("You have %d blood\n", hero->blood); // show blood
    return;
}

void encounter_dealer(adventurer *hero)
{
    // count the times you buy something
    int count_one = 0;
    int count_two = 0;
    int count_three = 0;
    int count_four = 0;
    printf("Hello welcome to my shop. Hurry up before the cops catch me! I only sell the most unique things.\n"); // welcome
    while (1)
    {
        printf("What do you want to do (1:buy potion of swiftness; 2: buy totem of undying; 3:buy golden apple; 4.buy potion of good vision; 5.leave)\n"); // ask what to buy

        // how much
        int potion_of_swiftness = 25;
        int totem_of_undying = 55;
        int golden_apple = 50;
        int potion_of_good_vision = 30;

        // decide
        int dowhat;
        scanf("%d", &dowhat);
        switch (dowhat)
        {
        case 1:                 // buy potion of swiftness
            if (count_one >= 1) // check if you already bought once during the visit
            {
                printf("You already brought it.\n");
                break;
            }
            else
            {
                printf("The potion of swiftness is %d coins\n(last for 1 day)", potion_of_swiftness); // description
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, potion_of_swiftness)) // check if you have enough money
                    break;
                hero->coins -= potion_of_swiftness; // sold
                hero->things.potion_of_swiftness++;
                printf("Nice! Now you can 2 times speed, but only cost 1 hunger.\n"); // description
                count_one++;                                                          // count++
                break;
            }
        case 2:
            if (count_two >= 1) // check if you already bought once during the visit
            {
                printf("You already brought it.\n");
                break;
            }
            else
            {
                printf("The totem of undying is %d coins(Note: can only use one time, after buying it you have to hold/use it to have the effect)\n", totem_of_undying); // description
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, totem_of_undying)) // check if you have enough money
                    break;
                hero->coins -= totem_of_undying; // sold
                hero->things.totem_of_undying++;
                printf("Nice! Now if you are strike to death, you will have 5 more blood.\n"); // description
                count_two++;                                                                   // count++
                break;
            }
        case 3:
            if (count_three >= 1) // check if you already bought once during the visit
            {
                printf("You already brought it.\n");
                break;
            }
            else
            {
                printf("The golden apple is %d coins(Note: can only use one time)\n", golden_apple); // description
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, golden_apple)) // check if you have enough money
                    break;
                hero->coins -= golden_apple; // sold
                hero->things.golden_apple++;
                printf("Nice! It will give you five more blood when you eat.\n"); // description
                count_three++;                                                    // count++
                break;
            }
        case 4:
            if (count_four >= 1) // check if you already bought once during the visit
            {
                printf("You already brought it.\n");
                break;
            }
            else
            {
                printf("The potion of good vision is %d coins(Note: last for two days)\n", potion_of_good_vision); // description
                printf("Do you want to buy it? [0]No [1]Yes:\n");
                int decide; // decide to buy or not
                scanf("%d", &decide);
                if (!decide) // decide no
                    break;
                if (!check_coins(hero, potion_of_good_vision)) // check if you have enough money
                    break;
                hero->coins -= potion_of_good_vision; // sold
                hero->things.potion_of_good_vision++;
                printf("Nice! Now you can see more when you see the map\n"); // description
                count_four++;                                                // count++
                break;
            }
        case 5: // say farewell
            printf("For now, bye bye! Hope we meet again soon and hide those things well or you will be chased by the cops!\n");
            return;
        default: // Invalid action
            printf("Invalid action! 1:buy potion of swiftness; 2: buy totem of undying; 3:buy golden apple; 4.buy potion of good vision; 5.leave\n");
            break;
        }
    }
}

void encounter_gun(adventurer *hero)
{
    printf("Congrats you have found a special gun! You may not find it again!!!\n");
    (hero->strength)++;                               // get +1 strength
    printf("You have %d strength\n", hero->strength); // show strength
    return;
}

void encounter_trap(adventurer *hero)
{
    int type = rand() % 2; // randomize which kind of trap
    if (!type)             // trap 1 => lose blood and strength
    {
        printf("OH OH!\n You have stepped on the trap\n");
        // lose blood and strength
        (hero->blood)--;
        hero->strength--;
        if (hero->strength <= 0) // if no more strength, lose blood
        {
            printf("You have no strength.\n");
            hero->strength = 0;
            printf("You have %d strength\n", hero->strength); // show strength
            printf("You will lose blood.\n");
            (hero->blood)--;
        }
        if (hero->blood <= 0) // if no more blood, you dead
        {
            printf("You DEAD!\n");
            return;
        }
        printf("You have %d blood\n", hero->blood); // show blood
    }
    else // trap 2 => lose coins
    {
        printf("OH OH!\n Someone stole your money\n");
        int stole = rand() % ((hero->coins) / 2); // random amount of coins to steal
        if (!check_coins(hero, 1))                // check if you have coins to steal
            hero->coins = 0;
        printf("You have %d coins left\n", hero->coins); // show coins
    }

    return;
}

char check_move(char **map, int *row, int *col, int rp, int cp, adventurer *hero) // check if special spot (map[r][c]!='.')
{
    if (map[rp][cp] != '.') // special spot
    {
        if (map[rp][cp] == 'v') // village
        {
            printf("you are now in the village\nWelcome!!!\n");
            in_village(hero); // go to village
            return 'l';
        }
        else if (map[rp][cp] == 'm') // monster
        {
            printf("you are fighting a monster\n");
            char res = encounter_monster(hero); // fight monster
            return res;
        }
        else if (map[rp][cp] == 'g') // gun
        {
            encounter_gun(hero); // get gun
            return 'w';
        }
        else if (map[rp][cp] == 't') // trap
        {
            encounter_trap(hero); // encounter trap
            return 'w';
        }
        else if (map[rp][cp] == 'd') // dealer's shop
        {
            encounter_dealer(hero); // go to dealer's shop
            return 'w';
        }
        else if (map[rp][cp] == 'c') // food
        {
            char res = encounter_food(hero); // get food
            return res;
        }
        else if (map[rp][cp] == 's') // shield
        {
            encounter_shield(hero); // get shield
            return 'w';
        }
        else if (map[rp][cp] == 'f') // fast travel station
        {
            return encounter_station(hero); // go to station
        }
        else if (map[rp][cp] == 'h') // health station
        {
            return encounter_clinic(hero); // go to station
        }
    }
    else
        return 'n'; // normal spot
}

void go_up(char **map, int *row, int *col, adventurer *hero, village *village) // up
{
    if (check_boundary(row, col, hero->rp - 1, hero->cp)) // check if out of range
        return;
    hero->hunger -= (1 / (hero->effect.potion_of_swiftness + 1)); // see if there is potion of swiftness effect and minus hunger
    hero->boot--;                                                 // time of day++
    int a = check_hunger(hero);                                   // see if hungry
    if (check_blood(hero))                                        // see if have blood
        return;
    char temp = map[hero->rp][hero->cp]; // temp for player standing spot
    if (temp == 'p')                     // if player standing spot is shown 'p', then it must be a normal('.') spot
        temp = '.';
    switch (check_move(map, row, col, hero->rp - 1, hero->cp, hero)) // try next step first(see if it is a special spot)
    {
    case 'w': // means player's 'p' can be shown on map
    {
        map[--(hero->rp)][hero->cp] = 'p';
        map[hero->rp + 1][hero->cp] = temp;
        return;
    }
    case 'f': // travel back to village
        --(hero->rp);
        map[hero->rp + 1][hero->cp] = temp;
        hero->rp = hero->rv;
        hero->cp = hero->cv;
        printf("You are now back in the village\n");
        in_village(hero);
        return;
    case 'l': // means player's 'p' can't be shown on map because it is a special spot
        --(hero->rp);
        map[hero->rp + 1][hero->cp] = temp;
        return;
    case 'D': // your dead
        return;
    }

    map[--(hero->rp)][hero->cp] = 'p';       // move p to the next spot
    map[hero->rp + 1][hero->cp] = temp;      // original standing spot is recovered
    print_map(map, row, col, hero, village); // print out map
}

void go_down(char **map, int *row, int *col, adventurer *hero, village *village) // down
{
    if (check_boundary(row, col, hero->rp + 1, hero->cp))
        return;
    hero->hunger -= (1 / (hero->effect.potion_of_swiftness + 1));
    hero->boot--;
    int a = check_hunger(hero);
    if (check_blood(hero))
        return;
    char temp = map[hero->rp][hero->cp];
    if (temp == 'p')
        temp = '.';
    switch (check_move(map, row, col, hero->rp + 1, hero->cp, hero))
    {
    case 'w':
    {
        map[++(hero->rp)][hero->cp] = 'p';
        map[hero->rp - 1][hero->cp] = temp;
        return;
    }
    case 'f':
        ++(hero->rp);
        map[hero->rp - 1][hero->cp] = temp;
        hero->rp = hero->rv;
        hero->cp = hero->cv;
        printf("You are now back in the village\n");
        in_village(hero);
        return;
    case 'l':
        ++(hero->rp);
        map[hero->rp - 1][hero->cp] = temp;
        return;
    case 'D':
        return;
    }

    map[++(hero->rp)][hero->cp] = 'p';
    map[hero->rp - 1][hero->cp] = temp;
    print_map(map, row, col, hero, village);
}

void go_left(char **map, int *row, int *col, adventurer *hero, village *village) // left
{
    if (check_boundary(row, col, hero->rp, hero->cp - 1))
        return;
    hero->hunger -= (1 / (hero->effect.potion_of_swiftness + 1));
    hero->boot--;
    int a = check_hunger(hero);
    if (check_blood(hero))
        return;
    char temp = map[hero->rp][hero->cp];
    if (temp == 'p')
        temp = '.';
    switch (check_move(map, row, col, hero->rp, hero->cp - 1, hero))
    {
    case 'w':
    {
        map[hero->rp][--(hero->cp)] = 'p';
        map[hero->rp][hero->cp + 1] = temp;
        return;
    }
    case 'f':
        --(hero->cp);
        map[hero->rp][hero->cp + 1] = temp;
        hero->rp = hero->rv;
        hero->cp = hero->cv;
        printf("You are now back in the village\n");
        in_village(hero);
        return;
    case 'l':
        --(hero->cp);
        map[hero->rp][hero->cp + 1] = temp;
        return;
    case 'D':
        return;
    }

    map[hero->rp][--(hero->cp)] = 'p';
    map[hero->rp][hero->cp + 1] = temp;
    print_map(map, row, col, hero, village);
}

void go_right(char **map, int *row, int *col, adventurer *hero, village *village) // right
{
    if (check_boundary(row, col, hero->rp, hero->cp + 1))
        return;
    hero->hunger -= (1 / (hero->effect.potion_of_swiftness + 1));
    hero->boot--;
    int a = check_hunger(hero);
    if (check_blood(hero))
        return;
    char temp = map[hero->rp][hero->cp];
    if (temp == 'p')
        temp = '.';
    switch (check_move(map, row, col, hero->rp, hero->cp + 1, hero))
    {
    case 'w':
    {
        map[hero->rp][++(hero->cp)] = 'p';
        map[hero->rp][hero->cp - 1] = temp;
        return;
    }
    case 'f':
        ++(hero->cp);
        map[hero->rp][hero->cp - 1] = temp;
        hero->rp = hero->rv;
        hero->cp = hero->cv;
        printf("You are now back in the village\n");
        in_village(hero);
        return;
    case 'l':
        ++(hero->cp);
        map[hero->rp][hero->cp - 1] = temp;
        return;
    case 'D':
        return;
    }
    map[hero->rp][++(hero->cp)] = 'p';
    map[hero->rp][hero->cp - 1] = temp;
    print_map(map, row, col, hero, village);
}

int move(char **map, int *row, int *col, adventurer *hero, village *village)
{
    int decision; // decision to go up /down....
    printf("[0] use inventory [1] go up [2] go down [3] go right [4] go left [5] see map [6] exit:");
    scanf("%d", &decision);
    switch (decision)
    {
    case 0: // use inventory
        printf("What do you want to use?(1:potion of swiftness; 2: totem of undying; 3:golden apple; 4.potion of good vision)\n");
        int decide; // decide what to do
        scanf("%d", &decide);
        switch (decide)
        {
        case 1:                                        // use potion of swiftness
            if (hero->things.potion_of_swiftness == 0) // check if you have it
            {
                printf("You don't have it.\n");
                return 1;
            }
            hero->things.potion_of_swiftness--;                                                    // use potion of swiftness
            hero->effect.potion_of_swiftness++;                                                    // get effect for potion of swiftness
            hero->boot *= 2;                                                                       // travel two times speed
            printf("Now you have %d potion(s) of swiftness", hero->things.potion_of_swiftness);    // show stats
            printf("Nice! Now you can 2 times speed, but only cost half the amount of hunger.\n"); // description
            return 1;
        case 2:                                     // use totem of undying
            if (hero->things.totem_of_undying == 0) // check if you have it
            {
                printf("You don't have it.\n");
                return 1;
            }
            hero->things.totem_of_undying--;                                               // use totem of undying
            hero->effect.totem_of_undying++;                                               // get effect for totem of undying
            printf("Now you have %d totem(s) of undying", hero->things.totem_of_undying);  // show stats
            printf("Nice! Now if you are strike to death, you will have 5 more blood.\n"); // description
            return 1;
        case 3:
            if (hero->things.golden_apple == 0) // check if you have it
            {
                printf("You don't have it.\n");
                return 1;
            }
            if (hero->blood > 5)
            {
                printf("you have %d blood now, do you still want to use it [0]No [1]Yes:\n", hero->blood);
                int decide;
                scanf("%d", &decide);
                if (!decide)
                    return 1;
            }
            hero->things.golden_apple--;            // use golden apple
            hero->blood += 5;                       // get effect for golden apple
            if (hero->blood >= hero->default_blood) // add to healthy
                hero->blood = hero->default_blood;
            printf("Now you have %d golden apple(s)", hero->things.golden_apple); // show stats
            printf("Nice! Now you have %d blood\n", hero->blood);                 // description
            return 1;
        case 4:                                          // use potion of good vision
            if (hero->things.potion_of_good_vision == 0) // check if you have it
            {
                printf("You don't have it.\n");
                return 1;
            }
            hero->things.potion_of_good_vision--;                                                   // use potion of good vision
            hero->effect.potion_of_good_vision++;                                                   // get effect for potion of good vision
            printf("Now you have %d potion(s) of good vision", hero->things.potion_of_good_vision); // show stats
            printf("Nice! Now you can see more when you see the map\n");                            // description
            return 1;
        default:
            printf("Invalid action!\n");
            return 1;
        }
    case 1: // up
        go_up(map, row, col, hero, village);
        return 1;
    case 2: // down
        go_down(map, row, col, hero, village);
        return 1;
    case 3: // right
        go_right(map, row, col, hero, village);
        return 1;
    case 4: // left
        go_left(map, row, col, hero, village);
        return 1;
    case 5: // see map
        print_map(map, row, col, hero, village);
        return 1;
    case 6: // quit game
        return 0;
    }
}

int travel_decide()
{
    int decide; // decide to create a new map or travel
    printf("[0] Create a new map [1] Start travel the map:");
    scanf("%d", &decide);
    return decide;
}

int main()
{
    int row, col;
    adventurer hero = {0, 10, 2, 20, 10, 1, 0, 0, 0, 0, 24, 1, {0, 0, 0, 0}, {0, 0, 0, 0}};
    printf("Welcome, Adventurer!\nYou have:\n");
    printf("Blood: %d\tStrength: %d\tHunger: %lf\tCoins: %d\tLevel: %d\n", hero.blood, hero.strength, hero.hunger, hero.coins, hero.level);
    village village = {0, 0, 1};

    char **map;
    do
    {
        // set map to all '.'
        map = create_map(&row, &col);
        // setup
        setup_village(map, &row, &col, &hero);
        hero.rp = hero.rv; // player row = village row
        hero.cp = hero.cv; // player column = village column
    } while (!travel_decide());

    int day = 1;
    while (1)
    {
        hero.default_blood += (10 * day);
        village.blood += (50 * day);
        setup_monster(map, &row, &col);
        setup_gun(map, &row, &col);
        setup_trap(map, &row, &col);
        setup_dealer(map, &row, &col);
        setup_food(map, &row, &col);
        setup_shield(map, &row, &col);
        setup_fast_travel(map, &row, &col);
        setup_clinic(map, &row, &col);
        print_map(map, &row, &col, &hero, &village);

        printf("This is Day %d of your village, you need to protect it from the monsters.\n If you fail(village died/you died), the game is over.\n\n", day);
        in_village(&hero);

        // player move
        int flag;
        while (move(map, &row, &col, &hero, &village))
        {
            // printf("+1\n");
            if (hero.blood <= 0)
                break;
            flag = check_time(map, &row, &col, &hero, &village); // check survival
            if (flag == 1)
                break;
        }
        if (flag == 1)
        {
            if (village.blood <= 0) // check village blood
            {
                printf("Your village has fallen\n"); // game over
                free(map);                           // free malloced map
                break;
            }
            printf("Congrats you and your village have lived another day.\n"); // village survive
            day++;                                                             // live another day
            continue;
        }
        free(map);
        break;
    }
    printf("thank you for playing!\nyou are welcome to play anytime again\nbye bye\n");

    return 0;
}
