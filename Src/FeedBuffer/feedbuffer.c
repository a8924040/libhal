#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "CommonInclude.h"
#include "Common.h"
#include "Log.h"
#include "feedbuffer.h"

#define FEED_BUFFER_TAG     0x24063432
#define FEED_BUFFER_CLOSING 0x00000001
#define FEED_BUFFER_READING 0x00000002
#define FEED_BUFFER_WRITTING    0x00000004

typedef struct tag_FEEDER_BUFFER
{
    unsigned long   tag;
    unsigned long   status;
    char        *databuffer;
    unsigned int    bufferlen;
    unsigned int    pRead;
    unsigned int    pWrite;
    pthread_mutex_t     PositionMutex;
    sem_t           bufferEvent;
} FEED_BUFFER, *PFEED_BUFFER;

static int ValidateFeedBufferHandle(U32 handle)
{
    FEED_BUFFER *fbr = (FEED_BUFFER *) handle;
    if(fbr && fbr->tag == FEED_BUFFER_TAG)
    {
        return 1;
    }
    return 0;
}

void myDelay(int dt)
{
    struct timeval tv;
    fd_set fdr;
    FD_ZERO(&fdr);
    tv.tv_sec = dt / 1000000;
    tv.tv_usec = dt % 1000000;
    select(1, &fdr, NULL, NULL, &tv);
}

U32 OpenFeedBuffer(U32 bsize)
{
    FEED_BUFFER *fbr;
    int res = 0;
    fbr = (FEED_BUFFER *) malloc(sizeof(FEED_BUFFER));
    memset(fbr, 0, sizeof(FEED_BUFFER));
    fbr->tag = FEED_BUFFER_TAG;
    res = sem_init(& (fbr->bufferEvent), 0, 1);
    if(res != 0)
    {
        free(fbr);
        return 0;
    }
    res = pthread_mutex_init(& (fbr->PositionMutex), NULL);
    if(res != 0)
    {
        sem_destroy(& (fbr->bufferEvent));
        free(fbr);
        return 0;
    }
    fbr->databuffer = (char *) malloc(bsize);
    if(!(fbr->databuffer))
    {
        sem_destroy(& (fbr->bufferEvent));
        pthread_mutex_destroy(& (fbr->PositionMutex));
        free(fbr);
        return 0;
    }
    fbr->pRead = fbr->pWrite = 0;
    fbr->bufferlen = bsize;
    return (U32) fbr;
}
int PauseFeedBuffer(U32 handle, int pause)
{
    FEED_BUFFER *fbr;
    if(!ValidateFeedBufferHandle(handle))
    {
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    if(pause)
    {
        fbr->status |= FEED_BUFFER_CLOSING;
        sem_post(& (fbr->bufferEvent));
    }
    else
    {
        fbr->status &= ~(FEED_BUFFER_CLOSING);
    }
    return 0;
}


int CloseFeedBuffer(U32 handle)
{
    FEED_BUFFER *fbr;
    int tt = 0;
    if(!ValidateFeedBufferHandle(handle))
    {
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    fbr->status |= FEED_BUFFER_CLOSING;
    LIBHAL_INFO("CloseFeedBuffer>>>before sem_post!!!,handle=%d", handle);
    sem_post(& (fbr->bufferEvent));
    while((fbr->status & FEED_BUFFER_READING) ||
            (fbr->status & FEED_BUFFER_WRITTING))
    {
        myDelay(10000);
        if(tt++ > 500)
        {
            LIBHAL_ALARM("CloseFeedBuffer>>>timeout!!!,handle=%d,status=%08x", handle, fbr->status);
            break;
        }
    }
    LIBHAL_INFO("\nCloseFeedBuffer>>>before pthread_mutex_lock");
    pthread_mutex_lock(& (fbr->PositionMutex));
    pthread_mutex_destroy(& (fbr->PositionMutex));
    sem_destroy(& (fbr->bufferEvent));
    if(fbr->databuffer)
    {
        free(fbr->databuffer);
    }
    memset(fbr, 0, sizeof(FEED_BUFFER));
    free(fbr);
    LIBHAL_INFO("\nCloseFeedBuffer>>>leave!!!");
    return 0;
}

int ReadFeedBuffer(U32 handle, char *dst, U32 blocklen, U32 nums)
{
    int totallen, bk1, bk2, rbks, rlen, bk3;
    FEED_BUFFER *fbr;
    fbr = (FEED_BUFFER *) handle;
    if(!ValidateFeedBufferHandle(handle))
    {
        return -1;
    }
    if(fbr->databuffer == NULL ||
            (fbr->status & FEED_BUFFER_CLOSING))
    {
        return -1;
    }
    fbr->status |= FEED_BUFFER_READING;
    pthread_mutex_lock(& (fbr->PositionMutex));
    if(fbr->pRead == fbr->pWrite)
    {
        bk1 = bk2 = 0;
        totallen = 0;
    }
    else if(fbr->pRead > fbr->pWrite)
    {
        bk1 = fbr->bufferlen - fbr->pRead;
        bk2 = fbr->pWrite;
        totallen = bk1 + bk2;
    }
    else
    {
        bk1 = fbr->pWrite - fbr->pRead;
        bk2 = 0;
        totallen = bk1;
    }
    pthread_mutex_unlock(& (fbr->PositionMutex));
    /*if(fbr->bufferlen - totallen >= 1024)*/
    if(fbr->bufferlen - totallen >= 100)
    {
        sem_post(& (fbr->bufferEvent));
    }
    if(totallen < (int) blocklen)
    {
        fbr->status &= ~(FEED_BUFFER_READING);
        return 0;
    }
    else
    {
        rbks = blocklen * nums;
        rlen = 0;
        pthread_mutex_lock(& (fbr->PositionMutex));
        if(bk1 > 0)
        {
            if(bk1 <= rbks)
            {
                memcpy(dst, & (fbr->databuffer[fbr->pRead]), bk1);
                rlen = bk1;
            }
            else
            {
                memcpy(dst, & (fbr->databuffer[fbr->pRead]), rbks);
                rlen = rbks;
            }
        }
        if(bk2 > 0 && rlen < rbks)
        {
            if(bk2 <= (rbks - rlen))
            {
                memcpy(&dst[rlen], & (fbr->databuffer[0]), bk2);
                rlen += bk2;
            }
            else
            {
                memcpy(&dst[rlen], & (fbr->databuffer[0]), rbks - rlen);
                rlen = rbks;
            }
        }
        bk3 = rlen / blocklen;
        rlen = bk3 * blocklen;
        if(bk3 > 0)
        {
            fbr->pRead += rlen;
            fbr->pRead %= fbr->bufferlen;
        }
        pthread_mutex_unlock(& (fbr->PositionMutex));
    }
    fbr->status &= ~(FEED_BUFFER_READING);
    return (rlen / blocklen);
}

int WriteFeedBuffer(U32 handle, char *src, U32 blocklen, U32 nums)
{
    int totallen, bk1, bk2, rbks, rlen, bk3;
    FEED_BUFFER *fbr;
    if(!ValidateFeedBufferHandle(handle))
    {
        LIBHAL_ALARM("in writeFeedBuffer handle invalid!!!");
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    if(fbr->databuffer == NULL ||
            (fbr->status & FEED_BUFFER_CLOSING))
    {
        LIBHAL_ERROR("fbr->databuffer == NULL");
        return -1;
    }
    fbr->status |= FEED_BUFFER_WRITTING;
    pthread_mutex_lock(& (fbr->PositionMutex));
    if(fbr->pWrite >= fbr->pRead)
    {
        if(fbr->pRead == 0)
        {
            bk2 = 0;
            bk1 = fbr->bufferlen - 1 - fbr->pWrite;
        }
        else
        {
            bk1 = fbr->bufferlen - fbr->pWrite;
            bk2 = fbr->pRead - 1;
        }
        totallen = bk1 + bk2;
    }
    else
    {
        bk1 = fbr->pRead - fbr->pWrite - 1;
        bk2 = 0;
        totallen = bk1;
    }
    pthread_mutex_unlock(& (fbr->PositionMutex));
    if(totallen < (int) blocklen)
    {
        fbr->status &= ~(FEED_BUFFER_WRITTING);
        sem_trywait(& (fbr->bufferEvent));
        if(fbr->status & FEED_BUFFER_CLOSING)
        {
            return -1;
        }
        return 0;
    }
    else
    {
        rbks = blocklen * nums;
        rlen = 0;
        pthread_mutex_lock(& (fbr->PositionMutex));
        if(bk1 > 0)
        {
            if(bk1 <= rbks)
            {
                memcpy(& (fbr->databuffer[fbr->pWrite]), src, bk1);
                rlen = bk1;
            }
            else
            {
                memcpy(& (fbr->databuffer[fbr->pWrite]), src, rbks);
                rlen = rbks;
            }
        }
        if(bk2 > 0 && rlen < rbks)
        {
            if(bk2 <= (rbks - rlen))
            {
                memcpy(& (fbr->databuffer[0]), &src[rlen], bk2);
                rlen += bk2;
            }
            else
            {
                memcpy(& (fbr->databuffer[0]), &src[rlen], rbks - rlen);
                rlen = rbks;
            }
        }
        bk3 = rlen / blocklen;
        if(bk3)
        {
            fbr->pWrite += rlen;
            fbr->pWrite %= fbr->bufferlen;
        }
        pthread_mutex_unlock(& (fbr->PositionMutex));
    }
    fbr->status &= ~(FEED_BUFFER_WRITTING);
    return (rlen / blocklen);
}

int WriteFeedBufferSync(U32 handle, char *src, U32 blocklen, U32 nums)
{
    int wlen = 0, rlen, totallen = 0;
    FEED_BUFFER *fbr;
    if(!ValidateFeedBufferHandle(handle))
    {
        LIBHAL_ERROR("nBUFFER_HANDLE invalid!!!");
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    if(fbr->status & FEED_BUFFER_CLOSING)
    {
        return -1;
    }
    while(wlen < (int) nums)
    {
        sem_wait(& (fbr->bufferEvent));
        if(fbr->status & FEED_BUFFER_CLOSING)
        {
            sem_post(& (fbr->bufferEvent));
            return -1;
        }
        sem_post(& (fbr->bufferEvent));
        rlen = WriteFeedBuffer(handle, &src[totallen], blocklen, nums - wlen);
        if(rlen > 0)
        {
            wlen += rlen;
            totallen = blocklen * wlen;
        }
        else if(rlen < 0)
        {
            return -1;
        }
    }
    return wlen;
}

int ReadFeedBufferSync(U32 handle, char *dst, U32 blocklen, U32 nums)
{
    int wlen = 0, rlen, totallen = 0;
    if(!ValidateFeedBufferHandle(handle))
    {
        return -1;
    }
    while(wlen < (int) nums)
    {
        rlen = ReadFeedBuffer(handle, &dst[totallen], blocklen, nums - wlen);
        if(rlen > 0)
        {
            wlen += rlen;
            totallen = blocklen * wlen;
        }
        else if(rlen < 0)
        {
            return wlen;
        }
        else
        {
            myDelay(10000);
        }
    }
    return wlen;
}
int ClearFeedBuffer(U32 handle)
{
    FEED_BUFFER *fbr;
    int tt = 0;
    if(!ValidateFeedBufferHandle(handle))
    {
        LIBHAL_ERROR("in writeFeedBuffer handle invalid!!!");
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    fbr->status |= FEED_BUFFER_CLOSING;
    sem_post(& (fbr->bufferEvent));
    while((fbr->status & FEED_BUFFER_READING) ||
            (fbr->status & FEED_BUFFER_WRITTING))
    {
        myDelay(10000);
        if(tt++ > 500)
        {
            break;
        }
    }
    pthread_mutex_lock(& (fbr->PositionMutex));
    fbr->pWrite = fbr->pRead = 0;
    memset(fbr->databuffer, 0, fbr->bufferlen);
    sem_post(& (fbr->bufferEvent));
    pthread_mutex_unlock(& (fbr->PositionMutex));
    fbr->status &= ~(FEED_BUFFER_CLOSING);
    return 0;
}
int GetBufferAvailiableLength(U32 handle)
{
    int totallen, bk1, bk2;
    FEED_BUFFER *fbr;
    if(!ValidateFeedBufferHandle(handle))
    {
        LIBHAL_ERROR("in writeFeedBuffer handle invalid!!!");
        return -1;
    }
    fbr = (FEED_BUFFER *) handle;
    pthread_mutex_lock(& (fbr->PositionMutex));
    if(fbr->pWrite >= fbr->pRead)
    {
        if(fbr->pRead == 0)
        {
            bk2 = 0;
            bk1 = fbr->bufferlen - 1 - fbr->pWrite;
        }
        else
        {
            bk1 = fbr->bufferlen - fbr->pWrite;
            bk2 = fbr->pRead - 1;
        }
        totallen = bk1 + bk2;
    }
    else
    {
        bk1 = fbr->pRead - fbr->pWrite - 1;
        bk2 = 0;
        totallen = bk1;
    }
    pthread_mutex_unlock(& (fbr->PositionMutex));
    return totallen;
}
