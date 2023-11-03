/*

CREDITS: Santangelo Lorenzo - Code
         Santangelo Lorenzo - Art
         No copyright - open source

Use whatever assets you like, just dont sell or distribute the main.c file or the main.exe/main file please.

*/



// The STDIO funcion is not needed in this case since
// there is no console input nor output

#include "raylib.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

//glsl if ill ever include shaders

#define GLSL_VERSION            100

//main

int main(){

    //time null for random numbers

    srand(time(NULL));

    //base height and width of the screen

    int screen_width = 800, screen_height = 600;

    //set resizability to true

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    //start the screen with 800 and 600 as base values

    InitWindow(screen_width,screen_height,"Shooter!");

    //Variables

    int score = 0; //score
    char ci[50]; //score as a string

    int game_state = 1; // game state (can be 0 (playing), 1 (menu), 2 (game over screen))
    int quitting = 0; //if quitting = 0 windowshouldclose() = true

    float player[3]; //player = {x,y,speed}
    float bullet[3]; //bullet = {x,y,speed}
    int shoot = 0; //eighter 0 or 1 (0 = base, 1 = shooting)

    float enemy1[3]; // enemy1 = {x,y,speed}
    float enemy2[3]; // enemy2 = {x,y,speed}

    float particles_enemy1[50][5]; // 50x5 floating point matrix

    //p = {{x,y,speedX,speedY,size};{...};...}

    float particles_enemy2[50][5]; //same as particles_enemy1

    //load an image
    Image player_img = LoadImage("images/player/player.png");
    //resize it to 32 pixel with screen_height and screen_width
    ImageResize(&player_img,screen_height/20,screen_height/20);
    //create a texture with the picture
    Texture2D player_text = LoadTextureFromImage(player_img);
    //unload the image
    UnloadImage(player_img);

    Image bullet_img = LoadImage("images/bullet.png");
    ImageResize(&bullet_img,screen_height/20,screen_height/20);
    Texture2D bullet_text = LoadTextureFromImage(bullet_img);
    UnloadImage(bullet_img);

    Image enemy_img = LoadImage("images/enemy.png");
    ImageResize(&enemy_img,screen_height/20,screen_height/20);
    Texture2D enemy_text = LoadTextureFromImage(enemy_img);
    UnloadImage(enemy_img);

    Image menu_img = LoadImage("images/manu.png");
    Texture2D menu_text = LoadTextureFromImage(menu_img);
    UnloadImage(menu_img);

    Image play_img = LoadImage("images/play.png");
    Texture2D play_text = LoadTextureFromImage(play_img);
    UnloadImage(play_img);

    Image pu_img = LoadImage("images/pu.png");
    Texture2D pu_text = LoadTextureFromImage(pu_img);
    UnloadImage(pu_img);

    Image shop_img = LoadImage("images/shop.png");
    Texture2D shop_text = LoadTextureFromImage(shop_img);
    UnloadImage(shop_img);

    //im not surrently using the shader but im loading it anyway for future purposes

    Shader c_shader = LoadShader(0,"shader/bloom.fs");

    //center the player and set its speed to 0

    player[0] = screen_width/2-32;
    player[1] = screen_height-50;
    player[2] = 0;

    //set the bullet x,y equal to the player's ones and its speed to 20

    bullet[0] = player[0];
    bullet[1] = player[1];
    bullet[2] = 20;

    //random value for enemy1's x and y

    enemy1[0] = rand()%(screen_width-32);
    enemy1[1] = -1*(rand()%screen_height);

    //enemy1's speed to 2.5

    enemy1[2] = 2.5;

    enemy2[0] = rand()%(screen_width-32);
    enemy2[1] = -1*(rand()%(screen_height*2));

    enemy2[2] = 2.5;

    //stabilize the FPS to 60

    //no need to make a DeltaTime value couse this is a super lightweight game
    //and litterally anything can run it at 60 fps

    SetTargetFPS(60);

        //if im not quitting nor closing the window

    while((!WindowShouldClose()) && quitting == 0){

        //if window got resized but its still not fullscreen

        if (IsWindowResized() && !IsWindowFullscreen())
        {

            //make it fullscreen

            ToggleFullscreen();

            //monitor x and y 

            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();

            player[0] = (screen_width/2)-(screen_width/20/2);
                
        }

        //if the game is playing (gotta change the if with a switch)

        if (game_state == 0){

            //update the player's x adding it to the velocity

            player[0] += player[2];

            //be sure to set the height every frame to not have any problems with the fullscreen

            player[1] = screen_height-screen_height/10;

            //unfortunately there is no way to not use Rectangle before assigning it, so i have
            //to assign it every frame

            //create all of the rectangles that are needed in the game

            Rectangle player_rect = {player[0],player[1],32,32};
            Rectangle bullet_rect = {bullet[0],bullet[1],32,32};
            Rectangle enemy1_rect = {enemy1[0],enemy1[1],32,32};
            Rectangle enemy2_rect = {enemy2[0],enemy2[1],32,32};

            //update the enemys's y by adding it to their speed

            enemy1[1] += enemy1[2];
            enemy2[1] += enemy2[2];

            //A cute switch (we love em)

            switch(shoot){
                case 1:

                    // if shoot = 1 then shoot the bullet
                    // by updating the y position by subtracting it to the speed

                    bullet[1] -= bullet[2];

                    break;
                    
                default:

                    // if shoot != 1 then make the bullet follow the player

                    bullet[0] = player[0];
                    bullet[1] = player[1];

                    break;

            }

            // if the bullet gets out of the screen then just say that its not shooting anymore

            if (bullet[1] <= 0){
                shoot = 0;
            }

            // screenwarp

            if (player[0] <= -32){
                player[0] = screen_width;
            }
            if (player[0] >= screen_width+32){
                player[0] = 0;
            }

            // end screenwarp

            //limit the accelleration to 15 so that the player
            //does not become sonic

            if (player[2] >= 15){
                player[2] = 15;
            }
            if (player[2] <= -15){
                player[2] = -15;
            }

            //if im pressing KEY_D

            if (IsKeyDown(KEY_D)){

                //add 0.5 to the speed to create an accelleration

                player[2] += 0.5;
            }
            //if the speed is higher than 0 and Im not pressing D
            //then slowly decellerate
            else if(player[2] > 0){

                //subtract 0.5 to simulate decelleration
                    
                player[2] -= 0.5;
            }

            //if im pressing KEY_A

            if (IsKeyDown(KEY_A)){

                //decrese 0.5 to speed to accellerate in the other direction

                player[2] -= 0.5;
            }

            //if the speed is less than 0 and im not pressing A
            //then slowly decrease

            else if(player[2] < 0){

                //

                player[2] += 0.5;
            }

            //if I press space than shoot

            if (IsKeyPressed(KEY_SPACE)){

                shoot = 1;

            }

            //if the enemy gets behind the player

            if (enemy1[1] >= screen_height){

                game_state = 2; //game over state
            }
            if (enemy2[1] >= screen_height){
                    
                game_state = 2; //game over state

            }

            //collision detection.

            //if the bullet collides with enemy1

            if (CheckCollisionRecs(bullet_rect, enemy1_rect)){

                //update the score

                score++;

                //create the particles

                for (int i = 0; i < 50; i++){
                    particles_enemy1[i][0] = bullet[0]; //starting position of the particle X
                    particles_enemy1[i][1] = bullet[1]; //starting position of the particle Y
                    particles_enemy1[i][2] = (rand()%10)-5; //random speed X
                    particles_enemy1[i][3] = (rand()%10)-5; //random speed y
                    particles_enemy1[i][4] = rand()%15; //random size
                }

                //reset the enemy's position (respawn)

                enemy1[0] = rand()%(screen_width-32);
                enemy1[1] = -1*(rand()%450);

                //make em go faster (brrrr) to get a difficulty curve

                enemy1[2] += 0.05;
                enemy2[2] += 0.05;
            }

            //watch enemy1 collision

            if (CheckCollisionRecs(bullet_rect, enemy2_rect)){

                score ++;

                for (int i = 0; i < 50; i++){
                    particles_enemy2[i][0] = bullet[0];
                    particles_enemy2[i][1] = bullet[1];
                    particles_enemy2[i][2] = (rand()%10)-5;
                    particles_enemy2[i][3] = (rand()%10)-5;
                    particles_enemy2[i][4] = rand()%15;
                }

                enemy2[0] = rand()%(screen_width-32);
                enemy2[1] = -1*(rand()%(450*2));

                enemy2[2] += 0.05;
                enemy1[2] += 0.05;
            }

            //if the enemy collides with the player

            if (CheckCollisionRecs(enemy1_rect,player_rect)){

                game_state = 2; //game over

            }
            if (CheckCollisionRecs(enemy2_rect,player_rect)){

                game_state = 2; //game over
                    
            }

            //start drawing (lets play to god that this works)

            BeginDrawing();

                //fill the background with dark gray

                ClearBackground(DARKGRAY);

                //begin a shadermode (default shader in this case)

                BeginShaderMode(c_shader);

                    //draw the textures

                    DrawTexture(bullet_text,bullet[0],bullet[1],WHITE);
                    DrawTexture(player_text,player[0],player[1],WHITE);

                    DrawTexture(enemy_text,enemy1[0],enemy1[1],WHITE);
                    DrawTexture(enemy_text,enemy2[0],enemy2[1],WHITE);

                    //make the ci (score int string) with sprintf

                    sprintf(ci,"%d",score);

                    //draw ci

                    DrawText(ci,10,10,30,WHITE);

                    //draw all of the particles if their size if >= 0.5;

                    for (int i = 0; i < 50; i++){

                        //if their size is less then 15 and more than 0.5

                        if (particles_enemy1[i][4] <= 15 && particles_enemy1[i][4] >= 0.5){

                            //draw them
                                
                            DrawCircle(particles_enemy1[i][0],particles_enemy1[i][1],particles_enemy1[i][4],LIGHTGRAY);

                        }

                        //if their size is more than 0.5 then reduce it

                        if (particles_enemy1[i][4] >= 0.5){
                                particles_enemy1[i][4] -= 0.25;
                        }

                        //use the speed to move em

                        particles_enemy1[i][0] += particles_enemy1[i][2];
                        particles_enemy1[i][1] += particles_enemy1[i][3];
                            
                    }

                    //se plarticles_enemy1

                    for (int i = 0; i < 50; i++){

                        if (particles_enemy2[i][4] <= 15){
                                
                            DrawCircle(particles_enemy2[i][0],particles_enemy2[i][1],particles_enemy2[i][4],LIGHTGRAY);

                        }


                        if (particles_enemy2[i][4] >= 0){
                            particles_enemy2[i][4] -= 0.25;
                        }
                        if (!(particles_enemy2 >= 5 || particles_enemy2 <= -5)){
                            particles_enemy2[i][2] -= 0.5;
                        }

                        if (particles_enemy2[i][4] <= 1){
                            particles_enemy2[i][4] = 0;
                        }

                        particles_enemy2[i][0] += particles_enemy2[i][2];
                        particles_enemy2[i][1] += particles_enemy2[i][3];
                            
                    }

                //end shader mode

                EndShaderMode();

            //finish drawing

            EndDrawing();

        }

        //if the game state is the menu (1)

        if (game_state == 1){

            //create rectangles

            Rectangle play_rect = {(screen_width/2)-(196/2),74+100,196,47};
            Rectangle quit_rect = {(screen_width/2)-(85/2),177.5+80,196,47};
            Vector2 mp = GetMousePosition();

            //get the mouse's rectangle

            Rectangle mouse = {GetMousePosition().x-5, GetMousePosition().y-5, 10,10};

            //if im pressing the mouse and if im hovering over the quit rectangle with my mouse

            if (CheckCollisionRecs(quit_rect,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                quitting = 1;
            }

            //if im pressing the mouse and if im hovering over the play button

            if (CheckCollisionRecs(play_rect,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                game_state = 0;
            }

            //start drawing

            BeginDrawing();

                //fill the background

                ClearBackground(DARKGRAY);

                //draw everything

                DrawTexture(menu_text,(screen_width/2)-(256/2),74,WHITE);
                DrawText("Shooter!",(screen_width/2)-(210/2),90,50,BLACK);

                DrawTexture(play_text,(screen_width/2)-(196/2),74+100,WHITE);
                DrawText("Play",(screen_width/2)-(85/2),177.5,40,BLACK);

                DrawTexture(play_text,(screen_width/2)-(196/2),74+100+80,WHITE);
                DrawText("Quit",(screen_width/2)-(85/2),177.5+80,40,BLACK);

                //draw the mouse

                DrawRectangleRec(mouse,RAYWHITE);

            //stop drawing

            EndDrawing();

        }

        //if the game state is the death screen

        if (game_state == 2){

            //create the rectangles

            Rectangle menu_button = {((screen_width/2)-100/2),74+95,196,47};
            Vector2 mp = GetMousePosition();
            Rectangle mouse = {GetMousePosition().x-5, GetMousePosition().y-5, 10,10};

            //if im clicking with the mouse

            if (CheckCollisionRecs(menu_button,mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                game_state = 1; //game state menu
            }

            //Reset every single value to reset.

            int shoot = 0;

            player[0] = (screen_width/2)-32;
            player[1] = screen_height- screen_height/10;
            player[2] = 0;

            bullet[0] = player[0];
            bullet[1] = player[1];
            bullet[2] = 17.5;

            enemy1[0] = rand()%(screen_width-32);
            enemy1[1] = -1*(rand()%screen_height);
            enemy1[2] = 2.5;

            enemy2[0] = rand()%(screen_width-32);
            enemy2[1] = -1*(rand()%(screen_height*2));
            enemy2[2] = 2.5;

            //set the particles all to 0

            for (int i = 0; i < 50; i++){
                particles_enemy1[i][0] = 0;
                particles_enemy1[i][1] = 0;
                particles_enemy1[i][2] = 0;
                particles_enemy1[i][3] = 0;
                particles_enemy1[i][4] = 0;


                particles_enemy2[i][0] = 0;
                particles_enemy2[i][1] = 0;
                particles_enemy2[i][2] = 0;
                particles_enemy2[i][3] = 0;
                particles_enemy2[i][4] = 0;
            }

            //set the score to 0

            score = 0;

            //start drawing

            BeginDrawing();

                //draw eveying and fill the background with dark gray

                ClearBackground(DARKGRAY);

                DrawTexture(menu_text,(screen_width/2)-(256/2),74,WHITE);

                DrawText("Game Over!",(screen_width/2)-(210/2),90,40,BLACK);

                DrawTexture(play_text,(screen_width/2)-(196/2),74+100,WHITE);

                DrawText("menu",(screen_width/2)-(95/2),74+100,40,BLACK);

                DrawRectangleRec(mouse,WHITE);

            //stop drawing

            EndDrawing();

        }

        }
    return 0;
}