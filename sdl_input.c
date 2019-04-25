#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define WINDOW_WIDTH (1000)
#define WINDOW_HEIGHT (480)

#define BRICK_HEIGHT (80)

#define SPEED (300)
#define ACC (20)

struct brick
{
    SDL_Rect br;

};

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Mariyo",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT,0);
    if (window == NULL)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* rend = SDL_CreateRenderer(window, -1, render_flags);
    if (rend == NULL)
    {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface1 = IMG_Load("images/mario_right.png");
    if (surface1 == NULL)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex1 = SDL_CreateTextureFromSurface(rend, surface1);
    SDL_FreeSurface(surface1);
    if (tex1 == NULL)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface2 = IMG_Load("images/mario_left.png");
    if (surface2 == NULL)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex2 = SDL_CreateTextureFromSurface(rend, surface2);
    SDL_FreeSurface(surface2);
    if (tex2 == NULL)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface3 = IMG_Load("images/mario_left.png");
    if (surface3 == NULL)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* tex_obst = SDL_CreateTextureFromSurface(rend, surface3);
    SDL_FreeSurface(surface3);
    if (tex2 == NULL)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Rect dest;
    SDL_Rect obstacle;

    SDL_Texture* tex_mario;

    SDL_QueryTexture(tex2, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 4;
    dest.h /= 4;

    SDL_QueryTexture(tex_obst, NULL, NULL, &obstacle.w, &obstacle.h);
    obstacle.w /= 4;
    obstacle.h /= 6;

    tex_mario = tex1;

    float x_obst = 0;
    float y_obst = 366-(obstacle.h/2);
    
    float x_pos = (WINDOW_WIDTH - dest.w) / 2;
    float y_pos = (WINDOW_HEIGHT - dest.h) / 2;
    float x_vel = 0;
    float y_vel = 0;

    float y_acc = ACC;

    int left = 0;
    int right = 0;

    int jump = 0;
    int jumped = 0;

    int close_requested = 0;

    double tot_time;
    clock_t start,end;
    
    while (!close_requested)
    {
        start = clock();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close_requested = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 1;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 1;
                    break;
                case SDL_SCANCODE_SPACE:
                    if(y_pos == WINDOW_HEIGHT-dest.h){
                    jump = 1;
		    	}
                    break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    left = 0;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    right = 0;
                    break;
                case SDL_SCANCODE_SPACE:
                    jump = 0;
		    jumped = 0;	
                    break;
                }
                break;
            }
        }

        x_vel = 0;
    
        if (left && !right) {
            x_vel = -SPEED;
            tex_mario = tex2;
        }
        if (right && !left){
            x_vel = SPEED;
            tex_mario = tex1;
        }

        if(!jumped && jump && y_pos == WINDOW_HEIGHT-dest.h){ y_vel = -600;
	jumped = 1;}
        else if(!jump){
            y_acc = ACC;
        }

        y_vel += y_acc;

        x_pos += x_vel / 60;
        y_pos += y_vel / 60;

        if (x_pos <= 0) x_pos = 0; 
        if (y_pos <= 0) y_pos = 0;
        if (x_pos >= WINDOW_WIDTH - dest.w) x_pos = WINDOW_WIDTH - dest.w;
        if (y_pos >= WINDOW_HEIGHT - dest.h) y_pos = WINDOW_HEIGHT - dest.h;

        if(y_pos <= y_obst + obstacle.h && y_pos >= y_obst && x_pos <= x_obst + obstacle.w) 
            {   
                y_pos = y_obst + obstacle.h;
                y_vel = 0;
                y_obst -= 4;
            }

        dest.y = (int) y_pos;
        dest.x = (int) x_pos;
        
        obstacle.x = (int) x_obst;
        obstacle.y = (int) y_obst; 

        SDL_SetRenderDrawColor(rend,255,255,255,255);
        SDL_RenderClear(rend);

        SDL_RenderCopy(rend, tex_obst, NULL, &obstacle);
        SDL_RenderCopy(rend, tex_mario, NULL, &dest);
        SDL_RenderPresent(rend);

        end = clock();
        tot_time = ((double)(end-start))/CLOCKS_PER_SEC;
        printf("%f\n", tot_time);
        SDL_Delay(1000/60 - tot_time);
    }
    
    SDL_DestroyTexture(tex1);
    SDL_DestroyTexture(tex2);
    SDL_DestroyTexture(tex_obst);
    SDL_DestroyTexture(tex_mario);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
