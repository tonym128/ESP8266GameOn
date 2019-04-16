#define INTERLACE
#define DETAIL_LEVEL FLOAT_TO_FIXP(0.2) //Smaller = higher detail
#define CACHESIZE 64
#define MAPSIZE map_height *map_width

// https://github.com/s-macke/VoxelSpace
#include "myGame.hpp"

struct Point
{
    FIXPOINT fx, fy, fangle, fmove, fturn, fdeltaMod, fdistance;
    int height, horizon, shift;
    FIXPOINT mapScaleFactor;

    int mapwidthperiod, mapheightperiod;
};

Point p;

#ifdef INTERLACE
int frame = 0;
#endif

int8_t ccmap[CACHESIZE];
bool cccolor[CACHESIZE];
int ccmapOffset = -1;

int8_t cmap;
bool ccolor;
int cmapOffset = -1;
int cheight;
int cheightonscreen;

void render(ScreenBuff *screenBuff)
{
    int cachehit = 0;
    int ccachehit = 0;
    int cacheheighthit = 0;
    int cachemiss = 0;
    int heightonscreen = 0;
#ifdef INTERLACE
    frame += 1;
    if (frame == 2)
        frame = 0;
#endif

    FIXPOINT fsinang = FIXPOINT_SIN(p.fangle);
    FIXPOINT fcosang = FIXPOINT_COS(p.fangle);

    int hiddeny[screenBuff->WIDTH];
    for (int i = 0; i < screenBuff->WIDTH; i += 1)
    {
        hiddeny[i] = screenBuff->HEIGHT;
#ifdef INTERLACE
        if (i % 2 == frame)
            drawVertLine2(screenBuff, i, 0, screenBuff->HEIGHT, 0);
#endif
    }

    FIXPOINT fdeltaz = FIXP_1;
    // Draw from front to back
    for (FIXPOINT fz = FIXP_1; fz < p.fdistance; fz += fdeltaz)
    {
        // 90 degree field of view
        FIXPOINT fsinfz = FIXP_MULT(fsinang, fz);
        FIXPOINT fcosfz = FIXP_MULT(fcosang, fz);

        FIXPOINT fplx = FIXP_MULT(-fcosang, fz) - fsinfz;
        FIXPOINT fply = fsinfz - fcosfz;
        FIXPOINT fprx = fcosfz - fsinfz;
        FIXPOINT fpry = FIXP_MULT(-fsinang, fz) - fcosfz;
        FIXPOINT fdx = (fprx - fplx) / screenBuff->WIDTH;
        FIXPOINT fdy = (fpry - fply) / screenBuff->WIDTH;
        FIXPOINT finvz = FIXP_DIV(FIXP_1, fz) * 240;

        fplx += p.fx;
        fply += p.fy;

#ifdef INTERLACE
        for (int i = frame; i < screenBuff->WIDTH; i += 2)
#else
        for (int i = 0; i < screenBuff->WIDTH; i += 1)
#endif
        {
            if (hiddeny[i] >= -5)
            {
                int mapoffset = ((FIXP_INT_PART(FIXP_DIV(fply, p.mapScaleFactor)) & p.mapwidthperiod) << p.shift) + (FIXP_INT_PART(FIXP_DIV(fplx, p.mapScaleFactor)) & p.mapheightperiod);
                if (cmapOffset == mapoffset)
                {
                    cachehit += 1;
                    if (cheight == p.height) {
                        cacheheighthit += 1;
                    } else {
                        cheightonscreen = heightonscreen = (FIXP_TO_INT((p.height - cmap) * finvz) + p.horizon);
                        cheight = p.height;
                    }

                }
                else
                {
                    cmap = map_data[mapoffset];
                    ccolor = map_colour[mapoffset];

                    cheight = p.height;
                    cheightonscreen = (FIXP_TO_INT((p.height - cmap) * finvz) + p.horizon);

                    cachemiss++;
                }

                if (cheightonscreen < hiddeny[i])
                {
                    drawVertLine2(screenBuff, i, cheightonscreen, hiddeny[i], ccolor);
                    hiddeny[i] = cheightonscreen;
                }

#ifdef INTERLACE
                fplx += fdx * 2;
                fply += fdy * 2;
#else
                fplx += fdx;
                fply += fdy;
#endif
            }
        }

        fdeltaz += p.fdeltaMod;
    }
}

void voxelInput(byte buttonVals, Point *p)
{
    p->fx -= FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove);
    p->fy -= FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove);

    if (processKey(buttonVals, P1_Top))
    {
        p->fx -= FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove);
        p->fy -= FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove);
    }
    if (processKey(buttonVals, P1_Bottom))
    {
        p->fx += FIXP_MULT(FIXPOINT_SIN(p->fangle), p->fmove);
        p->fy += FIXP_MULT(FIXPOINT_COS(p->fangle), p->fmove);
    }
    if (processKey(buttonVals, P1_Left))
    {
        p->fangle += p->fturn;
    }
    if (processKey(buttonVals, P1_Right))
    {
        p->fangle -= p->fturn;
    }

    if (processKey(buttonVals, P2_Top))
    {
        p->height += 1;
    }
    if (processKey(buttonVals, P2_Bottom))
    {
        p->height -= 1;
    }
    if (processKey(buttonVals, P2_Left))
    {
        p->horizon += 1;
    }
    if (processKey(buttonVals, P2_Right))
    {
        p->horizon -= 1;
    }

    // Collision detection. Don't fly below the surface.
    /*
    int mapoffset = (((int)(FIXP_INT_PART(p->fy / p->mapScaleFactor)) & (map_width - 1)) << p->shift) + (((int)FIXP_INT_PART(p->fx / p->mapScaleFactor)) & (map_height - 1));
    if ((map_data[mapoffset] + 10) > p->height)
        p->height = map_data[mapoffset] + 10;
    */
}
bool firstRun = true;
bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	if (firstRun) {
		firstRun = false;
		myGameInit();
	}
    voxelInput(buttonVals, &p);
#ifndef INTERLACE
    displayClear(screenBuff, 1, false);
#endif
    render(screenBuff);
    return false; // Not done
}

void myGameInit()
{
    p.mapwidthperiod = map_width - 1;
    p.mapheightperiod = map_height - 1;

    p.fdistance = INT_TO_FIXP(800);
    p.fx = INT_TO_FIXP(75);
    p.fy = INT_TO_FIXP(75);
    p.fangle = FLOAT_TO_FIXP(-0.6);

    p.fdeltaMod = DETAIL_LEVEL;
    p.fmove = FLOAT_TO_FIXP(4.);
    p.fturn = FLOAT_TO_FIXP(0.1);

    p.height = 60;
    p.horizon = 15;
    p.shift = 7;
    p.mapScaleFactor = INT_TO_FIXP(8);
}
