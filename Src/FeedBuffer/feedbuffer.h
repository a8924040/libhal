#ifndef _FEEDER_BUFFER_H_
#define _FEEDER_BUFFER_H_
#ifdef __cplusplus
extern "C"
{
#endif
    U32 OpenFeedBuffer(U32 bsize);
    S32 CloseFeedBuffer(U32 handle);
    S32 ClearFeedBuffer(U32 handle);
    S32 PauseFeedBuffer(U32 handle, S32 pause);

    S32 ReadFeedBuffer(U32 handle, S8 *dst, U32 blocklen, U32 nums);
    S32 ReadFeedBufferSync(U32 handle, S8 *dst, U32 blocklen, U32 nums);
    S32 WriteFeedBuffer(U32 handle, S8 *src, U32 blocklen, U32 nums);
    S32 WriteFeedBufferSync(U32 handle, S8 *src, U32 blocklen, U32 nums);
    S32 GetBufferAvailiableLength(U32 handle);
    void myDelay(S32 dt);
#ifdef __cplusplus
}
#endif
#endif
