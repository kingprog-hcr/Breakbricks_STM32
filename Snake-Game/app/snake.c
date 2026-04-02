/*
 * snake.c
 *
 *  Created on: Feb 02, 2026
 *      Author: Chrys-Renabell HASSAM
 */

#include "config.h"
#include "snake.h"
#include "button.h"
#include "display.h"
#include "stdbool.h"
#include "stm32g4_systick.h"
#include "stm32g4_utils.h"
#include <stdlib.h>
#include "TFT_ili9341/stm32g4_ili9341.h"


// VARIABLES PRIVEES

// Serpent
static snake_t snake;

// Pomme du mode normal
static segment_t apple;

// Pommes du mode vortex (2 portails)
static segment_t apples[2];

// Timing du deplacement automatique
static uint32_t last_move_tick = 0;

// Score et vitesse
static int16_t score    = 0;
static int16_t fps      = 200;   // intervalle entre deux mouvements en ms (plus petit = plus rapide)
static int16_t min_fps  = 90;   // vitesse maximale (intervalle minimal)

// Gestion pause
static bool paused     = false;
static bool lastButton = false;  // etat precedent du bouton center pour detection front

// Verrou de direction : empeche deux changements de direction entre deux mouvements
// (evite le bug de demi-tour par double appui rapide)
static bool direction_changed = false;


// Mode Bombe

static segment_t  bombs[MAX_BOMBS];           // positions des bombes
static bool       bomb_active[MAX_BOMBS];      // indique si chaque bombe est active
static uint32_t   bomb_spawn_tick[MAX_BOMBS];  // tick de la derniere action sur chaque bombe

static uint8_t  current_bomb_count = 1;        // nombre de bombes actives selon le score
static uint32_t bomb_lifetime      = 6000;     // duree de vie d'une bombe en ms
static uint32_t bomb_interval      = 1500;     // delai avant reapparition d'une bombe en ms
// Gestion du menu

static int8_t choice      = 4;      // option selectionnee (0 a 3), on initialise par defaut a  4 pour eviter le bug d'affichage au cas ou on choist le mode Wall au debut
static bool   last_center = false;  // etat precedent bouton center
static bool   last_up     = false;  // etat precedent bouton haut
static bool   last_down   = false;  // etat precedent bouton bas


// Machine a etats

static state_e state          = INIT;          // etat courant
static state_e previous_state = INFINITE_MODE; // etat precedent (force l'entree au premier tick)


// Mode Mur


static wall_t walls[MAX_WALLS];  // tableau des murs
static uint8_t wall_count = 0;   // nombre de murs actifs



// PROTOTYPES PRIVES


// Logique de base du serpent
static void SNAKE_update_direction(bool up, bool down, bool right, bool left);
static void SNAKE_move(void);
static void Generate_random_apple(segment_t *obj);
static void SNAKE_grow_up(void);
static void SNAKE_check_collisin(void);
static void SNAKE_game_over(void);

// Menu
static void GAME_menu(void);

// Bombes
static void BOMB_spawn(uint8_t i);
static void BOMB_update(void);
static void BOMB_check_collision(void);
static void BOMB_update_difficulty(void);

// Vortex
static void SNAKE_vortex(void);



// MACHINE A ETATS PRINCIPALE

/*
 * GAME_process : appelee en boucle depuis main().
 * Gere les transitions entre etats et dispatche vers la bonne logique de jeu.
 */
void GAME_process(void)
{
    // Detection d'une transition d'etat (entree dans un nouvel etat)
    bool entrance = (state != previous_state);
    previous_state = state;

    switch(state)
    {

        case INIT:
            if(entrance)
            {
                BUTTONS_init();   // initialisation des boutons
                DISPLAY_init();   // effacement ecran et init affichage
                last_up = last_down = last_center = false;
                choice = 0;       // selectionner le premier item du menu
            }
            GAME_menu();  // affichage et navigation dans le menu a chaque tick
        break;


        case WALL_MODE:
            if(entrance)
            {
                // Definition des 4 murs : 2 verticaux, 2 horizontaux
                wall_count = 4;

                wall_t *v_wall_1 = &walls[0];
                wall_t *v_wall_2 = &walls[1];
                wall_t *h_wall_1 = &walls[2];
                wall_t *h_wall_2 = &walls[3];

                // Mur vertical gauche : 7 briques
                v_wall_1->bricks_count = 7;
                v_wall_1->bricks[0].x1 = 3*SEG_SIZE;
                v_wall_1->bricks[0].x2 = 4*SEG_SIZE;
                v_wall_1->bricks[0].y1 = 4*SEG_SIZE;
                v_wall_1->bricks[0].y2 = 5*SEG_SIZE;

                // Mur vertical droit : 7 briques
                v_wall_2->bricks_count = 7;
                v_wall_2->bricks[0].x1 = 17*SEG_SIZE;
                v_wall_2->bricks[0].x2 = 18*SEG_SIZE;
                v_wall_2->bricks[0].y1 = 4*SEG_SIZE;
                v_wall_2->bricks[0].y2 = 5*SEG_SIZE;

                // Generation des briques suivantes par decalage vertical
                for(int8_t i = 1; i < v_wall_1->bricks_count; i++)
                {
                    v_wall_1->bricks[i].x1 = v_wall_1->bricks[0].x1;
                    v_wall_1->bricks[i].x2 = v_wall_1->bricks[0].x2;
                    v_wall_1->bricks[i].y1 = v_wall_1->bricks[0].y1 + i*SEG_SIZE;
                    v_wall_1->bricks[i].y2 = v_wall_1->bricks[0].y2 + i*SEG_SIZE;

                    v_wall_2->bricks[i].x1 = v_wall_2->bricks[0].x1;
                    v_wall_2->bricks[i].x2 = v_wall_2->bricks[0].x2;
                    v_wall_2->bricks[i].y1 = v_wall_2->bricks[0].y1 + i*SEG_SIZE;
                    v_wall_2->bricks[i].y2 = v_wall_2->bricks[0].y2 + i*SEG_SIZE;
                }

                // Mur horizontal haut : 10 briques
                h_wall_1->bricks_count = 10;
                h_wall_1->bricks[0].x1 = 5*SEG_SIZE;
                h_wall_1->bricks[0].x2 = 6*SEG_SIZE;
                h_wall_1->bricks[0].y1 = 2*SEG_SIZE;
                h_wall_1->bricks[0].y2 = 3*SEG_SIZE;

                // Mur horizontal bas : 10 briques
                h_wall_2->bricks_count = 10;
                h_wall_2->bricks[0].x1 = 5*SEG_SIZE;
                h_wall_2->bricks[0].x2 = 6*SEG_SIZE;
                h_wall_2->bricks[0].y1 = 13*SEG_SIZE;
                h_wall_2->bricks[0].y2 = 14*SEG_SIZE;

                // Generation des briques suivantes par decalage horizontal
                for(int8_t i = 1; i < h_wall_1->bricks_count; i++)
                {
                    h_wall_1->bricks[i].x1 = h_wall_1->bricks[0].x1 + i*SEG_SIZE;
                    h_wall_1->bricks[i].x2 = h_wall_1->bricks[0].x2 + i*SEG_SIZE;
                    h_wall_1->bricks[i].y1 = h_wall_1->bricks[0].y1;
                    h_wall_1->bricks[i].y2 = h_wall_1->bricks[0].y2;

                    h_wall_2->bricks[i].x1 = h_wall_2->bricks[0].x1 + i*SEG_SIZE;
                    h_wall_2->bricks[i].x2 = h_wall_2->bricks[0].x2 + i*SEG_SIZE;
                    h_wall_2->bricks[i].y1 = h_wall_2->bricks[0].y1;
                    h_wall_2->bricks[i].y2 = h_wall_2->bricks[0].y2;
                }

                SNAKE_Init();

                // Affichage initial des murs (une seule fois a l'entree)
                for(int8_t b = 0; b < wall_count; b++)
                    DISPLAY_WALL(&walls[b]);
            }
            SNAKE_process_main();
        break;

        case BOMB_MODE:
            if(entrance)
            {
                SNAKE_Init();

                // Reset de toutes les bombes
                uint32_t tick = HAL_GetTick();
                for(int i = 0; i < MAX_BOMBS; i++)
                {
                    bomb_active[i]    = false;
                    bomb_spawn_tick[i] = tick;
                }
            }
            SNAKE_process_main();
        break;

        case VORTEX:
            if(entrance)
            {
                // Reset de l'affichage des pommes vortex avant de relancer

                SNAKE_Init();

                // Generation des deux portails (pommes vortex)
                Generate_random_apple(&apples[0]);
                Generate_random_apple(&apples[1]);
            }
            SNAKE_process_main();
        break;


        case INFINITE_MODE:
            if(entrance)
            {
                SNAKE_Init();
            }
            SNAKE_process_main();
        break;

        default:
        break;
    }
}



// MENU

/*
 * GAME_menu : gere la navigation dans le menu et le lancement du mode choisi.
 * Appelee a chaque tick quand on est dans l'etat INIT.
 */

static void GAME_menu(void)
{
    bool up     = BUTTON_up_read();
    bool down   = BUTTON_down_read();
    bool center = BUTTON_center_read();

    // Detection front montant bouton haut : deplacement vers le haut dans le menu
    if(up && !last_up)
    {
        choice--;
        if(choice < 0) choice = 3;
    }
    last_up = up;

    // Detection front montant bouton bas : deplacement vers le bas dans le menu
    if(down && !last_down)
    {
        choice++;
        if(choice > 3) choice = 0;
    }
    last_down = down;

    // Detection front montant bouton center : validation du choix
    if(center && !last_center)
    {
        switch(choice)
        {
            case 0: state = WALL_MODE;     break;
            case 1: state = BOMB_MODE;     break;
            case 2: state = VORTEX;        break;
            case 3: state = INFINITE_MODE; break;
        }
    }
    last_center = center;

    // Rafraichissement affichage menu
    DISPLAY_Menu(&choice);
}


// INITIALISATION

/*
 * SNAKE_Init : reinitialise completement l'etat du jeu.
 * Appelee a chaque entree dans un mode de jeu ET implicitement par SNAKE_game_over
 * via le retour a INIT puis la selection d'un mode.
 */

void SNAKE_Init(void)
{
    int16_t center_x = (SCREEN_WIDTH  / 2 / SEG_SIZE) * SEG_SIZE;
    int16_t center_y = (SCREEN_HEIGHT / 2 / SEG_SIZE) * SEG_SIZE;

    // Reset longueur et direction initiale
    snake.length = 3;
    snake.dir_x  = 1;
    snake.dir_y  = 0;

    // Placement du serpent au centre de l'ecran, oriente vers la droite
    for(int8_t i = 0; i < snake.length; i++)
    {
        int16_t seg_x = center_x - i * SEG_SIZE;
        int16_t seg_y = center_y;

        snake.body[i].x1 = seg_x;
        snake.body[i].y1 = seg_y;
        snake.body[i].x2 = seg_x + SEG_SIZE - 1;
        snake.body[i].y2 = seg_y + SEG_SIZE - 1;
    }

    // Reset score et vitesse
    score = 0;
    fps   = 200;

    // Reset pause et verrou de direction
    paused            = false;
    direction_changed = false;

    // Effacement ecran et reinitialisation affichage
    DISPLAY_init();

    // Nouvelle graine aleatoire basee sur le temps courant
    srand(HAL_GetTick());

    // Generation de la pomme normale
    Generate_random_apple(&apple);
}



// BOUCLE PRINCIPALE DU JEU


/*
 * SNAKE_process_main : appelee a chaque tick quand un mode de jeu est actif.
 * Gere la pause, le timing, le deplacement, les collisions et l'affichage.
 */

void SNAKE_process_main(void)
{
    bool button_up     = BUTTON_up_read();
    bool button_down   = BUTTON_down_read();
    bool button_right  = BUTTON_right_read();
    bool button_left   = BUTTON_left_read();
    bool button_center = BUTTON_center_read();

    // Mise a jour direction (bloquee si direction_changed est true)
    SNAKE_update_direction(button_up, button_down, button_right, button_left);

    // Bascule pause sur front montant du bouton center
    if(button_center && !lastButton)
        paused = !paused;
    lastButton = button_center;

    if(!paused)
    {
        uint32_t current_tick = HAL_GetTick();

        // Deplacement automatique selon l'intervalle fps
        if((current_tick - last_move_tick) > fps)
        {
            last_move_tick = current_tick;

            // Deplacement du serpent et reset du verrou de direction
            SNAKE_move();

            // Verification des collisions (corps, bords, murs)
            SNAKE_check_collisin();

            // Logique specifique au mode Bombe
            if(state == BOMB_MODE)
            {
                // Mise a jour cycle de vie des bombes et difficulte
                BOMB_update();
                // Verification collision tete avec une bombe
                BOMB_check_collision();
                // Rafraichissement affichage de toutes les bombes (actives ou non)
                for(uint8_t i = 0; i < current_bomb_count; i++)
                    DISPLAY_refresh_bomb(&bombs[i], bomb_active[i], i);
            }

            // Logique specifique au mode Vortex
            if(state == VORTEX)
            {
                // Teleportation et regeneration des portails si collision
                SNAKE_vortex();
                // Rafraichissement affichage des deux portails
                DISPLAY_refresh_apple_indexed(&apples[0], 0);
                DISPLAY_refresh_apple_indexed(&apples[1], 1);
            }

            // Rafraichissement affichage serpent et pomme normale (tous modes)
            DISPLAY_refresh_snake(&snake);
            DISPLAY_refresh_apple(&apple);

            // Verification si la tete est sur la pomme normale
            SNAKE_grow_up();
        }
    }
}



// GESTION DE LA DIRECTION

/*
 * SNAKE_update_direction : met a jour la direction du serpent selon les boutons.
 * Le verrou direction_changed empeche deux changements entre deux mouvements,
 * ce qui evite le bug de demi-tour par double appui rapide.
 */

static void SNAKE_update_direction(bool up, bool down, bool right, bool left)
{
    // Si une direction a deja ete prise ce cycle, on ignore les nouveaux appuis
    if(direction_changed) return;

    if(up && snake.dir_y != -1)
    {
        snake.dir_x = 0;
        snake.dir_y = 1;
        direction_changed = true;
    }
    else if(down && snake.dir_y != 1)
    {
        snake.dir_x = 0;
        snake.dir_y = -1;
        direction_changed = true;
    }
    else if(right && snake.dir_x != -1)
    {
        snake.dir_x = 1;
        snake.dir_y = 0;
        direction_changed = true;
    }
    else if(left && snake.dir_x != 1)
    {
        snake.dir_x = -1;
        snake.dir_y = 0;
        direction_changed = true;
    }
}



// DEPLACEMENT DU SERPENT


/*
 * SNAKE_move : fait avancer le serpent d'un segment dans la direction courante.
 * Chaque segment prend la position du segment precedent (decalage de queue a tete).
 * Remet le verrou direction_changed a false apres le deplacement.
 */

static void SNAKE_move(void)
{
    // Decalage du corps de la queue vers la tete
    for(int8_t i = snake.length - 1; i > 0; i--)
        snake.body[i] = snake.body[i - 1];

    // Deplacement de la tete selon la direction courante
    snake.body[0].x1 += snake.dir_x * SEG_SIZE;
    snake.body[0].x2 += snake.dir_x * SEG_SIZE;
    snake.body[0].y1 += snake.dir_y * SEG_SIZE;
    snake.body[0].y2 += snake.dir_y * SEG_SIZE;

    // Autorisation d'un nouveau changement de direction pour le prochain cycle
    direction_changed = false;
}



// GENERATION D'UNE POMME ALEATOIRE
/*
 * Generate_random_apple : place une pomme a une position aleatoire valide.
 * Verifie qu'elle ne chevauche pas : le serpent, les murs, les bombes actives,
 * la pomme normale et les pommes vortex.
 */

static void Generate_random_apple(segment_t *obj)
{
    bool valid = false;

    while(!valid)
    {
        valid = true;

        obj->x1 = (rand() % (SCREEN_WIDTH  / SEG_SIZE)) * SEG_SIZE;
        obj->y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;
        obj->x2 = obj->x1 + SEG_SIZE - 1;
        obj->y2 = obj->y1 + SEG_SIZE - 1;

        // Verifier superposition avec le corps du serpent
        for(int8_t i = 0; i < snake.length; i++)
        {
            if(snake.body[i].x1 == obj->x1 && snake.body[i].y1 == obj->y1)
            {
                valid = false;
                break;
            }
        }

        // Verifier superposition avec les briques de mur
        for(uint8_t w = 0; w < wall_count && valid; w++)
        {
            for(uint8_t b = 0; b < walls[w].bricks_count; b++)
            {
                if(obj->x1 == walls[w].bricks[b].x1 && obj->y1 == walls[w].bricks[b].y1)
                {
                    valid = false;
                    break;
                }
            }
        }

        // Verifier superposition avec les bombes actives
        for(uint8_t b = 0; b < MAX_BOMBS && valid; b++)
        {
            if(bomb_active[b])
            {
                if(obj->x1 == bombs[b].x1 && obj->y1 == bombs[b].y1)
                {
                    valid = false;
                    break;
                }
            }
        }

        // Verifier superposition avec la pomme normale (si obj est different)
        if(obj != &apple)
        {
            if(obj->x1 == apple.x1 && obj->y1 == apple.y1)
                valid = false;
        }

        // Verifier superposition avec les pommes vortex (entre elles et avec obj)
        for(uint8_t a = 0; a < 2 && valid; a++)
        {
            if(obj != &apples[a])
            {
                if(obj->x1 == apples[a].x1 && obj->y1 == apples[a].y1)
                {
                    valid = false;
                    break;
                }
            }
        }
    }
}



// CROISSANCE DU SERPENT

/*
 * SNAKE_grow_up : verifie si la tete est sur la pomme normale.
 * Si oui : allonge le serpent, incremente le score, regenere la pomme
 * et augmente la vitesse jusqu'a la limite min_fps.
 */
static void SNAKE_grow_up(void)
{
    if(snake.body[0].x1 == apple.x1 && snake.body[0].y1 == apple.y1)
    {
        snake.length++;
        score++;
        Generate_random_apple(&apple);

        // Acceleration progressive (fps diminue = deplacement plus frequent)
        if(fps >= min_fps)
            fps -= 5;
    }
}



// DETECTION DES COLLISIONS

/*
 * SNAKE_check_collisin : verifie toutes les collisions fatales.
 * Collision avec : le corps du serpent, les bords de l'ecran, les murs.
 * Appelle SNAKE_game_over() en cas de collision.
 */

static void SNAKE_check_collisin(void)
{
    segment_t tete = snake.body[0];

    // Collision avec le corps du serpent (a partir du second segment)
    for(int8_t i = 1; i < snake.length; i++)
    {
        if(tete.x1 == snake.body[i].x1 && tete.y1 == snake.body[i].y1)
            SNAKE_game_over();
    }

    // Collision avec les bords gauche et droit
    if(tete.x1 >= (SCREEN_WIDTH / SEG_SIZE) * SEG_SIZE || tete.x1 < 0)
        SNAKE_game_over();

    // Collision avec les bords haut et bas
    if(tete.y2 > (SCREEN_HEIGHT / SEG_SIZE) * SEG_SIZE || tete.y1 < 0)
        SNAKE_game_over();

    // Collision avec les briques de mur (mode Wall uniquement)
    for(uint8_t w = 0; w < wall_count; w++)
    {
        for(uint8_t b = 0; b < walls[w].bricks_count; b++)
        {
            if(tete.x1 == walls[w].bricks[b].x1 && tete.y1 == walls[w].bricks[b].y1)
                SNAKE_game_over();
        }
    }
}



// FIN DE PARTIE

/*
 * SNAKE_game_over : affiche l'ecran de fin avec le score, attend 2 secondes
 * puis retourne au menu principal.
 * Le reset complet (score, vitesse, serpent) est desormais dans SNAKE_Init()
 * qui sera appelee lors de la prochaine entree dans un mode de jeu.
 */

static void SNAKE_game_over(void)
{
    int16_t center_x = SCREEN_WIDTH  / 2;
    int16_t center_y = SCREEN_HEIGHT / 2;

    // Affichage "GAME OVER" en rouge au centre
    DISPLAY_string("GAME OVER",
                   ILI9341_COLOR_RED,
                   ILI9341_COLOR_WHITE,
                   center_x - 4 * SEG_SIZE,
                   center_y);

    // Affichage du score final
    char score_str[10];
    char text[20];
    itoa(score, score_str, 10);
    sprintf(text, "SCORE : %s", score_str);
    DISPLAY_string(text,
                   ILI9341_COLOR_BLACK,
                   ILI9341_COLOR_WHITE,
                   center_x - (strlen(text)/2) * SEG_SIZE,
                   center_y + 2*SEG_SIZE);

    // Pause avant retour menu
    HAL_Delay(2000);

    // Retour au menu (le reset complet se fera dans SNAKE_Init au prochain mode)
    state      = INIT;
    wall_count = 0;
}



// MODE BOMBE


/*
 * BOMB_spawn : fait apparaitre une bombe a une position aleatoire
 * qui ne chevauche pas le serpent.
 */

static void BOMB_spawn(uint8_t i)
{
    bool valid = false;

    while(!valid)
    {
        valid = true;

        bombs[i].x1 = (rand() % (SCREEN_WIDTH  / SEG_SIZE)) * SEG_SIZE;
        bombs[i].y1 = (rand() % (SCREEN_HEIGHT / SEG_SIZE)) * SEG_SIZE;
        bombs[i].x2 = bombs[i].x1 + SEG_SIZE - 1;
        bombs[i].y2 = bombs[i].y1 + SEG_SIZE - 1;

        // Verifier superposition avec le serpent
        for(int8_t s = 0; s < snake.length; s++)
        {
            if(snake.body[s].x1 == bombs[i].x1 && snake.body[s].y1 == bombs[i].y1)
            {
                valid = false;
                break;
            }
        }
    }

    bomb_active[i]    = true;
    bomb_spawn_tick[i] = HAL_GetTick();
}


/*
 * BOMB_update : gere le cycle de vie de chaque bombe.
 * Fait apparaitre une nouvelle bombe apres bomb_interval ms,
 * et desactive une bombe apres bomb_lifetime ms.
 */

static void BOMB_update(void)
{
    uint32_t current_tick = HAL_GetTick();

    // Mise a jour du nombre de bombes selon le score
    BOMB_update_difficulty();

    for(uint8_t i = 0; i < current_bomb_count; i++)
    {
        // Bombe inactive depuis assez longtemps : on la fait reapparaitre
        if(!bomb_active[i] && (current_tick - bomb_spawn_tick[i] > bomb_interval))
            BOMB_spawn(i);

        // Bombe active depuis trop longtemps : on la desactive
        if(bomb_active[i] && (current_tick - bomb_spawn_tick[i] > bomb_lifetime))
        {
            bomb_active[i]    = false;
            bomb_spawn_tick[i] = current_tick;
        }
    }
}


/*
 * BOMB_check_collision : verifie si la tete du serpent touche une bombe active.
 * Appelle SNAKE_game_over() si collision detectee.
 */

static void BOMB_check_collision(void)
{
    for(uint8_t i = 0; i < current_bomb_count; i++)
    {
        if(!bomb_active[i]) continue;

        if(snake.body[0].x1 == bombs[i].x1 && snake.body[0].y1 == bombs[i].y1)
            SNAKE_game_over();
    }
}


/*
 * BOMB_update_difficulty : ajuste le nombre de bombes simultanees selon le score.
 * Plus le score est eleve, plus il y a de bombes actives.
 */
static void BOMB_update_difficulty(void)
{
    if     (score < 5)  current_bomb_count = 1;
    else if(score < 10) current_bomb_count = 3;
    else if(score < 15) current_bomb_count = 4;
    else if(score < 25) current_bomb_count = 6;
    else                current_bomb_count = 8;
}



// MODE VORTEX


/*
 * SNAKE_vortex : gere la teleportation du serpent entre les deux portails.
 * Si la tete touche apples[0], le serpent est teleporte sur apples[1], et inversement.
 * Apres chaque teleportation, deux nouveaux portails sont generes aleatoirement.
 * Le serpent grandit d'un segment et le score est incremente a chaque passage.
 */
static void SNAKE_vortex(void)
{
    // Tete sur le portail 0  teleportation vers le portail 1
    if(snake.body[0].x1 == apples[0].x1 && snake.body[0].y1 == apples[0].y1)
    {
        snake.body[0].x1 = apples[1].x1;
        snake.body[0].y1 = apples[1].y1;
        snake.body[0].x2 = apples[1].x1 + SEG_SIZE - 1;
        snake.body[0].y2 = apples[1].y1 + SEG_SIZE - 1;

        Generate_random_apple(&apples[0]);
        Generate_random_apple(&apples[1]);

        snake.length++;
        score++;
    }
    // Tete sur le portail 1  teleportation vers le portail 0
    else if(snake.body[0].x1 == apples[1].x1 && snake.body[0].y1 == apples[1].y1)
    {
        snake.body[0].x1 = apples[0].x1;
        snake.body[0].y1 = apples[0].y1;
        snake.body[0].x2 = apples[0].x1 + SEG_SIZE - 1;
        snake.body[0].y2 = apples[0].y1 + SEG_SIZE - 1;

        Generate_random_apple(&apples[0]);
        Generate_random_apple(&apples[1]);

        snake.length++;
        score++;
    }
}
