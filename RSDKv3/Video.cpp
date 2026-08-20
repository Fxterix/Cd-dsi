#include "RetroEngine.hpp"
#include <nds.h>
#include <string>
#include <vector>

int currentVideoFrame = 0;
int videoFrameCount   = 0;
int videoWidth        = 0;
int videoHeight       = 0;
float videoAR         = 0;

// Variables de video simplificadas para DSi
byte videoSurface = 0;
int videoFilePos  = 0;
int videoPlaying  = 0;
int vidFrameMS    = 0;
int vidBaseticks  = 0;

bool videoSkipped = false;

// Funciones de lectura simuladas o adaptadas al sistema de archivos de la DS
static long videoRead(void *buf, long buflen, FileIO *file)
{
    const size_t br = fRead(buf, 1, buflen * sizeof(byte), file);
    if (br == 0)
        return -1;
    return (int)br;
}

static void videoClose(FileIO *file)
{
    fClose(file);
}

void PlayVideoFile(char *filePath)
{
    // En la DSi saltamos la reproducción de cinemáticas OGV pesadas para evitar cuelgues
    PrintLog("Video playback bypassed on DSi for performance.");
    videoPlaying = 0;
    Engine.gameMode = ENGINE_STAGE; // Forzar salto directo al juego
}

void UpdateVideoFrame()
{
    if (videoPlaying == 2) {
        if (currentVideoFrame < videoFrameCount) {
            GFXSurface *surface = &gfxSurface[videoSurface];
            byte fileBuffer     = 0;
            ushort fileBuffer2  = 0;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 24;

            byte clr[3];
            for (int i = 0; i < 0x80; ++i) {
                FileRead(&clr, 3);
                activePalette32[i].r = clr[0];
                activePalette32[i].g = clr[1];
                activePalette32[i].b = clr[2];
                activePalette[i]     = ((ushort)(clr[0] >> 3) << 11) | 32 * (clr[1] >> 2) | (clr[2] >> 3);
            }

            FileRead(&fileBuffer, 1);
            while (fileBuffer != ',') FileRead(&fileBuffer, 1);

            FileRead(&fileBuffer2, 2);
            FileRead(&fileBuffer2, 2);
            FileRead(&fileBuffer2, 2);
            FileRead(&fileBuffer2, 2);
            FileRead(&fileBuffer, 1);
            bool interlaced = (fileBuffer & 0x40) >> 6;
            if (fileBuffer >> 7 == 1) {
                int c = 0x80;
                do {
                    ++c;
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                } while (c != 0x100);
            }
            ReadGifPictureData(surface->width, surface->height, interlaced, graphicData, surface->dataPosition);

            SetFilePosition(videoFilePos);
            ++currentVideoFrame;
        }
        else {
            videoPlaying = 0;
            CloseFile();
        }
    }
}

int ProcessVideo()
{
    // Desactivado temporalmente en DSi para mantener estabilidad en tasa de fotogramas
    return 0;
}

void StopVideoPlayback()
{
    videoPlaying = 0;
}

void SetupVideoBuffer(int width, int height)
{
    // Configuración de pantallas de la DSi usando libnds:
    // Pantalla Superior (A) habilitada para el motor gráfico del juego.
    // Pantalla Inferior (B) en negro absoluto.
    
    videoSetMode(MODE_5_2D);          
    videoSetModeSub(MODE_0_2D);       

    vramSetBankA(VRAM_A_MAIN_BG);     
    vramSetBankB(VRAM_B_MAIN_SPRITE); 

    // Asegurar que la pantalla inferior no renderice nada (pantalla en negro)
    bgHide(0);
    
    PrintLog("DSi video hardware initialized: Top screen active, Bottom screen black.");
}

void CloseVideoBuffer()
{
    videoPlaying = 0;
}
  if (!videoVidData) {
            PrintLog("Video Error!");
            return;
        }

        videoWidth  = videoVidData->width;
        videoHeight = videoVidData->height;
        // commit video Aspect Ratio.
        videoAR = float(videoWidth) / float(videoHeight);

        SetupVideoBuffer(videoWidth, videoHeight);
        vidBaseticks = SDL_GetTicks();
        vidFrameMS   = (videoVidData->fps == 0.0) ? 0 : ((Uint32)(1000.0 / videoVidData->fps));
        videoPlaying = 1; // playing ogv
        trackID      = TRACK_COUNT - 1;

        videoSkipped    = false;
        Engine.gameMode = ENGINE_VIDEOWAIT;
    }
    else {
        PrintLog("Couldn't find file '%s'!", filepath);
    }
}

void UpdateVideoFrame()
{
    if (videoPlaying == 2) {
        if (currentVideoFrame < videoFrameCount) {
            GFXSurface *surface = &gfxSurface[videoSurface];
            byte fileBuffer     = 0;
            ushort fileBuffer2  = 0;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 8;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 16;
            FileRead(&fileBuffer, 1);
            videoFilePos += fileBuffer << 24;

            byte clr[3];
            for (int i = 0; i < 0x80; ++i) {
                FileRead(&clr, 3);
                activePalette32[i].r = clr[0];
                activePalette32[i].g = clr[1];
                activePalette32[i].b = clr[2];
                activePalette[i]     = ((ushort)(clr[0] >> 3) << 11) | 32 * (clr[1] >> 2) | (clr[2] >> 3);
            }

            FileRead(&fileBuffer, 1);
            while (fileBuffer != ',') FileRead(&fileBuffer, 1); // gif image start identifier

            FileRead(&fileBuffer2, 2); // IMAGE LEFT
            FileRead(&fileBuffer2, 2); // IMAGE TOP
            FileRead(&fileBuffer2, 2); // IMAGE WIDTH
            FileRead(&fileBuffer2, 2); // IMAGE HEIGHT
            FileRead(&fileBuffer, 1);  // PaletteType
            bool interlaced = (fileBuffer & 0x40) >> 6;
            if (fileBuffer >> 7 == 1) {
                int c = 0x80;
                do {
                    ++c;
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                    FileRead(&fileBuffer, 1);
                } while (c != 0x100);
            }
            ReadGifPictureData(surface->width, surface->height, interlaced, graphicData, surface->dataPosition);

            SetFilePosition(videoFilePos);
            ++currentVideoFrame;
        }
        else {
            videoPlaying = 0;
            CloseFile();
        }
    }
}

int ProcessVideo()
{
    if (videoPlaying == 1) {
        CheckKeyPress(&keyPress, 0xFF);

        if (videoSkipped && fadeMode < 0xFF) {
            fadeMode += 8;
        }

        if (anyPress || touches > 0) {
            if (!videoSkipped)
                fadeMode = 0;

            videoSkipped = true;
        }

        if (!THEORAPLAY_isDecoding(videoDecoder) || (videoSkipped && fadeMode >= 0xFF)) {
            StopVideoPlayback();
            ResumeSound();
            return 1; // video finished
        }

        // Don't pause or it'll go wild
        if (videoPlaying == 1) {
            const Uint32 now = (SDL_GetTicks() - vidBaseticks);

            if (!videoVidData)
                videoVidData = THEORAPLAY_getVideo(videoDecoder);

            // Play video frames when it's time.
            if (videoVidData && (videoVidData->playms <= now)) {
                if (vidFrameMS && ((now - videoVidData->playms) >= vidFrameMS)) {

                    // Skip frames to catch up, but keep track of the last one+
                    //  in case we catch up to a series of dupe frames, which
                    //  means we'd have to draw that final frame and then wait for
                    //  more.

                    const THEORAPLAY_VideoFrame *last = videoVidData;
                    while ((videoVidData = THEORAPLAY_getVideo(videoDecoder)) != NULL) {
                        THEORAPLAY_freeVideo(last);
                        last = videoVidData;
                        if ((now - videoVidData->playms) < vidFrameMS)
                            break;
                    }

                    if (!videoVidData)
                        videoVidData = last;
                }

                // do nothing; we're far behind and out of options.
                if (!videoVidData) {
                    // video lagging uh oh
                }

#if RETRO_USING_OPENGL
                glBindTexture(GL_TEXTURE_2D, videoBuffer);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoVidData->width, videoVidData->height, GL_RGBA, GL_UNSIGNED_BYTE, videoVidData->pixels);
                glBindTexture(GL_TEXTURE_2D, 0);
#elif RETRO_USING_SDL2
                int half_w     = videoVidData->width / 2;
                const Uint8 *y = (const Uint8 *)videoVidData->pixels;
                const Uint8 *u = y + (videoVidData->width * videoVidData->height);
                const Uint8 *v = u + (half_w * (videoVidData->height / 2));

                SDL_UpdateYUVTexture(Engine.videoBuffer, NULL, y, videoVidData->width, u, half_w, v, half_w);
#elif RETRO_USING_SDL1
                memcpy(Engine.videoBuffer->pixels, videoVidData->pixels, videoVidData->width * videoVidData->height * sizeof(uint));
#endif

                THEORAPLAY_freeVideo(videoVidData);
                videoVidData = NULL;
            }

            return 2; // its playing as expected
        }
    }

    return 0; // its not even initialised
}

void StopVideoPlayback()
{
    if (videoPlaying == 1) {
        // `videoPlaying` and `videoDecoder` are read by
        // the audio thread, so lock it to prevent a race
        // condition that results in invalid memory accesses.
        SDL_LockAudio();

        if (videoSkipped && fadeMode >= 0xFF)
            fadeMode = 0;

        if (videoVidData) {
            THEORAPLAY_freeVideo(videoVidData);
            videoVidData = NULL;
        }
        if (videoDecoder) {
            THEORAPLAY_stopDecode(videoDecoder);
            videoDecoder = NULL;
        }

        CloseVideoBuffer();
        videoPlaying = 0;

        SDL_UnlockAudio();
    }
}

void SetupVideoBuffer(int width, int height)
{
#if RETRO_USING_OPENGL
    if (videoBuffer > 0) {
        glDeleteTextures(1, &videoBuffer);
        videoBuffer = 0;
    }
    glGenTextures(1, &videoBuffer);
    glBindTexture(GL_TEXTURE_2D, videoBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, videoVidData->width, videoVidData->height, GL_RGBA, GL_UNSIGNED_BYTE, videoVidData->pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
#elif RETRO_USING_SDL1
    Engine.videoBuffer = SDL_CreateRGBSurface(0, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    if (!Engine.videoBuffer)
        PrintLog("Failed to create video buffer!");

    if (Engine.videoBuffer)
        SDL_FillRect(Engine.videoBuffer, NULL, SDL_MapRGBA(Engine.videoBuffer->format, 0, 0, 0, 255));
#elif RETRO_USING_SDL2
    Engine.videoBuffer = SDL_CreateTexture(Engine.renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, width, height);

    if (!Engine.videoBuffer)
        PrintLog("Failed to create video buffer!");

    if (Engine.videoBuffer) {
        int hw = width / 2;
        int hh = height / 2;

        auto y  = std::vector<byte>(width * height, 0x00);
        auto uv = std::vector<byte>(hw * hh, 0x80);

        SDL_UpdateYUVTexture(Engine.videoBuffer, NULL, y.data(), width, uv.data(), hw, uv.data(), hw);
    }
#endif
}

void CloseVideoBuffer()
{
    if (videoPlaying == 1) {
#if RETRO_USING_OPENGL
        if (videoBuffer > 0) {
            glDeleteTextures(1, &videoBuffer);
            videoBuffer = 0;
        }
#elif RETRO_USING_SDL1
        SDL_FreeSurface(Engine.videoBuffer);
        Engine.videoBuffer = nullptr;
#elif RETRO_USING_SDL2
        SDL_DestroyTexture(Engine.videoBuffer);
        Engine.videoBuffer = nullptr;
#endif
    }
}
